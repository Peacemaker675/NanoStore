#pragma once

#include"../common/Types.hpp"
#include<queue>
#include<mutex>
#include<optional>
#include<condition_variable>

class BlockingQueue{
public:
    explicit BlockingQueue(size_t capacity); // explicit because we dont want implicit conversions
    ~BlockingQueue();

    void push(Task task);
    std::optional<Task> pop(); // optional as a thread might be waiting when we close and if we dont have any task then we return nullopt
    void shutdown();
private:
    std::queue<Task> q;
    std::mutex mtx;
    std::condition_variable condition;
    size_t q_capacity;
    bool stopped;
};