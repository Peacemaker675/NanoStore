#pragma once
#include<unordered_map>
#include<string>
#include<mutex>
#include<fstream>
#include"common/Types.hpp"

class Database{
public:
    Database();
    ~Database();
    void set(std::string key, int ttl, std::string data);
    std::string get(std::string key);
    void del(std::string key);
    void recover();
private:
    std::unordered_map<std::string, Value> mpp;
    std::fstream file;
    std::mutex db_mtx;
    std::mutex log_mtx;
};