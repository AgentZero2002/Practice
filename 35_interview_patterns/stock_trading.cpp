/**
 * 股票买卖系列 — DP 经典演化
 *
 * 这六道题展示了 DP 从简单到复杂的一步步扩展，
 * 是面试中考察 DP 思维的最经典系列。
 *
 * I:   最多 1 次交易                           → 贪心
 * II:  无限次交易                                → 贪心/DP
 * III: 最多 2 次交易                             → DP 状态扩展
 * IV:  最多 K 次交易                             → DP 一般化
 *
 * 核心状态定义: dp[day][t][hold]
 *   day: 第几天
 *   t:   已完成 t 次交易
 *   hold: 0=不持股, 1=持股
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

// ==================== I: 最多 1 次交易 ====================
int maxProfit_I(const vector<int>& prices) {
    int minPrice = INT_MAX, maxProfit = 0;
    for (int p : prices) {
        minPrice = min(minPrice, p);
        maxProfit = max(maxProfit, p - minPrice);
    }
    return maxProfit;
}

// ==================== II: 无限次交易 ====================
int maxProfit_II(const vector<int>& prices) {
    int profit = 0;
    for (int i = 1; i < (int)prices.size(); i++)
        if (prices[i] > prices[i - 1])
            profit += prices[i] - prices[i - 1];
    return profit;
}

// ==================== III: 最多 2 次交易 ====================
int maxProfit_III(const vector<int>& prices) {
    int buy1 = INT_MIN, sell1 = 0;
    int buy2 = INT_MIN, sell2 = 0;

    for (int p : prices) {
        buy1 = max(buy1, -p);         // 第一次买
        sell1 = max(sell1, buy1 + p);  // 第一次卖
        buy2 = max(buy2, sell1 - p);   // 第二次买 (用第一次利润)
        sell2 = max(sell2, buy2 + p);  // 第二次卖
    }
    return sell2;
}

// ==================== IV: 最多 K 次交易 ====================
int maxProfit_IV(int K, const vector<int>& prices) {
    int n = prices.size();
    if (n == 0 || K == 0) return 0;

    // K 足够大 → 退化为无限次 (LeetCode II)
    if (K >= n / 2) return maxProfit_II(prices);

    vector<int> buy(K + 1, INT_MIN);
    vector<int> sell(K + 1, 0);

    for (int p : prices) {
        for (int k = 1; k <= K; k++) {
            buy[k] = max(buy[k], sell[k - 1] - p);
            sell[k] = max(sell[k], buy[k] + p);
        }
    }
    return sell[K];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  股票买卖系列 — Best Time Series        " << endl;
    cout << "========================================" << endl << endl;

    vector<int> prices = {7, 1, 5, 3, 6, 4};

    cout << "  股价: "; for (int p : prices) cout << p << " ";
    cout << endl << endl;

    cout << "  I:   最多 1 次  → " << maxProfit_I(prices) << " (期望: 5, 买入1卖出6)" << endl;
    cout << "  II:  无限次     → " << maxProfit_II(prices) << " (期望: 7)" << endl;
    cout << "  III: 最多 2 次  → " << maxProfit_III(prices) << " (期望: 7)" << endl;
    cout << "  IV:  最多 K=2 次 → " << maxProfit_IV(2, prices) << " (同上)" << endl;

    cout << endl << "[DP 思维链]" << endl;
    cout << "  I:   minPrice + 贪心" << endl;
    cout << "  II:  每天赚差价 (无限次)" << endl;
    cout << "  III: 4 状态变量 (buy1,sell1,buy2,sell2)" << endl;
    cout << "  IV:  2K 状态数组 — 一般化 DP" << endl;
    cout << "  面试时从 I→IV 逐步推导, 展示 DP 思维过程" << endl;

    return 0;
}
