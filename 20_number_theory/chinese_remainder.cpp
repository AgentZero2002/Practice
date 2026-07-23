/**
 * 中国剩余定理 (Chinese Remainder Theorem, CRT)
 *
 * 求解同余方程组:
 *   x ≡ r₁ (mod m₁)
 *   x ≡ r₂ (mod m₂)
 *   ...
 *   x ≡ rₙ (mod mₙ)
 * 其中 m₁, m₂, ..., mₙ 两两互质。
 *
 * 标准 CRT:
 *   令 M = ∏ mᵢ
 *   令 Mᵢ = M / mᵢ
 *   令 tᵢ = Mᵢ⁻¹ mod mᵢ (模逆元)
 *   则 x = Σ rᵢ · Mᵢ · tᵢ (mod M)
 *
 * 扩展 CRT (exCRT):
 *   当模数不互质时, 两两合并:
 *     x ≡ r₁ (mod m₁)  →  x = r₁ + k·m₁
 *     x ≡ r₂ (mod m₂)  →  代入解 k
 *     有解条件: gcd(m₁,m₂) | (r₂ - r₁)
 *
 * 时间复杂度:
 *   CRT: O(n log M)
 *   exCRT: O(n log M)
 */

#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
using namespace std;

// 扩展欧几里得 (复用)
tuple<long long, long long, long long> exgcd(long long a, long long b) {
    if (b == 0) return {a, 1, 0};
    auto [g, x, y] = exgcd(b, a % b);
    return {g, y, x - (a / b) * y};
}

// ==================== 1. 标准 CRT (模数互质) ====================
long long crt(const vector<long long>& remainders,
              const vector<long long>& mods) {
    int n = remainders.size();
    // 计算 M = ∏ mᵢ
    long long M = 1;
    for (long long m : mods) M *= m;

    long long result = 0;
    for (int i = 0; i < n; i++) {
        long long Mi = M / mods[i];
        auto [g, inv, y] = exgcd(Mi, mods[i]);
        result = (result + remainders[i] * Mi % M * inv % M) % M;
    }
    return (result + M) % M;
}

// ==================== 2. 扩展 CRT (模数可不互质) ====================
// 两两合并, 返回 {解, 模数}, 无解返回 {-1, -1}
pair<long long, long long> excrt(vector<long long> r, vector<long long> m) {
    int n = r.size();
    long long R = r[0], M = m[0];

    for (int i = 1; i < n; i++) {
        // 合并方程: x ≡ R (mod M) 和 x ≡ rᵢ (mod mᵢ)
        auto [g, p, q] = exgcd(M, m[i]);
        long long diff = r[i] - R;

        if (diff % g != 0) return {-1, -1};  // 无解

        // 新模数 = lcm(M, mᵢ)
        long long newMod = M / g * m[i];

        // 特解
        long long t = diff / g * p % (m[i] / g);
        R = (R + t * M % newMod + newMod) % newMod;
        M = newMod;
    }
    return {R, M};
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  中国剩余定理 (CRT + exCRT)              " << endl;
    cout << "========================================" << endl << endl;

    // 1. 标准 CRT
    cout << "[CRT] 韩信点兵" << endl;
    vector<long long> r = {2, 3, 2};   // 余数
    vector<long long> m = {3, 5, 7};   // 模数
    cout << "  x≡2(mod3), x≡3(mod5), x≡2(mod7)" << endl;
    cout << "  x = " << crt(r, m) << " (期望: 23)" << endl;
    cout << "  验证: 23%3=" << 23%3
         << ", 23%5=" << 23%5
         << ", 23%7=" << 23%7 << endl;
    cout << endl;

    // 2. 扩展 CRT (不互质)
    cout << "[exCRT] 模数不互质" << endl;
    vector<long long> r2 = {3, 1, 5};
    vector<long long> m2 = {6, 4, 15};
    auto [sol, mod] = excrt(r2, m2);
    if (mod != -1) {
        cout << "  x = " << sol << " (mod " << mod << ")" << endl;
        cout << "  验证: " << sol << "%6=" << sol%6
             << ", %4=" << sol%4 << ", %15=" << sol%15 << endl;
    } else {
        cout << "  无解!" << endl;
    }
    cout << endl;

    cout << "[CRT 应用场景]" << endl;
    cout << "  - 大整数模运算 (避免溢出)" << endl;
    cout << "  - RSA 解密加速 (CRT 加速 4x)" << endl;
    cout << "  - 竞争编程中组合数取模 (模数非素数)" << endl;

    return 0;
}
