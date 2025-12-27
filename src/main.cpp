#include"server/Server.hpp"

int main(){
    Server server_(8080, 6, 10000);
    server_.start();
}