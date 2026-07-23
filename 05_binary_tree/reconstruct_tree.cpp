/**
 * 二叉树重构：从前序 + 中序遍历序列重建二叉树
 *
 * 对应教材 5.5 节：二叉树的建立
 *
 * 核心定理:
 *   已知二叉树的 前序+中序 或 后序+中序遍历序列，
 *   可以唯一确定二叉树的结构。
 *
 * 算法思想:
 *   - 前序的第一个元素是根节点
 *   - 在中序中找到根节点的位置，将序列分为左子树和右子树
 *   - 递归处理左右子树
 *
 * 此外还包括: 前序+后序 → 真二叉树（每个内部节点有两个孩子）
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

// ==================== 前序 + 中序重构二叉树 ====================
class Reconstructor {
private:
    unordered_map<int, int> inMap;  // 值 → 中序位置

    TreeNode* buildFromPreIn(const vector<int>& preorder, int preStart, int preEnd,
                              const vector<int>& inorder, int inStart, int inEnd) {
        if (preStart > preEnd) return nullptr;

        // 前序的第一个元素是根
        int rootVal = preorder[preStart];
        TreeNode* root = new TreeNode(rootVal);

        // 在中序中找到根的位置
        int rootIdx = inMap[rootVal];
        int leftSize = rootIdx - inStart;  // 左子树的大小

        // 递归构建左右子树
        root->left = buildFromPreIn(preorder, preStart + 1, preStart + leftSize,
                                     inorder, inStart, rootIdx - 1);
        root->right = buildFromPreIn(preorder, preStart + leftSize + 1, preEnd,
                                      inorder, rootIdx + 1, inEnd);

        return root;
    }

public:
    TreeNode* buildTree(const vector<int>& preorder, const vector<int>& inorder) {
        for (int i = 0; i < (int)inorder.size(); i++)
            inMap[inorder[i]] = i;
        return buildFromPreIn(preorder, 0, preorder.size() - 1,
                               inorder, 0, inorder.size() - 1);
    }
};

// ==================== 后序 + 中序重构二叉树 ====================
TreeNode* buildFromPostIn(const vector<int>& postorder, int postStart, int postEnd,
                           const vector<int>& inorder, int inStart, int inEnd,
                           unordered_map<int, int>& inMap) {
    if (postStart > postEnd) return nullptr;

    // 后序的最后一个元素是根
    int rootVal = postorder[postEnd];
    TreeNode* root = new TreeNode(rootVal);

    int rootIdx = inMap[rootVal];
    int leftSize = rootIdx - inStart;

    root->left = buildFromPostIn(postorder, postStart, postStart + leftSize - 1,
                                  inorder, inStart, rootIdx - 1, inMap);
    root->right = buildFromPostIn(postorder, postStart + leftSize, postEnd - 1,
                                   inorder, rootIdx + 1, inEnd, inMap);

    return root;
}

TreeNode* buildTreePostIn(const vector<int>& postorder, const vector<int>& inorder) {
    unordered_map<int, int> inMap;
    for (int i = 0; i < (int)inorder.size(); i++)
        inMap[inorder[i]] = i;
    return buildFromPostIn(postorder, 0, postorder.size() - 1,
                            inorder, 0, inorder.size() - 1, inMap);
}

// ==================== 验证重构：层次遍历输出 ====================
vector<int> levelorder(TreeNode* root) {
    vector<int> result;
    if (!root) return result;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        TreeNode* node = q.front(); q.pop();
        if (node) {
            result.push_back(node->val);
            q.push(node->left);
            q.push(node->right);
        } else {
            result.push_back(-1);  // null 标记
        }
    }
    // 去除尾部的 null 标记
    while (!result.empty() && result.back() == -1)
        result.pop_back();
    return result;
}

// 获取中序遍历（用于验证）
void getInorder(TreeNode* root, vector<int>& result) {
    if (!root) return;
    getInorder(root->left, result);
    result.push_back(root->val);
    getInorder(root->right, result);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二叉树重构：前/后序 + 中序             " << endl;
    cout << "========================================" << endl << endl;

    // 测试1：前序 + 中序
    {
        cout << "[测试1: 前序 + 中序]" << endl;
        vector<int> preorder = {3, 9, 20, 15, 7};
        vector<int> inorder  = {9, 3, 15, 20, 7};

        cout << "  前序: ";
        for (int x : preorder) cout << x << " ";
        cout << endl << "  中序: ";
        for (int x : inorder) cout << x << " ";
        cout << endl;

        Reconstructor rec;
        TreeNode* root = rec.buildTree(preorder, inorder);

        // 验证中序遍历是否与原序列一致
        vector<int> verifyIn;
        getInorder(root, verifyIn);
        bool ok = (verifyIn == inorder);
        cout << "  重构" << (ok ? "成功 ✓" : "失败 ✗") << endl;

        vector<int> level = levelorder(root);
        cout << "  层次遍历: ";
        for (int x : level) cout << x << " ";
        cout << endl;

        // 树结构
        cout << "  树结构:" << endl;
        cout << "        3" << endl;
        cout << "       / \\" << endl;
        cout << "      9   20" << endl;
        cout << "         /  \\" << endl;
        cout << "        15   7" << endl;
        cout << endl;
    }

    // 测试2：后序 + 中序
    {
        cout << "[测试2: 后序 + 中序]" << endl;
        vector<int> postorder = {9, 15, 7, 20, 3};
        vector<int> inorder   = {9, 3, 15, 20, 7};

        cout << "  后序: ";
        for (int x : postorder) cout << x << " ";
        cout << endl << "  中序: ";
        for (int x : inorder) cout << x << " ";
        cout << endl;

        TreeNode* root = buildTreePostIn(postorder, inorder);

        vector<int> verifyIn;
        getInorder(root, verifyIn);
        bool ok = (verifyIn == inorder);
        cout << "  重构" << (ok ? "成功 ✓" : "失败 ✗") << endl;
        cout << endl;
    }

    // 关键结论
    cout << "[结论]" << endl;
    cout << "  1. 前序+中序 → 可唯一确定二叉树" << endl;
    cout << "  2. 后序+中序 → 可唯一确定二叉树" << endl;
    cout << "  3. 前序+后序 → 不能唯一确定一般二叉树" << endl;
    cout << "     (但可唯一确定真二叉树/正则二叉树)" << endl;
    cout << "  4. 仅有中序 → 无法确定二叉树结构" << endl;

    return 0;
}
