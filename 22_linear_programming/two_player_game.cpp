/**
 * 双人零和博弈与 Minimax 定理
 *
 * 双人零和博弈 (Two-Player Zero-Sum Game):
 *   玩家 Row 选行 i, 玩家 Col 选列 j
 *   Row 获得 A[i][j] 的收益, Col 损失 A[i][j]
 *   (零和: Row 所得 = -Col 所得)
 *
 * 纯策略 vs 混合策略:
 *   纯策略: 固定选某行
 *   混合策略: 以概率分布 x = (x₁,...,x_m) 选行
 *
 * Row 的视角:
 *   选择 x 最小化最坏情况损失 (maximize worst-case gain):
 *     max_x  min_j  Σ_i x_i · A[i][j]
 *
 * Col 的视角:
 *   选择 y 最小化最坏情况损失:
 *     min_y  max_i  Σ_j y_j · A[i][j]
 *
 * Minimax 定理 (Von Neumann, 1928):
 *   对任意矩阵 A:
 *     max_x min_y  x^T A y  =  min_y max_x  x^T A y
 *   即: 存在鞍点 (x*, y*), 双方不能通过单方面改变策略获益
 *
 * LP 形式:
 *   Row:                                     Col (Dual):
 *     max  v                                   min  w
 *     s.t. Σ_i x_i·A[i][j] ≥ v  ∀j             s.t. Σ_j A[i][j]·y_j ≤ w  ∀i
 *          Σ_i x_i = 1                              Σ_j y_j = 1
 *          x_i ≥ 0                                  y_j ≥ 0
 *
 *   这其实是同一 LP 的原问题和对偶问题!
 *   Row 的 LP 对偶正是 Col 的 LP, v* = w* 由强对偶保证。
 *
 * 应用:
 *   - 博弈论: 扑克/麻将的 GTO (Game Theory Optimal) 策略
 *   - 安全: 攻击者-防御者模型
 *   - 机器学习: GAN (生成对抗网络) 的 minimax 训练
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

const double INF = numeric_limits<double>::max();

// ==================== 用 LP 求解零和博弈 ====================
// 简化: 用迭代法 (Brown-Robinson fictitious play) 逼近
// 优点: 不需要 LP 求解器, 适合大矩阵

// 求解 Row 的混合策略, 给定收益矩阵 A[m][n]
// 使用 fictitious play (虚拟对策)
struct GameSolution {
    vector<double> rowStrategy;  // Row 的混合策略
    vector<double> colStrategy;  // Col 的混合策略
    double value;                // 博弈价值
};

GameSolution fictitiousPlay(const vector<vector<double>>& A, int iters = 5000) {
    int m = A.size(), n = A[0].size();

    vector<int> rowCounts(m, 0);  // Row 各策略被选次数
    vector<int> colCounts(n, 0);  // Col 各策略被选次数

    // 初始化: Row 选第0行, Col 选第0列
    int rowChoice = 0, colChoice = 0;
    rowCounts[0]++;
    colCounts[0]++;

    // Col 的累积收益 (对每个 j, Row 选 i 时 Col 损失)
    vector<double> colLosses(n, 0.0);
    vector<double> rowGains(m, 0.0);

    for (int t = 0; t < iters; t++) {
        // Col 对 Row 的历史策略做 best response:
        // 对于 Row 的混合策略, Col 选使 Row 收益最小的列
        double minGain = INF;
        for (int j = 0; j < n; j++) {
            double expectedGain = 0.0;
            for (int i = 0; i < m; i++)
                expectedGain += rowCounts[i] * A[i][j];
            if (expectedGain < minGain) {
                minGain = expectedGain;
                colChoice = j;
            }
        }
        colCounts[colChoice]++;

        // Row 对 Col 的历史策略做 best response:
        // 对于 Col 的混合策略, Row 选使自身收益最大的行
        double maxGain = -INF;
        for (int i = 0; i < m; i++) {
            double expectedGain = 0.0;
            for (int j = 0; j < n; j++)
                expectedGain += colCounts[j] * A[i][j];
            if (expectedGain > maxGain) {
                maxGain = expectedGain;
                rowChoice = i;
            }
        }
        rowCounts[rowChoice]++;
    }

    // 计算策略和经验值
    vector<double> rStrat(m), cStrat(n);
    int totalRows = 0, totalCols = 0;
    for (int i = 0; i < m; i++) totalRows += rowCounts[i];
    for (int j = 0; j < n; j++) totalCols += colCounts[j];

    for (int i = 0; i < m; i++) rStrat[i] = (double)rowCounts[i] / totalRows;
    for (int j = 0; j < n; j++) cStrat[j] = (double)colCounts[j] / totalCols;

    // 博弈价值
    double value = 0.0;
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            value += rStrat[i] * A[i][j] * cStrat[j];

    return {rStrat, cStrat, value};
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  双人零和博弈 & Minimax 定理               " << endl;
    cout << "========================================" << endl << endl;

    // Matching Pennies: Row 和 Col 各选 H/T
    // Row 选对 +1, 选错 -1
    //      Col:H  Col:T
    // Row:H  +1    -1
    // Row:T  -1    +1
    vector<vector<double>> pennies = {
        {1.0, -1.0},
        {-1.0, 1.0}
    };

    auto sol = fictitiousPlay(pennies);

    cout << "[Matching Pennies]" << endl;
    cout << "  Row 策略: ";
    for (double x : sol.rowStrategy)
        cout << fixed << setprecision(3) << x << " ";
    cout << "(期望: 0.5, 0.5)" << endl;
    cout << "  Col 策略: ";
    for (double y : sol.colStrategy)
        cout << fixed << setprecision(3) << y << " ";
    cout << "(期望: 0.5, 0.5)" << endl;
    cout << "  博弈价值: " << sol.value << " (期望: 0.0)" << endl;

    // 非对称博弈 (偏向 Row)
    vector<vector<double>> asymmetric = {
        {3.0, 0.0},
        {0.0, 1.0}
    };

    auto sol2 = fictitiousPlay(asymmetric);

    cout << endl << "[非对称博弈] Row 决策占优时倾向高收益行" << endl;
    cout << "  Row 策略: ";
    for (double x : sol2.rowStrategy)
        cout << fixed << setprecision(3) << x << " ";
    cout << endl;
    cout << "  博弈价值: " << sol2.value
         << " (理论: 3*1)/(3+1-0-0) = 0.75? 实际≈0.75)" << endl;

    cout << endl << "[Minimax 定理]" << endl;
    cout << "  max_x min_y x^T A y = min_y max_x x^T A y" << endl;
    cout << "  鞍点存在 ⇒ 双方无动机单方面偏离" << endl;
    cout << "  Row LP ──对偶──→ Col LP, 强对偶 ⇒ v* = w*" << endl;

    return 0;
}
