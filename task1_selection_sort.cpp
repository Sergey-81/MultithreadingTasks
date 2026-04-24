#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <algorithm>
#include <random>

using namespace std;

// Асинхронная задача для поиска минимального элемента в диапазоне
future<int> async_find_min(vector<int>& arr, int start, int end) {
    return async(launch::async, [&arr, start, end]() -> int {
        int min_index = start;
        for (int i = start + 1; i < end; i++) {
            if (arr[i] < arr[min_index]) {
                min_index = i;
            }
        }
        return min_index;
        });
}

// Сортировка выбором с использованием асинхронных задач
void selection_sort_async(vector<int>& arr) {
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        // Асинхронный поиск минимального элемента
        future<int> min_future = async_find_min(arr, i, n);

        // Получаем результат (блокировка происходит здесь)
        int min_index = min_future.get();

        // Обмен элементов
        if (min_index != i) {
            swap(arr[i], arr[min_index]);
        }
    }
}

// Версия для сравнения: синхронный поиск минимума
int find_min_sync(vector<int>& arr, int start, int end) {
    int min_index = start;
    for (int i = start + 1; i < end; i++) {
        if (arr[i] < arr[min_index]) {
            min_index = i;
        }
    }
    return min_index;
}

void selection_sort_sync(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int min_index = find_min_sync(arr, i, n);
        if (min_index != i) {
            swap(arr[i], arr[min_index]);
        }
    }
}

// Функция для генерации случайного массива
vector<int> generate_random_array(int size, int min_val = 1, int max_val = 100) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(min_val, max_val);

    vector<int> arr(size);
    for (int i = 0; i < size; i++) {
        arr[i] = dist(gen);
    }
    return arr;
}

// Функция для печати массива
void print_array(const vector<int>& arr, const string& title = "") {
    if (!title.empty()) {
        cout << title << ": ";
    }
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== Задание 1: Сортировка выбором с promise/future ===" << endl;
    cout << endl;

    // Тест 1: Небольшой массив
    vector<int> arr1 = { 64, 25, 12, 22, 11 };
    cout << "Тест 1 - Небольшой массив:" << endl;
    print_array(arr1, "Исходный массив");
    selection_sort_async(arr1);
    print_array(arr1, "Отсортированный массив");
    cout << endl;

    // Тест 2: Массив с дубликатами
    vector<int> arr2 = { 5, 2, 8, 2, 9, 1, 5, 5 };
    cout << "Тест 2 - Массив с дубликатами:" << endl;
    print_array(arr2, "Исходный массив");
    selection_sort_async(arr2);
    print_array(arr2, "Отсортированный массив");
    cout << endl;

    // Тест 3: Уже отсортированный массив
    vector<int> arr3 = { 1, 2, 3, 4, 5, 6 };
    cout << "Тест 3 - Уже отсортированный массив:" << endl;
    print_array(arr3, "Исходный массив");
    selection_sort_async(arr3);
    print_array(arr3, "Отсортированный массив");
    cout << endl;

    // Тест 4: Сравнение производительности
    cout << "=== Сравнение производительности (10000 элементов) ===" << endl;

    vector<int> arr_sync = generate_random_array(10000);
    vector<int> arr_async = arr_sync;

    auto start = chrono::high_resolution_clock::now();
    selection_sort_sync(arr_sync);
    auto end = chrono::high_resolution_clock::now();
    auto sync_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    start = chrono::high_resolution_clock::now();
    selection_sort_async(arr_async);
    end = chrono::high_resolution_clock::now();
    auto async_time = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Синхронная версия: " << sync_time << " мс" << endl;
    cout << "Асинхронная версия: " << async_time << " мс" << endl;
    cout << endl;

    cout << "Пример использования promise и future:" << endl;
    cout << "- promise используется для передачи результата из асинхронной задачи" << endl;
    cout << "- future используется для получения результата в основном потоке" << endl;
    cout << "- async() создаёт задачу и сразу возвращает future" << endl;

    return 0;
}