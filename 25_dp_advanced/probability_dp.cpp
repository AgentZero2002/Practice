/**
 * 概率 DP + 期望 DP
 *
 * 概率/期望 DP 是竞赛和面试中的进阶题型。
 *
 * 概率 DP:
 *   求某个事件发生的概率。
 *   状态转移按概率分支:
 *     dp[state] = Σ p(transition) × dp[nextState]
 *
 * 期望 DP:
 *   求到达某个状态的期望步数/值。
 *   通常需要解方程 (自环/成环):
 *     dp[i] = cost + Σ p(j|i) × dp[j]
 *     移项: dp[i] - Σ p(j|i) × dp[j] = cost → 高斯消元
 *
 * 经典问题:
 *   1. 掷骰子游戏: 期望掷几次到达目标分数
 *   2. 醉鬼走路: 从0出发, 以概率p向右, 1-p向左, 碰到N或0的概率
 *   3. 抽卡问题: 集齐全套的期望次数
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

// ==================== 1. 掷骰子到 N 的期望次数 ====================
// 从 0 开始, 每次掷均匀骰子 (1~6), 加到当前分数上,
// 首次超过或等于 N 时停止。求期望掷骰子次数。
double expectedDiceRolls(int N) {
    vector<double> dp(N + 7, 0);  // dp[i] = 从 i 到 ≥N 的期望次数
    for (int i = N - 1; i >= 0; i--) {
        double sum = 0;
        for (int d = 1; d <= 6; d++)
            sum += dp[i + d];
        dp[i] = 1 + sum / 6.0;
    }
    return dp[0];
}

// ==================== 2. 收集卡片问题 (Coupon Collector) ====================
// 每包随机包含一种卡片, n 种等概率, 集齐的期望包数
double couponCollector(int n) {
    double expected = 0;
    for (int i = 1; i <= n; i++)
        expected += (double)n / i;  // 调和数 H_n × n
    return expected;
}

// ==================== 3. 赌博者破产问题 (Gambler's Ruin) ====================
// 从 x 元开始, 每次赢 1 元的概率为 p, 输 1 元的概率为 1-p,
// 到达 N 为胜, 输光为负
double gamblersRuin(int x, int N, double p) {
    if (fabs(p - 0.5) < 1e-9) {
        // p = 0.5: 对称随机游走
        return (double)x / N;
    }
    double q = 1 - p;
    double ratio = q / p;
    return (1 - pow(ratio, x)) / (1 - pow(ratio, N));
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  概率 DP + 期望 DP                       " << endl;
    cout << "========================================" << endl << endl;

    cout << fixed << setprecision(4);

    cout << "[掷骰子] N=10, 期望次数: "
         << expectedDiceRolls(10) << endl;

    cout << "[收集卡片] n=10 种, 期望购买: "
         << couponCollector(10) << " 包" << endl;
    cout << "  (理论: 10·H₁₀ ≈ 29.29)" << endl;

    cout << "[赌博破产] 本金=50, 目标=100, p=0.45: "
         << gamblersRuin(50, 100, 0.45) << endl;
    cout << "  (p<0.5 → 期望胜率 < 50%)" << endl;

    cout << endl << "[概率/期望DP技巧]" << endl;
    cout << "  线性期望: 直接递推" << endl;
    cout << "  有环期望: 高斯消元 / 方程组" << endl;
    cout << "  贡献法: 考虑每个元素对总期望的贡献" << endl;

    return 0;
}
