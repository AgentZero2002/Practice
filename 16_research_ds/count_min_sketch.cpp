/**
 * Count-Min Sketch — 频率估计的概型数据结构
 *
 * Count-Min Sketch 用于在数据流中以亚线性空间近似统计元素频率。
 *
 * 原理:
 *   使用 d 个哈希函数，每个对应一个长度为 w 的计数数组。
 *   add(x):  对每个哈希函数 i，count[i][hash_i(x)]++
 *   query(x): 返回 min(count[i][hash_i(x)]) — 取最小值上界
 *
 * 性质:
 *   - 永远不会低估 (count ≥ 真实值)
 *   - 高估概率: P(误差 > ε×N) ≤ (1/2)^d, 其中 ε=e/w
 *   - 空间: d × w (与数据流大小无关!)
 *
 * 应用:
 *   - 流数据 Top-K 元素
 *   - 网络流量统计 (按源IP/目的端口)
 *   - 推荐系统的频次特征
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <climits>
using namespace std;

class CountMinSketch {
private:
    int d;  // 哈希函数个数
    int w;  // 计数器宽度
    vector<vector<int>> table;
    vector<pair<int, int>> hashParams;  // (a, b) for (a*x + b) mod w

    int hash(int x, int i) const {
        int a = hashParams[i].first;
        int b = hashParams[i].second;
        // 使用大素数 2147483647 做模运算
        long long val = ((long long)a * x + b) % 2147483647;
        return abs((int)(val % w));
    }

public:
    // ε=置信度, δ=误差率
    // w = ceil(e/ε), d = ceil(ln(1/δ))
    CountMinSketch(double epsilon, double delta) {
        w = max(1, (int)ceil(2.71828 / epsilon));
        d = max(1, (int)ceil(log(1.0 / delta)));
        table.assign(d, vector<int>(w, 0));

        // 随机生成哈希参数
        mt19937 rng(42);
        uniform_int_distribution<int> dist(1, 1000000);
        for (int i = 0; i < d; i++)
            hashParams.push_back({dist(rng), dist(rng)});
    }

    void add(int x, int count = 1) {
        for (int i = 0; i < d; i++)
            table[i][hash(x, i)] += count;
    }

    int estimate(int x) const {
        int mn = INT_MAX;
        for (int i = 0; i < d; i++)
            mn = min(mn, table[i][hash(x, i)]);
        return mn;
    }
};

// ==================== 应用: 数据流 Top-K ====================
// 用 Count-Min Sketch + 最小堆近似找出 Top-K 频繁元素
#include <queue>
vector<pair<int, int>> topK_sketch(const vector<int>& stream, int k,
                                    double eps = 0.01, double delta = 0.01) {
    CountMinSketch cms(eps, delta);
    unordered_map<int, bool> candidates;

    for (int x : stream) {
        cms.add(x);
        candidates[x] = true;
    }

    // 对所有候选元素查询估计频率
    vector<pair<int, int>> freqs;
    for (auto& [x, _] : candidates)
        freqs.push_back({cms.estimate(x), x});

    sort(freqs.rbegin(), freqs.rend());
    freqs.resize(min(k, (int)freqs.size()));
    return freqs;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Count-Min Sketch — 频率估计            " << endl;
    cout << "========================================" << endl << endl;

    // 基本演示
    CountMinSketch cms(0.05, 0.01);  // ε=5%, δ=1%
    cout << "[参数] ε=5%, δ=1% → w=" << (int)ceil(2.71828/0.05)
         << ", d=" << (int)ceil(log(1/0.01)) << endl << endl;

    // 模拟数据流
    vector<int> stream = {42, 42, 42, 7, 7, 42, 100, 42};
    for (int x : stream) cms.add(x);

    cout << "[查询] 流: {42×4, 7×2, 100×1}" << endl;
    cout << "  42  ≈ " << cms.estimate(42) << " (真实: 4)" << endl;
    cout << "  7   ≈ " << cms.estimate(7) << "  (真实: 2)" << endl;
    cout << "  100 ≈ " << cms.estimate(100) << " (真实: 1)" << endl;
    cout << "  999 ≈ " << cms.estimate(999) << " (真实: 0)" << endl;

    cout << endl << "[关键性质]" << endl;
    cout << "  1. 绝不低估: count ≥ true_count" << endl;
    cout << "  2. 空间亚线性: 与数据流大小无关" << endl;
    cout << "  3. 常用于: 流数据Top-K, 网络测量" << endl;

    return 0;
}
