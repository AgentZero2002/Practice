/**
 * 快速傅里叶变换 (Fast Fourier Transform, FFT)
 *
 * 在 O(n log n) 时间内计算多项式乘法 / 大整数乘法。
 *
 * 核心思想:
 *   1. 将 n 次多项式从系数表示 → 2n 个点值表示 (FFT)
 *   2. 点值相乘 O(n)
 *   3. 从点值表示 → 系数表示 (IFFT, 逆变换)
 *
 * Cooley-Tukey 算法 (递归分治):
 *   A(x) = A_even(x²) + x · A_odd(x²)
 *   利用 ω_n^k = -ω_n^(k+n/2) 将规模减半
 *
 * 实现要点:
 *   - 复数运算: 用 complex<double> 自然支持
 *   - 蝴蝶变换 (bit-reversal): 迭代版更高效
 *   - 精度: double 可支持到 ~10^5 项
 *
 * 时间复杂度: O(n log n)
 */

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
using namespace std;

using Complex = complex<double>;
const double PI = acos(-1.0);

// 迭代版 FFT (invert=false: 正变换, invert=true: 逆变换)
void fft(vector<Complex>& a, bool invert) {
    int n = a.size();

    // 蝴蝶变换: 按 bit-reversal 排序
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j) swap(a[i], a[j]);
    }

    // Cooley-Tukey 迭代
    for (int len = 2; len <= n; len <<= 1) {
        double angle = 2 * PI / len * (invert ? -1 : 1);
        Complex wlen(cos(angle), sin(angle));
        for (int i = 0; i < n; i += len) {
            Complex w(1);
            for (int j = 0; j < len / 2; j++) {
                Complex u = a[i + j];
                Complex v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    // 逆变换除以 n
    if (invert)
        for (Complex& x : a) x /= n;
}

// 多项式乘法
vector<long long> multiply(const vector<int>& A, const vector<int>& B) {
    vector<Complex> fa(A.begin(), A.end()), fb(B.begin(), B.end());
    int n = 1;
    while (n < (int)(A.size() + B.size())) n <<= 1;
    fa.resize(n);
    fb.resize(n);

    fft(fa, false);
    fft(fb, false);
    for (int i = 0; i < n; i++) fa[i] *= fb[i];
    fft(fa, true);

    vector<long long> result(A.size() + B.size() - 1);
    for (size_t i = 0; i < result.size(); i++)
        result[i] = (long long)round(fa[i].real());
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  快速傅里叶变换 (FFT)                    " << endl;
    cout << "========================================" << endl << endl;

    // 多项式乘法: (1 + 2x + 3x²) × (1 + x + x²)
    vector<int> A = {1, 2, 3};  // 1 + 2x + 3x²
    vector<int> B = {1, 1, 1};  // 1 + x + x²

    cout << "[多项式乘法]" << endl;
    cout << "  A(x) = 1 + 2x + 3x²" << endl;
    cout << "  B(x) = 1 + x + x²" << endl;

    auto C = multiply(A, B);
    cout << "  C(x) = ";
    for (size_t i = 0; i < C.size(); i++) {
        if (i > 0) cout << " + ";
        cout << C[i] << "x^" << i;
    }
    cout << endl;
    cout << "  期望: 1 + 3x + 6x² + 5x³ + 3x⁴" << endl;
    cout << endl;

    // 大整数乘法的思想
    cout << "[大整数乘法 (FFT 思想)]" << endl;
    cout << "  将大整数视为 x=10 的多项式," << endl;
    cout << "  用 FFT 做多项式乘法后处理进位即可" << endl;
    cout << "  复杂度: O(n log n) vs 朴素 O(n²)" << endl;
    cout << endl;

    cout << "[FFT 应用]" << endl;
    cout << "  - 多项式乘法 / 大整数乘法" << endl;
    cout << "  - 卷积 (信号处理, 图像处理)" << endl;
    cout << "  - 生成函数 (组合数学)" << endl;
    cout << "  - NTT (数论变换): 用原根替代单位根, 避免精度误差" << endl;

    return 0;
}
