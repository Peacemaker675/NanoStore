#pragma once
#include<unordered_map>
#include<string>
#include<mutex>
#include<fstream>
#include<memory>
#include"common/Types.hpp"

class Database{
public:
    Database();
    ~Database();
    size_t get_db_index(const std::string key);
    void set(std::string key, int ttl, std::string data);
    std::string get(std::string key);
    void del(std::string key);
    void recover(std::fstream& file, std::unordered_map<std::string, Value>& mpp);
private:
    const int NUM_DB = 16;
    std::vector<std::unique_ptr<Shard>> shards;
};