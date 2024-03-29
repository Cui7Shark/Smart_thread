#pragma once
#include <future> // 异步
#include <functional> // 函数对象
#include <deque>  // 双端队列
#include <cstdlib>
#include <type_traits> // 类型获取

namespace wsp::details {

using sz_t = size_t;

// type trait 支持不同编译版本
#if __cplusplus >= 201703L 
template <typename F, typename... Args>
using result_of_t = std::invoke_result_t<F, Args...>; 
#else
template <typename F, typename... Args>
using result_of_t = typename std::result_of<F(Args...)>::type; 
#endif

// 事件类型
struct normal {};
struct urgent {};
struct sequence {};

/**
*@brief std::future 控制
*@tparam T 返回值类型
**/
template <typename T>
class futures {
    
private:
//定义了一个std::deque容器，用于存储多个异步操作的结果
    std::deque<std::future<T>> futs;
public:
    using iterator = typename std::deque<std::future<T>>::iterator;

    void wait() {
        for (auto& each: futs) {
            each.wait();
        }
    }
    sz_t size() {
        return futs.size();
    }
    /**
     * @brief get set of result
     * @return std::vector<T>
     */  
    auto get() ->std::vector<T> {
        std::vector<T> res;
        for (auto& each : futs) {
            res.emplace_back(each.get());
        }
        return res;
    }
    auto end() -> iterator& {
        return futs.end();
    }
    auto begin()->iterator& {
        return futs.begin();
    }
    void add_back(std::future<T>&& fut) {
        futs.emplace_back(std::move(fut));
    }
    void add_front(std::future<T>&& fut) {
        futs.emplace_front(std::move(fut));
    }
    //for_each() 重载
    void for_each(std::function<void(std::future<T>&)> deal) {
        for (auto& each: futs) {
            deal(each);
        }
    }
    void for_each(const iterator& first, std::function<void(std::future<T>&)> deal) {
        for (auto it = first; it != end(); ++it) {
            deal(*it);
        }
    }
    void for_each(const iterator& first, const iterator& last, std::function<void(std::future<T>&)> deal) {
        for (auto it = first; it != last; ++it) {
            deal(*it);
        }
    }
    // [] 重载
    auto operator [] (sz_t idx) -> std::future<T>& {
        return futs[idx];
    }
};
}

