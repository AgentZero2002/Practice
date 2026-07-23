/**
 * Cartesian Tree + LCA → RMQ
 *
 * 笛卡尔树 (Cartesian Tree) 与 RMQ 之间的优美关系:
 *
 * 笛卡尔树:
 *   数组 A 的笛卡尔树是一棵二叉树, 满足:
 *   1. 按位置: 是中序遍历结果 (保持原序)
 *   2. 按值: 满足堆性质 (父结点值 ≤ 子结点值, 对最小堆)
 *
 * 关键关系:
 *   RMQ(l, r) = A[LCA(l, r)]  !
 *   (区间 [l, r] 的最小值 = 笛卡尔树中 l 和 r 的 LCA 的值)
 *
 * 证明:
 *   笛卡尔树的根是全局最小值的位置,
 *   LCA(l, r) 恰好是 [l, r] 范围内的最小值
 *   (因为左右子树分别对应左右子区间)
 *
 * 由此: RMQ 问题可转化为 LCA 问题!
 *   - 构建笛卡尔树 O(n) (单调栈)
 *   - LCA 查询 O(1) (欧拉序 + RMQ→LCA→±1RMQ 自循环)
 *
 * 笛卡尔树是 Treap 的一部分 (Treap = 按值heap + 按键BST)
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

struct CartNode {
    int value;
    int index;  // 在原数组中的位置
    CartNode *left, *right;
    CartNode(int v, int i) : value(v), index(i), left(nullptr), right(nullptr) {}
};

// O(n) 用单调栈构建笛卡尔树
CartNode* buildCartesian(const vector<int>& A) {
    int n = A.size();
    if (n == 0) return nullptr;

    stack<CartNode*> st;
    for (int i = 0; i < n; i++) {
        CartNode* node = new CartNode(A[i], i);
        CartNode* lastPopped = nullptr;

        // 维护单调栈 (递增: 父结点 < 子结点)
        while (!st.empty() && st.top()->value > A[i]) {
            lastPopped = st.top();
            st.pop();
        }

        node->left = lastPopped;  // 最后弹出的作为左儿子 (值比当前大且位置在左)

        if (!st.empty())
            st.top()->right = node;  // 当前作为栈顶的右儿子

        st.push(node);
    }

    // 栈底是根
    CartNode* root = nullptr;
    while (!st.empty()) { root = st.top(); st.pop(); }
    return root;
}

// 在笛卡尔树中查找 LCA (朴素 O(log n), 实际可 O(1))
CartNode* lca(CartNode* root, int i, int j) {
    if (!root) return nullptr;
    if (root->index == i || root->index == j) return root;

    CartNode* left = lca(root->left, i, j);
    CartNode* right = lca(root->right, i, j);

    if (left && right) return root;
    return left ? left : right;
}

// 中序遍历验证
void inorder(CartNode* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->value << "(" << root->index << ") ";
    inorder(root->right);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Cartesian Tree + LCA → RMQ              " << endl;
    cout << "========================================" << endl << endl;

    vector<int> A = {5, 3, 7, 1, 8, 2, 6};
    cout << "[数组] ";
    for (int x : A) cout << x << " ";
    cout << endl;

    CartNode* root = buildCartesian(A);

    cout << "[笛卡尔树中序 (位置顺序)] ";
    inorder(root);
    cout << endl;

    // RMQ via LCA
    cout << endl << "[RMQ via LCA]" << endl;
    auto testRMQ = [&](int l, int r) {
        CartNode* anc = lca(root, l, r);
        int actual = *min_element(A.begin() + l, A.begin() + r + 1);
        cout << "  RMQ(" << l << "," << r << ") = A[" << anc->index
             << "] = " << anc->value
             << " (期望: " << actual << ") ✓" << endl;
    };
    testRMQ(0, 2);  // min(5,3,7)=3
    testRMQ(3, 6);  // min(1,8,2,6)=1
    testRMQ(4, 6);  // min(8,2,6)=2

    cout << endl << "[RMQ 转化链]" << endl;
    cout << "  RMQ → Cartesian Tree → LCA → ±1RMQ → O(1)" << endl;

    return 0;
}
