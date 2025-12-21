#include"../threadpool/ThreadPool.hpp"
#include"../database/Database.hpp"
#include<sys/socket.h>
#include<netinet/in.h>
class Server{
public:
    Server(int port, int num_threads, int queue_length);
    void start();
private:
    sockaddr_in server_address;
    socklen_t addr_len;
    ThreadPool pool;
    Database db;
    std::string handle_client(std::string request, Database& db);
};