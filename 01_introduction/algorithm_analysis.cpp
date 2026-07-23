/**
 * 算法复杂度分析示例
 *
 * 对应教材 1.2 节：复杂度度量
 *
 * 演示内容:
 *   1. O(1), O(log n), O(n), O(n log n), O(n^2), O(2^n) 各阶复杂度
 *   2. 实际运行时间测量
 *   3. Hailstone 序列（Collatz 猜想）的长度分析
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <numeric>
using namespace std;
using namespace std::chrono;

// ==================== 不同复杂度函数的实现 ====================

// O(1) - 常数时间
int constant_time(int n) {
    return n * (n + 1) / 2;  // 无论 n 多大，固定 3 次运算
}

// O(log n) - 对数时间（二分查找思想）
int logarithmic(int n) {
    int count = 0;
    for (int i = n; i > 1; i /= 2) count++;
    return count;  // 循环 log₂(n) 次
}

// O(n) - 线性时间
long long linear(int n) {
    long long sum = 0;
    for (int i = 0; i < n; i++) sum += i;
    return sum;
}

// O(n log n) - 线性对数时间
long long n_log_n(int n) {
    long long count = 0;
    for (int i = 0; i < n; i++)
        for (int j = n; j > 1; j /= 2) count++;
    return count;
}

// O(n²) - 平方时间
long long quadratic(int n) {
    long long count = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) count++;
    return count;
}

// O(2^n) - 指数时间（递归计算 Fibonacci — 仅供演示）
long long exponential(int n) {
    if (n <= 1) return 1;
    return exponential(n - 1) + exponential(n - 2);
}

// ==================== Hailstone 序列 ====================
// Collatz 猜想: 对任意正整数 n，以下操作总能到达 1
// Hailstone(n) = { n/2          if n is even
//                 { 3n + 1       if n is odd
//
// 返回序列长度。是否有穷？——至今未被证明！

int hailstone_length(long long n) {
    int length = 1;
    while (1 < n) {
        n = (n % 2) ? (3 * n + 1) : (n / 2);
        length++;
    }
    return length;
}

// 打印 Hailstone 序列
void hailstone_print(long long n) {
    cout << "Hailstone(" << n << ") 序列: ";
    int steps = 0;
    while (1 < n && steps < 100) {  // 限制打印步数
        cout << n << " → ";
        n = (n % 2) ? (3 * n + 1) : (n / 2);
        steps++;
    }
    cout << n;
    if (n != 1) cout << " ... (截断)";
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  算法复杂度分析示例                     " << endl;
    cout << "========================================" << endl << endl;

    // 1. Hailstone 序列
    cout << "[Hailstone 序列 / Collatz 猜想]" << endl;
    for (int n : {7, 27, 42}) {
        hailstone_print(n);
        cout << "  序列长度: " << hailstone_length(n) << endl << endl;
    }

    // 2. 大O记号速查
    cout << "[大O记号速查] 常见复杂度阶次（由低到高）:" << endl;
    cout << "  O(1)       < 常数时间   < 不能更好了" << endl;
    cout << "  O(log n)   < 对数时间   < 如二分查找" << endl;
    cout << "  O(n)       < 线性时间   < 如遍历数组" << endl;
    cout << "  O(n log n) < 线性对数   < 如归并排序" << endl;
    cout << "  O(n²)      < 平方时间   < 如冒泡排序" << endl;
    cout << "  O(2ⁿ)      < 指数时间   < 如递归Fibonacci" << endl;
    cout << "  O(n!)      < 阶乘时间   < 如全排列枚举" << endl;
    cout << endl;

    // 3. 度量运行时间（对数坐标对比）
    cout << "[运行时间度量] 输入规模 n 对各复杂度的影响:" << endl;
    cout << "  注：为避免过长等待，跳过 O(2^n) 的大规模测试" << endl << endl;

    vector<int> sizes = {100, 1000, 10000, 100000, 1000000};

    cout << "  n        O(log n) O(n)      O(n log n)  O(n²)" << endl;
    cout << "  " << string(55, '-') << endl;

    for (int n : sizes) {
        auto t0 = high_resolution_clock::now();

        auto start = high_resolution_clock::now();
        logarithmic(n);
        auto t_log = duration_cast<nanoseconds>(high_resolution_clock::now() - start).count();

        start = high_resolution_clock::now();
        linear(n);
        auto t_n = duration_cast<nanoseconds>(high_resolution_clock::now() - start).count();

        start = high_resolution_clock::now();
        n_log_n(n);
        auto t_nlogn = duration_cast<nanoseconds>(high_resolution_clock::now() - start).count();

        int small_n = min(n, 5000);  // 限制 n² 测试规模
        start = high_resolution_clock::now();
        quadratic(small_n);
        auto t_n2 = duration_cast<nanoseconds>(high_resolution_clock::now() - start).count();
        if (small_n != n) t_n2 = t_n2 * (long long)n / small_n * (long long)n / small_n;  // 估算

        // 转换为微秒或毫秒方便阅读
        cout << "  ";
        cout.width(8); cout << left << n;
        cout.width(10); cout << left << t_log;
        cout.width(10); cout << left << (t_n / 1000 > 0 ? to_string(t_n / 1000) + "μs" : to_string(t_n) + "ns");
        cout.width(12); cout << left << (t_nlogn / 1000 > 0 ? to_string(t_nlogn / 1000) + "μs" : to_string(t_nlogn) + "ns");
        cout << (t_n2 / 1000000 > 0 ? to_string(t_n2 / 1000000) + "ms" : to_string(t_n2 / 1000) + "μs") << endl;
    }

    // 4. 封底估算
    cout << endl << "[封底估算] 假设每条指令 1ns (1GHz CPU):" << endl;
    cout << "  n = 10^3:   O(n²) = 10^6 ns = 1 ms" << endl;
    cout << "  n = 10^6:   O(n²) = 10^12 ns = 1000 秒 ≈ 16.7 分钟" << endl;
    cout << "  n = 10^9:   O(n) = 10^9 ns = 1 秒" << endl;
    cout << "  n = 10^9:   O(log n) ≈ 30 次操作，几乎瞬间" << endl;

    return 0;
}
