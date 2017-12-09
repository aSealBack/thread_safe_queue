#include "thread_safe_queue.h"
#include <iostream>

int main() {
    thread_safe_queue<int> q;
    q.push(1);
    q.emplace(23);
    std::cout << "Front element: " << q.front() << std::endl;
    std::cout << "Back element: " << q.back() << std::endl;
    q.pop();
    if (q.empty())
        std::cout << "'q' is empty" << std::endl;
    std::cin.get();
    return 0;
}
