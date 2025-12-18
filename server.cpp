#include <cstdint>
#include <iostream>
#include <netinet/in.h>
#include<sys/socket.h>
#include <unistd.h>

using namespace std;

// struct in_addr{
//     uint32_t s_addr; //IPv4 in big-endian
// };

// struct sockaddr_in{
//     uint16_t sin_family; // AF_IFNET
//     uint16_t sin_port; // port in big-endian (1.2.3.4 is represented by htonl(0x01020304))
//     struct in_addr sin_addr; // IPv4
// };

int main(){
    /*
        IPv4+TCP	socket(AF_INET, SOCK_STREAM, 0)
        IPv6+TCP	socket(AF_INET6, SOCK_STREAM, 0)
        IPv4+UDP	socket(AF_INET, SOCK_DGRAM, 0)
        IPv6+UDP	socket(AF_INET6, SOCK_DGRAM, 0)
    */
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // ipv4+TCP, this fucntion returns a file descriptor
    int val = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    sockaddr_in server_address;
    server_address.sin_family = AF_INET; // Family means IPaddress type 
    server_address.sin_port = htons(8080); // htons converts this number into big_endian number
    server_address.sin_addr.s_addr = INADDR_ANY; // the socket is not bound to any particular ip , i.e it can listen to all ips
    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)); // we bind the socket to the address
    int flag = 0;
    while(!flag){
        listen(server_socket, 1);
        int client_socket = accept(server_socket, nullptr, nullptr);
        char buffer[1024] = {0};
        recv(client_socket, buffer, sizeof(buffer), 0);
        cout<<"MESSAGE : "<<buffer<<endl;
        if(buffer == "bye") flag = 1;
        char response[100] = "hello";
        write(client_socket, response, sizeof(response));
    }
    close(server_socket);
    return 0;
}