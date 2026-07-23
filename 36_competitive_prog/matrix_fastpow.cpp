/**
 * 矩阵快速幂 (Matrix Fast Exponentiation)
 *
 * 将线性递推转化为矩阵乘法，然后用快速幂 O(m³ log n) 加速。
 *
 * 经典: Fibonacci O(log n)
 *   [F(n+1)]   [1 1]   [F(n)  ]
 *   [F(n)  ] = [1 0] × [F(n-1)]
 *
 *   即 [F(n+1); F(n)] = M^n × [F(1); F(0)]
 *
 * 扩展到一般线性递推:
 *   对 d 阶递推 a[n] = Σ c[i]·a[n-i]
 *   构造 d×d 矩阵，快速幂解 O(d³ log n)
 *
 * 应用: 线性递推, 图论(路径计数), DP 状态转移加速
 */

#include <iostream>
#include <vector>
using namespace std;

typedef vector<vector<long long>> Matrix;

const int MOD = 1000000007;

// ==================== 矩阵乘法 O(n³) ====================
Matrix matMul(const Matrix& A, const Matrix& B) {
    int n = A.size();
    Matrix C(n, vector<long long>(n, 0));

    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++)
            if (A[i][k])
                for (int j = 0; j < n; j++)
                    C[i][j] = (C[i][j] + A[i][k] * B[k][j]) % MOD;

    return C;
}

// ==================== 矩阵快速幂 O(n³ log p) ====================
Matrix matPow(Matrix A, long long p) {
    int n = A.size();
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; i++) result[i][i] = 1;  // 单位矩阵

    while (p > 0) {
        if (p & 1) result = matMul(result, A);
        A = matMul(A, A);
        p >>= 1;
    }
    return result;
}

// ==================== Fibonacci O(log n) ====================
long long fib(long long n) {
    if (n == 0) return 0;
    if (n == 1) return 1;

    Matrix M = {{1, 1}, {1, 0}};
    Matrix result = matPow(M, n - 1);
    return result[0][0];  // F(n)
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  矩阵快速幂 (Matrix Fast Exponentiation)" << endl;
    cout << "========================================" << endl << endl;

    // Fibonacci 快速幂
    cout << "[Fibonacci via Matrix FastPow]" << endl;
    for (long long n : {0LL, 1LL, 5LL, 10LL, 30LL, 50LL}) {
        cout << "  F(" << setw(2) << n << ") = " << setw(12) << fib(n);
        if (n <= 50) {
            // 验证
            long long f0 = 0, f1 = 1, fn = n < 2 ? n : 0;
            for (long long i = 2; i <= n; i++)
                fn = (f0 + f1) % MOD, f0 = f1, f1 = fn;
            cout << " (迭代验证: " << fn << (fn == fib(n) ? " ✓" : " ✗") << ")";
        }
        cout << endl;
    }

    cout << endl << "[应用场景]" << endl;
    cout << "  1. O(log n) 求第 n 项线性递推" << endl;
    cout << "  2. 图论: 走 k 步的路径数 = A^k" << endl;
    cout << "  3. DP 加速: 状态转移矩阵快速幂" << endl;

    return 0;
}
