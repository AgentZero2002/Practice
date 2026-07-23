/**
 * 数据流中位数 (Find Median from Data Stream)
 *
 * 经典题: LeetCode 295
 *
 * 问题: 维护一个数据流，随时返回当前所有数据的中位数。
 *
 * 解法: 双堆 (Dual Heap)
 *   - 最大堆 (maxHeap): 存储较小的一半数据
 *   - 最小堆 (minHeap): 存储较大的一半数据
 *   - 平衡: |maxHeap| - |minHeap| ≤ 1
 *   - 中位数 = 若不等 → maxHeap.top()
 *             若相等 → (maxHeap.top() + minHeap.top()) / 2.0
 *
 * 插入复杂度 O(log n)，查询中位数 O(1)
 */

#include <iostream>
#include <queue>
#include <vector>
using namespace std;

class MedianFinder {
private:
    // 最大堆: 存储较小的一半 (使用 STL 默认大顶堆)
    priority_queue<int> maxHeap;
    // 最小堆: 存储较大的一半
    priority_queue<int, vector<int>, greater<int>> minHeap;

public:
    void addNum(int num) {
        // 先放入最大堆
        maxHeap.push(num);

        // 平衡: 将最大堆的最大值移到最小堆
        minHeap.push(maxHeap.top());
        maxHeap.pop();

        // 保持最大堆 ≥ 最小堆的大小
        if (maxHeap.size() < minHeap.size()) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
        }
    }

    double findMedian() {
        if (maxHeap.size() > minHeap.size())
            return maxHeap.top();
        else
            return (maxHeap.top() + minHeap.top()) / 2.0;
    }

    void printState() {
        // 复制堆来打印（STL priority_queue 不可直接遍历）
        auto maxCopy = maxHeap;
        auto minCopy = minHeap;

        cout << "  maxHeap (较小半): ";
        while (!maxCopy.empty()) { cout << maxCopy.top() << " "; maxCopy.pop(); }
        cout << endl;
        cout << "  minHeap (较大半): ";
        while (!minCopy.empty()) { cout << minCopy.top() << " "; minCopy.pop(); }
        cout << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  数据流中位数 (Dual Heap)               " << endl;
    cout << "========================================" << endl << endl;

    MedianFinder mf;

    cout << "[逐步插入数据流]" << endl << endl;

    vector<int> stream = {5, 15, 1, 3, 8, 7, 9};

    for (int x : stream) {
        mf.addNum(x);
        cout << "  插入 " << x << " → 中位数 = " << mf.findMedian() << endl;
        mf.printState();
        cout << endl;
    }

    cout << "[复杂度]" << endl;
    cout << "  插入: O(log n) — 堆的 push/pop" << endl;
    cout << "  查询: O(1)     — 直接读堆顶" << endl;
    cout << "  空间: O(n)" << endl;
    cout << endl;
    cout << "[推广: 滑动窗口中位数] O(n log k) 配合延迟删除" << endl;

    return 0;
}
