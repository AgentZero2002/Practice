/**
 * 堆的应用：Top-K 问题与多路归并
 *
 * 对应教材 10.5 节：优先级队列的应用
 *
 * 1. Top-K 问题: 从海量数据中找出最大（或最小）的 K 个元素
 *    方法: 维护一个大小为 K 的最小堆
 *    复杂度: O(n log K) 时间, O(K) 空间
 *
 * 2. 多路归并: 合并 K 个有序序列
 *    方法: 使用大小为 K 的最小堆
 *    复杂度: O(N log K)，N 为总元素数
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

// ==================== Top-K (最小堆) ====================
// 找出最大的 K 个元素
vector<int> topK(const vector<int>& data, int k) {
    // 维护一个大小为 k 的最小堆
    priority_queue<int, vector<int>, greater<int>> minHeap;

    for (int x : data) {
        if ((int)minHeap.size() < k) {
            minHeap.push(x);
        } else if (x > minHeap.top()) {
            minHeap.pop();      // 移除当前最小的
            minHeap.push(x);    // 加入更大的
        }
    }

    vector<int> result;
    while (!minHeap.empty()) {
        result.push_back(minHeap.top());
        minHeap.pop();
    }
    // topK 结果通常不需要排序，但这里排一下方便查看
    sort(result.rbegin(), result.rend());

    return result;
}

// ==================== 多路归并 ====================
// 合并 K 个有序 vector，输出一个有序 vector
vector<int> kWayMerge(const vector<vector<int>>& sequences) {
    // 最小堆: {值, 来自哪个序列, 在该序列中的索引}
    typedef tuple<int, int, int> HeapItem;

    auto cmp = [](const HeapItem& a, const HeapItem& b) {
        return get<0>(a) > get<0>(b);  // 小顶堆
    };

    priority_queue<HeapItem, vector<HeapItem>, decltype(cmp)> pq(cmp);

    // 每个序列的第一个元素入堆
    for (int i = 0; i < (int)sequences.size(); i++) {
        if (!sequences[i].empty()) {
            pq.push({sequences[i][0], i, 0});
        }
    }

    vector<int> result;
    while (!pq.empty()) {
        auto [val, seqIdx, elemIdx] = pq.top(); pq.pop();
        result.push_back(val);

        // 该序列的下一个元素入堆
        if (elemIdx + 1 < (int)sequences[seqIdx].size()) {
            pq.push({sequences[seqIdx][elemIdx + 1], seqIdx, elemIdx + 1});
        }
    }

    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  堆的应用: Top-K 与 多路归并            " << endl;
    cout << "========================================" << endl << endl;

    // 1. Top-K
    cout << "[Top-K] 找出最大的 3 个元素" << endl;
    vector<int> data = {7, 3, 8, 1, 6, 9, 2, 10, 4, 5, 7, 3};
    cout << "  数据: ";
    for (int x : data) cout << x << " ";

    vector<int> top3 = topK(data, 3);
    cout << endl << "  最大的 3 个: ";
    for (int x : top3) cout << x << " ";
    cout << endl;
    cout << "  复杂度: O(n log k), k=3 → 极低开销" << endl << endl;

    // 2. 多路归并
    cout << "[多路归并] 合并 3 个有序序列" << endl;
    vector<vector<int>> sequences = {
        {2, 5, 8, 11, 14},
        {1, 3, 7, 10, 13},
        {4, 6, 9, 12, 15}
    };

    for (int i = 0; i < (int)sequences.size(); i++) {
        cout << "  序列 " << i << ": ";
        for (int x : sequences[i]) cout << x << " ";
        cout << endl;
    }

    vector<int> merged = kWayMerge(sequences);
    cout << "  合并后: ";
    for (int x : merged) cout << x << " ";
    cout << endl;
    cout << "  复杂度: O(N log K), N=" << merged.size() << ", K=" << sequences.size() << endl;

    cout << endl << "[扩展应用]" << endl;
    cout << "  1. 外部排序: 多路归并 + 内部排序" << endl;
    cout << "  2. 流数据处理: 维护滑动窗口 Top-K" << endl;
    cout << "  3. Dijkstra 算法: 优先队列优化" << endl;
    cout << "  4. Huffman 编码: 最小堆构造树" << endl;
    cout << "  5. 合并 K 个有序链表 (LeetCode 23)" << endl;

    return 0;
}
