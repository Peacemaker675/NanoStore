#include"database/Database.hpp"
#include<fstream>
#include<format>
#include<chrono>

/*
    Replace std::flush with '\n' to increase performance, it slows down due to disk ops :)
*/


Database::Database(){
    file.open("../logs/logs.txt", std::ios::out | std::ios::app | std::ios::in); // this file path is with respect to build
    file.clear();
    recover();
}

Database::~Database(){
    file.close();
}

void Database::set(std::string key, int ttl, std::string data){
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    Value v;
    v.data = std::move(data);
    v.TTl = ttl;
    v.creation_time = now;
    {
        std::unique_lock<std::mutex> lock(log_mtx);
        std::string log_entry = std::format("SET {} {} {} {}\n", timestamp, ttl, key, v.data);
        file.clear();
        file << log_entry << std::flush;
    }
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
    file<<"DEL "<<key<<"\n"<<std::flush;
    mpp.erase(key);
}

void Database::recover() {
    if (!file.is_open()) return;

    std::string line;
    file.clear();
    file.seekg(0, std::ios::beg); // move file reador to begining of file
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string command, key, val;
        long long timestamp; 
        int ttl;

        ss >> command;
        if (command == "SET") {
            ss >> timestamp >> ttl >> key;
            std::getline(ss, val);
            auto created_time = std::chrono::steady_clock::time_point(
                std::chrono::milliseconds(timestamp)
            );
            auto now = std::chrono::steady_clock::now();
            
            if (now - created_time < std::chrono::seconds(ttl)) {
                Value v;
                v.data = val;
                v.TTl = ttl;
                v.creation_time = created_time;
                mpp[key] = v;
            }
        } 
        else if (command == "DEL") {
            ss >> key;
            mpp.erase(key);
        }
    }
}