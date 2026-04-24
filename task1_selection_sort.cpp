#include <iostream>
#include <vector>
#include <future>
#include <thread>
#include <chrono>

using namespace std;

// Асинхронный поиск минимального элемента
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

// Сортировка выбором с async/future
void selection_sort_async(vector<int>& arr) {
    int n = arr.size();

    for (int i = 0; i < n - 1; i++) {
        // Запускаем асинхронный поиск минимума
        future<int> min_future = async_find_min(arr, i, n);

        // Получаем результат (блокировка здесь)
        int min_index = min_future.get();

        // Обмениваем элементы
        if (min_index != i) {
            swap(arr[i], arr[min_index]);
        }
    }
}

// Функция для вывода массива
void print_array(const vector<int>& arr, const string& title) {
    cout << title << ": ";
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== Задание 1: Сортировка выбором (асинхронная) ===" << endl;
    cout << endl;

    // Тест 1
    vector<int> arr1 = { 64, 25, 12, 22, 11 };
    print_array(arr1, "Исходный массив");
    selection_sort_async(arr1);
    print_array(arr1, "Отсортированный массив");
    cout << endl;

    // Тест 2
    vector<int> arr2 = { 5, 2, 8, 2, 9, 1, 5 };
    print_array(arr2, "Исходный массив");
    selection_sort_async(arr2);
    print_array(arr2, "Отсортированный массив");
    cout << endl;

    // Тест 3: уже отсортированный
    vector<int> arr3 = { 1, 2, 3, 4, 5, 6 };
    print_array(arr3, "Исходный массив");
    selection_sort_async(arr3);
    print_array(arr3, "Отсортированный массив");

    cout << endl;
    cout << "=== Объяснение ===" << endl;
    cout << "1. Для каждого i запускается асинхронная задача поиска минимума" << endl;
    cout << "2. Future.get() блокирует поток до получения результата" << endl;
    cout << "3. Сложность O(n²), но поиск минимума выполняется в отдельном потоке" << endl;

    return 0;
}