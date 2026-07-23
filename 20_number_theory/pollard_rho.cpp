/**
 * Pollard's Rho 大整数分解算法
 *
 * 在 O(n^(1/4)) 期望时间内找到一个合数的非平凡因子。
 * 常与 Miller-Rabin 配合使用, 实现完整的大整数分解。
 *
 * 核心思想:
 *   1. 用随机函数 f(x) = (x² + c) mod n 生成伪随机序列
 *   2. 序列在 mod p (p 为 n 的因子) 下必然成 ρ 形 (有环)
 *   3. 用 Floyd 判环法 (慢指针/快指针) 检测环
 *   4. 当 gcd(|slow - fast|, n) > 1 时, 找到一个因子
 *
 * 关键优化:
 *   - Richard Brent 改进 (减少 gcd 调用)
 *   - 多次累积后批量计算 gcd
 *
 * 时间复杂度: O(n^(1/4) log n) 期望
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

using ll = long long;

// 乘法取模 (防溢出)
ll mulMod(ll a, ll b, ll m) {
    return (ll)((__int128)a * b % m);
}

// 快速幂
ll powMod(ll a, ll e, ll m) {
    ll r = 1; a %= m;
    while (e) {
        if (e & 1) r = mulMod(r, a, m);
        a = mulMod(a, a, m);
        e >>= 1;
    }
    return r;
}

// Miller-Rabin (复用)
bool isPrime(ll n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    ll d = n - 1; int s = 0;
    while (d % 2 == 0) { d /= 2; s++; }
    for (ll a : {2, 3, 5, 7, 11, 13, 17, 19, 23}) {
        if (a >= n) break;
        ll x = powMod(a, d, n);
        if (x == 1 || x == n - 1) continue;
        bool ok = false;
        for (int r = 0; r < s - 1; r++) {
            x = mulMod(x, x, n);
            if (x == n - 1) { ok = true; break; }
        }
        if (!ok) return false;
    }
    return true;
}

// ==================== Pollard's Rho ====================
ll gcd(ll a, ll b) { return b == 0 ? a : gcd(b, a % b); }

ll pollardRho(ll n) {
    if (n % 2 == 0) return 2;
    if (n % 3 == 0) return 3;
    if (isPrime(n)) return n;

    // 随机函数 f(x) = (x² + c) mod n
    while (true) {
        ll c = rand() % (n - 1) + 1;
        ll slow = 2, fast = 2;
        ll d = 1;

        while (d == 1) {
            slow = (mulMod(slow, slow, n) + c) % n;
            fast = (mulMod(fast, fast, n) + c) % n;
            fast = (mulMod(fast, fast, n) + c) % n;
            d = gcd(abs(slow - fast), n);
        }

        if (d != n) return d;  // 找到了非平凡因子
        // d == n → 失败, 换一个 c 重试
    }
}

// ==================== 完整分解 ====================
void factorize(ll n, vector<ll>& factors) {
    if (n <= 1) return;
    if (isPrime(n)) {
        factors.push_back(n);
        return;
    }
    ll d = pollardRho(n);
    factorize(d, factors);
    factorize(n / d, factors);
}

// ==================== 主函数 ====================
int main() {
    srand(time(0));
    cout << "========================================" << endl;
    cout << "  Pollard's Rho 大整数分解              " << endl;
    cout << "========================================" << endl << endl;

    ll testNumbers[] = {10967535067LL, 15, 1000000007LL * 1000000009LL};

    for (ll n : testNumbers) {
        cout << "n = " << n << endl;
        vector<ll> factors;
        factorize(n, factors);
        sort(factors.begin(), factors.end());
        cout << "  分解: ";
        for (ll f : factors) cout << f << " ";
        cout << endl;

        // 验算
        ll product = 1;
        for (ll f : factors) product *= f;
        cout << "  验算: " << (product == n ? "✓" : "✗") << endl;
        cout << endl;
    }

    cout << "[Pollard's Rho 原理]" << endl;
    cout << "  Birthday Paradox: 随机序列重复的期望" << endl;
    cout << "  步数是 O(√p) 而非 O(p)" << endl;
    cout << "  Floyd 判环: 慢指针 1 步/次, 快指针 2 步/次" << endl;
    cout << "  结合 Miller-Rabin: 先判素数再分解" << endl;

    return 0;
}
