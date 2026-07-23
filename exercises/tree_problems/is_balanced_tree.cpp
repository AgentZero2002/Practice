/**
 * 平衡二叉树判定
 *
 * 经典题: LeetCode 110 — Balanced Binary Tree
 *
 * 判断一棵二叉树是否为高度平衡的：
 *   任意节点的左右子树高度差 ≤ 1
 *
 * 自底向上 O(n):
 *   后序遍历，同时返回高度和是否平衡
 *   避免自顶向下 O(n²) 的重复计算
 */

#include <iostream>
#include <algorithm>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

// ==================== 自底向上法 O(n) ====================
// 返回值: -1 表示不平衡, ≥0 表示子树高度
int checkHeight(TreeNode* root) {
    if (!root) return 0;

    int leftH = checkHeight(root->left);
    if (leftH == -1) return -1;  // 左子树不平衡

    int rightH = checkHeight(root->right);
    if (rightH == -1) return -1;  // 右子树不平衡

    if (abs(leftH - rightH) > 1) return -1;  // 当前节点不平衡

    return max(leftH, rightH) + 1;
}

bool isBalanced(TreeNode* root) {
    return checkHeight(root) != -1;
}

// ==================== 辅助函数 ====================
TreeNode* buildBalancedTree() {
    //     1
    //    / \
    //   2   3
    //  / \
    // 4   5
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    return root;
}

TreeNode* buildUnbalancedTree() {
    //     1
    //    /
    //   2
    //  /
    // 3
    //  \
    //   4
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->left->left = new TreeNode(3);
    root->left->left->right = new TreeNode(4);
    return root;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  平衡二叉树判定                         " << endl;
    cout << "========================================" << endl << endl;

    TreeNode* t1 = buildBalancedTree();
    TreeNode* t2 = buildUnbalancedTree();

    cout << "[判定结果]" << endl;
    cout << "  平衡树: " << (isBalanced(t1) ? "平衡 ✓" : "不平衡 ✗") << endl;
    cout << "  退化树: " << (isBalanced(t2) ? "平衡 ✗" : "不平衡 ✓") << endl;

    // 补充说明
    cout << endl << "[复杂度对比]" << endl;
    cout << "  自顶向下: O(n²) — 每节点都计算其子树高度" << endl;
    cout << "  自底向上: O(n)  — 后序遍历，高度和平衡一并返回" << endl;
    cout << "  核心技巧: 用 -1 同时表示"不平衡"和"高度"，一个返回值办两件事" << endl;

    return 0;
}
