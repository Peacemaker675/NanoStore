#pragma once
#include<string>
#include<vector>
#include"database/Database.hpp"

class Parser{
public:
    static std::string process(std::string_view request, Database& db);
};