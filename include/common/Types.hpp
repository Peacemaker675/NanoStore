#pragma once

#include<functional>
#include<string>
#include<chrono>

using Task = std::function<void()>;

struct Value{
    std::string data;
    int TTl;
    std::chrono::time_point<std::chrono::steady_clock> creation_time;
};