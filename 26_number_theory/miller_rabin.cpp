/**
 * Miller-Rabin 素性测试 (Miller-Rabin Primality Test)
 *
 * 概率型素数判定算法, 在竞赛和密码学中广泛使用。
 *
 * 原理 (基于费马小定理的强化):
 *   对于奇素数 n:
 *     令 n-1 = d · 2^s (d 为奇数)
 *     则对任意 a ∈ [2, n-2], 要么:
 *       a^d ≡ 1 (mod n), 或者
 *       a^(d·2^r) ≡ -1 (mod n) 对某 r ∈ [0, s-1]
 *   若找到了违反上述条件的 a, 则 n 一定是合数。
 *
 * 确定性判定 (n < 2^64):
 *   只需要测试 a ∈ {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37} 即可保证。
 *
 * 时间复杂度: O(k log³ n), k 为测试轮数
 */

#include <iostream>
#include <cstdint>
#include <random>
using namespace std;

// 快速幂取模 (a^e mod m)
long long powMod(long long a, long long e, long long m) {
    long long result = 1;
    a %= m;
    while (e) {
        if (e & 1) result = (__int128)result * a % m;
        a = (__int128)a * a % m;
        e >>= 1;
    }
    return result;
}

// Miller-Rabin 单轮测试
bool millerTest(long long n, long long a) {
    if (n == 2 || n == a) return true;
    if (n % 2 == 0) return false;

    // 分解 n-1 = d * 2^s
    long long d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }

    long long x = powMod(a, d, n);
    if (x == 1 || x == n - 1) return true;

    for (int r = 0; r < s - 1; r++) {
        x = (__int128)x * x % n;
        if (x == n - 1) return true;
    }
    return false;
}

// 确定性 Miller-Rabin (n < 2^64)
const long long TEST_BASES[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

bool isPrime(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (long long a : TEST_BASES) {
        if (a >= n) break;
        if (!millerTest(n, a)) return false;
    }
    return true;
}

// ==================== 快速幂 (拓展) ====================
long long fastPow(long long a, long long e) {
    long long result = 1;
    while (e) {
        if (e & 1) result *= a;
        a *= a;
        e >>= 1;
    }
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Miller-Rabin 素性测试                  " << endl;
    cout << "========================================" << endl << endl;

    cout << "[素数测试]" << endl;
    long long tests[] = {2, 7, 97, 1000000007, 1000000009,
                         1000000021, 10000000019, 10000000033};
    for (long long n : tests) {
        cout << "  " << n << " → " << (isPrime(n) ? "素数 ✓" : "合数 ✗") << endl;
    }
    cout << endl;

    cout << "[合数测试]" << endl;
    long long composites[] = {4, 15, 561, 1105, 1000000022};
    for (long long n : composites) {
        cout << "  " << n << " → " << (isPrime(n) ? "素数" : "合数 ✓") << endl;
    }
    cout << endl;

    cout << "[64位确定性判定]" << endl;
    cout << "  对于 n < 2^64, 使用前 12 个素数作为基底" << endl;
    cout << "  即可 100% 确定素数 (非概率)" << endl;
    cout << endl;

    cout << "[注意事项]" << endl;
    cout << "  使用 (__int128) 防止 64 位乘法溢出" << endl;
    cout << "  Carmichael 数 (如 561) 骗过费马测试," << endl;
    cout << "  但 Miller-Rabin 可检测它们" << endl;

    return 0;
}
