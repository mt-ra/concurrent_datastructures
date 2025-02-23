// #pragma once

#include <iostream>
#include <mutex>
#include <memory>
#include <condition_variable>

/*
Multi-producer multi-consumer unbounded queue (lock-based)
    
    Based on C++ Concurrency in Action (Anthony Williams)

*/

template <typename T, typename Alloc = std::allocator<T>>
class MPMCUnboundedQueueLB1 {
private: // nested classes 
    struct node {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;
    };

private: // member variables
    std::mutex head_mutex_;
    std::mutex tail_mutex_;
    std::condition_variable nonempty_cond_;
    Alloc alloc_;

    std::shared_ptr<node> head_;
    std::shared_ptr<node> tail_;

public: // resource management
    MPMCUnboundedQueueLB1() : head_{std::allocate_shared<node>(alloc_)}, tail_{head_} {}
    MPMCUnboundedQueueLB1(const MPMCUnboundedQueueLB1& other) = delete;
    MPMCUnboundedQueueLB1& operator=(const MPMCUnboundedQueueLB1& other) = delete;

public: // wait methods
    void wait_push(std::shared_ptr<T> data) {
        try_push(data); // being full is not possible
    }

    void wait_pop(std::shared_ptr<T>& output) {
        std::unique_lock<std::mutex> head_lock{_await_nonempty()};
        auto old_head = _pop_head();
        output = old_head->data;
    }

    bool try_push(std::shared_ptr<T> data) {
        std::unique_lock<std::mutex> tail_lock{tail_mutex_};
        auto old_tail = tail_;
        tail_ = std::allocate_shared<node>(alloc_);
        old_tail->data = data;                      
        old_tail->next = tail_;
        nonempty_cond_.notify_one();
        return true;
    }

    bool try_pop(std::shared_ptr<T>& output) {
        std::unique_lock<std::mutex> head_lock{head_mutex_};
        if (head_ == _get_tail()) return false;
        auto old_head = _pop_head();
        output = old_head->data;
        return true;
    }

private: // helper functions
    std::shared_ptr<node> _pop_head() {
        auto old_head = head_;
        head_ = old_head->next;
        return old_head;
    }

    std::shared_ptr<node> _get_tail() {
        std::unique_lock<std::mutex> tail_lock{tail_mutex_};
        return tail_;
    }

    std::unique_lock<std::mutex> _await_nonempty() {
        std::unique_lock<std::mutex> head_lock{head_mutex_};
        nonempty_cond_.wait(head_lock, [&]{return head_ != _get_tail();});
        return std::move(head_lock);
    }
};