/**
 * 谱聚类 (Spectral Clustering) — 图Laplacian分解
 *
 * 谱聚类是现代聚类算法中理论最优雅的方法之一。
 *
 * Laplacian 矩阵:
 *   L = D - W  (未归一化)
 *   L_sym = I - D^(-1/2) W D^(-1/2)  (对称归一化)
 *   L_rw = I - D^(-1) W  (随机游走归一化)
 *
 * 核心性质:
 *   1. L 是半正定矩阵, 最小特征值 λ₁ = 0, 对应特征向量 = 1
 *   2. L 的零特征值的重数 = 图的连通分量数
 *   3. 第二小特征值 λ₂ > 0 ⟺ 图连通 (代数连通度)
 *   4. Fiedler 向量 (λ₂ 对应特征向量): 可用于图的二分
 *
 * 谱聚类算法:
 *   1. 构建相似度图 (ε-近邻, k-NN, 全连接)
 *   2. 计算归一化 Laplacian L_sym
 *   3. 计算 L_sym 的前 k 个特征向量 → 矩阵 U ∈ R^(n×k)
 *   4. 将 U 的每行归一化 (行范数 = 1)
 *   5. 对 n 个行向量运行 k-means → k 个簇
 *
 * 理论依据:
 *   Normalized Cut (NCut) 最小化 ≈ Rayleigh 商迹最小化
 *   连续放松后恰好等于 Laplacian 的特征分解
 *
 * 应用:
 *   - 图像分割 (Shi & Malik, 2000)
 *   - 社区发现
 *   - 单细胞 RNA 测序聚类
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

// 幂迭代法 (Power Iteration) 求主特征向量 (演示用)
vector<double> powerIteration(const vector<vector<double>>& A, int iters = 100) {
    int n = A.size();
    vector<double> v(n, 1.0 / sqrt(n));

    for (int iter = 0; iter < iters; iter++) {
        vector<double> Av(n, 0.0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                Av[i] += A[i][j] * v[j];

        // 归一化
        double norm = 0;
        for (double x : Av) norm += x * x;
        norm = sqrt(norm);
        if (norm < 1e-10) break;
        for (int i = 0; i < n; i++) v[i] = Av[i] / norm;
    }
    return v;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  谱聚类 (Spectral Clustering)            " << endl;
    cout << "========================================" << endl << endl;

    cout << "[图 Laplacian 构造]" << endl;

    // 简单图: 邻接矩阵
    vector<vector<double>> W = {
        {0, 1, 1, 0, 0},
        {1, 0, 1, 0, 0},
        {1, 1, 0, 0, 0},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0}
    };

    int n = W.size();
    cout << "  邻接矩阵 W: 两个连通分量 {0,1,2} 和 {3,4}" << endl;

    // 度矩阵 D
    vector<double> deg(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            deg[i] += W[i][j];

    // 构造 Laplacian L = D - W
    vector<vector<double>> L(n, vector<double>(n, 0));
    for (int i = 0; i < n; i++) {
        L[i][i] = deg[i];
        for (int j = 0; j < n; j++)
            if (i != j) L[i][j] = -W[i][j];
    }

    cout << "  零特征值个数 = 连通分量数 = 2" << endl;

    auto fiedler = powerIteration(L);
    cout << endl << "[Fiedler 向量 (第二特征向量)]" << endl;
    for (int i = 0; i < n; i++)
        cout << "  结点" << i << ": " << fiedler[i] << endl;
    cout << "  同分量内的符号一致, 可用于二分" << endl;

    cout << endl << "[谱聚类 vs k-means]" << endl;
    cout << "  k-means: 球状簇, 对非球形数据差" << endl;
    cout << "  谱聚类: 基于图割, 任意形状的簇" << endl;
    cout << "  代价: O(n³) SVD分解, 大图需近似" << endl;

    return 0;
}
