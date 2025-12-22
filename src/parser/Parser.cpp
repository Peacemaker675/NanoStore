#include<ranges>
#include"parser/Parser.hpp"

std::string Parser::process(std::string request, Database& db){
    std::vector<std::string> tokens = tokenize(request);
    if(tokens.size() < 2) return std::string("ERR : USAGE <CMD> ...\n");
    std::string command = std::move(tokens[0]);
    // convert command to lowercase
    std::transform(command.begin(), command.end(), command.begin(), [] (unsigned char c) {
        return std::tolower(c);
    });
    
    if(command == "set"){
        if(tokens.size() < 4) return std::string("ERR : USAGE SET <KEY> <TTL> <VAL>\n");
        try{
            db.set(tokens[1], stoi(tokens[2]), tokens[3]);
        }catch(...){
            return std::string("ERR : TTL SHOULD BE INT\n");
        }
        return std::string("QUERY OK\n");
    }else if(command == "get"){
        std:: string response;
        try{
            response = db.get(tokens[1]);
        }catch(std::string e){
            return e;
        }
        return response;
    }else if(command == "del"){
        try{
            db.del(tokens[1]);
        }catch(std::string e){
            return e;
        }
        return std::string("QUERY OK\n");
    }else{
        return std::string("ERR : INVALID CMD {SET, GET, DEL}\n");
    }
}

std::vector<std::string> Parser::tokenize(std::string request){
    std::vector<std::string> tokens;
    size_t j = 0;
    std::string token = "";
    for(size_t i=0; i<request.size(); i++){
        if(request[i] != ' ' || j == 3){ // if j==3 we are reading the VAL so include space
            token += request[i];
        }else{
            if(!token.empty()){
                tokens.emplace_back(std::move(token));
                token = "";
                j++;
            }
        }
    }
    if(!token.empty()) tokens.emplace_back(std::move(token));
    return tokens;
}