#include <iostream>
#include <thread>
#include <memory>

#include <lock-based/MPMCUnboundedQueueLB1.hpp>

template <typename Queue>
struct Pusher {
    std::shared_ptr<Queue> queue_;
    Pusher(std::shared_ptr<Queue> queue) : queue_{queue} {}
    void operator()() {
        for (int i = 0; i < 1000000; ++i) {
            queue_->wait_push(std::make_shared<int>(i));
        }
    }
};

template <typename Queue>
struct Popper {
    std::shared_ptr<Queue> queue_;
    Popper(std::shared_ptr<Queue> queue) : queue_{queue} {}
    void operator()() {
        for (int i = 0; i < 1000000; ++i) {
            std::shared_ptr<int> store;
            queue_->wait_pop(store);
        }
    }
};

int main() {
    auto q = std::make_shared<MPMCUnboundedQueueLB1<int>>();

    std::thread t1{Popper<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t2{Popper<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t3{Pusher<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t4{Pusher<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t5{Popper<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t6{Popper<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t7{Pusher<MPMCUnboundedQueueLB1<int>>{q}};
    std::thread t8{Pusher<MPMCUnboundedQueueLB1<int>>{q}};

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
}