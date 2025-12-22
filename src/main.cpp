#include"server/Server.hpp"

int main(){
    Server server_(8080, 16, 1000);
    server_.start();
}