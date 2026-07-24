/**
 * 蓄水池抽样 (Reservoir Sampling)
 *
 * 问题: 从长度未知的数据流中, 等概率地随机选取 k 个元素。
 * 只能遍历一次数据, 不能用辅助存储保存所有元素。
 *
 * 算法 R (Alan Waterman, Jeffrey Vitter):
 *   1. 将前 k 个元素放入"蓄水池" R[0..k-1]
 *   2. 对于第 i 个元素 (i ≥ k):
 *      以概率 k/i 决定: 用 x_i 替换蓄水池中随机选择的一个元素
 *      以概率 1-k/i: 丢弃 x_i
 *
 * 正确性证明 (归纳法):
 *   在处理了 n 个元素后, 每个元素在蓄水池中的概率 = k/n
 *
 *   归纳基础 n=k: 所有 k 个元素都在蓄水池中, 概率 = 1 = k/k ✓
 *
 *   归纳步骤 n→n+1:
 *     - 第 n+1 个元素被选入: k/(n+1) ✓
 *     - 对于已在蓄水池中的元素 j (j ≤ n):
 *       P(j保留) = P(j在阶段n存活) × [1 - P(被第n+1个替换)]
 *                = (k/n) × [1 - (k/(n+1)) × (1/k)]
 *                = (k/n) × [1 - 1/(n+1)]
 *                = (k/n) × [n/(n+1)]
 *                = k/(n+1) ✓
 *
 * 变体:
 *   - 加权蓄水池抽样 (Weighted Reservoir Sampling):
 *     每个元素 x_i 有权重 w_i, 以 w_i/Σw 的概率保留
 *     A-Res (Algorithm A-Res) / A-Chao
 *   - 分布式蓄水池抽样: 多机合并
 *
 * 应用:
 *   - 数据库: 从表中随机抽样 (TABLESAMPLE)
 *   - 在线流式: 随机样本统计 (中位数估计等)
 *   - 大数据: MPP (大规模并行处理) 中的抽样算子
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iomanip>
using namespace std;

// ==================== 算法 R: 标准蓄水池抽样 ====================
vector<int> reservoirSample(const vector<int>& stream, int k, mt19937& rng) {
    vector<int> reservoir(k);
    int n = stream.size();

    // 初始化: 前 k 个放入蓄水池
    for (int i = 0; i < k && i < n; i++)
        reservoir[i] = stream[i];

    // 处理后续元素
    for (int i = k; i < n; i++) {
        uniform_int_distribution<int> dist(0, i);  // [0, i]
        int j = dist(rng);
        if (j < k) {
            reservoir[j] = stream[i];
        }
    }
    return reservoir;
}

// ==================== 加权蓄水池抽样 (A-Res) ====================
// 每个元素有独立权重, 以与权重成比例的概率保留
struct WeightedItem {
    int value;
    double weight;
};

vector<int> weightedReservoirSample(const vector<WeightedItem>& stream,
                                     int k, mt19937& rng) {
    // 使用 A-Res 算法: 对每个元素生成 key = U^(1/w), 保留 key 最大的 k 个
    // 其中 U ~ Uniform(0,1)
    uniform_real_distribution<double> uni(0.0, 1.0);

    // 用 min-heap 保留 key 最大的 k 个
    // 简化: 全量计算 (实际应用中可以流式维护)
    struct Candidate {
        int value;
        double key;
        bool operator<(const Candidate& o) const { return key < o.key; }
    };

    vector<Candidate> heap;
    for (auto& item : stream) {
        double u = uni(rng);
        // 避免 log(0)
        if (u < 1e-12) u = 1e-12;
        double key = pow(u, 1.0 / item.weight);

        if ((int)heap.size() < k) {
            heap.push_back({item.value, key});
            push_heap(heap.begin(), heap.end());  // max-heap
        } else if (key < heap[0].key) {
            pop_heap(heap.begin(), heap.end());
            heap.pop_back();
            heap.push_back({item.value, key});
            push_heap(heap.begin(), heap.end());
        }
    }

    vector<int> result;
    for (auto& c : heap) result.push_back(c.value);
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  蓄水池抽样 (Reservoir Sampling)          " << endl;
    cout << "========================================" << endl << endl;

    mt19937 rng(42);

    // 1. 标准蓄水池抽样: 1..1000, 抽样 k=5
    vector<int> stream(1000);
    for (int i = 0; i < 1000; i++) stream[i] = i + 1;

    auto sample = reservoirSample(stream, 5, rng);
    cout << "[标准蓄水池抽样] k=5, N=1000" << endl;
    cout << "  样本: ";
    for (int x : sample) cout << x << " ";
    cout << endl;

    // 2. 验证均匀性 (多次实验)
    const int TRIALS = 50000;
    vector<int> freq(20, 0);  // 前20个被选中的次数
    for (int t = 0; t < TRIALS; t++) {
        auto s = reservoirSample(stream, 5, rng);
        for (int x : s)
            if (x <= 20) freq[x - 1]++;
    }
    double expected = (double)TRIALS * 5 / 1000;  // 每个元素期望次数
    cout << endl << "[均匀性验证] " << TRIALS << " 次实验, 期望每元素 "
         << expected << " 次" << endl;
    cout << "  前20个元素被选次数: ";
    for (int i = 0; i < 20; i++)
        cout << freq[i] << " ";
    cout << endl;

    // 3. 加权抽样
    vector<WeightedItem> wstream = {
        {1, 5.0}, {2, 1.0}, {3, 5.0}, {4, 1.0},
        {5, 10.0}, {6, 1.0}, {7, 5.0}, {8, 1.0}
    };
    auto ws = weightedReservoirSample(wstream, 3, rng);
    cout << endl << "[加权蓄水池抽样] k=3, 高权重元素优先" << endl;
    cout << "  权重: ";
    for (auto& item : wstream) cout << item.value << ":" << item.weight << " ";
    cout << endl;
    cout << "  样本: ";
    for (int x : ws) cout << x << " ";
    cout << endl;

    cout << endl << "[蓄水池抽样核心]" << endl;
    cout << "  时间复杂度 O(n), 空间复杂度 O(k)" << endl;
    cout << "  概率 k/i 的替换保证均匀随机" << endl;
    cout << "  扩展: 加权/滑动窗口/分布式" << endl;

    return 0;
}
