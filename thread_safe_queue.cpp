#include "thread_safe_queue.h"

template <class T>
void thread_safe_queue<T>::add(T && elem) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(std::forward(elem));
    sentinel.notify_one();
}

template <class T>
T thread_safe_queue<T>::try_get(const std::chrono::milliseconds & timeout) {
    std::unique_lock<std::mutex> lock(mutex);
    if (sentinel.wait_for(lock, timeout, [this]() -> bool { return !queue.empty(); }))
        return T();
    T res = queue.front();
    queue.pop();
    return res;
}
