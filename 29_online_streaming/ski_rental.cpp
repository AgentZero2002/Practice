/**
 * 滑雪租赁问题 (Ski Rental Problem)
 *
 * 经典在线决策问题, 也是竞争比分析的入门案例。
 *
 * 场景:
 *   你每天去滑雪, 但不确定总共会滑多少天 T。
 *   选择一: 每次租雪板, 花费 1 元/天
 *   选择二: 买一副雪板, 花费 B 元 (之后永久免费)
 *   你事先不知道 T, 每天只知道自己"今天还要不要滑"。
 *
 *   目标: 最小化 regret = (你的花费) / OPT
 *         其中 OPT 是事后知道 T 的最优花费
 *
 * OPT 策略 (离线最优):
 *   如果 T ≤ B:  每天租, 花费 T
 *   如果 T > B:   直接买, 花费 B
 *   OPT = min(T, B)
 *
 * 在线算法 (确定型):
 *   前 B-1 天租, 第 B 天买 (break-even rule)
 *   竞争比 = 2 - 1/B ≈ 2
 *   即: 花费最多是 OPT 的 2 倍
 *
 *   证明:
 *     如果 T < B:  花 T = OPT  (竞争比 1)
 *     如果 T ≥ B:  花 (B-1) + B = 2B-1
 *                  OPT = B
 *                  竞争比 = (2B-1)/B = 2 - 1/B
 *
 * 随机化在线算法:
 *   在第 i 天以概率 p_i 决定"买", 否则"租"
 *   最优随机化竞争比 ≈ e/(e-1) ≈ 1.58
 *   (比确定型的 2-1/B 更好!)
 *
 * 推广:
 *   - Bahncard 问题 (铁路打折卡): 租 vs 买折扣卡
 *   - TCP 超时重传: ACK 等待 vs 重发
 *   - 资本预算: 租用 vs 购买设备
 *   - 云资源: on-demand vs reserved instances
 *
 * 与 online_paging.cpp 的联系:
 *   分页也是在线问题, 竞争比 = k (LRU/OPT)
 *   Ski Rental 是最简单的在线决策问题
 */

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <cmath>
using namespace std;

// ==================== 在线算法 (确定型) ====================
// 返回: {总花费, OPT花费, 竞争比}
struct Result {
    int cost;
    int opt;
    double ratio;
};

Result deterministicSkiRental(int B, int T) {
    int cost = 0;
    bool bought = false;

    for (int day = 1; day <= T; day++) {
        if (bought) {
            // 已买, 无额外花费
            break;  // 之后永远免费
        } else if (day < B) {
            cost += 1;  // 租
        } else {
            cost += B;  // 买
            bought = true;
        }
    }
    // 如果到最后都没买, 花费 = T (全租)
    if (!bought && T < B) cost = T;

    int opt = min(T, B);
    return {cost, opt, (double)cost / opt};
}

// ==================== 随机化在线算法 ====================
// 以概率 1/B 在某天决定买
Result randomizedSkiRental(int B, int T, mt19937& rng) {
    uniform_real_distribution<double> uni(0.0, 1.0);

    int cost = 0;
    bool bought = false;

    // 每天以概率 p ≈ 1/(B-i+1) 决定是否买
    // 这是 e/(e-1) 竞争比的最优随机化算法
    for (int day = 1; day <= T; day++) {
        if (bought) break;

        // 第 day 天以递增概率决定"买"
        double prob = 1.0 / (B - day + 1.0);
        if (day >= B) prob = 1.0;  // 最后一天必买

        if (uni(rng) < prob) {
            cost += B;  // 买
            bought = true;
        } else {
            cost += 1;  // 租
        }
    }
    if (!bought && T < B) cost = T;

    int opt = min(T, B);
    return {cost, opt, (double)cost / opt};
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  滑雪租赁 (Ski Rental)                    " << endl;
    cout << "========================================" << endl << endl;

    int B = 10;  // 买的价格 = 10倍日租金

    cout << "[场景] B=" << B << " (买=10天租金)" << endl << endl;

    // 不同 T 的在线决策表现
    cout << "  滑雪天数T   确定型花费   OPT    竞争比" << endl;
    cout << "  " << string(45, '-') << endl;

    for (int T : {3, 7, 10, 15, 20, 50}) {
        auto res = deterministicSkiRental(B, T);
        cout << "  " << setw(8) << T
             << "     " << setw(7) << res.cost
             << "     " << setw(5) << res.opt
             << "     " << fixed << setprecision(3) << res.ratio << endl;
    }

    cout << endl << "[最坏情况分析]" << endl;
    cout << "  确定型竞争比 = 2 - 1/B = " << (2.0 - 1.0/B) << endl;
    cout << "  即: 在线花费 ≤ " << (2.0 - 1.0/B) << "× OPT" << endl;

    // 随机化算法平均表现
    cout << endl << "[随机化算法] 5000次实验, T=15, B=10" << endl;
    mt19937 rng(42);
    double totalRatio = 0.0;
    int trials = 5000;
    for (int t = 0; t < trials; t++) {
        totalRatio += randomizedSkiRental(B, 15, rng).ratio;
    }
    cout << "  平均竞争比: " << totalRatio / trials
         << " (理论最优 ≈ e/(e-1) = " << (exp(1.0)/(exp(1.0)-1.0)) << ")" << endl;

    cout << endl << "[竞争比层次]" << endl;
    cout << "  确定型: 2 - 1/B  (≈ 2)" << endl;
    cout << "  随机化: e/(e-1)  (≈ 1.58)" << endl;
    cout << "  下限:   1 + 1/B  (任何在线算法的最小 regret)" << endl;

    return 0;
}
