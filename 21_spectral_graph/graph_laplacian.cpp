/**
 * 图 Laplacian 矩阵与 Fiedler 向量
 *
 * 图 Laplacian 是谱图论的核心对象，它将图的组合性质编码为矩阵的代数性质。
 *
 * 定义:
 *   L = D - W
 *   其中 W = 邻接矩阵 (或权重矩阵), D = 度矩阵 (D_ii = Σ_j W_ij)
 *
 * 归一化 Laplacian (对称):
 *   L_sym = D^(-1/2) L D^(-1/2) = I - D^(-1/2) W D^(-1/2)
 *
 * 归一化 Laplacian (随机游走):
 *   L_rw = D^(-1) L = I - D^(-1) W
 *
 * Laplacian 的谱性质 (特征值与特征向量):
 *   1. L 是对称半正定矩阵, 所有特征值 λ_i ≥ 0
 *   2. λ_1 = 0, 对应特征向量 1 = (1,1,...,1)
 *   3. λ_2 (第二小特征值) = 代数连通度 (algebraic connectivity)
 *      - λ_2 = 0 ⇔ 图不连通
 *      - λ_2 越大, 图连通性越强
 *   4. λ_n (最大特征值) ≤ 2·max_degree
 *
 * Fiedler 向量:
 *   λ_2 对应的特征向量 v_2 称为 Fiedler 向量
 *   它给出图的"最自然"二分: 按 v_2 分量的正负号划分
 *
 * 应用:
 *   - 谱聚类: 用 Fiedler 向量进行图分割 (RatioCut / NCut)
 *   - 图绘制: Laplacian 特征向量作为节点坐标
 *   - 扩张图 (Expander): λ_2 大 ⇒ 图是好的扩张器
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
using namespace std;

// ==================== 构建 Laplacian 矩阵 ====================
vector<vector<double>> buildLaplacian(const vector<vector<int>>& adj,
                                       bool normalized = false) {
    int n = adj.size();
    vector<vector<double>> L(n, vector<double>(n, 0.0));

    // 构建 D - W
    for (int i = 0; i < n; i++) {
        double deg = adj[i].size();
        L[i][i] = deg;
        for (int j : adj[i]) {
            L[i][j] = -1.0;
        }
    }

    // 归一化: D^(-1/2) L D^(-1/2)
    if (normalized) {
        for (int i = 0; i < n; i++) {
            double d_inv_sqrt = 1.0 / sqrt(max(1.0, (double)adj[i].size()));
            for (int j = 0; j < n; j++) {
                double d_j_inv_sqrt = 1.0 / sqrt(max(1.0, (double)adj[j].size()));
                L[i][j] *= d_inv_sqrt * d_j_inv_sqrt;
            }
        }
    }
    return L;
}

// ==================== 幂迭代法求 Fiedler 向量 ====================
// 求第二小特征值: 将矩阵"抬高"使第一特征值变为最大，然后幂迭代求次大
// 实际上我们用 inverse iteration 的变体: 对 (λ·I - L) 做幂迭代
// 简化为: 对 L 做幂迭代求最大特征值, 然后 "deflate" 找第二大
//
// 这里使用简化的 shifted power iteration
vector<double> powerIteration(const vector<vector<double>>& A, int iters = 200) {
    int n = A.size();
    vector<double> v(n, 1.0 / sqrt(n));

    for (int iter = 0; iter < iters; iter++) {
        // v = A * v
        vector<double> Av(n, 0.0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                Av[i] += A[i][j] * v[j];

        // 归一化
        double norm = 0.0;
        for (double x : Av) norm += x * x;
        norm = sqrt(norm);
        if (norm < 1e-12) break;
        for (int i = 0; i < n; i++) v[i] = Av[i] / norm;
    }
    return v;
}

// 求 λ_2 对应的 Fiedler 向量: 用 L' = λ_max*I - L 把最小特征值映射为最大
// 然后对 L' 做幂迭代, 得到的就是 Fiedler 向量
vector<double> fiedlerVector(const vector<vector<double>>& L, int iters = 200) {
    int n = L.size();
    // 估计 λ_max (用 Gershgorin 圆盘定理: λ_max ≤ 2·max_degree)
    double lambda_max = 0.0;
    for (int i = 0; i < n; i++) {
        double rowSum = 0.0;
        for (int j = 0; j < n; j++) rowSum += fabs(L[i][j]);
        lambda_max = max(lambda_max, rowSum);
    }

    // 构造 shifted 矩阵: M = lambda_max * I - L
    // M 的特征值: λ_max - λ_i, 所以 λ_2 对应 M 的最大特征值
    vector<vector<double>> M(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        M[i][i] = lambda_max;
        for (int j = 0; j < n; j++)
            M[i][j] -= L[i][j];
    }

    // 对 M 做幂迭代
    vector<double> v = powerIteration(M, iters);

    // 正交化掉全1向量 (把分量减去均值)
    double mean = 0.0;
    for (double x : v) mean += x;
    mean /= n;
    for (double& x : v) x -= mean;

    // 重新归一化
    double norm = 0.0;
    for (double x : v) norm += x * x;
    norm = sqrt(norm);
    if (norm > 1e-12)
        for (double& x : v) x /= norm;

    return v;
}

// ==================== 图的代数连通度 λ_2 ====================
double algebraicConnectivity(const vector<vector<double>>& L) {
    // 瑞利商估计: λ_2 ≈ min_{v⊥1} (v^T L v) / (v^T v)
    // 用 Fiedler 向量的瑞利商来近似
    vector<double> v = fiedlerVector(L);

    double numerator = 0.0, denominator = 0.0;
    int n = L.size();
    for (int i = 0; i < n; i++) {
        denominator += v[i] * v[i];
        for (int j = 0; j < n; j++)
            numerator += v[i] * L[i][j] * v[j];
    }
    return sqrt(numerator / denominator);  // 近似
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  图 Laplacian 与 Fiedler 向量             " << endl;
    cout << "========================================" << endl << endl;

    // 示例图: 两团 (clique) 之间只有一条边连接
    // 节点 0,1,2 构成团A; 节点 3,4,5 构成团B; 边 (2,3) 连接
    vector<vector<int>> adj(6);
    // 团A: {0,1,2} 两两相连
    adj[0] = {1, 2};       adj[1] = {0, 2};       adj[2] = {0, 1, 3};
    // 团B: {3,4,5} 两两相连
    adj[3] = {2, 4, 5};    adj[4] = {3, 5};       adj[5] = {3, 4};

    auto L = buildLaplacian(adj);
    auto L_norm = buildLaplacian(adj, true);

    cout << "[Laplacian 矩阵] (非归一化)" << endl;
    for (int i = 0; i < 6; i++) {
        cout << "  ";
        for (int j = 0; j < 6; j++)
            cout << setw(4) << L[i][j];
        cout << endl;
    }

    cout << endl << "[Fiedler 向量] 按正负二分:" << endl;
    auto fiedler = fiedlerVector(L);
    cout << "  团A: ";
    for (int i = 0; i < 3; i++)
        cout << (fiedler[i] >= 0 ? "+ " : "- ") << " ";
    cout << endl << "  团B: ";
    for (int i = 3; i < 6; i++)
        cout << (fiedler[i] >= 0 ? "+ " : "- ") << " ";
    cout << endl;

    cout << endl << "[Laplacian 谱性质]" << endl;
    cout << "  λ₁ = 0 (全1向量)" << endl;
    cout << "  λ₂ > 0 ⇔ 图连通 (代数连通度)" << endl;
    cout << "  Fiedler向量: 自然二分 → 谱聚类" << endl;
    cout << "  Cheeger不等式: λ₂/2 ≤ 传导率 ≤ √(2λ₂)" << endl;

    return 0;
}
