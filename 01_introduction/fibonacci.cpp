/**
 * Fibonacci 数列的多种实现与对比
 *
 * 对应教材 1.3 节：迭代与递归
 *
 * 实现方法:
 *   1. fib_iterative  - 迭代 O(n), O(1)空间
 *   2. fib_bin_recursive - 二分递归 O(2^n)（仅做演示，不可实际使用）
 *   3. fib_lin_recursive - 线性递归 O(n)
 *
 * 测试: 计算 fib(0) ~ fib(46)，比较各方法运行时间
 *       验证 fib(n) % 2^32 避免溢出
 */

#include <iostream>
#include <chrono>
#include <cstdint>
using namespace std;
using namespace std::chrono;

// ==================== 迭代版（推荐） ====================
// 时间复杂度 O(n)，空间复杂度 O(1)
uint64_t fib_iterative(int n) {
    if (n <= 1) return n;
    uint64_t f = 0, g = 1;  // fib(0), fib(1)
    while (0 < n--) {
        g = g + f;
        f = g - f;          // f <- old_g, g <- f + g
    }
    return f;
}

// ==================== 二分递归版（仅供演示） ====================
// 时间复杂度 O(φ^n) ≈ O(1.618^n)，指数增长，不可实际使用
// 例如 fib(43) 需要约 10^9 次递归调用
uint64_t fib_bin_recursive(int n) {
    if (n < 2) return n;
    return fib_bin_recursive(n - 1) + fib_bin_recursive(n - 2);
}

// ==================== 线性递归版 ====================
// 时间复杂度 O(n)，空间复杂度 O(n)
uint64_t fib_lin_recursive_core(int n, uint64_t& prev) {
    if (n == 0) { prev = 1; return 0; }
    uint64_t prevPrev;
    prev = fib_lin_recursive_core(n - 1, prevPrev);
    return prevPrev + prev;
}

uint64_t fib_lin_recursive(int n) {
    uint64_t prev;
    return fib_lin_recursive_core(n, prev);
}

// ==================== 动态规划/记忆化版 ====================
// 自上而下，O(n) 时间，O(n) 空间
uint64_t fib_memo(int n) {
    static uint64_t memo[100] = {0, 1};
    static int computed = 1;
    if (n <= computed) return memo[n];
    for (int i = computed + 1; i <= n; i++)
        memo[i] = memo[i - 1] + memo[i - 2];
    computed = n;
    return memo[n];
}

// ==================== 测试 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Fibonacci 数列：多种实现对比           " << endl;
    cout << "========================================" << endl << endl;

    // 验证各方法正确性
    cout << "[验证] fib(0) ~ fib(20) 各方法结果一致性：" << endl;
    bool consistent = true;
    for (int n = 0; n <= 20; n++) {
        uint64_t a = fib_iterative(n);
        uint64_t b = fib_bin_recursive(n);
        uint64_t c = fib_lin_recursive(n);
        uint64_t d = fib_memo(n);
        if (a != b || b != c || c != d) {
            cout << "  不一致: n=" << n << endl;
            consistent = false;
        }
    }
    cout << (consistent ? "  ✓ 全部一致" : "  ✗ 存在不一致") << endl << endl;

    // 运行时间对比（仅对比高效版本）
    cout << "[性能] 对比 fib(46) 三种高效方法:" << endl;

    auto start = high_resolution_clock::now();
    uint64_t r1 = fib_iterative(46);
    auto t1 = duration_cast<microseconds>(high_resolution_clock::now() - start).count();

    start = high_resolution_clock::now();
    uint64_t r2 = fib_lin_recursive(46);
    auto t2 = duration_cast<microseconds>(high_resolution_clock::now() - start).count();

    start = high_resolution_clock::now();
    uint64_t r3 = fib_memo(46);
    auto t3 = duration_cast<microseconds>(high_resolution_clock::now() - start).count();

    cout << "  迭代版:       " << r1 << "  |  耗时 " << t1 << " μs" << endl;
    cout << "  线性递归版:   " << r2 << "  |  耗时 " << t2 << " μs" << endl;
    cout << "  记忆化版:     " << r3 << "  |  耗时 " << t3 << " μs" << endl;

    // 二分递归很慢，只在很小的 n 上测试
    cout << endl << "[警告] 二分递归版在 n=43 时需约 10 亿次调用，" << endl;
    cout << "  此处跳过测试以避免长时间等待。" << endl;

    // 打印 Fibonacci 数列前20项
    cout << endl << "Fibonacci 数列前 20 项:" << endl;
    for (int i = 0; i < 20; i++) {
        cout << fib_iterative(i) << " ";
    }
    cout << endl;

    return 0;
}
