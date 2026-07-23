/**
 * 线性筛 (Linear Sieve) + 积性函数
 *
 * 在 O(n) 时间内筛出 [1,n] 内所有素数, 同时可在线性时间内
 * 计算各种积性函数 (欧拉函数 φ, 莫比乌斯函数 μ 等)。
 *
 * 核心原理:
 *   每个合数只被它的最小质因子 (LPF) 筛掉一次:
 *     for each prime p:
 *       for each i where i * p ≤ n:
 *         if i % p == 0 → 这是最小质因子, 跳出
 *         否则 → p 就是最小质因子
 *
 * 对比:
 *   埃氏筛 (Eratosthenes): O(n log log n), 一个合数被多个质因子筛
 *   线性筛:               O(n), 每个合数只被筛一次
 *
 * 积性函数:
 *   若 f(ab) = f(a)·f(b) 当 gcd(a,b)=1, 则为积性函数
 *   线性筛可一站式计算多个积性函数
 */

#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

struct LinearSieve {
    vector<int> primes;       // 素数列表
    vector<bool> isComposite; // isComposite[i] = (i 是合数)
    vector<int> phi;          // 欧拉函数 φ(i)
    vector<int> mu;           // 莫比乌斯函数 μ(i)
    vector<int> lpf;          // 最小质因子 (Least Prime Factor)

    LinearSieve(int n) : isComposite(n + 1, false),
                         phi(n + 1, 0), mu(n + 1, 0),
                         lpf(n + 1, 0) {
        phi[1] = 1;
        mu[1] = 1;

        for (int i = 2; i <= n; i++) {
            if (!isComposite[i]) {
                primes.push_back(i);
                lpf[i] = i;
                phi[i] = i - 1;          // 素数 p: φ(p) = p-1
                mu[i] = -1;               // 素数 p: μ(p) = -1
            }
            for (int p : primes) {
                if (i * p > n) break;
                isComposite[i * p] = true;
                lpf[i * p] = p;

                if (i % p == 0) {
                    // p 是 i 的最小质因子 → 积性函数特殊处理
                    phi[i * p] = phi[i] * p;
                    mu[i * p] = 0;         // 含平方因子 → μ = 0
                    break;  // 关键! 保证线性
                } else {
                    // p 与 i 互质 → 直接乘
                    phi[i * p] = phi[i] * (p - 1);
                    mu[i * p] = -mu[i];
                }
            }
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  线性筛 + 积性函数                      " << endl;
    cout << "========================================" << endl << endl;

    int n = 30;
    LinearSieve sieve(n);

    cout << "[1," << n << "] 的素数 (" << sieve.primes.size() << "个):" << endl;
    for (int p : sieve.primes) cout << p << " ";
    cout << endl << endl;

    cout << "[欧拉函数 φ(n)] 小于 n 且与 n 互质的数的个数:" << endl;
    cout << "  n : ";
    for (int i = 1; i <= n; i++) cout << setw(2) << i << " ";
    cout << endl << "  φ : ";
    for (int i = 1; i <= n; i++) cout << setw(2) << sieve.phi[i] << " ";
    cout << endl << endl;

    cout << "[莫比乌斯函数 μ(n)]" << endl;
    cout << "  n : ";
    for (int i = 1; i <= n; i++) cout << setw(2) << i << " ";
    cout << endl << "  μ : ";
    for (int i = 1; i <= n; i++) cout << setw(2) << sieve.mu[i] << " ";
    cout << endl;
    cout << "  μ= 1: 偶数个不同素因子" << endl;
    cout << "  μ=-1: 奇数个不同素因子" << endl;
    cout << "  μ= 0: 含平方因子" << endl;
    cout << endl;

    cout << "[线性筛 vs 埃氏筛]" << endl;
    cout << "  埃氏筛: O(n log log n), 每个合数被多个质因子筛" << endl;
    cout << "  线性筛: O(n), 每个合数只被最小质因子筛一次" << endl;
    cout << "  关键: if (i % p == 0) break;" << endl;

    return 0;
}
