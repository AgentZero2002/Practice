/**
 * 缓存忘却算法 (Cache-Oblivious Algorithms)
 *
 * "缓存忘却" 意味着算法不依赖缓存参数 (B, M),
 * 但在任何缓存层级上都能达到近似最优的 I/O 复杂度。
 *
 * 核心思想:
 *   传统算法调优依赖 B (缓存行大小) 和 M (缓存容量)
 *   Cache-Oblivious 算法不硬编码这些参数,
 *   通过递归分治自动适配所有缓存层级 (L1/L2/L3/内存/磁盘)。
 *
 * 关键模型 (Ideal Cache Model):
 *   - 两级存储: 容量 M, 块大小 B (一行的字节数)
 *   - 全关联 (fully associative)
 *   - 最优替换 (OPT, Belady)
 *   - Tall cache: M = Ω(B²) (实际 L1: 32KB, line=64B → 512行 ≥ 64? 满足)
 *
 * 经典算法:
 *
 * 1. Cache-Oblivious Matrix Multiply:
 *    递归分块乘法, 不假设任何缓存大小
 *    I/O = Θ(N³/(B√M))  ← 与显式 tiled 版本相同!
 *
 * 2. Cache-Oblivious B-Tree (分层 van Emde Boas 布局):
 *    以 vEB 递归方式布局 B-Tree 的节点
 *    每个 cache line 容纳 ~B 个元素
 *
 * 3. Funnel Sort:
 *    Cache-oblivious 外部排序, O(N/B · log_{M/B} N) I/O
 *
 * 本文件展示:
 *   - Cache-Oblivious 矩阵乘法 (递归分块)
 *   - 分形布局 (Morton order / Z-order)
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
using namespace std;
using namespace std::chrono;

// ==================== Cache-Oblivious 矩阵乘法 ====================
// Strassen-like 的递归分治, 但这里是标准的 CO 乘法
// 矩阵 A[m×n] × B[n×p] = C[m×p]

// 递归 Cache-Oblivious 乘法
void coMatrixMultiply(const vector<vector<int>>& A,
                       const vector<vector<int>>& B,
                       vector<vector<int>>& C,
                       int rA, int cA,  // A 的起始
                       int rB, int cB,  // B 的起始
                       int rC, int cC,  // C 的起始
                       int size) {
    if (size == 1) {
        C[rC][cC] += A[rA][cA] * B[rB][cB];
        return;
    }

    // 递归分治: 分成 4 个子矩阵
    int half = size / 2;

    // C11 += A11 × B11
    coMatrixMultiply(A, B, C, rA, cA, rB, cB, rC, cC, half);
    // C11 += A12 × B21
    coMatrixMultiply(A, B, C, rA, cA + half, rB + half, cB, rC, cC, half);

    // C12 += A11 × B12
    coMatrixMultiply(A, B, C, rA, cA, rB, cB + half, rC, cC + half, half);
    // C12 += A12 × B22
    coMatrixMultiply(A, B, C, rA, cA + half, rB + half, cB + half, rC, cC + half, half);

    // C21 += A21 × B11
    coMatrixMultiply(A, B, C, rA + half, cA, rB, cB, rC + half, cC, half);
    // C21 += A22 × B21
    coMatrixMultiply(A, B, C, rA + half, cA + half, rB + half, cB, rC + half, cC, half);

    // C22 += A21 × B12
    coMatrixMultiply(A, B, C, rA + half, cA, rB, cB + half, rC + half, cC + half, half);
    // C22 += A22 × B22
    coMatrixMultiply(A, B, C, rA + half, cA + half, rB + half, cB + half, rC + half, cC + half, half);
}

// 划到 base case 再用朴素乘法 (更好: 当子矩阵 ≤ 某个阈值时切到朴素)
void coMatrixMultiplyBase(const vector<vector<int>>& A,
                           const vector<vector<int>>& B,
                           vector<vector<int>>& C,
                           int rA, int cA,
                           int rB, int cB,
                           int rC, int cC,
                           int m, int n, int p) {
    // 当 m*n*p 很小 → 朴素乘法
    if (m <= 16 || n <= 16 || p <= 16) {
        for (int i = 0; i < m; i++)
            for (int k = 0; k < n; k++)
                for (int j = 0; j < p; j++)
                    C[rC + i][cC + j] += A[rA + i][cA + k] * B[rB + k][cB + j];
        return;
    }

    int m2 = m / 2, n2 = n / 2, p2 = p / 2;
    // 递归 (简化: 只对 2 的幂方阵)
    coMatrixMultiplyBase(A, B, C, rA, cA, rB, cB, rC, cC, m2, n2, p2);
    coMatrixMultiplyBase(A, B, C, rA, cA + n2, rB + n2, cB, rC, cC, m2, n - n2, p2);
    coMatrixMultiplyBase(A, B, C, rA, cA, rB, cB + p2, rC, cC + p2, m2, n2, p - p2);
    coMatrixMultiplyBase(A, B, C, rA, cA + n2, rB + n2, cB + p2, rC, cC + p2, m2, n - n2, p - p2);
}

// ==================== Morton Order (Z-Order) 空间填充曲线 ====================
// 用于布局矩阵: 使相邻元素在内存中也相邻 → 缓存友好
int mortonEncode(int x, int y) {
    int z = 0;
    for (int i = 0; i < 16; i++) {
        z |= ((x >> i) & 1) << (2 * i);
        z |= ((y >> i) & 1) << (2 * i + 1);
    }
    return z;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  缓存忘却算法 (Cache-Oblivious)            " << endl;
    cout << "========================================" << endl << endl;

    // 1. Cache-Oblivious 矩阵乘法 vs 朴素
    const int N = 128;  // 2的幂
    vector<vector<int>> A(N, vector<int>(N));
    vector<vector<int>> B(N, vector<int>(N));
    vector<vector<int>> C_co(N, vector<int>(N, 0));
    vector<vector<int>> C_naive(N, vector<int>(N, 0));

    // 填充随机数
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            A[i][j] = (i + j) % 10;
            B[i][j] = (i * j) % 10;
        }

    // Cache-Oblivious 版本
    auto start = high_resolution_clock::now();
    coMatrixMultiply(A, B, C_co, 0, 0, 0, 0, 0, 0, N);
    auto t_co = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

    // 朴素 i-k-j (缓存友好版)
    start = high_resolution_clock::now();
    for (int i = 0; i < N; i++)
        for (int k = 0; k < N; k++)
            for (int j = 0; j < N; j++)
                C_naive[i][j] += A[i][k] * B[k][j];
    auto t_naive = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

    cout << "[矩阵乘法 " << N << "×" << N << "]" << endl;
    cout << "  Cache-Oblivious: " << t_co << " ms" << endl;
    cout << "  朴素 (i-k-j):    " << t_naive << " ms" << endl;

    // 验证正确性
    bool correct = true;
    for (int i = 0; i < N && correct; i++)
        for (int j = 0; j < N && correct; j++)
            if (C_co[i][j] != C_naive[i][j]) correct = false;
    cout << "  正确性: " << (correct ? "✓" : "✗") << endl;

    // 2. Morton order 演示
    cout << endl << "[Morton Order (Z-Curve)] 4×4 格点编码:" << endl;
    for (int y = 0; y < 4; y++) {
        cout << "  ";
        for (int x = 0; x < 4; x++)
            cout << setw(3) << mortonEncode(x, y);
        cout << endl;
    }

    cout << endl << "[Cache-Oblivious 原理]" << endl;
    cout << "  不依赖 B, M 参数" << endl;
    cout << "  递归分治 → 自动裁剪到适合缓存" << endl;
    cout << "  I/O = Θ(N³/(B√M)) = 最优" << endl;
    cout << "  vEB 布局: B-Tree 的 CO 版本" << endl;

    return 0;
}
