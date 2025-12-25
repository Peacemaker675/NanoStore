#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed. Is the server running?" << std::endl;
        return -1;
    }

    std::cout << "Connected to server. Type 'exit' to quit." << std::endl;

    while (true) {
        std::string input;
        std::cout << "Client >> ";
        std::getline(std::cin, input);
        if (input == "exit") break;
        input += '\n';
        if (send(sock, input.c_str(), input.length(), 0) < 0) {
            std::cerr << "Send failed" << std::endl;
            break;
        }
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        
        if (valread > 0) {
            std::cout << "Server << " << buffer << std::endl;
        } else if (valread == 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        } else {
            std::cerr << "Read error" << std::endl;
            break;
        }
    }

    close(sock);
    std::cout << "Connection closed." << std::endl;

    return 0;
}