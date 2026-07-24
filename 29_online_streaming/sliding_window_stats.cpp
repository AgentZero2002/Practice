/**
 * 滑动窗口统计 (Sliding Window Statistics)
 *
 * 数据流模型的一种重要变体: 我们只关心最近的 N 个元素
 * (滑动窗口), 而不是全部历史。
 *
 * 挑战: 空间复杂度必须 sublinear (o(N)), 不能存储整个窗口!
 *
 * 经典问题:
 *
 * 1. Sliding Window Count (DGIM 算法):
 *    统计最近 N 个比特中 1 的个数。
 *    用"桶"结构: 每个桶记录连续 1 的个数和结束时间戳。
 *    桶的大小是 2 的幂, 每种大小最多保留 2 个桶。
 *    误差: εN (where ε controls bucket granularity)
 *
 * 2. Sliding Window Sum/Max/Min:
 *    - Sum: 同上 DGIM
 *    - Max: 单调队列 (deque) O(1) 均摊, O(W) 最坏空间
 *    - Median: 滑动窗口中位数 (两个堆 + 惰性删除)
 *
 * 3. Sliding Window Heavy Hitters:
 *    Misra-Gries + windowed count decay
 *
 * 本文件展示:
 *   - DGIM (Datar–Gionis–Indyk–Motwani) 滑动窗口计数
 *   - 滑动窗口最大值 (Monotonic Deque)
 *   - 滑动窗口中位数 (Two Heaps with Lazy Deletion)
 */

#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <unordered_map>
using namespace std;

// ==================== 滑动窗口最大值 (Monotonic Deque) ====================
// O(n), 每个元素进出 deque 各一次
vector<int> slidingWindowMax(const vector<int>& arr, int k) {
    vector<int> result;
    deque<int> dq;  // 存下标, 维护递减顺序 (队首=最大值)

    for (int i = 0; i < (int)arr.size(); i++) {
        // 移除过期元素 (窗口外)
        while (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // 维护递减: 移除所有 ≤ arr[i] 的元素
        while (!dq.empty() && arr[dq.back()] <= arr[i])
            dq.pop_back();

        dq.push_back(i);

        // 窗口形成后, 记录答案
        if (i >= k - 1)
            result.push_back(arr[dq.front()]);
    }
    return result;
}

// ==================== 滑动窗口中位数 (Two Heaps + Lazy Deletion) ====================
// 维护窗口 [l, r), 用两个堆 + 延迟删除
class SlidingWindowMedian {
    // max-heap (存较小半) 和 min-heap (存较大半)
    priority_queue<int> small;                          // max-heap
    priority_queue<int, vector<int>, greater<int>> large; // min-heap
    unordered_map<int, int> lazyDeletion;  // delayed removal count
    int smallSize = 0, largeSize = 0;
    deque<int> window;

    // 平衡两个堆
    void balance() {
        if (smallSize > largeSize + 1) {
            large.push(-small.top());
            small.pop();
            smallSize--; largeSize++;
        } else if (largeSize > smallSize) {
            small.push(-large.top());
            large.pop();
            smallSize++; largeSize--;
        }

        // 清理被标记删除的栈顶
        while (!small.empty() && lazyDeletion[-small.top()] > 0) {
            lazyDeletion[-small.top()]--;
            small.pop();
        }
        while (!large.empty() && lazyDeletion[large.top()] > 0) {
            lazyDeletion[large.top()]--;
            large.pop();
        }
    }

public:
    void add(int x) {
        window.push_back(x);
        if (small.empty() || x <= -small.top()) {
            small.push(-x);
            smallSize++;
        } else {
            large.push(x);
            largeSize++;
        }
        balance();
    }

    void remove(int x) {
        lazyDeletion[x]++;
        if (-small.top() == x)
            smallSize--;
        else
            largeSize--;
        window.pop_front();
        balance();
    }

    double median() {
        if (smallSize > largeSize)
            return -small.top();
        else
            return (-small.top() + large.top()) / 2.0;
    }
};

// ==================== DGIM: 滑动窗口1的计数 ====================
// 简化为: 用指数级桶近似计数
class DGIM {
    struct Bucket {
        int size;      // 桶中 1 的个数 (2的幂)
        int timestamp; // 最近一个1的时间戳
    };
    deque<Bucket> buckets;
    int currentTime = 0;
    int windowSize;

public:
    DGIM(int N) : windowSize(N) {}

    void add(int bit) {
        currentTime++;

        // 过期检查
        while (!buckets.empty() &&
               buckets.front().timestamp <= currentTime - windowSize)
            buckets.pop_front();

        if (bit == 0) return;

        // 新桶 (size=1)
        buckets.push_back({1, currentTime});

        // 合并: 每种大小的桶最多保留 2 个
        // 简化: 只检查最新桶
        while (true) {
            int count = 0;
            int sz = buckets.back().size;
            for (auto it = buckets.rbegin();
                 it != buckets.rend() && it->size == sz; ++it)
                count++;
            if (count <= 2) break;

            // 合并最后两个同大小桶
            // (简化实现: 如果是3个同大小则合并第2,3个)
            auto it1 = buckets.end(); --it1;
            auto it2 = it1; --it2;
            if (it1->size == it2->size) {
                it2->size *= 2;  // 合并: size 翻倍
                it2->timestamp = it1->timestamp;  // 保留较新的时间戳
                buckets.erase(it1);
            } else {
                break;
            }
        }
    }

    int count() {
        // 过期检查
        while (!buckets.empty() &&
               buckets.front().timestamp <= currentTime - windowSize)
            buckets.pop_front();

        if (buckets.empty()) return 0;

        int sum = 0;
        for (int i = 0; i < (int)buckets.size(); i++) {
            if (i == 0) sum += buckets[i].size / 2;  // 最老桶只算一半
            else sum += buckets[i].size;
        }
        return sum;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  滑动窗口统计                              " << endl;
    cout << "========================================" << endl << endl;

    // 1. 滑动窗口最大值
    vector<int> arr = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    auto maxs = slidingWindowMax(arr, k);

    cout << "[滑动窗口最大值] k=" << k << endl;
    cout << "  数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    cout << "  各窗口最大值: ";
    for (int x : maxs) cout << x << " ";
    cout << endl;

    // 2. 滑动窗口中位数
    cout << endl << "[滑动窗口中位数] k=3" << endl;
    SlidingWindowMedian swm;
    vector<int> arr2 = {1, 3, -1, -3, 5, 3, 6, 7};
    for (int i = 0; i < 3; i++) swm.add(arr2[i]);
    cout << "  窗口 [1,3,-1]: median=" << swm.median() << endl;
    swm.remove(1); swm.add(-3);
    cout << "  窗口 [3,-1,-3]: median=" << swm.median() << endl;
    swm.remove(3); swm.add(5);
    cout << "  窗口 [-1,-3,5]: median=" << swm.median() << endl;

    // 3. DGIM 近似计数
    cout << endl << "[DGIM 滑动窗口计数] N=10" << endl;
    DGIM dgim(10);
    vector<int> bits = {1,1,0,1,0,0,1,1,1,0, 1,1,1,1,0};
    for (int b : bits) {
        dgim.add(b);
        cout << "  +" << b << " → 窗口1的个数≈" << dgim.count() << endl;
    }

    cout << endl << "[滑动窗口技术]" << endl;
    cout << "  Monotonic Deque: O(n) 滑动窗口最值" << endl;
    cout << "  Two Heaps + Lazy: 滑动窗口中位数" << endl;
    cout << "  DGIM: O(log N) 空间近似滑动窗口计数" << endl;

    return 0;
}
