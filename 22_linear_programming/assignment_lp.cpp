/**
 * 指派问题的线性规划形式
 *
 * 指派问题 (Assignment Problem): n 个工人, n 个任务,
 * 工人 i 完成任务 j 的成本为 c_ij, 每个工人分配一个任务,
 * 每个任务由一个工人完成, 最小化总成本。
 *
 * LP 形式:
 *   min  Σ_i Σ_j c_ij · x_ij
 *   s.t. Σ_j x_ij = 1   ∀i  (每个工人一项任务)
 *        Σ_i x_ij = 1   ∀j  (每个任务一个工人)
 *        x_ij ≥ 0
 *
 * 关键性质:
 *   约束矩阵是 doubly stochastic 多面体的顶点
 *   Birkhoff-von Neumann 定理:
 *     任何 doubly stochastic 矩阵是排列矩阵的凸组合
 *   ⇒ LP 的最优解自动为 0/1 排列矩阵!
 *   ⇒ 用单纯形或匈牙利算法都能得到整数解
 *
 * 匈牙利算法 (Hungarian Algorithm):
 *   组合算法, O(n³), 基于对偶变量 (行势+列势)
 *   本质上是对偶梯度下降: 维护可行对偶解, 逐步满足互补松弛
 *
 * 对偶 LP:
 *   max  Σ_i u_i + Σ_j v_j
 *   s.t. u_i + v_j ≤ c_ij   ∀i,j
 *        u_i, v_j unrestricted
 *
 * 互补松弛: x_ij > 0 ⇒ u_i + v_j = c_ij
 *
 * 推广:
 *   - 非方阵: 加虚工人/任务 (成本=0)
 *   - 最大权重匹配: 取反 c_ij → -c_ij
 *   - 运输问题: 容量不等的推广
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

const int INF = numeric_limits<int>::max();

// ==================== 匈牙利算法 (最小成本指派) ====================
// O(n³) 实现, n 个工人 × n 个任务
class Hungarian {
    int n;
    vector<vector<int>> cost;
    vector<int> u, v;     // 对偶变量 (行势, 列势)
    vector<int> p, way;   // 匹配 + 增广路径记录

public:
    Hungarian(const vector<vector<int>>& c)
        : n(c.size()), cost(c), u(n + 1), v(n + 1), p(n + 1), way(n + 1) {}

    // 返回 [最小总成本, 匹配: worker i → task assignment[i]]
    pair<int, vector<int>> solve() {
        for (int i = 1; i <= n; i++) {
            p[0] = i;
            int j0 = 0;
            vector<int> minv(n + 1, INF);
            vector<bool> used(n + 1, false);

            do {
                used[j0] = true;
                int i0 = p[j0];
                int delta = INF;
                int j1 = 0;

                // 找最小松弛量
                for (int j = 1; j <= n; j++) {
                    if (!used[j]) {
                        int cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                        if (cur < minv[j]) {
                            minv[j] = cur;
                            way[j] = j0;
                        }
                        if (minv[j] < delta) {
                            delta = minv[j];
                            j1 = j;
                        }
                    }
                }

                // 更新对偶变量
                for (int j = 0; j <= n; j++) {
                    if (used[j]) {
                        u[p[j]] += delta;
                        v[j] -= delta;
                    } else {
                        minv[j] -= delta;
                    }
                }
                j0 = j1;
            } while (p[j0] != 0);

            // 沿增广路径更新匹配
            do {
                int j1 = way[j0];
                p[j0] = p[j1];
                j0 = j1;
            } while (j0);
        }

        // 提取匹配和成本
        vector<int> assignment(n);
        for (int j = 1; j <= n; j++) {
            if (p[j] > 0)
                assignment[p[j] - 1] = j - 1;
        }

        int totalCost = -v[0];  // 对偶目标值 = 最小成本
        return {totalCost, assignment};
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  指派问题 (Assignment Problem)            " << endl;
    cout << "========================================" << endl << endl;

    // 4 工人 × 4 任务
    vector<vector<int>> cost = {
        {9, 2, 7, 8},
        {6, 4, 3, 7},
        {5, 8, 1, 8},
        {7, 6, 9, 4}
    };

    cout << "[成本矩阵 C_ij]" << endl;
    for (auto& row : cost) {
        cout << "  ";
        for (int c : row) cout << c << " ";
        cout << endl;
    }

    Hungarian hung(cost);
    auto [total, assign] = hung.solve();

    cout << endl << "[最优指派]" << endl;
    for (int i = 0; i < 4; i++)
        cout << "  工人" << i << " → 任务" << assign[i]
             << " (成本 " << cost[i][assign[i]] << ")" << endl;
    cout << "  总成本: " << total << endl;

    cout << endl << "[Birkhoff-von Neumann]" << endl;
    cout << "  约束矩阵 totally unimodular" << endl;
    cout << "  LP 基本可行解 = 排列矩阵 (0/1)" << endl;
    cout << "  匈牙利算法 = 组合化的对偶上升" << endl;

    return 0;
}
