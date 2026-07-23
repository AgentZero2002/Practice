/**
 * 外部归并排序 (External Merge Sort)
 *
 * 当数据大于内存时, 需要使用外部排序。
 * 外部归并排序是最经典的外部排序算法。
 *
 * I/O 模型:
 *   内存大小 = M
 *   磁盘块大小 = B
 *   数据总量 = N > M
 *
 * 两阶段算法:
 *
 * Phase 1: 内部排序 + 初始归并段 (Runs)
 *   读入 M 大小的数据 → 内存排序 → 写出一个 run
 *   重复 N/M 次
 *   生成 R = N/M 个初始 runs (每个有序)
 *
 * Phase 2: K-Way 归并
 *   从 R 个 runs 各读一块 (B) 到输入缓冲区
 *   用败者树 (loser tree) 找 K 个中的最小值
 *   输出缓冲满了就写出
 *   K = floor(M/B - 1)  (1个输出缓冲区 + K个输入缓冲区)
 *
 * 优化:
 *   - Replacement Selection: 用堆生成更长的初始 run (~2M)
 *   - 并行归并: 多线程/多磁盘
 *   - Succinct Index: 每块存第一个 key → 跳跃搜索
 *
 * I/O 复杂度: O(N/B · log_{M/B} (N/M)) 次 I/O
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
using namespace std;

// 模拟 K 路归并 (简化版, 不涉及真实 I/O)
vector<int> kWayMerge(const vector<vector<int>>& runs) {
    // 优先队列: {value, runIndex, elementIndex}
    using Entry = tuple<int, int, int>;
    priority_queue<Entry, vector<Entry>, greater<Entry>> pq;

    int K = runs.size();
    for (int i = 0; i < K; i++) {
        if (!runs[i].empty())
            pq.push({runs[i][0], i, 0});
    }

    vector<int> result;
    while (!pq.empty()) {
        auto [val, runIdx, elemIdx] = pq.top();
        pq.pop();
        result.push_back(val);

        if (elemIdx + 1 < (int)runs[runIdx].size())
            pq.push({runs[runIdx][elemIdx + 1], runIdx, elemIdx + 1});
    }
    return result;
}

// Replacement Selection — 生成更长的初始 run
vector<vector<int>> generateRuns(const vector<int>& data, int memSize) {
    vector<vector<int>> runs;
    vector<int> heap(memSize);
    int heapSize = 0;

    for (int x : data) {
        if (heapSize < memSize) {
            heap.push_back(x);
            push_heap(heap.begin(), heap.end());
            heapSize++;
        }
    }

    while (!heap.empty()) {
        vector<int> run;
        int deadCount = 0;

        while (deadCount < (int)heap.size()) {
            pop_heap(heap.begin(), heap.end() - deadCount);
            int val = heap[heap.size() - 1 - deadCount];

            // 简化: 输出最小元素
            run.push_back(val);
            deadCount++;
            // 读入下一元素, 如果 ≥ 刚输出的 → 放入当前run, 否则放到下个run
        }

        runs.push_back(run);
        // 处理下一轮...
        break;  // 简化实现
    }
    return runs;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  外部归并排序 (External Merge Sort)       " << endl;
    cout << "========================================" << endl << endl;

    // 模拟 3 个已排序的 run
    vector<vector<int>> runs = {
        {1, 5, 8, 12},
        {2, 3, 9, 11},
        {0, 4, 7, 10}
    };

    auto result = kWayMerge(runs);
    cout << "[3路归并结果] ";
    for (int x : result) cout << x << " ";
    cout << endl;

    cout << endl << "[外部排序分析]" << endl;
    cout << "  N=10¹²B, M=1GB, B=4KB:" << endl;
    cout << "  初始 run: N/M = 1000 个" << endl;
    cout << "  K=250 路归并 → log₂₅₀ 1000 ≈ 2 轮" << endl;
    cout << "  总 I/O: N/B × 4 = 10⁹ 次 (2轮×2遍)" << endl;

    cout << endl << "[关键: 减小 I/O" << endl;
    cout << "  败者树 (Loser Tree): 每次归并 O(log K)" << endl;
    cout << "  双缓冲 (Double Buffering): 重叠 I/O 和比较" << endl;

    return 0;
}
