#pragma once

#include<functional>

using Task = std::function<void()>;

typedef struct Value{
    std::string data;
    int TTl;
};