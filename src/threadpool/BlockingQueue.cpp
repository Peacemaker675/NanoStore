#include"threadpool/BlockingQueue.hpp"

BlockingQueue::BlockingQueue(size_t capacity) :q_capacity(capacity), stopped(false){
}

void BlockingQueue::push(Task task){

    {
        std::unique_lock<std::mutex> lock(mtx);
        condition.wait(lock, [this] {return q.size() < q_capacity || stopped;}); // wait till we have room for new task or we have stopped
        if(stopped) return;
        q.emplace(std::move(task)); // emplace and move for memory optimization
    }
    condition.notify_one(); // notify a random thread that a new task is available
}

std::optional<Task> BlockingQueue::pop(){
    Task task;
    {
        std::unique_lock<std::mutex> lock(mtx);
        condition.wait(lock, [this] {return !q.empty() || stopped;}); // wait till we have a task or we have stopped
        if(stopped && q.empty()) return std::nullopt; // return nullopt (empty obj) of we have stopped and q is empty (graceful shutdown)
        task = std::move(q.front());
        q.pop();
    }
    condition.notify_one();
    return task;
}

void BlockingQueue::shutdown(){
    {
        std::lock_guard<std::mutex> lock(mtx);
        stopped = true;
    }
    condition.notify_all();
}

BlockingQueue::~BlockingQueue(){
}