/**
 * 随机化舍入 (Randomized Rounding)
 *
 * 随机化舍入是近似算法和在线算法的核心技术。
 * 把 LP 松弛的分数解随机舍入为整数解, 保证期望约束。
 *
 * 核心思想:
 *   1. 求解 LP 松弛: x ∈ [0,1]   (多项式时间)
 *   2. 随机舍入: x_i = 1 with prob x_i*
 *   3. 分析期望质量: E[目标值] ≤ α·OPT
 *
 * 典型应用:
 *
 * 1. Set Cover (集合覆盖):
 *    LP 松弛 → 随机舍入 → O(log n) 近似比
 *    每轮: 每个集合以 f·x_S 的概率选中, 重复 O(log n) 轮
 *
 * 2. MAX-SAT (最大可满足子句):
 *    LP 松弛 → 随机舍入 → (1-1/e) 近似比
 *    每个变量 x_i = 1 with prob y_i*
 *
 * 3. 拥塞最小化 (Congestion Minimization):
 *    LP 路径选择 → 随机舍入 → 期望拥塞 ≤ OPT
 *
 * 本文件展示 Set Cover 的随机化舍入。
 *
 * Set Cover 问题:
 *   给定 n 个元素 U = {1,...,n} 和 m 个集合 S₁,...,Sₘ ⊆ U,
 *   每个集合有权重 wⱼ, 选出代价最小的子集族覆盖所有元素。
 *
 *   NP-hard, 贪心 O(log n) 近似, LP舍入 O(log n) 近似。
 *
 * LP 形式:
 *   min  Σ_j w_j · x_j
 *   s.t. Σ_{j: i∈S_j} x_j ≥ 1   ∀i ∈ U   (每个元素被至少一个集合覆盖)
 *        x_j ≥ 0                          (松弛为分数)
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cmath>
using namespace std;

// ==================== Set Cover — 贪心算法 (基线) ====================
// 每次选 覆盖新元素数/权重 比率最高的集合
vector<int> greedySetCover(const vector<vector<int>>& sets,
                            const vector<double>& weights, int n) {
    int m = sets.size();
    vector<bool> covered(n, false);
    vector<bool> selected(m, false);
    int remaining = n;

    while (remaining > 0) {
        // 找最佳性价比集合
        int best = -1;
        double bestRatio = 0.0;
        for (int j = 0; j < m; j++) {
            if (selected[j]) continue;
            int newElems = 0;
            for (int elem : sets[j])
                if (!covered[elem]) newElems++;
            if (newElems == 0) continue;
            double ratio = newElems / weights[j];
            if (ratio > bestRatio) {
                bestRatio = ratio;
                best = j;
            }
        }
        if (best == -1) break;  // 无法完全覆盖

        selected[best] = true;
        for (int elem : sets[best])
            if (!covered[elem]) {
                covered[elem] = true;
                remaining--;
            }
    }

    vector<int> result;
    for (int j = 0; j < m; j++)
        if (selected[j]) result.push_back(j);
    return result;
}

// ==================== Set Cover — LP + 随机化舍入 ====================
// (简化版: 不真解 LP, 用分数启发式模拟舍入)
// 真实的 LP 舍入: 先解 LP → 得 x* → 舍入
// 这里展示舍入框架

vector<int> randomizedRoundingSetCover(
    const vector<vector<int>>& sets,
    const vector<double>& weights, int n, mt19937& rng) {

    int m = sets.size();

    // Step 1: 解 LP 松弛 (这里用贪心分数解替代, 实际应用用 Simplex/IPM)
    // 简化: 贪心给出的选择视为 x*_j ≈ 1/degree_j 的分数解
    vector<double> xStar(m, 0.0);
    vector<int> coverCount(n, 0);
    for (int j = 0; j < m; j++)
        for (int elem : sets[j])
            coverCount[elem]++;

    for (int j = 0; j < m; j++) {
        // 分数解: 约等于 1/权重的软阈值
        double avgDeg = 0;
        for (int elem : sets[j]) avgDeg += coverCount[elem];
        avgDeg /= max(1, (int)sets[j].size());
        xStar[j] = min(1.0, 1.0 / avgDeg);
    }

    // Step 2: 随机舍入 (带放大因子)
    // 以 c·ln(n)·x*_j 的概率独立选中每个集合, c≥1
    double c = 2.0;
    double factor = c * log(n + 1);

    uniform_real_distribution<double> uni(0.0, 1.0);

    vector<int> selected;
    vector<bool> covered(n, false);

    for (int round = 0; round < (int)factor + 1; round++) {
        for (int j = 0; j < m; j++) {
            double prob = min(1.0, xStar[j] * factor / (factor + 1));
            if (uni(rng) < prob) {
                selected.push_back(j);
            }
        }
    }

    return selected;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  随机化舍入 (Randomized Rounding)          " << endl;
    cout << "========================================" << endl << endl;

    // 简单 Set Cover 实例
    int n = 6;  // 元素 0..5
    vector<vector<int>> sets = {
        {0, 1, 2},       // S0 覆盖 {0,1,2}
        {0, 3},          // S1
        {1, 4},          // S2
        {2, 5},          // S3
        {3, 4, 5},       // S4
        {0, 1, 2, 3, 4, 5}  // S5 全集 (高成本)
    };
    vector<double> weights = {3.0, 1.0, 1.0, 1.0, 3.0, 10.0};

    cout << "[Set Cover: n=6, m=6]" << endl;
    cout << "  集合及权重:" << endl;
    for (int j = 0; j < 6; j++) {
        cout << "  S" << j << " (w=" << weights[j] << "): {";
        for (int i = 0; i < (int)sets[j].size(); i++)
            cout << sets[j][i] << (i+1 < (int)sets[j].size() ? "," : "");
        cout << "}" << endl;
    }

    // 贪心基线
    auto greedy = greedySetCover(sets, weights, n);
    double greedyCost = 0;
    for (int j : greedy) greedyCost += weights[j];
    cout << endl << "[贪心] 选中: {";
    for (int j : greedy) cout << "S" << j << " ";
    cout << "}, 代价 = " << greedyCost << endl;

    // 随机化舍入
    mt19937 rng(42);
    auto rs = randomizedRoundingSetCover(sets, weights, n, rng);
    double rsCost = 0;
    for (int j : rs) rsCost += weights[j];
    cout << "[随机化舍入] 选中: (共" << rs.size() << "个副本), 总代价 = " << rsCost << endl;

    cout << endl << "[随机化舍入框架]" << endl;
    cout << "  1. LP松弛解 → 分数 x*_j" << endl;
    cout << "  2. 独立随机舍入: Pr[x_j=1] = x*_j" << endl;
    cout << "  3. 期望约束: E[cost] = Σ w_j·x*_j ≤ OPT" << endl;
    cout << "  4. 集中不等式: 重复 O(log n) 次保证高概率" << endl;

    return 0;
}
