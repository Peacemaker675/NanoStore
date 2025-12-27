# NANOSTORE : High Performance C++ Redis like Database
<hr>

## Description
Nanostore is a key-value database supporting three core operations: SET, GET, and DEL. It achieves high-speed request processing by utilizing a thread pool architecture with blocking queues and database partitioning. While designed natively for Linux, it can be run on Windows via WSL.

## Usage
You can run the binaries inside bin/ folder - 
1. Nanostore - This is the binary for the database (without persistence, i.e it will not retain data after a crash)
2. Nanostore_persistent - This is the persistenet version which will retain data even after a carsh (slower because of std::flush).
3. client - This can be used to communicate with the database.
4. stress_duration - This is used to test database for a fixed time interval (60s , you can change the value in test/ folder and recompile the c++ file for another value).
5. stress_test - This is used to test the database for a fixed number of requests.

## Benchmarks
System Specs - Ryzen 5 5500U, 16GB ram, WSL2 on windows.
| Version | Test | Throughput |
| --- | --- | --- |
| Nanostore | Stress_test | 961,606.67 req/sec |
| Nanostore | Stress_duration | 2,626,585 req/sec |
| Nanostore_persistent | Stress_test | 14,596.4 req/sec |
| Nanostore_persistent | Stress_duration | 31721.7 req/sec |

## Future Updates
1. Make file persistence faster , its slower because we writing commands directly.
2. Add error types in Types.hpp for better modularity and management.
3. Implement EPOLL without degrading performance.
