#include "../include/threadpool/ThreadPool.hpp"
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ranges>
#include <mutex>
#include <string_view>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

std::mutex db_mtx;

struct Value {
    string data;
    int TTL;
};

// Safely process request and RETURN the response string
std::string process_request(std::string& request, unordered_map<string, Value>& db) {
    // 1. Tokenize
    vector<std::string> tokens;
    auto split_view = request | std::views::split(' ');
    for (const auto word : split_view) {
        // Handle consecutive spaces resulting in empty tokens
        if (!word.empty()) {
            tokens.emplace_back(word.begin(), word.end());
        }
    }

    if (tokens.empty()) return "ERR: Empty Command\n";

    // 2. Convert to lower case
    std::string command = tokens[0];
    std::transform(command.begin(), command.end(), command.begin(),  
                   [](unsigned char c) { return std::tolower(c); });

    // 3. Handle SET
    if (command == "set") {
        if (tokens.size() < 3) return "ERR: Usage: set <key> <value> <ttl>\n";

        std::string key = tokens[1];
        
        // Try parsing TTL from the last token
        int TTL = 0;
        try {
            // Remove newline if present in the last token
            string& ttl_str = tokens.back();
            if (!ttl_str.empty() && ttl_str.back() == '\n') ttl_str.pop_back();
            
            TTL = stoi(ttl_str);
        } catch (...) {
            return "ERR: Invalid TTL format\n";
        }

        // Combine middle tokens into data
        std::string data = "";
        for (size_t i = 2; i < tokens.size() - 1; i++) {
            data += tokens[i] + (i == tokens.size() - 2 ? "" : " "); // Avoid trailing space
        }

        Value v;
        v.data = data;
        v.TTL = TTL;

        {
            std::unique_lock<std::mutex> lock(db_mtx);
            db[key] = v;
        }
        return "OK\n";
    }
    // 4. Handle GET
    else if (command == "get") {
        if (tokens.size() < 2) return "ERR: Usage: get <key>\n";
        
        std::string key = tokens[1];
        // Clean key if it has newline
        if (!key.empty() && key.back() == '\n') key.pop_back();

        std::string response_data;
        bool found = false;
        {
            std::unique_lock<std::mutex> lock(db_mtx);
            if (db.count(key)) {
                response_data = db[key].data;
                found = true;
            }
        }
        
        if (found) return response_data + "\n";
        else return "NULL\n";
    }
    // 5. Handle DEL
    else if (command == "del") {
        if (tokens.size() < 2) return "ERR: Usage: del <key>\n";
        std::string key = tokens[1];
        if (!key.empty() && key.back() == '\n') key.pop_back();

        {
            std::unique_lock<std::mutex> lock(db_mtx);
            db.erase(key);
        }
        return "OK\n";
    }

    return "ERR: Unknown Command\n";
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0); // socket returns a file descriptor
    if (server_socket < 0) {
        perror("Socket creation failed");
        return 1;
    }

    int val = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // converts 8080 into big-endian
    server_address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0 

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    unordered_map<string, Value> db;
    ThreadPool pool(4,10); // num_threads, size_task_queue (increase carefully)
    
    listen(server_socket, 5); // Queue upto 5 clients before refusing , TODO - change while testing
    std::cout << "Server listening on port 8080...\n";

    while (true) {
        socklen_t addr_size = sizeof(server_address);
        int client_fd = accept(server_socket, (struct sockaddr*)&server_address, &addr_size); // accept a client
        
        if (client_fd < 0) continue;

        pool.enqueue([client_fd, &db]() mutable {
            char buffer[1024]; // Use a fixed buffer for raw reading
            
            while (true) {
                int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0); // recv returns the size of read string
                if (n <= 0) {
                    // 0 = Client closed, -1 = Error
                    break; 
                }
                buffer[n] = '\0'; // Null terminate the received data
                std::string request(buffer);
                std::string response = process_request(request, db);
                write(client_fd, response.c_str(), response.size());
            }
            close(client_fd);
        });
    }
    close(server_socket);
    return 0;
}