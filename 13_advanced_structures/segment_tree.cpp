/**
 * 线段树 (Segment Tree)
 *
 * 线段树是一种二叉树，每个节点代表一段区间。
 * 支持:
 *   - 区间查询 (Range Query): O(log n)
 *   - 单点修改 (Point Update): O(log n)
 *   - 区间修改 (Range Update): O(log n) 含 lazy 标记
 *
 * 应用场景:
 *   - 区间求和、区间最值
 *   - 逆序对统计
 *   - 矩形面积并 (扫描线)
 *
 * 本实现: 区间求和 + 区间修改 (lazy propagation)
 */

#include <iostream>
#include <vector>
#include <climits>
using namespace std;

class SegmentTree {
private:
    int n;
    vector<long long> tree;  // 线段树数组 (4n)
    vector<long long> lazy;  // 懒标记

    // 建树: 递归构建 [l, r] 区间，节点索引 idx
    void build(const vector<int>& arr, int idx, int l, int r) {
        if (l == r) {
            tree[idx] = arr[l];
            return;
        }
        int mid = (l + r) / 2;
        build(arr, idx * 2, l, mid);
        build(arr, idx * 2 + 1, mid + 1, r);
        tree[idx] = tree[idx * 2] + tree[idx * 2 + 1];
    }

    // 下推 lazy 标记
    void pushDown(int idx, int l, int r) {
        if (lazy[idx] == 0) return;
        int mid = (l + r) / 2;
        int leftLen = mid - l + 1;
        int rightLen = r - mid;

        // 下推给左右孩子
        tree[idx * 2] += lazy[idx] * leftLen;
        tree[idx * 2 + 1] += lazy[idx] * rightLen;
        lazy[idx * 2] += lazy[idx];
        lazy[idx * 2 + 1] += lazy[idx];
        lazy[idx] = 0;
    }

    // 区间更新: [ql, qr] 上的所有元素加上 val
    void update(int idx, int l, int r, int ql, int qr, long long val) {
        if (ql <= l && r <= qr) {  // 完全包含
            tree[idx] += val * (r - l + 1);
            lazy[idx] += val;
            return;
        }
        pushDown(idx, l, r);
        int mid = (l + r) / 2;
        if (ql <= mid) update(idx * 2, l, mid, ql, qr, val);
        if (qr > mid)  update(idx * 2 + 1, mid + 1, r, ql, qr, val);
        tree[idx] = tree[idx * 2] + tree[idx * 2 + 1];
    }

    // 区间查询: [ql, qr] 的和
    long long query(int idx, int l, int r, int ql, int qr) {
        if (ql <= l && r <= qr) return tree[idx];
        pushDown(idx, l, r);
        int mid = (l + r) / 2;
        long long sum = 0;
        if (ql <= mid) sum += query(idx * 2, l, mid, ql, qr);
        if (qr > mid)  sum += query(idx * 2 + 1, mid + 1, r, ql, qr);
        return sum;
    }

public:
    SegmentTree(const vector<int>& arr) : n(arr.size()) {
        tree.resize(4 * n);
        lazy.resize(4 * n, 0);
        build(arr, 1, 0, n - 1);
    }

    void update(int l, int r, long long val) {
        update(1, 0, n - 1, l, r, val);
    }

    long long query(int l, int r) {
        return query(1, 0, n - 1, l, r);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  线段树 (Segment Tree)                  " << endl;
    cout << "  — 区间求和 + 懒标记                     " << endl;
    cout << "========================================" << endl << endl;

    // 初始数组
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    SegmentTree st(arr);

    cout << "[初始数组] ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // 区间查询
    cout << "[区间查询]" << endl;
    cout << "  sum[0..5] = " << st.query(0, 5) << " (期望: 36)" << endl;
    cout << "  sum[1..3] = " << st.query(1, 3) << " (期望: 15)" << endl;

    // 区间修改
    cout << endl << "[区间修改] arr[2..4] += 10" << endl;
    st.update(2, 4, 10);

    cout << "  sum[0..5] = " << st.query(0, 5) << " (期望: 36+30=66)" << endl;
    cout << "  sum[2..4] = " << st.query(2, 4) << " (期望: 5+7+9+30=51)" << endl;

    // 单点修改 (用区间修改实现)
    cout << endl << "[单点修改] arr[0] = 100" << endl;
    long long old = st.query(0, 0);
    st.update(0, 0, 100 - old);
    cout << "  arr[0] = " << st.query(0, 0) << " (期望: 100)" << endl;
    cout << "  sum[0..5] = " << st.query(0, 5) << " (期望: 66-1+100=165)" << endl;

    cout << endl << "[复杂度]" << endl;
    cout << "  建树:       O(n)" << endl;
    cout << "  区间查询:   O(log n)" << endl;
    cout << "  区间修改:   O(log n) (懒标记)" << endl;
    cout << "  空间:       O(4n)" << endl;
    cout << endl;
    cout << "[区别] 树状数组更轻量但只支持前缀和/单点" << endl;
    cout << "       线段树支持任意可结合操作 (和/最值/异或等)" << endl;

    return 0;
}
