#pragma once

#include<stdalign.h>
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

struct alignas(64) Shard{
    std::unordered_map<std::string, Value> map;
    std::mutex mtx;
    std::fstream file;
};