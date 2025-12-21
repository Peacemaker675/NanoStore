#pragma once
#include<unordered_map>
#include<string>
#include<mutex>
#include"../common/Types.hpp"

class Database{
public:
    void set(std::string key, std::string data, int ttl);
    std::string get(std::string key);
    void del(std::string key);
private:
    std::unordered_map<std::string, Value> mpp;
    std::mutex db_mtx;
};