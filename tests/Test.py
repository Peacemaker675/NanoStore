import socket
import threading
import time
import random
import string

# Configuration
HOST = '127.0.0.1'
PORT = 8080
NUM_THREADS = 50        # Simulate 50 concurrent users
REQUESTS_PER_THREAD = 100 # Each user sends 100 commands

# Stats
success_count = 0
error_count = 0
lock = threading.Lock()

def generate_random_string(length=8):
    return ''.join(random.choices(string.ascii_letters + string.digits, k=length))

def client_task(thread_id):
    global success_count, error_count
    
    try:
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect((HOST, PORT))
        
        for i in range(REQUESTS_PER_THREAD):
            key = f"key_{thread_id}_{i}"
            value = generate_random_string()
            
            msg = f"SET {key} 60 {value}"
            client.sendall(msg.encode())
            response = client.recv(1024).decode()
            if "OK" not in response and "QUERY OK" not in response:
                print(f"[Thread {thread_id}] SET Error: {response}")

            msg = f"GET {key}"
            client.sendall(msg.encode())
            response = client.recv(1024).decode()
            if value not in response:
                print(f"[Thread {thread_id}] DATA CORRUPTION Expected {value}, got {response}")
                with lock: error_count += 1
            else:
                with lock: success_count += 1
                
        client.close()
        
    except Exception as e:
        print(f"[Thread {thread_id}] Connection Failed: {e}")
        with lock: error_count += 1

def run_test():
    threads = []
    start_time = time.time()
    
    print(f"Starting Stress Test: {NUM_THREADS} threads x {REQUESTS_PER_THREAD} requests")
    
    for i in range(NUM_THREADS):
        t = threading.Thread(target=client_task, args=(i,))
        threads.append(t)
        t.start()
        
    for t in threads:
        t.join()
        
    duration = time.time() - start_time
    total_reqs = NUM_THREADS * REQUESTS_PER_THREAD * 2 # SET + GET
    

    print(f"\nTest Complete in {duration:.2f} seconds")
    print(f"Total Operations: {total_reqs}")
    print(f"Throughput: {total_reqs / duration:.2f} req/sec")
    print(f"Success: {success_count}")
    print(f" Errors:  {error_count}")

if __name__ == "__main__":
    run_test()