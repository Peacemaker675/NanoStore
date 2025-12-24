#pragma once

#include<functional>
#include<string>
#include<chrono>
#include<mutex>
#include<fstream>

using Task = std::function<void()>;

struct Value{
    std::string data;
    int TTl;
    std::chrono::time_point<std::chrono::steady_clock> creation_time;
};

struct Shard{
    std::unordered_map<std::string, Value> map;
    std::mutex mtx;
    std::fstream file;
};