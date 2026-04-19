#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>

using namespace std;

// Класс Data с мьютексом
class Data {
private:
    int value;
    mutex mtx;

public:
    Data(int val) : value(val) {}

    int getValue() const { return value; }
    void setValue(int val) { value = val; }

    mutex& getMutex() { return mtx; }

    void print(const string& name) {
        cout << name << " value: " << value << endl;
    }
};

// Вариант 1: swap с использованием lock()
void swap_lock(Data& a, Data& b) {
    // Захватываем оба мьютекса
    lock(a.getMutex(), b.getMutex());

    // Владеем мьютексами
    lock_guard<mutex> lock_a(a.getMutex(), adopt_lock);
    lock_guard<mutex> lock_b(b.getMutex(), adopt_lock);

    // Обмен данными
    int temp = a.getValue();
    a.setValue(b.getValue());
    b.setValue(temp);
}

// Вариант 2: swap с использованием scoped_lock (C++17)
void swap_scoped_lock(Data& a, Data& b) {
    scoped_lock lock(a.getMutex(), b.getMutex());

    int temp = a.getValue();
    a.setValue(b.getValue());
    b.setValue(temp);
}

// Вариант 3: swap с использованием unique_lock
void swap_unique_lock(Data& a, Data& b) {
    unique_lock<mutex> lock_a(a.getMutex(), defer_lock);
    unique_lock<mutex> lock_b(b.getMutex(), defer_lock);

    lock(lock_a, lock_b);

    int temp = a.getValue();
    a.setValue(b.getValue());
    b.setValue(temp);
}

int main() {
    cout << "=== Task 3: Protected Data Swap ===" << endl;
    cout << endl;

    // Создаём два объекта Data
    Data data1(100);
    Data data2(200);

    cout << "Before swap:" << endl;
    data1.print("Data1");
    data2.print("Data2");
    cout << endl;

    // Тестируем три варианта
    cout << "1. Using lock() + lock_guard:" << endl;
    swap_lock(data1, data2);
    data1.print("Data1");
    data2.print("Data2");
    cout << endl;

    // Меняем обратно
    swap_lock(data1, data2);

    cout << "2. Using scoped_lock (C++17):" << endl;
    swap_scoped_lock(data1, data2);
    data1.print("Data1");
    data2.print("Data2");
    cout << endl;

    // Меняем обратно
    swap_scoped_lock(data1, data2);

    cout << "3. Using unique_lock + defer_lock:" << endl;
    swap_unique_lock(data1, data2);
    data1.print("Data1");
    data2.print("Data2");
    cout << endl;

    // Демонстрация многопоточного обмена
    cout << "\n=== Multithreaded swap test ===" << endl;

    Data shared1(1);
    Data shared2(2);

    cout << "Initial values: ";
    shared1.print("Shared1");
    shared2.print("Shared2");

    // Запускаем несколько потоков для обмена
    thread t1([&]() {
        for (int i = 0; i < 10; i++) {
            swap_scoped_lock(shared1, shared2);
            this_thread::sleep_for(chrono::milliseconds(1));
        }
        });

    thread t2([&]() {
        for (int i = 0; i < 10; i++) {
            swap_scoped_lock(shared1, shared2);
            this_thread::sleep_for(chrono::milliseconds(1));
        }
        });

    t1.join();
    t2.join();

    cout << "Final values after 20 swaps: ";
    shared1.print("Shared1");
    shared2.print("Shared2");

    return 0;
}