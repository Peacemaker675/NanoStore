#include"database/Database.hpp"
#include<fstream>
#include<format>
#include<chrono>

/*
    Replace std::flush with '\n' to increase performance, it slows down due to disk ops :)
*/


Database::Database(){
    shards.reserve(NUM_DB);
    for(size_t i=0; i<NUM_DB; i++){
        auto new_shard = std::make_unique<Shard>();
        new_shard->file.open("../logs/logs_"+std::to_string(i)+".txt", std::ios::out | std::ios::app | std::ios::in);
        new_shard->file.clear();
        recover(new_shard->file, new_shard->map);
        shards.emplace_back(std::move(new_shard));
    }
}

Database::~Database(){
    for(size_t i=0; i<NUM_DB; i++){
        shards[i]->file.close();
    }
}

size_t Database::get_db_index(const std::string key){
    std::hash<std::string> hasher;
    return hasher(key) % NUM_DB;
}

void Database::set(std::string key, int ttl, std::string data){
    size_t db_idx = get_db_index(key);
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    Value v;
    v.data = std::move(data);
    v.TTl = ttl;
    v.creation_time = now;
    std::unique_lock<std::mutex> lock(shards[db_idx]->mtx);
    std::string log_entry = std::format("SET {} {} {} {}\n", timestamp, ttl, key, v.data);
    shards[db_idx]->file.clear();
    shards[db_idx]->file << log_entry << std::flush;
    shards[db_idx]->map[key] = v;
}

std::string Database::get(std::string key){
    size_t db_idx = get_db_index(key);
    auto now = std::chrono::steady_clock::now();
    std::unique_lock<std::mutex> lock(shards[db_idx]->mtx);
    if(shards[db_idx]->map.find(key) == shards[db_idx]->map.end()){
        lock.unlock();
        throw std::string("ERR: KEY NOT FOUND\n");
    }
    if(duration_cast<std::chrono::seconds>(now - shards[db_idx]->map[key].creation_time).count() >= shards[db_idx]->map[key].TTl){
        shards[db_idx]->map.erase(key);
        throw std::string("ERR: KEY EXPIRED\n");
    }
    return shards[db_idx]->map[key].data+'\n';
}

void Database::del(std::string key){
    size_t db_idx = get_db_index(key);
    std::unique_lock<std::mutex> lock(shards[db_idx]->mtx);
    if(shards[db_idx]->map.find(key) == shards[db_idx]->map.end()){
        lock.unlock();
        throw std::string("ERR: KEY NOT FOUND\n");
    }
    shards[db_idx]->file<<"DEL "<<key<<"\n"<<std::flush;
    shards[db_idx]->map.erase(key);
}

void Database::recover(std::fstream& file, std::unordered_map<std::string, Value>& mpp) {
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