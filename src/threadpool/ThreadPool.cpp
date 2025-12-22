#include"threadpool/ThreadPool.hpp"
#include<iostream>

ThreadPool::ThreadPool(size_t num_threads, size_t capacity) : task_queue(capacity){
    for(size_t i=0; i < num_threads; i++){
        workers.emplace_back([this] {
            for(;;){
                auto task_opt = task_queue.pop(); // task_queue sends an optional so first we need to check if it has any value
                if(!task_opt.has_value()) {
                    std::cout << "Thread " << std::this_thread::get_id() << " exiting..." << std::endl;
                    return;
                }
                Task task = std::move(task_opt.value()); // move the value from task_opt to task
                task(); // begins task
            }
        });
    }
}

ThreadPool::~ThreadPool(){
    task_queue.shutdown(); // we first need to close task_queue so it can set stop flag
    for(std::thread& t : workers){
        if(t.joinable()) t.join(); // join all threads
    }
}