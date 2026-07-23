/**
 * 扩展欧几里得算法 (Extended Euclidean Algorithm)
 *
 * 求解 Bézout 等式: ax + by = gcd(a, b)
 *
 * 原理:
 *   在欧几里得算法的递归回退阶段, 维护系数 x, y:
 *     gcd(a, b) = gcd(b, a%b)
 *     a%b = a - ⌊a/b⌋ · b
 *     代入得: gcd = b·x' + (a%b)·y' = b·x' + (a - ⌊a/b⌋·b)·y'
 *                = a·y' + b·(x' - ⌊a/b⌋·y')
 *     所以: x = y', y = x' - ⌊a/b⌋·y'
 *
 * 核心应用:
 *   1. 求模逆元: a 在 mod m 下的逆元 = x mod m (当 gcd(a,m)=1)
 *   2. 解线性同余方程: ax ≡ b (mod m)
 *   3. 求解线性丢番图方程
 *
 * 时间复杂度: O(log min(a,b))
 */

#include <iostream>
#include <tuple>
using namespace std;

// ==================== 扩展欧几里得 ====================
// 返回 {gcd(a,b), x, y} 满足 ax + by = gcd(a,b)
tuple<long long, long long, long long> extendedGCD(long long a, long long b) {
    if (b == 0)
        return {a, 1, 0};
    auto [gcd, x1, y1] = extendedGCD(b, a % b);
    return {gcd, y1, x1 - (a / b) * y1};
}

// ==================== 模逆元 ====================
// 求 a 在 mod m 下的乘法逆元, 即 a*x ≡ 1 (mod m)
// 需要 gcd(a, m) == 1
long long modInverse(long long a, long long m) {
    auto [gcd, x, y] = extendedGCD(a, m);
    if (gcd != 1) return -1;  // 逆元不存在
    return (x % m + m) % m;   // 保证正数
}

// ==================== 解线性同余方程 ax ≡ b (mod m) ====================
// 返回一个解, 通解为 x + k·(m/gcd), k ∈ Z
long long solveCongruence(long long a, long long b, long long m) {
    auto [gcd, x, y] = extendedGCD(a, m);
    if (b % gcd != 0) return -1;  // 无解
    x = x * (b / gcd) % m;
    return (x + m) % m;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  扩展欧几里得 + 模逆元                   " << endl;
    cout << "========================================" << endl << endl;

    // 1. Bézout 等式
    long long a = 30, b = 12;
    auto [gcd, x, y] = extendedGCD(a, b);
    cout << "[Bézout] " << a << " * (" << x << ") + " << b
         << " * (" << y << ") = " << gcd << endl;
    cout << "  验证: " << a*x + b*y << " = " << gcd << endl;
    cout << endl;

    // 2. 模逆元
    cout << "[模逆元] " << endl;
    a = 3; long long m = 11;
    long long inv = modInverse(a, m);
    cout << "  3⁻¹ mod 11 = " << inv
         << " (验证: 3×" << inv << "=" << 3*inv << " ≡ " << (3*inv)%m << " mod 11)" << endl;

    a = 10; m = 15;
    inv = modInverse(a, m);
    cout << "  10⁻¹ mod 15 = " << inv
         << " (gcd(10,15)=5≠1, 逆元不存在)" << endl;
    cout << endl;

    // 3. 同余方程
    cout << "[同余方程]" << endl;
    cout << "  解 5x ≡ 3 (mod 7): x = "
         << solveCongruence(5, 3, 7) << endl;
    cout << "  解 6x ≡ 2 (mod 9): x = "
         << solveCongruence(6, 2, 9) << endl;
    cout << endl;

    cout << "[关键公式]" << endl;
    cout << "  gcd(a,b) = ax + by (Bézout 恒等式)" << endl;
    cout << "  a⁻¹ mod m = x mod m (当 gcd(a,m)=1)" << endl;
    cout << "  同余方程 ax≡b(mod m) 有解 ⇔ gcd(a,m)|b" << endl;

    return 0;
}
