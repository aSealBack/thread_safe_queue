#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

template <class T>
class thread_safe_queue {
public:
    thread_safe_queue();

    void add(T && elem);
    T try_get(const std::chrono::milliseconds & timeout);
private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable sentinel;
};
