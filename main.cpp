/*
Дано:
N - длина массива

p - число потоков

Как-то инициализируем массив

        Нужно найти максимум из элементов и домножить на него

O(N / p)
*/
#include <random>
#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <cstring>

void initialize_mas(int *begin, int len, int rand_low, int rand_high) {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(rand_low, rand_high);
    // https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution

    for (int i = 0; i < len; ++i, ++begin) {
        *begin = distrib(gen);
    }
}

// Считает максимум на подмассиве от begin до begin + len
int calc_max(const int *begin, int len) {
    int res = *begin;
    for (int i = 0; i < len; ++i, ++begin) {
        res = std::max(res, *begin);
    }
    return res;
}

// Умножает на значение val все элементы подмассива
void mult_val(int *begin, int len, int val) {
    for (int i = 0; i < len; ++i, ++begin) {
        *begin *= val;
    }
}

// аргументы: N p [check]
// N - длина, p - число потоков, check - выводить линейный STL максимум для сверки
int main(int argc, char **argv) {
    int N, p;
    N = strtol(argv[1], nullptr, 10);
    p = strtol(argv[2], nullptr, 10);
    int *mas = new int[N];
    initialize_mas(mas, N, 0, 100);

    std::cout << "Array just after initialization:\n";
    for (int i = 0; i < N; ++i) {
        std::cout << mas[i] << ' ';
    }
    std::cout << '\n';

    if (argc > 3 && std::strcmp(argv[3], "check") == 0) {
        std::cout << "Normal result: " << *std::max_element(mas, mas + N) << '\n';
    }
    /*
    std::vector<std::thread> threads;
    for (int i = 0; i < p; ++i) {
        threads.emplace_back(solve, i * N / p);
    }
    for (const &thread;
    */

    // Понял, что с threads не очень удобно взаимодействовать: тяжело получить назад значение
    // len и begin считаю нехитрой арифметикой: для случая в 49 элементов, 5 потоков это будет 0, 9, 18, 27, 36
    // Для 50..54, 5: 0, 10, 20, 30, 40
    std::vector<std::future<int>> futures_max;
    for (int i = 0; i < p - 1; ++i) {
        futures_max.push_back(
                std::async(std::launch::async, calc_max, mas + i * (N / p), N / p));
    }
    futures_max.push_back(std::async(std::launch::async, calc_max, mas + N - N / p - N % p, N / p + N % p));

    int *results = new int[p];
    for (int i = 0; i < p; ++i) {
        results[i] = futures_max[i].get();
    }

    int max_value = calc_max(results, p);

    std::cout << "My multithreaded answer: " << max_value << '\n';

    // Решил использовать threads тут, потому что возврат значения не нужен
    std::vector<std::thread> mod_threads;
    for (int i = 0; i < p - 1; ++i) {
        mod_threads.emplace_back(mult_val, mas + i * (N / p), N / p, max_value);
    }
    mod_threads.emplace_back(mult_val, mas + N - N / p - N % p, N / p + N % p, max_value);

    for (int i = 0; i < p; ++i) {
        mod_threads[i].join();
    }

    std::cout << "Array after max multiplication:\n";
    for (int i = 0; i < N; ++i) {
        std::cout << mas[i] << ' ';
    }

    delete[] mas;
    delete[] results;
    return 0;
}
