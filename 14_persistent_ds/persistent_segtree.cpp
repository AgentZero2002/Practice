/**
 * 可持久化线段树 (Persistent Segment Tree / 主席树)
 *
 * 可持久化 = 每次修改保留历史版本，支持查询任意历史版本。
 *
 * 主席树 (Chairman Tree / 函数式线段树):
 *   每次单点修改仅新增 O(log n) 个节点，其余共享。
 *   经典应用: 静态区间第 K 小
 *
 * 构建:
 *   1. 对每个前缀 [1..i] 建一棵线段树 (值域线段树)
 *   2. 第 i 棵树 = 第 i-1 棵树 + 插入 A[i]
 *   3. 查询 [l, r] 第 K 小:
 *      用第 r 棵树 - 第 l-1 棵树 得到区间信息
 *      在线段树上二分查找第 K 小
 *
 * 时间复杂度: O(n log n) 构建, O(log n) 查询
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct SegNode {
    int sum;
    SegNode *left, *right;
    SegNode() : sum(0), left(nullptr), right(nullptr) {}
};

// 在 prev 版本的基础上，在位置 pos 处 +delta，返回新版本根
SegNode* update(SegNode* prev, int l, int r, int pos, int delta) {
    SegNode* node = new SegNode();
    if (prev) *node = *prev;  // 复制旧版本信息
    node->sum += delta;

    if (l == r) return node;

    int mid = (l + r) >> 1;
    if (pos <= mid)
        node->left = update(prev ? prev->left : nullptr, l, mid, pos, delta);
    else
        node->right = update(prev ? prev->right : nullptr, mid + 1, r, pos, delta);

    return node;
}

// 在 (rootR - rootL) 的线段树中查询第 k 小
int query(SegNode* rootL, SegNode* rootR, int l, int r, int k) {
    if (l == r) return l;

    int mid = (l + r) >> 1;
    int leftSum = (rootR && rootR->left ? rootR->left->sum : 0)
                - (rootL && rootL->left ? rootL->left->sum : 0);

    if (k <= leftSum)
        return query(rootL ? rootL->left : nullptr, rootR ? rootR->left : nullptr, l, mid, k);
    else
        return query(rootL ? rootL->right : nullptr, rootR ? rootR->right : nullptr, mid + 1, r, k - leftSum);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  可持久化线段树 (主席树)                 " << endl;
    cout << "  — 静态区间第 K 小                      " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {5, 1, 8, 3, 7, 2, 6, 4};
    int n = arr.size();

    // 离散化
    vector<int> sorted = arr;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());
    int maxVal = sorted.size();

    // 为每个前缀建线段树
    vector<SegNode*> roots(n + 1, nullptr);
    roots[0] = new SegNode();  // 空树

    for (int i = 0; i < n; i++) {
        int rank = lower_bound(sorted.begin(), sorted.end(), arr[i]) - sorted.begin() + 1;
        roots[i + 1] = update(roots[i], 1, maxVal, rank, 1);
    }

    cout << "[数组] 5 1 8 3 7 2 6 4" << endl;
    cout << "[区间第 K 小查询]" << endl;

    struct Query { int l, r, k, expected; };
    vector<Query> queries = {
        {1, 4, 2, 3},   // [1,3,5,8] → 第2小 = 3
        {1, 8, 4, 4},   // 第4小 = 4
        {3, 6, 1, 2},   // [8,3,7,2] → 第1小 = 2
    };

    for (auto& q : queries) {
        int rank = query(roots[q.l - 1], roots[q.r], 1, maxVal, q.k);
        int val = sorted[rank - 1];
        cout << "  第 " << q.k << " 小 [" << q.l << ".." << q.r
             << "] = " << val << (val == q.expected ? " ✓" : " ✗") << endl;
    }

    cout << endl << "[可持久化思想]" << endl;
    cout << "  每次修改: 只新建 O(log n) 节点，其余共享" << endl;
    cout << "  应用: 区间第K大, 历史版本查询可持久化Trie" << endl;

    return 0;
}
