#pragma once
#include<cstdlib>
#include<map>
#include<memory>
#include<future>
#include<iostream>
#include<condition_variable>

#include <workspace/taskqueue.h>
#include <workspace/autothread.h>
#include <workspace/utility.h>

namespace wsp::details {

class workbranch {
private:

    using worker = autothread<detach>; // detach 模板类 ， 线程分离
    using worker_map = std::map<worker::id, worker>; //存储id，任务线程 映射

    sz_t decline = 0;
    sz_t task_done_workers = 0;
    bool is_waiting = false;
    bool destructing =false;

    worker_map workers = {};
    taskqueue<std::function<void()>> tq = {};

    std::mutex lok = {};
    //条件变量， 线程间通信
    std::condition_variable thread_cv = {}; 
    std::condition_variable task_done_cv = {};
public:
    /**
     * @brief 构造函数
     * @param wks_n 初始化任务数
    */
   //显示的构造函数
   //预防以下的隐式的转换
   //int num = 5;
   //workbranch wb = num;
   explicit workbranch(int wks_n = 1) {
        for (int i = 0; i < std::max(wks_n, 1); ++i) {
            add_worker();
        }
   }

   workbranch(const workbranch&) = delete;
    workbranch(workbranch&&) = delete;

    /* 使用thread_cv条件变量的wait()函数，将当前线程等待在lock上，
    直到decline变量的值变为0。这样做的目的是等待所有worker处理完任务后再销毁对象。
    
    在wait()函数中，使用lambda函数作为条件，判断decline变量的值是否为0。
    wait()函数会在条件满足时解锁lock，并阻塞当前线程。
    当条件不满足时，wait()函数会重新加锁lock，并继续等待。
    
    总之，该析构函数的作用是等待所有worker处理完任务后再销毁workbranch对象。
    通过使用互斥锁和条件变量，确保在销毁对象之前所有任务都已完成，
    避免了对象销毁过程中的数据竞争和不一致性 */
    ~workbranch() { 
        std::unique_lock<std::mutex> lock(lok);
        decline = workers.size();
        destructing = true;
        thread_cv.wait(lock, [this]{ return !decline; });
    } 


public:
    /**
     * @brief 添加任务
     * @note O(logN)
    */
    void add_worker() {
        std::lock_guard<std::mutex> lock(lok);
        std::thread t(&workbranch::mission, this);
        workers.emplace(t.get_id(), std::move(t));
    }

    /**
     * @brief delete one worker
     * @note O(1)
     */
    void del_worker() {
        std::lock_guard<std::mutex> lock(lok);
        if (workers.empty()) {
            throw std::runtime_error("workspace: No worker in workbranch to delete");
        } else {
            decline++;
        }
    }

    /**
     * @brief Wait for all tasks done.
     * @brief This interface will pause all threads(workers) in workbranch to relieve system's stress. 
     * @param timeout timeout for waiting (ms)
     * @return return true if all tasks done 
     */
    bool wait_tasks(unsigned timeout = -1) {
        bool res;
        {
            std::unique_lock<std::mutex> locker(lok);
            is_waiting = true; // task_done_workers == 0
            res = task_done_cv.wait_for(locker, std::chrono::milliseconds(timeout), [this]{
                return task_done_workers >= workers.size();  // use ">=" to avoid supervisor delete workers  
            }); 
            task_done_workers = 0;
            is_waiting = false;
        }
        thread_cv.notify_all();  // recover
        return res;
    }
public:
    /**
     * @brief get number of workers
     * @return number 
     */
    sz_t num_workers() {
        std::lock_guard<std::mutex> lock(lok);
        return workers.size();
    }
    /**
     * @brief get number of tasks in the task queue
     * @return number 
     */
    sz_t num_tasks() {
        return tq.length();
    }
public:
    /**
     * @brief async execute the task 
     * @param task runnable object (normal)
     * @return void
     */

    template <typename T = normal, typename F, 
        typename R = details::result_of_t<F>,  
        typename DR = typename std::enable_if<std::is_void<R>::value>::type>
    auto submit(F&& task) -> typename std::enable_if<std::is_same<T, normal>::value>::type {
        tq.push_back([task]{
            try {
                task();
            } catch (const std::exception& ex) {
                std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught exception:\n  what(): "<<ex.what()<<'\n'<<std::flush;
            } catch (...) {
                std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught unknown exception\n"<<std::flush;
            }
        }); // function 将lambda表达式作为任务函数提交到任务队列中
    }

    /**
     * @brief async execute the task 
     * @param task runnable object (urgent)
     * @return void
     */
    template <typename T, typename F,  
        typename R = details::result_of_t<F>, 
        typename DR = typename std::enable_if<std::is_void<R>::value>::type>
    auto submit(F&& task) -> typename std::enable_if<std::is_same<T, urgent>::value>::type {
        tq.push_front([task]{
            try {
                task();
            } catch (const std::exception& ex) {
                std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught exception:\n  what(): "<<ex.what()<<'\n'<<std::flush;
            } catch (...) {
                std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught unknown exception\n"<<std::flush;
            }
        });
    }

