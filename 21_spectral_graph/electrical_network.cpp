/**
 * 电阻网络与图 (Electrical Networks)
 *
 * 图的电阻网络视角是谱图论最美的应用之一:
 * 将图看作电路 — 每条边是 1Ω 电阻, 节点是接线点。
 *
 * 有效电阻 (Effective Resistance) R_eff(u,v):
 *   在 u, v 之间加 1V 电压, 流过电路的电流 I = 1/R_eff
 *   由 Ohm 定律和 Kirchhoff 定律唯一确定各边的电流。
 *
 * 核心定理 (Chandra–Ranjan–Doyle):
 *   往返时间 C(u,v) = 2|E| × R_eff(u,v)
 *   即: 随机游走的"时间代价" = 电路的"电阻代价" × 2|E|
 *
 * 有效电阻的性质:
 *   1. R_eff(u,v) 是一个度量 (metric)
 *   2. 串联: R_eff = R_eff(u,x) + R_eff(x,v)  (串联边)
 *   3. 并联: 1/R_eff = 1/R_eff(u,v) + 1/R_eff(u,x) (平行路径)
 *   4. Rayleigh 单调性: 加边只会降低 R_eff
 *
 * 计算 R_eff 用 Laplacian 伪逆:
 *   R_eff(u,v) = L⁺_uu + L⁺_vv - 2·L⁺_uv
 *   其中 L⁺ 是 Laplacian L 的 Moore-Penrose 伪逆
 *
 * 应用:
 *   - 图稀疏化 (Spectral Sparsification): 保留 R_eff 最大的边
 *   - 边的"重要性"度量: R_eff 大的边是桥/瓶颈
 *   - 电路仿真: SPICE 等工具的基础
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <numeric>
using namespace std;

// ==================== 构建 Laplacian ====================
vector<vector<double>> buildLaplacian(const vector<vector<int>>& adj) {
    int n = adj.size();
    vector<vector<double>> L(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        L[i][i] = (double)adj[i].size();
        for (int j : adj[i]) L[i][j] = -1.0;
    }
    return L;
}

// ==================== 计算伪逆 L⁺ (简化版) ====================
// 通过特征分解近似: L⁺ ≈ Σ_{i=2}^{n} (1/λ_i) v_i v_i^T
// 这里使用数值方法: 加正则化项, 然后求逆
// 公式: L⁺ = (L + (1/n)J)^(-1) - (1/n)J, 其中 J 是全1矩阵
vector<vector<double>> pseudoInverse(const vector<vector<double>>& L) {
    int n = L.size();

    // 构建增广矩阵: (L + J/n | I), 然后高斯消元
    // 简化: 使用 Sherman-Morrison 或直接求解
    // 这里我们构建增广系统 [L + e*e^T/n, I] 然后消元

    // 实际上对于小规模矩阵, 直接构造 (L + J/n)^{-1} 再减去 J/n
    // 构建 L_aug = L + (1/n)·J
    vector<vector<double>> A(n, vector<double>(2 * n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = L[i][j] + 1.0 / n;  // L + J/n
        }
        A[i][n + i] = 1.0;  // 单位矩阵
    }

    // 高斯消元 (Gauss-Jordan)
    for (int col = 0; col < n; col++) {
        // 选主元
        int pivot = col;
        for (int i = col + 1; i < n; i++)
            if (fabs(A[i][col]) > fabs(A[pivot][col]))
                pivot = i;
        swap(A[col], A[pivot]);

        double pv = A[col][col];
        if (fabs(pv) < 1e-10) continue;
        for (int j = 0; j < 2 * n; j++)
            A[col][j] /= pv;

        for (int i = 0; i < n; i++) {
            if (i == col) continue;
            double factor = A[i][col];
            for (int j = 0; j < 2 * n; j++)
                A[i][j] -= factor * A[col][j];
        }
    }

    // 提取逆矩阵, 再减去 J/n 得到伪逆
    vector<vector<double>> Lpinv(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            Lpinv[i][j] = A[i][n + j] - 1.0 / n;

    return Lpinv;
}

// ==================== 计算有效电阻 ====================
double effectiveResistance(const vector<vector<double>>& Lpinv,
                            int u, int v) {
    return Lpinv[u][u] + Lpinv[v][v] - 2.0 * Lpinv[u][v];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  电阻网络 (Electrical Network)            " << endl;
    cout << "========================================" << endl << endl;

    // 路径图 0—1—2—3 (串联4个1Ω电阻, R_eff(0,3) = 3)
    vector<vector<int>> path(4);
    path[0] = {1};
    path[1] = {0, 2};
    path[2] = {1, 3};
    path[3] = {2};

    auto L_path = buildLaplacian(path);
    auto Lpinv_path = pseudoInverse(L_path);

    cout << "[路径图 0—1—2—3] 串联" << endl;
    cout << "  R_eff(0,3) = " << effectiveResistance(Lpinv_path, 0, 3)
         << " (期望 3.0)" << endl;
    cout << "  R_eff(0,1) = " << effectiveResistance(Lpinv_path, 0, 1)
         << " (期望 1.0)" << endl;

    // 并联: 0—1—2, 添加边 (0,2) 形成三角形
    vector<vector<int>> triangle(3);
    triangle[0] = {1, 2};
    triangle[1] = {0, 2};
    triangle[2] = {0, 1};

    auto L_tri = buildLaplacian(triangle);
    auto Lpinv_tri = pseudoInverse(L_tri);

    cout << endl << "[三角形 0—1—2] 每边1Ω, 0-2有直连+经1路径" << endl;
    cout << "  R_eff(0,2) = " << effectiveResistance(Lpinv_tri, 0, 2)
         << " (期望 2/3 ≈ 0.667, 1Ω || 2Ω)" << endl;

    // 验证 Chandra–Ranjan–Doyle 定理
    int m = 3;  // 边数
    cout << endl << "[Chandra–Ranjan–Doyle]" << endl;
    cout << "  R_eff(0,2) × 2|E| = "
         << effectiveResistance(Lpinv_tri, 0, 2) * (2 * m) << endl;
    cout << "  = 往返时间 C(0,2) 的期望" << endl;

    cout << endl << "[Rayleigh 单调性]" << endl;
    cout << "  加边只会降低(或不变) R_eff" << endl;
    cout << "  桥的 R_eff = 1 (去掉它图就不连通)" << endl;
    cout << "  稀疏化: 保留 R_eff 最大的边" << endl;

    return 0;
}
