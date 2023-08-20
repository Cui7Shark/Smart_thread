#pragma once
#include <thread>

namespace wsp::details {

struct join {};
struct detach {};

template <typename T>
class autothread {};

template <>
class autothread<join> {

private:
    std::thread thrd;

public:
    //用传输线程对象t初始化成员变量thrd ， 右值引用 移动语义
    autothread(std::thread&& t) : thrd(std::move(t)) {}
    autothread(const autothread& other) = delete;
    autothread(autothread&& other) = default;
    /*确保在autothread对象被销毁时，
    已经创建的线程能够正常结束。如果线程已经结束，那么join()函数不会有任何影响。*/
    ~autothread() {
        if (thrd.joinable())
            thrd.join();
    }
    //别名 线程id
    using id = std::thread::id;
    id get_id() {
        return thrd.get_id();
    }
};

template<>
class autothread<detach> {

private:
    std::thread thrd;
public:
    autothread(std::thread&& t): thrd(std::move(t)) {}
    autothread(const autothread& other) = delete;
    autothread(autothread&& other) = default;
    ~autothread() { if (thrd.joinable()) thrd.detach(); }

    using id = std::thread::id;
    id get_id() { return thrd.get_id(); }
};
}
//封装线程类，包含id，join(), detach()函数