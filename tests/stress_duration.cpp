#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

const std::string HOST = "127.0.0.1";
const int PORT = 8080;
const int NUM_THREADS = 16;        
const int TEST_DURATION_SECONDS = 60;
const int BATCH_SIZE = 50; 

std::atomic<long long> total_success_count(0);
std::atomic<bool> keep_running(true);

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
    int ops_in_batch = 0;
    
    for (int j = 0; j < BATCH_SIZE; ++j) {
        std::string key = key_prefix + std::to_string(j);
        batch_payload += "SET " + key + " 60 val\n";
        batch_payload += "GET " + key + "\n";
        ops_in_batch += 2;
    }

    while (keep_running) {
        send(sock, batch_payload.c_str(), batch_payload.size(), 0);
        int responses_received = 0;
        while (responses_received < ops_in_batch) {
            int n = recv(sock, buffer, sizeof(buffer), 0);
            if (n <= 0) break;
            for (int k = 0; k < n; ++k) {
                if (buffer[k] == '\n') responses_received++;
            }
        }
        total_success_count += responses_received;
    }

    close(sock);
}

int main() {
    std::cout << "Starting "<<TEST_DURATION_SECONDS<<" -Second Stability Test (" << NUM_THREADS << " threads)..." << std::endl;
    
    std::vector<std::thread> threads;
    for(int i = 0; i < NUM_THREADS; i++) {
        threads.emplace_back(client_task, i);
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::seconds(TEST_DURATION_SECONDS));
    keep_running = false;

    for(auto& t : threads) {
        t.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    long long total = total_success_count.load();

    std::cout << "Time Elapsed: " << duration.count() << "s" << std::endl;
    std::cout << "Total Operations: " << total << std::endl;
    std::cout << "Throughput: " << (total / duration.count()) << " req/sec" << std::endl;

    return 0;
}