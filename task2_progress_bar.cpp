#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <iomanip>
#include <random>

using namespace std;

mutex cout_mutex; // Для синхронизации вывода

// Функция для генерации случайной задержки (миллисекунды)
int random_delay(int min_ms, int max_ms) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dist(min_ms, max_ms);
    return dist(gen);
}

// Функция работы потока с прогресс-баром
void progress_bar_thread(int thread_num, int total_steps, int bar_width) {
    // Получаем ID потока
    thread::id thread_id = this_thread::get_id();

    // Симуляция работы
    for (int step = 0; step <= total_steps; step++) {
        // Случайная задержка (чтобы потоки двигались с разной скоростью)
        this_thread::sleep_for(chrono::milliseconds(random_delay(50, 200)));

        // Вычисляем прогресс
        int progress = static_cast<int>((static_cast<double>(step) / total_steps) * bar_width);

        // Синхронизированный вывод
        {
            lock_guard<mutex> lock(cout_mutex);

            // Перемещаем курсор в нужную строку (ANSI escape codes)
            cout << "\033[" << thread_num + 1 << ";1H";

            // Выводим прогресс-бар
            cout << "Thread " << thread_num << " (ID: " << thread_id << ") [";
            for (int i = 0; i < bar_width; i++) {
                if (i < progress) {
                    cout << "=";
                }
                else if (i == progress) {
                    cout << ">";
                }
                else {
                    cout << " ";
                }
            }
            cout << "] " << (step * 100 / total_steps) << "%   ";
            cout.flush();
        }
    }

    // Вывод времени завершения
    auto end_time = chrono::system_clock::now();
    {
        lock_guard<mutex> lock(cout_mutex);
        cout << "\033[" << thread_num + 1 << ";1H";
        cout << "Thread " << thread_num << " (ID: " << thread_id << ") COMPLETED!        ";
        cout.flush();
    }
}

int main() {
    cout << "=== Task 2: Progress Bar ===" << endl;
    cout << endl;

    // Параметры
    int num_threads = 4;
    int total_steps = 20;
    int bar_width = 30;

    // Очищаем консоль
    cout << "\033[2J\033[1;1H";

    // Создаём и запускаем потоки
    vector<thread> threads;
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(progress_bar_thread, i, total_steps, bar_width);
    }

    // Ожидаем завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }

    cout << "\n\nAll threads completed!" << endl;

    return 0;
}