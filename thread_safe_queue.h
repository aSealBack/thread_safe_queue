#pragma once

#ifndef _THREAD_SQFE_QUEUE_
#define _THREAD_SQFE_QUEUE_
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

template <class T, class Container = std::deque<T>>
class thread_safe_queue {
public:
    /// Typedefs
    using value_type = typename Container::value_type;
    using container_type = Container;
    using size_type = typename Container::size_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;

    explicit thread_safe_queue()
    : underlying_container() {}

    // DESIGN thread_safe_queue(const thread_safe_queue & another);
    // DESIGN thread_safe_queue(container_type && container);
    // DESIGN thread_safe_queue & operator=(const thread_safe_queue &);

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return underlying_container.empty();
    }

    size_type size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return underlying_container.size();
    }

    reference front() {
        std::lock_guard<std::mutex> lock(mutex);
        return underlying_container.front();
    }

    const_reference front() const {
        std::lock_guard<std::mutex> lock(mutex);
        return underlying_container.front();
    }

    reference back() {
        std::lock_guard<std::mutex> lock(mutex);
        return underlying_container.back();
    }

    const_reference back() const {
        std::lock_guard<std::mutex> lock(mutex);
        return underlying_container.back();
    }

    void push(value_type && elem) {
        std::lock_guard<std::mutex> lock(mutex);
        underlying_container.push_back(std::forward<value_type>(elem));
        sentinel.notify_one();
    }

    template <class... Args>
    void emplace(Args && ... args) {
        std::lock_guard<std::mutex> lock(mutex);
        underlying_container.push_back(std::forward<Args>(args) ...);
        sentinel.notify_one();
    }

    void pop() {
        std::lock_guard<std::mutex> lock(mutex);
        unsafe_pop();
    }

    value_type try_get(const std::chrono::milliseconds & timeout) {
        std::unique_lock<std::mutex> lock(mutex);
        if (sentinel.wait_for(lock, timeout, [this]() -> bool { return !queue.empty(); }))
            return value_type();
        value_type res = unsafe_front();
        unsafe_pop();
        return res;
    }

private:
    container_type underlying_container;
    mutable std::mutex mutex;
    // checks that the queue is not empty
    std::condition_variable sentinel;

private:
    reference unsafe_front() {
        return underlying_container.front();
    }

    void unsafe_pop() {
        underlying_container.pop_front();
    }
};

#endif // _THREAD_SQFE_QUEUE_
