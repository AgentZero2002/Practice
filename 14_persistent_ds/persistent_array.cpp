/**
 * 可持久化数组 (Persistent Array) — 函数式数据结构基础
 *
 * 可持久化数组是所有可持久化数据结构的基础。
 * 它支持在 O(log n) 时间内读取任意历史版本的任意位置,
 * 并在 O(log n) 时间内创建修改后的新版本。
 *
 * 实现: 用可持久化线段树 (Path Copying)
 *   每个版本 = 一棵线段树的根
 *   修改一个位置时, 沿路径复制 O(log n) 个节点
 *   未修改的子树与旧版本共享
 *
 * 应用:
 *   - 函数式编程的不可变数组 (Clojure, Scala Vector)
 *   - 可持久化并查集的基础
 *   - 可持久化平衡树的节点存储
 *
 * 空间复杂度: O(n + q log n), n 为数组大小, q 为修改次数
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Node {
    int val;
    int left, right;  // 子节点索引 (-1 表示空)
    Node(int v = 0) : val(v), left(-1), right(-1) {}
};

class PersistentArray {
private:
    vector<Node> nodes;
    vector<int> roots;  // 每个版本的根节点

    int build(int l, int r, const vector<int>& arr) {
        int idx = nodes.size();
        nodes.push_back(Node());
        if (l == r) {
            nodes[idx].val = arr[l];
        } else {
            int mid = (l + r) / 2;
            nodes[idx].left = build(l, mid, arr);
            nodes[idx].right = build(mid + 1, r, arr);
        }
        return idx;
    }

    int update(int oldIdx, int l, int r, int pos, int val) {
        int idx = nodes.size();
        nodes.push_back(nodes[oldIdx]);  // 复制
        if (l == r) {
            nodes[idx].val = val;
        } else {
            int mid = (l + r) / 2;
            if (pos <= mid)
                nodes[idx].left = update(nodes[oldIdx].left, l, mid, pos, val);
            else
                nodes[idx].right = update(nodes[oldIdx].right, mid + 1, r, pos, val);
        }
        return idx;
    }

    int query(int idx, int l, int r, int pos) const {
        if (l == r) return nodes[idx].val;
        int mid = (l + r) / 2;
        if (pos <= mid)
            return query(nodes[idx].left, l, mid, pos);
        else
            return query(nodes[idx].right, mid + 1, r, pos);
    }

    int n;

public:
    PersistentArray(const vector<int>& arr) : n(arr.size()) {
        roots.push_back(build(0, n - 1, arr));
    }

    // 在版本 ver 上修改 pos 为 val, 返回新版本号
    int set(int ver, int pos, int val) {
        int newRoot = update(roots[ver], 0, n - 1, pos, val);
        roots.push_back(newRoot);
        return roots.size() - 1;
    }

    // 读取版本 ver 的 pos 位置
    int get(int ver, int pos) const {
        return query(roots[ver], 0, n - 1, pos);
    }

    int numVersions() const { return roots.size(); }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  可持久化数组 (Persistent Array)          " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {10, 20, 30, 40, 50};
    PersistentArray pa(arr);

    cout << "[版本0] 原始数组: ";
    for (int i = 0; i < 5; i++) cout << pa.get(0, i) << " ";
    cout << endl;

    // 版本 1: 修改 pos=2 为 999
    int v1 = pa.set(0, 2, 999);
    cout << "[版本1] arr[2]=999: ";
    for (int i = 0; i < 5; i++) cout << pa.get(v1, i) << " ";
    cout << endl;

    // 版本 2: 在版本 1 上修改 pos=0 为 777
    int v2 = pa.set(v1, 0, 777);
    cout << "[版本2] arr[0]=777: ";
    for (int i = 0; i < 5; i++) cout << pa.get(v2, i) << " ";
    cout << endl;

    // 版本 0 仍然不变
    cout << "[版本0] 仍然不变:  ";
    for (int i = 0; i < 5; i++) cout << pa.get(0, i) << " ";
    cout << endl;

    cout << endl << "[可持久化数组本质]" << endl;
    cout << "  用 Path Copying 在平衡树上实现不可变数组" << endl;
    cout << "  set() 只复制 O(log n) 个节点, 其余共享" << endl;
    cout << "  是函数式编程中 Vector/List 的底层实现" << endl;

    return 0;
}
