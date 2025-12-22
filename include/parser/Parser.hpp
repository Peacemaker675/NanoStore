#pragma once
#include<string>
#include<vector>
#include"database/Database.hpp"

class Parser{
public:
    static std::string process(std::string request, Database& db);
private:
    static std::vector<std::string> tokenize(std::string request);
};