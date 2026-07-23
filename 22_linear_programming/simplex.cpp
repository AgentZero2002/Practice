/**
 * 线性规划 (Linear Programming) — Simplex 单纯形算法入门
 *
 * 线性规划是优化理论中最基本的问题:
 *   max c^T x
 *   s.t. Ax ≤ b, x ≥ 0
 *
 * Simplex 算法 (Dantzig, 1947):
 *   从可行域的一个顶点出发, 沿边移动到更好的相邻顶点,
 *   直到到达最优解。
 *
 * 几何直觉:
 *   可行域是一个凸多面体 (polytope),
 *   最优解一定在某个顶点 (extreme point) 上取得,
 *   Simplex 在多面体表面沿边搜索。
 *
 * 标准形式:
 *   等式约束: Ax = b, x ≥ 0
 *   引入松弛变量 (slack variables) 将不等式转为等式。
 *
 * 基变量与非基变量:
 *   基变量 (Basic): m 个, 对应一个基矩阵 B
 *   非基变量 (Non-basic): n 个, 取值为 0
 *   基解: x_B = B⁻¹b, x_N = 0 → 顶点
 *
 * 转轴 (Pivot):
 *   选择一个非基变量入基, 一个基变量出基,
 *   移动到相邻顶点。
 *
 * 复杂度:
 *   最坏指数时间 (Klee-Minty 立方体),
 *   平均多项式时间 (Spielman-Teng smoothed analysis).
 *   实际中极高效 (通常 O(n+m) 次迭代).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <iomanip>
using namespace std;

const double EPS = 1e-9;
const double INF = 1e18;

// 简化版 Simplex: 标准形式 max c^T x, s.t. Ax ≤ b, x ≥ 0
// 用两阶段法 (Two-Phase) 处理初始可行解
class Simplex {
    int m, n;       // m 行 (约束), n 列 (变量)
    vector<vector<double>> a;  // 表格 [0..m][0..n+m+1]
    vector<int> basis;

public:
    Simplex(const vector<vector<double>>& A,
            const vector<double>& b,
            const vector<double>& c) {
        m = A.size();
        n = c.size();
        a.resize(m + 1, vector<double>(n + m + 2, 0));
        basis.resize(m);

        // 填充表格
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) a[i][j] = A[i][j];
            a[i][n + i] = 1;     // 松弛变量
            a[i][n + m] = b[i];   // RHS
            basis[i] = n + i;
        }
        for (int j = 0; j < n; j++) a[m][j] = -c[j];  // 目标函数 (min)
    }

    bool solve(vector<double>& x, double& obj) {
        while (true) {
            // Bland规则选入基变量 (最小下标)
            int inCol = -1;
            for (int j = 0; j < n + m; j++)
                if (a[m][j] < -EPS) { inCol = j; break; }
            if (inCol == -1) break;  // 最优解

            // 选出基变量 (最小比值测试)
            int outRow = -1;
            double minRatio = INF;
            for (int i = 0; i < m; i++) {
                if (a[i][inCol] > EPS) {
                    double ratio = a[i][n + m] / a[i][inCol];
                    if (ratio < minRatio) {
                        minRatio = ratio;
                        outRow = i;
                    }
                }
            }
            if (outRow == -1) return false;  // 无界

            pivot(outRow, inCol);
            basis[outRow] = inCol;
        }

        x.assign(n, 0);
        for (int i = 0; i < m; i++)
            if (basis[i] < n) x[basis[i]] = a[i][n + m];
        obj = -a[m][n + m];  // 目标函数值 = -表格右下角
        return true;
    }

    void pivot(int row, int col) {
        double pivotVal = a[row][col];
        for (int j = 0; j <= n + m; j++)
            a[row][j] /= pivotVal;

        for (int i = 0; i <= m; i++) {
            if (i != row && fabs(a[i][col]) > EPS) {
                double factor = a[i][col];
                for (int j = 0; j <= n + m; j++)
                    a[i][j] -= factor * a[row][j];
            }
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  线性规划 — Simplex 单纯形算法              " << endl;
    cout << "========================================" << endl << endl;

    // 例子: max x₁ + 2x₂
    // s.t. x₁ + x₂ ≤ 3
    //      x₁ - x₂ ≤ 1
    //      x₁, x₂ ≥ 0
    vector<vector<double>> A = {{1, 1}, {1, -1}};
    vector<double> b = {3, 1};
    vector<double> c = {1, 2};  // 目标: max x₁ + 2x₂

    Simplex sim(A, b, c);
    vector<double> x;
    double obj;
    if (sim.solve(x, obj)) {
        cout << "[最优解]" << endl;
        cout << "  x₁ = " << x[0] << ", x₂ = " << x[1] << endl;
        cout << "  最优值 = " << obj << " (期望: x₁=1, x₂=2, obj=5)" << endl;
    }
    cout << endl;

    cout << "[线性规划理论]" << endl;
    cout << "  强对偶定理: 原问题最优值 = 对偶问题最优值" << endl;
    cout << "  最坏指数时间, 平均 O(n+m) 次迭代" << endl;
    cout << "  内点法 (Karmarkar): 多项式 O(n³·⁵ L)" << endl;

    return 0;
}
