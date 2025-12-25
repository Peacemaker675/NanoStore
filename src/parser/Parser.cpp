#include "parser/Parser.hpp"
#include <algorithm>

std::string Parser::process(std::string_view request, Database& db) {
    size_t pos1 = request.find(' '); // find first space to isolate the command
    std::string_view cmd_view = request.substr(0, pos1);
    std::string command(cmd_view); 
    std::transform(command.begin(), command.end(), command.begin(), 
                   [](unsigned char c){ return std::tolower(c); });

    if(command == "set"){
        if(pos1 == std::string_view::npos) return "ERR : USAGE SET <KEY> <TTL> <VAL>\n";
        size_t pos2 = request.find(' ', pos1 + 1); // space between key and ttl
        if(pos2 == std::string_view::npos) return "ERR : USAGE SET <KEY> <TTL> <VAL>\n";
        size_t pos3 = request.find(' ', pos2 + 1); // space between ttl and val
        if(pos3 == std::string_view::npos) return "ERR : USAGE SET <KEY> <TTL> <VAL>\n";

        std::string_view key_view = request.substr(pos1 + 1, pos2 - (pos1 + 1));
        std::string_view ttl_view = request.substr(pos2 + 1, pos3 - (pos2 + 1));
        std::string_view val_view = request.substr(pos3 + 1);

        try{
            int ttl = std::stoi(std::string(ttl_view));
            db.set(std::string(key_view), ttl, std::string(val_view));
        }catch(...){
            return "ERR : TTL SHOULD BE INT\n";
        }
        return "QUERY OK\n";
    }else if(command == "get"){
        if(pos1 == std::string_view::npos) return "ERR : USAGE GET <KEY>\n";

        std::string_view key_view = request.substr(pos1 + 1);
        if(!key_view.empty() && key_view.back() == '\n') key_view.remove_suffix(1);
        try{
            return db.get(std::string(key_view));
        }catch(const std::string& e){
            return e;
        }
    }else if(command == "del"){
        if (pos1 == std::string_view::npos) return "ERR : USAGE DEL <KEY>\n";
        
        std::string_view key_view = request.substr(pos1 + 1);
        if(!key_view.empty() && key_view.back() == '\n') key_view.remove_suffix(1);
        try{
            db.del(std::string(key_view));
        }catch(const std::string& e) {
            return e;
        }
        return "QUERY OK\n";
    }
    return "ERR : INVALID CMD {SET, GET, DEL}\n";
}