#include"../threadpool/ThreadPool.hpp"
#include"database/Database.hpp"
#include"parser/Parser.hpp"
#include<sys/socket.h>
#include<netinet/in.h>
class Server{
public:
    Server(int port, int num_threads, int queue_length);
    void start();
private:
    int server_socket;
    int q_len;
    int port;
    sockaddr_in server_address;
    socklen_t addr_len;
    ThreadPool pool;
    Database db;
    void handle_client(int client_socket);
};