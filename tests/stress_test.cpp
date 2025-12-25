#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

const std::string HOST = "127.0.0.1";
const int PORT = 8080;
const int NUM_THREADS = 16;        
const int REQS_PER_THREAD = 200000; 
const int BATCH_SIZE = 50;

std::atomic<int> success_count(0);

void client_task(int id) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, HOST.c_str(), &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sock);
        return;
    }

    char buffer[16384]; 
    std::string key_prefix = "key_" + std::to_string(id) + "_";
    std::string batch_payload;
    batch_payload.reserve(4096);

    for (int i = 0; i < REQS_PER_THREAD; i += BATCH_SIZE) {
        batch_payload.clear();
        int ops_in_this_batch = 0;
        for (int j = 0; j < BATCH_SIZE && (i + j) < REQS_PER_THREAD; ++j) {
            std::string key = key_prefix + std::to_string(i + j);
            std::string val = "val";
            batch_payload += "SET " + key + " 60 " + val + "\n";
            batch_payload += "GET " + key + "\n";
            ops_in_this_batch += 2;
        }
        send(sock, batch_payload.c_str(), batch_payload.size(), 0);
        int responses_received = 0;
        while (responses_received < ops_in_this_batch) {
            int n = recv(sock, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            for (int k = 0; k < n; ++k) {
                if (buffer[k] == '\n') {
                    responses_received++;
                }
            }
        }
        
        success_count += responses_received;
    }
    close(sock);
}

int main() {
    std::cout << "Starting Pipelined C++ Benchmark (" << NUM_THREADS << " threads, Batch Size " << BATCH_SIZE << ")..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for(int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(client_task, i);
    }

    for(auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    int total = NUM_THREADS * REQS_PER_THREAD * 2;
    std::cout << "Time: " << duration.count() << "s" << std::endl;
    std::cout << "Total Operations: " << total << std::endl;
    std::cout << "Throughput: " << (total / duration.count()) << " req/sec" << std::endl;

    return 0;
}