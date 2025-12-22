#include"server/Server.hpp"

int main(){
    Server server_(8080, 3, 5);
    server_.start();
}