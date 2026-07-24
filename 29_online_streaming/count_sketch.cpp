/**
 * Count-Sketch (计数草图)
 *
 * Count-Sketch 是 Count-Min Sketch 的"无偏"表亲。
 * 相同目标 (频率估计), 不同的方法: Count-Min 给出高估,
 * Count-Sketch 给出无偏估计 (带 variance)。
 *
 * Count-Min Sketch:
 *   d 行哈希表, 每行 m 列
 *   add(x): 对每个 i: table[i][h_i(x)] += 1
 *   query(x): min_i  table[i][h_i(x)]  ← 总是 ≥ 真实值 (高估)
 *
 * Count-Sketch:
 *   d 行, 每行 m 列
 *   额外: sign function s_i(x) ∈ {+1, -1}
 *   add(x, delta): 对每个 i: table[i][h_i(x)] += s_i(x) × delta
 *   query(x): median_i  s_i(x) × table[i][h_i(x)]  ← 无偏估计!
 *
 * 为什么无偏?
 *   E[s_i(x) × table[i][h_i(x)]] = f_x
 *   因为对于 y ≠ x, E[s_i(x)·s_i(y)] = 0  (独立符号)
 *   所以交叉项期望为 0, 只有 f_x 项贡献。
 *
 * 误差界:
 *   |f̂_x - f_x| ≤ ε·||f_{-x}||₂  with prob ≥ 1 − δ
 *   空间: O(1/ε² · log(1/δ))
 *
 * Count-Min vs Count-Sketch:
 *   - Count-Min: 单向误差 (高估), 对偏斜分布更好
 *   - Count-Sketch: 无偏, 对 L₂ 范数误差更好
 *
 * 应用:
 *   - 寻找 Heavy Hitters (与 Misra-Gries 互补)
 *   - 流式内积估计
 *   - 压缩感知 (Compressed Sensing)
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cmath>
using namespace std;

// ==================== Count-Sketch 实现 ====================
class CountSketch {
    int rows, cols;  // d 行 × m 列
    vector<vector<int>> table;
    vector<vector<int>> hashA, hashB;  // h(x) = (a·x + b) mod P mod cols
    vector<vector<int>> signs;          // s(x) ∈ {+1, -1}
    int prime;

public:
    CountSketch(double epsilon, double delta) {
        // 保证: |f̂-f| ≤ ε·||f_{-x}||₂ w.p. 1-δ
        rows = max(1, (int)ceil(log(1.0 / delta)));
        cols = max(1, (int)ceil(3.0 / (epsilon * epsilon)));
        prime = 1000000007;

        table.assign(rows, vector<int>(cols, 0));
        hashA.assign(rows, vector<int>(2));
        hashB.assign(rows, vector<int>(2));
        signs.assign(rows, vector<int>(2));

        mt19937 rng(42);
        uniform_int_distribution<int> aDist(1, prime - 1);
        uniform_int_distribution<int> bDist(0, prime - 1);

        for (int i = 0; i < rows; i++) {
            hashA[i][0] = aDist(rng);
            hashB[i][0] = bDist(rng);
            hashA[i][1] = aDist(rng);  // 独立哈希用于符号
            hashB[i][1] = bDist(rng);
        }
    }

    int hash(int i, int x, int which) const {
        long long h = ((long long)hashA[i][which] * x + hashB[i][which]) % prime;
        return (int)(h % cols);
    }

    int sign(int i, int x) const {
        // 2-universal hash → sign ∈ {+1, -1}
        long long h = ((long long)hashA[i][1] * x + hashB[i][1]) % prime;
        return (h % 2 == 0) ? 1 : -1;
    }

    void add(int x, int delta = 1) {
        for (int i = 0; i < rows; i++) {
            int bucket = hash(i, x, 0);
            table[i][bucket] += sign(i, x) * delta;
        }
    }

    int query(int x) const {
        // 取各行估计的中位数
        vector<int> estimates;
        for (int i = 0; i < rows; i++) {
            int bucket = hash(i, x, 0);
            estimates.push_back(sign(i, x) * table[i][bucket]);
        }
        sort(estimates.begin(), estimates.end());
        return estimates[rows / 2];  // median
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Count-Sketch (计数草图)                  " << endl;
    cout << "========================================" << endl << endl;

    // ε=0.2, δ=0.01
    CountSketch cs(0.2, 0.01);

    // 插入数据: 3个"大象" + 很多"老鼠"
    cout << "[数据流]" << endl;
    cout << "  大象: A=1000次, B=500次, C=300次" << endl;
    cout << "  老鼠: D~Z 各 1~5 次" << endl;

    // 用简单字符映射到整数 ID
    auto id = [](char c) { return c - 'A'; };

    for (int i = 0; i < 1000; i++) cs.add(id('A'));
    for (int i = 0; i < 500; i++) cs.add(id('B'));
    for (int i = 0; i < 300; i++) cs.add(id('C'));
    for (int i = 0; i < 5; i++) cs.add(id('D'));
    for (int i = 0; i < 3; i++) cs.add(id('E'));
    for (int i = 0; i < 2; i++) cs.add(id('F'));

    cout << endl << "[Count-Sketch 估计]" << endl;
    cout << "  字母   真实频率   估计(中位数)" << endl;
    for (char c = 'A'; c <= 'F'; c++) {
        int trueFreq = 0;
        if (c == 'A') trueFreq = 1000;
        else if (c == 'B') trueFreq = 500;
        else if (c == 'C') trueFreq = 300;
        else if (c == 'D') trueFreq = 5;
        else if (c == 'E') trueFreq = 3;
        else if (c == 'F') trueFreq = 2;

        cout << "   " << c << "      " << setw(4) << trueFreq
             << "        " << setw(4) << cs.query(id(c)) << endl;
    }

    cout << endl << "[Count-Min vs Count-Sketch]" << endl;
    cout << "  Count-Min:  单向高估, 无 low-side 误差" << endl;
    cout << "  Count-Sketch: 无偏, ± 方差, 中位数降低噪声" << endl;
    cout << "  空间: O(1/ε² · log(1/δ))" << endl;

    return 0;
}
