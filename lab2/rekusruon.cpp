#include <iostream>
#include <iomanip>
#include <chrono>
#include <boost/multiprecision/cpp_dec_float.hpp>

using namespace std;
using namespace chrono;
using namespace boost::multiprecision;


using bigfloat = cpp_dec_float_100;


bigfloat iteration(int n) {
    bigfloat fact = 1;
    for (int i = 1; i <= n; i++) fact *= i;
    bigfloat denom = pow(bigfloat(2), n);
    return fact / denom;
}


bigfloat recursion(int n) {
    if (n == 0) return 1;
    return bigfloat(n) * recursion(n - 1) / 2;
}


int main() {


    cout << "Проверка корректности функций:\n";
    int array_n1[] = {1, 2, 5};
    for (int n : array_n1) {
        cout << "A_" << n << " = " << iteration(n)
             << " (итеративный) = " << recursion(n)
             << " (рекурсивный)\n";
    }
    
    cout << scientific << setprecision(30);
    cout << "\n";
    int array_n2[] = {1000, 5000, 10000};
    for (int n : array_n2) {
        auto start = high_resolution_clock::now();
        bigfloat a_it = iteration(n);
        auto end = high_resolution_clock::now();
        double time_it = duration<double>(end - start).count();

        cout << "Итеративный алгоритм посчитал A_" << n
             << " = " << a_it << " за " << time_it << " с.\n";

        start = high_resolution_clock::now();
        bigfloat a_rec = recursion(n);
        end = high_resolution_clock::now();
        double time_rec = duration<double>(end - start).count();

        cout << "Рекурсивный алгоритм посчитал A_" << n
             << " = " << a_rec << " за " << time_rec << " с.\n\n";
    }

    return 0;
}