     /**
     * @brief async execute tasks 
     * @param task runnable object (sequence)
     * @param tasks other parameters
     * @return void
     */
    template <typename T, typename F, typename... Fs>
    auto submit(F&& task, Fs&&... tasks) -> typename std::enable_if<std::is_same<T, sequence>::value>::type {
        tq.push_back([=]{
            try {
                this->rexec(task, tasks...);
            } catch (const std::exception& ex) {
                std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught exception:\n  what(): "<<ex.what()<<'\n'<<std::flush;
            } catch (...) {
                std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught unknown exception\n"<<std::flush;
            }
        });
    }

    /**
     * @brief async execute the task
     * @param task runnable object (normal)
     * @return std::future<R>
     */
    /**
     * std::promise<> 是一个在异步操作中传递和设置结果值或异常值的类模板
     * 
     * std::promise<R>被用于创建一个用于存储任务结果的对象,
     * 用于在异步操作中传递和设置结果值或异常。
     * task_promise 设置为任务的结果
     * 通过`std::promise`和`std::future`的配合使用，可以实现任务的异步执行和获取结果。
     * `std::promise`用于设置任务的结果，而`std::future`则用于获取任务的结果。
     * */ 
    template <typename T = normal, typename F, 
        typename R = details::result_of_t<F>, 
        typename DR = typename std::enable_if<!std::is_void<R>::value, R>::type>
    auto submit(F&& task, typename std::enable_if<std::is_same<T, normal>::value, normal>::type = {}) -> std::future<R> {
        std::function<R()> exec(std::forward<F>(task));
        std::shared_ptr<std::promise<R>> task_promise = std::make_shared<std::promise<R>>();
        tq.push_back([exec, task_promise] {
            try {
                task_promise->set_value(exec()); // 将任务的返回值设置为promise的值
            } catch (...) {
                try { 
                // 如果任务执行过程中发生了异常，`std::exception_ptr`会被捕获
                // 并通过`task_promise->set_exception()`设置为`promise`的异常。
                    task_promise->set_exception(std::current_exception());
                } catch (const std::exception& ex) {
                    std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught exception:\n  what(): "<<ex.what()<<'\n'<<std::flush;
                } catch (...) {
                    std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught unknown exception\n"<<std::flush;
                }
            }
        });
        return task_promise->get_future();
    }

    /**
     * @brief async execute the task
     * @param task runnable object (urgent)
     * @return std::future<R>
     */
    template <typename T,  typename F,  
        typename R = details::result_of_t<F>, 
        typename DR = typename std::enable_if<!std::is_void<R>::value, R>::type>
    auto submit(F&& task, typename std::enable_if<std::is_same<T, urgent>::value, urgent>::type = {}) -> std::future<R> {
        std::function<R()> exec(std::forward<F>(task));
        std::shared_ptr<std::promise<R>> task_promise = std::make_shared<std::promise<R>>();
        tq.push_front([exec, task_promise] {
            try {
                task_promise->set_value(exec());
            } catch (...) {
                try {
                    task_promise->set_exception(std::current_exception());
                } catch (const std::exception& ex) {
                    std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught exception:\n  what(): "<<ex.what()<<'\n'<<std::flush;
                } catch (...) {
                    std::cerr<<"workspace: worker["<< std::this_thread::get_id()<<"] caught unknown exception\n"<<std::flush;
                }
            }
        });
        return task_promise->get_future();
    }

private:
    // thread 一直循环
    void mission() {
        std::function<void()>task;

    while (true) {
            if (decline <= 0 && tq.try_pop(task)) {
                task(); 
            } else if (decline > 0) {
                std::lock_guard<std::mutex> lock(lok);
                if (decline > 0 && decline--) { // double check
                    workers.erase(std::this_thread::get_id()); 
                    if (is_waiting)  
                        task_done_cv.notify_one();
                    if (destructing) 
                        thread_cv.notify_one();
                    return;
                }
            } else {
                if (is_waiting) { 
                    std::unique_lock<std::mutex> locker(lok);
                    task_done_workers++;
                    task_done_cv.notify_one();
                    thread_cv.wait(locker);  
                } else {
                    std::this_thread::yield(); 
                }
            }
        }
    }
    // 递归执行 只有一个函数作为参数
    template <typename F>
    void rexec(F &&func) {
        func();
    }

    /*
    首先调用了func()来执行第一个函数，
    然后使用递归调用rexec(std::forward<Fs>(funcs)...)来执行剩余的函数。
    这里使用了std::forward来完美转发函数参数。
    */
   template <typename F, typename... Fs>
   void rexec(F &&func, Fs &&... funcs) {
        func();
        rexec(std::forward<Fs>(funcs)...);
   }

};

}