#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<queue>
#include<functional>
#include<condition_variable>

class ThreadPool {
public:
    ThreadPool(size_t num_threads) : stop(false){
        for(size_t i=0; i < num_threads; i++){
            workers.emplace_back([this] {
                for(;;){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(taskQueue_mtx); // sets a lock on task_queue so no other thread can use it meanwhile
                        condition.wait(lock, [this]{ return stop || !task_queue.empty(); }); // this unlocks the lock and waits for condition to relock the mutex
                        if(stop && task_queue.empty()){ // to terminate the thread if no task and threadpool is destroyed
                            return;
                        }
                        task = std::move(task_queue.front()); // transfers the owenership of the task,if we dont use move it will make a deep copy
                        task_queue.pop();
                    }
                    task(); // begins task
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& task){  // this create F a forwarding reference (lvalue & rvalue) preserving user intent
        {
            std::unique_lock<std::mutex> lock(taskQueue_mtx);
            task_queue.emplace(std::forward<F>(task)); // we forward the task into task_qeue (rvalue remains rvalue , lvalue remains lvalue)
        }
        condition.notify_one(); // notify a random thread that task_queue is unlocked
    }

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lock(taskQueue_mtx); // we need to lock the task queue first as some thread might read it when we are updating stop
            stop = true;
        }
        condition.notify_all(); // notify all threads that stop has been changed
        for(std::thread& t : workers){
            if(t.joinable()) t.join(); // join all threads
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> task_queue;
    std::condition_variable condition;
    std::mutex taskQueue_mtx;
    bool stop;
};

int main(){
    ThreadPool pool(4);
    for(int i=0;i<4;i++){
        pool.enqueue([i]{
            std::cout<<"TASK "<<i<<"\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        });
    }
}