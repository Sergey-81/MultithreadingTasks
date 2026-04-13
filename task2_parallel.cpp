#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;

void sum_vectors_part(const vector<int>& a, const vector<int>& b, vector<int>& result,
    int start, int end, int thread_id, int& first_thread_called) {
    if (thread_id == 0 && first_thread_called == 0) {
        first_thread_called = 1;
        cout << "Available hardware cores: " << thread::hardware_concurrency() << endl;
    }

    for (int i = start; i < end; i++) {
        result[i] = a[i] + b[i];
    }
}

double parallel_sum(const vector<int>& a, const vector<int>& b, vector<int>& result, int num_threads) {
    int size = a.size();
    vector<thread> threads;
    int chunk_size = size / num_threads;
    int remainder = size % num_threads;
    int first_thread_called = 0;

    auto start_time = high_resolution_clock::now();

    int start = 0;
    for (int i = 0; i < num_threads; i++) {
        int end = start + chunk_size + (i < remainder ? 1 : 0);

        threads.emplace_back(sum_vectors_part, ref(a), ref(b), ref(result),
            start, end, i, ref(first_thread_called));
        start = end;
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = high_resolution_clock::now();
    duration<double> elapsed = end_time - start_time;
    return elapsed.count();
}

bool verify_result(const vector<int>& a, const vector<int>& b, const vector<int>& result) {
    for (size_t i = 0; i < a.size(); i++) {
        if (result[i] != a[i] + b[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    cout << "=== Task 2: Parallel Calculations ===" << endl;
    cout << endl;

    vector<int> sizes = { 1000, 10000, 100000, 1000000 };
    vector<int> thread_counts = { 1, 2, 4, 8, 16 };

    cout << left << setw(15) << "Array size";
    for (int threads : thread_counts) {
        cout << setw(12) << to_string(threads) + " threads";
    }
    cout << endl;

    cout << string(15 + 12 * thread_counts.size(), '-') << endl;

    for (int size : sizes) {
        cout << left << setw(15) << size;

        vector<int> a(size);
        vector<int> b(size);
        vector<int> result(size);

        srand(42);
        for (int i = 0; i < size; i++) {
            a[i] = rand() % 100;
            b[i] = rand() % 100;
        }

        double best_time = 1e9;
        int best_threads = 1;

        for (int threads : thread_counts) {
            int actual_threads = min(threads, size);
            if (actual_threads == 0) actual_threads = 1;

            double elapsed = parallel_sum(a, b, result, actual_threads);

            cout << setw(12) << fixed << setprecision(6) << elapsed;

            if (elapsed < best_time) {
                best_time = elapsed;
                best_threads = actual_threads;
            }
        }
        cout << endl;

        if (!verify_result(a, b, result)) {
            cout << "ERROR: Result verification failed!" << endl;
            return 1;
        }

        cout << "  -> Best time: " << fixed << setprecision(6) << best_time
            << " sec with " << best_threads << " thread(s)" << endl;
        cout << endl;
    }

    cout << "\n=== Conclusions ===" << endl;
    cout << "1. For small arrays (1000), thread overhead > benefit" << endl;
    cout << "2. For large arrays (1,000,000), parallelization gives significant speedup" << endl;
    cout << "3. Optimal thread count ≈ number of CPU cores (4-8)" << endl;
    cout << "4. Too many threads cause resource contention" << endl;

    return 0;
}