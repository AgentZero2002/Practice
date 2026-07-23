/**
 * 树状数组 (Fenwick Tree / Binary Indexed Tree)
 *
 * 树状数组利用二进制位的性质，以 O(log n) 实现前缀和操作。
 * 比线段树更轻量 (常数小、代码短)，但功能更受限。
 *
 * 核心操作:
 *   lowbit(x) = x & (-x)    取 x 的最低有效 1 位
 *   add(i, delta): 将位置 i 加上 delta，并更新所有相关节点
 *   sum(i): 查询 [1, i] 的前缀和
 *
 * 经典应用:
 *   1. 统计逆序对 — 从右向左扫描，查询小于当前数的个数
 *   2. 区间修改+单点查询 — 用差分数组
 *   3. 区间修改+区间查询 — 维护两个 BIT
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class FenwickTree {
private:
    vector<long long> tree;
    int n;

public:
    FenwickTree(int n) : n(n), tree(n + 1, 0) {}

    // 在位置 idx 加上 delta (1-indexed)
    void add(int idx, long long delta) {
        while (idx <= n) {
            tree[idx] += delta;
            idx += idx & (-idx);  // 下一个受影响的节点
        }
    }

    // 查询前缀和 [1..idx]
    long long sum(int idx) {
        long long result = 0;
        while (idx > 0) {
            result += tree[idx];
            idx -= idx & (-idx);  // 上一个区间
        }
        return result;
    }

    // 查询区间和 [l, r]
    long long rangeSum(int l, int r) {
        return sum(r) - sum(l - 1);
    }
};

// ==================== 应用1: 求逆序对 ====================
// 离散化后，从右向左扫描：
// 对于每个元素 x，查询比 x 小的元素个数（在它右侧）
long long countInversions(const vector<int>& arr) {
    int n = arr.size();

    // 离散化：将值映射到 [1, n]
    vector<pair<int, int>> sorted;
    for (int i = 0; i < n; i++)
        sorted.push_back({arr[i], i});
    sort(sorted.begin(), sorted.end());

    vector<int> rank(n);
    for (int i = 0; i < n; i++)
        rank[sorted[i].second] = i + 1;  // 排名从1开始

    FenwickTree bit(n);
    long long inv = 0;

    // 从右向左扫描
    for (int i = n - 1; i >= 0; i--) {
        inv += bit.sum(rank[i] - 1);  // 查询比当前数小的
        bit.add(rank[i], 1);           // 将当前数加入 BIT
    }

    return inv;
}

// ==================== 应用2: 区间修改 + 单点查询 (差分 BIT) ====================
class DiffBIT {
private:
    FenwickTree bit;
public:
    DiffBIT(int n) : bit(n) {}

    void rangeAdd(int l, int r, long long delta) {
        bit.add(l, delta);
        bit.add(r + 1, -delta);
    }

    long long pointQuery(int idx) {
        return bit.sum(idx);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  树状数组 (Fenwick Tree / BIT)           " << endl;
    cout << "========================================" << endl << endl;

    // 1. 基本前缀和
    cout << "[前缀和演示]" << endl;
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    FenwickTree bit(arr.size());

    for (int i = 0; i < (int)arr.size(); i++)
        bit.add(i + 1, arr[i]);

    cout << "  构建数组: 1 3 5 7 9 11" << endl;
    cout << "  前缀和[1..4]: " << bit.sum(4) << " (期望: 16)" << endl;
    cout << "  区间和[2..5]: " << bit.rangeSum(2, 5) << " (期望: 24)" << endl;

    // 更新
    bit.add(3, 10);  // arr[3] += 10
    cout << "  arr[3]+=10 后 sum[1..6]: " << bit.sum(6) << " (期望: 46)" << endl << endl;

    // 2. 逆序对
    cout << "[应用: 逆序对统计]" << endl;
    vector<int> arr2 = {7, 5, 3, 1, 2, 4, 6};
    cout << "  数组: ";
    for (int x : arr2) cout << x << " ";
    long long inv = countInversions(arr2);
    cout << endl << "  逆序对数: " << inv << " (期望: 10)" << endl << endl;

    // 3. 差分 BIT
    cout << "[应用: 区间修改+单点查询—差分BIT]" << endl;
    DiffBIT diff(6);
    diff.rangeAdd(1, 3, 5);   // [1,3] 全部 +5
    diff.rangeAdd(2, 5, 3);   // [2,5] 全部 +3
    cout << "  [1,3]+=5, [2,5]+=3 后:" << endl;
    for (int i = 1; i <= 6; i++)
        cout << "    arr[" << i << "] = " << diff.pointQuery(i) << endl;

    cout << endl << "[BIT vs 线段树]" << endl;
    cout << "   BIT:  代码短, 常数小, 仅支持前缀/差分" << endl;
    cout << "   线段树: 更通用, 支持任意区间操作, 但常数大" << endl;
    return 0;
}
