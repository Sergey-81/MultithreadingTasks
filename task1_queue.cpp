#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

int client_count = 0;
int max_clients = 0;
bool working = true;
mutex mtx;
condition_variable cv;

void client_thread() {
    for (int i = 0; i < max_clients; i++) {
        this_thread::sleep_for(chrono::seconds(1));

        lock_guard<mutex> lock(mtx);
        client_count++;
        cout << "[Client] New client arrived. Queue size: " << client_count << endl;
        cv.notify_one();
    }

    working = false;
    cv.notify_all();
    cout << "[Client] No more clients. Queue closed." << endl;
}

void operator_thread() {
    while (working || client_count > 0) {
        this_thread::sleep_for(chrono::seconds(2));

        unique_lock<mutex> lock(mtx);
        cv.wait_for(lock, chrono::seconds(1), [] { return client_count > 0 || !working; });

        if (client_count > 0) {
            client_count--;
            cout << "[Operator] Client served. Remaining in queue: " << client_count << endl;
        }
    }

    cout << "[Operator] All clients served. Work day finished!" << endl;
}

int main() {
    cout << "=== Task 1: Client Queue ===" << endl;
    cout << "Enter maximum number of clients: ";
    cin >> max_clients;

    if (max_clients <= 0) {
        cout << "Invalid value. Exiting." << endl;
        return 1;
    }

    thread client(client_thread);
    thread oper(operator_thread);

    client.join();
    oper.join();

    cout << "\nProgram finished." << endl;

    return 0;
}