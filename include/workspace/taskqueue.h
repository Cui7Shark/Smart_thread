#pragma once
#include<deque>
#include<mutex>

namespace wsp::details
{
/**
 * @brief 线程安全的任务队列
 * @tparam T runnable object
 * @note The performance of pushing back is better
 */

//定义一个任务队列类 -- 入队 出队，队长度
template<typename T>
class taskqueue {

private:
    std::mutex tq_lok;
    std::deque<T> q;
public:
    using size_type = typename std::deque<T>::size_type;
    taskqueue() = default;
    taskqueue(const taskqueue&) = delete; // 不需要拷贝构造
    taskqueue(taskqueue&&) = default; 
public:
    void push_back(T &v) {
    /*  std::lock_guard是一个RAII（资源获取即初始化）类，
    用于在其作用域结束时自动释放锁。它接受一个互斥量（std::mutex）作为参数，
    并在构造函数中获取该互斥量的锁，析构函数中释放锁。

    在这里，tq_lok是一个std::mutex对象，用于实现线程安全。
    通过创建std::lock_guard对象并传入tq_lok，可以确保在lock_guard对象的作用域结束时，
    tq_lok的锁将被自动释放。

    这种使用std::lock_guard的方式可以简化锁的管理，
    避免忘记释放锁而导致的死锁或资源竞争问题 */
        std::lock_guard<std::mutex> lock(tq_lok);
        q.emplace_back(v);
    }
    void push_back(T &&v) {
        std::lock_guard<std::mutex> lock(tq_lok);
        q.emplace_back(std::move(v));
    }
    void push_front(T& v) {
        std::lock_guard<std::mutex> lock(tq_lok);
        q.emplace_front(v);
    }
    void push_front(T&& v) {
        std::lock_guard<std::mutex> lock(tq_lok);
        q.emplace_front(std::move(v));
    }
    bool try_pop(T &tmp) {
        std::lock_guard<std::mutex> lock(tq_lok);
        if (!q.empty()) {
            tmp = std::move(q.front());
            q.pop_front();
            return true;
        }
        return false;
    }
    size_type length() {
        std::lock_guard<std::mutex> lock(tq_lok);
        return q.size();
    }
};

} // namespace wsp::details
