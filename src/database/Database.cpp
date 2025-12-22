#include"database/Database.hpp"
#include<chrono>

void Database::set(std::string key, int ttl, std::string data){
    Value v;
    v.data = std::move(data);
    v.TTl = ttl;
    v.creation_time = std::chrono::steady_clock::now();
    std::unique_lock<std::mutex> lock(db_mtx);
    mpp[key] = v;
}

std::string Database::get(std::string key){
    auto now = std::chrono::steady_clock::now();
    std::unique_lock<std::mutex> lock(db_mtx);
    if(mpp.find(key) == mpp.end()){
        lock.unlock();
        throw std::string("ERR: KEY NOT FOUND\n");
    }
    if(duration_cast<std::chrono::seconds>(now - mpp[key].creation_time).count() >= mpp[key].TTl){
        mpp.erase(key);
        throw std::string("ERR: KEY EXPIRED\n");
    }
    return mpp[key].data+'\n';
}

void Database::del(std::string key){
    std::unique_lock<std::mutex> lock(db_mtx);
    if(mpp.find(key) == mpp.end()){
        lock.unlock();
        throw std::string("ERR: KEY NOT FOUND\n");
    }
    mpp.erase(key);
}