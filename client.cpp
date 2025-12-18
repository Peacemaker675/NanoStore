#include<iostream>
#include<netinet/in.h>
#include<sys/socket.h>
#include<unistd.h>

using namespace std;

int main(){
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;
    char buffer[1024] = "hello server";
    int flag = 0;
    while(!flag){
        connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
        cin>>buffer;
        if(buffer == "bye") flag = 1;
        write(client_socket, buffer, sizeof(buffer));
        recv(client_socket, buffer, sizeof(buffer), 0);
        cout<<"MESSAGE : "<<buffer<<endl;
    }
    close(client_socket);
    return 0;
}