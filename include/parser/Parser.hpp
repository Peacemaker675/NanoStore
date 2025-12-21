#pragma once
#include<string>
#include"../database/Database.hpp"

class Parser{
public:
    static std::string process(std::string request, Database& db);
};