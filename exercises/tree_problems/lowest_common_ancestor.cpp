/**
 * 最近公共祖先 (LCA — Lowest Common Ancestor)
 *
 * 经典题, 2023年826真题
 *
 * 给定一棵二叉树，求两个节点的最近公共祖先。
 *
 * 递归法:
 *   从根开始后序遍历:
 *   - 若当前节点等于 p 或 q，返回当前节点
 *   - 递归查找左右子树
 *   - 若 p、q 分别位于左右子树，当前节点即为 LCA
 *   - 否则返回非空的那侧
 *
 * 时间复杂度 O(n)，空间 O(h)
 */

#include <iostream>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

// ==================== 递归法 (后序遍历) ====================
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    // 递归基
    if (!root || root == p || root == q)
        return root;

    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);

    // p 和 q 分别位于左右子树 → root 就是 LCA
    if (left && right) return root;

    // 都在一侧 → 返回非空的那个
    return left ? left : right;
}

// ==================== 带节点存在的 LCA ====================
// 更严谨的实现：先确认两个节点都在树中
bool findNode(TreeNode* root, TreeNode* target) {
    if (!root) return false;
    if (root == target) return true;
    return findNode(root->left, target) || findNode(root->right, target);
}

TreeNode* LCA_safe(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!findNode(root, p) || !findNode(root, q))
        return nullptr;  // 节点不在树中
    return lowestCommonAncestor(root, p, q);
}

// ==================== 构建测试树 ====================
//         3
//       /   \
//      5     1
//     / \   / \
//    6   2 0   8
//       / \
//      7   4
TreeNode* buildTree() {
    TreeNode* root = new TreeNode(3);
    root->left = new TreeNode(5);
    root->right = new TreeNode(1);
    root->left->left = new TreeNode(6);
    root->left->right = new TreeNode(2);
    root->left->right->left = new TreeNode(7);
    root->left->right->right = new TreeNode(4);
    root->right->left = new TreeNode(0);
    root->right->right = new TreeNode(8);
    return root;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  最近公共祖先 (LCA)                     " << endl;
    cout << "========================================" << endl << endl;

    TreeNode* root = buildTree();
    cout << "[测试树]" << endl;
    cout << "         3" << endl;
    cout << "       /   \\" << endl;
    cout << "      5     1" << endl;
    cout << "     / \\   / \\" << endl;
    cout << "    6   2 0   8" << endl;
    cout << "       / \\" << endl;
    cout << "      7   4" << endl << endl;

    // 测试
    vector<tuple<int, int, int>> tests = {
        {5, 1, 3},   // LCA(5,1) = 3
        {5, 4, 5},   // LCA(5,4) = 5
        {6, 4, 5},   // LCA(6,4) = 5
        {7, 8, 3},   // LCA(7,8) = 3
        {6, 6, 6},   // LCA(6,6) = 6
    };

    cout << "[LCA 测试]" << endl;
    cout << "  p | q | LCA | 期望" << endl;
    cout << "  " << string(25, '-') << endl;

    for (auto& [vp, vq, expected] : tests) {
        // 通过值找节点（简化实现）
        function<TreeNode*(TreeNode*, int)> findVal = [&](TreeNode* r, int val) -> TreeNode* {
            if (!r) return nullptr;
            if (r->val == val) return r;
            TreeNode* l = findVal(r->left, val);
            return l ? l : findVal(r->right, val);
        };

        TreeNode* p = findVal(root, vp);
        TreeNode* q = findVal(root, vq);
        TreeNode* lca = lowestCommonAncestor(root, p, q);

        cout << "  " << vp << " | " << vq << " | "
             << (lca ? to_string(lca->val) : "?")
             << "   | " << expected
             << (lca && lca->val == expected ? " ✓" : " ✗") << endl;
    }

    cout << endl << "[2023年826真题]" << endl;
    cout << "  已知前序和中序遍历序列，求两个节点的LCA" << endl;
    cout << "  解法: 无需完全重构树 — 利用遍历序列性质直接定位" << endl;

    return 0;
}
