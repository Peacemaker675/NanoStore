#include<server/Server.hpp>

Server::Server(int port, int num_threads, int queue_length): pool(num_threads, queue_length){
    q_len = queue_length;
    this->port =  port;
}

void Server::start(){
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0){
        throw std::string("ERR : SOCKET CREATION FAILED");
    }
    int val = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;
    addr_len = sizeof(server_address);
    if(bind(server_socket, (const sockaddr*)& server_address, addr_len) < 0){
        throw std::string("ERR : FAILED TO BIND");
    }
    listen(server_socket, q_len);
    while(true){
        int client_socket = accept(server_socket, (sockaddr*)& server_address, &addr_len);
        if(client_socket < 0) continue;
        pool.enqueue([client_socket, this]() mutable{
            handle_client(client_socket);
        });
    }
}

void Server::handle_client(int client_socket){
    char buffer[1024];
    while(true){
        int n = recv(client_socket, buffer, sizeof(buffer), 0);
        if(n <= 0) break;
        buffer[n]='\0';
        std::string request(buffer);
        std::string response = Parser::process(request, db);
        write(client_socket, response.c_str(), response.size());
    }
    close(client_socket);
}