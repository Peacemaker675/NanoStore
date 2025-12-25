#include"server/Server.hpp"

int main(){
    Server server_(8080, 4, 1000);
    server_.start();
}