#pragma once

#include"BlockingQueue.hpp"
#include<vector>
#include<queue>
#include<thread>
#include<functional>
#include<mutex>
#include<condition_variable>

class ThreadPool{
public:
    explicit ThreadPool(size_t num_threads, size_t capacity);
    ~ThreadPool();
    
    template<class F>
    void enqueue(F&& task){  // this create F a forwarding reference (lvalue & rvalue) preserving user intent
        task_queue.push(std::forward<F>(task)); // we forward the task into task_qeue (rvalue remains rvalue , lvalue remains lvalue)
    }
private:
    std::vector<std::thread> workers;
    BlockingQueue task_queue;
};