# NANOSTORE: High-Performance C++ Redis-like Database

## Description
Nanostore is a key-value database supporting three core operations: `SET`, `GET`, and `DEL`. It achieves high-speed request processing by utilizing a thread pool architecture with blocking queues and database partitioning. While designed natively for Linux, it can be run on Windows via WSL.

## Usage
You can run the binaries located inside the `bin/` folder:

* **`Nanostore`**
    The core database binary. This version runs entirely in memory and does **not** retain data after a crash (Non-persistent).
    
* **`Nanostore_persistent`**
    The persistent version of the database. It retains data after a crash but is slower due to disk I/O overhead (uses `std::flush`).

* **`client`**
    A CLI tool used to communicate with the database.

* **`stress_test`**
    A benchmarking tool that tests the database for a fixed number of requests.

* **`stress_duration`**
    A benchmarking tool that tests the database for a fixed time interval (default: 60s). 
    *Note: To change the duration, modify the value in the `test/` folder and recompile.*

## Benchmarks
**System Specs:** Ryzen 5 5500U, 16GB RAM, WSL2 on Windows.

| Version | Test Type | Throughput |
| :--- | :--- | ---: |
| **Nanostore** | Stress_test | 961,606.67 req/sec |
| **Nanostore** | Stress_duration | 2,626,585.00 req/sec |
| **Nanostore_persistent** | Stress_test | 14,596.40 req/sec |
| **Nanostore_persistent** | Stress_duration | 31,721.70 req/sec |

## Future Updates
* **Optimize Persistence:** Improve file writing speed (currently slow due to direct synchronous command writing).
* **Error Handling:** Add distinct error types in `Types.hpp` for better modularity and management.
* **EPOLL Implementation:** Migrate to `epoll` for event notification without degrading performance.
