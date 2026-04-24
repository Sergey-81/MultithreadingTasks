#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <numeric>

using namespace std;

// Рекурсивный параллельный for_each
template<typename Iterator, typename Function>
void parallel_for_each(Iterator begin, Iterator end, Function func) {
    const int min_chunk_size = 1000;
    int size = distance(begin, end);

    // Базовый случай: маленький кусок обрабатываем синхронно
    if (size <= min_chunk_size) {
        for_each(begin, end, func);
        return;
    }

    // Разделяем на две половины
    Iterator middle = begin;
    advance(middle, size / 2);

    // Левую половину обрабатываем асинхронно
    future<void> left_task = async(launch::async, parallel_for_each<Iterator, Function>,
        begin, middle, func);

    // Правую половину обрабатываем в текущем потоке
    parallel_for_each(middle, end, func);

    // Ждём завершения левой задачи
    left_task.wait();
}

template<typename Container, typename Function>
void parallel_for_each_wrapper(Container& container, Function func) {
    parallel_for_each(container.begin(), container.end(), func);
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "=== Задание 2: Параллельный for_each ===" << endl;
    cout << endl;

    // Создаём массив
    vector<int> numbers(10000);
    iota(numbers.begin(), numbers.end(), 1);

    cout << "Первые 10 элементов до обработки: ";
    for (int i = 0; i < 10; i++) cout << numbers[i] << " ";
    cout << endl;

    // Применяем функцию возведения в квадрат
    parallel_for_each_wrapper(numbers, [](int& x) { x = x * x; });

    cout << "Первые 10 элементов после обработки: ";
    for (int i = 0; i < 10; i++) cout << numbers[i] << " ";
    cout << endl;

    // Проверка правильности
    bool ok = true;
    for (int i = 0; i < 10000; i++) {
        if (numbers[i] != (i + 1) * (i + 1)) {
            ok = false;
            break;
        }
    }

    cout << "\nРезультат: " << (ok ? "ВСЕ ВЕРНО!" : "Ошибка!") << endl;

    return 0;
}