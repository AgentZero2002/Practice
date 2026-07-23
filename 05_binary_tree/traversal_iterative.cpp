/**
 * 二叉树非递归遍历
 *
 * 对应教材 5.4 节：遍历的迭代实现
 *
 * 使用显式栈模拟递归，避免递归带来的栈溢出风险。
 *
 * 三种非递归实现:
 *   先序: 沿左链下行时访问，右子树入栈
 *   中序: 沿左链下行到底后回退访问（最经典）
 *   后序: 最复杂，需记录上次访问节点来判断是否访问过右子树
 *
 * 邓俊辉教材对先序遍历给出了两个精致的迭代版本，
 * 本文件展示其第二版（更优雅的版本）。
 */

#include <iostream>
#include <stack>
#include <vector>
using namespace std;

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr) {}
};

// ==================== 先序遍历（非递归） ====================
// 算法：沿最左侧通路自顶而下访问节点，并将右子树入栈
// 复杂度：O(n) 时间, O(h) 空间
vector<int> preorder_iterative(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> S;

    if (root) S.push(root);
    while (!S.empty()) {
        TreeNode* node = S.top(); S.pop();
        result.push_back(node->val);

        // 注意：先压右后压左（栈是 LIFO）
        if (node->right) S.push(node->right);
        if (node->left) S.push(node->left);
    }

    return result;
}

// ==================== 先序遍历（非递归，邓俊辉版） ====================
// 更优雅的版本：每次只压右子树，沿左链深入
vector<int> preorder_iterative_v2(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> S;

    TreeNode* x = root;
    while (true) {
        // 沿左链深入
        while (x) {
            result.push_back(x->val);  // 访问当前节点
            if (x->right) S.push(x->right);  // 右子树暂存
            x = x->left;
        }
        if (S.empty()) break;
        x = S.top(); S.pop();  // 处理下一颗右子树
    }

    return result;
}

// ==================== 中序遍历（非递归） ====================
// 算法：沿左链深入到底，回退时访问，然后转入右子树
vector<int> inorder_iterative(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> S;

    TreeNode* x = root;
    while (true) {
        // 沿左链深入到底
        while (x) {
            S.push(x);
            x = x->left;
        }
        if (S.empty()) break;

        x = S.top(); S.pop();
        result.push_back(x->val);  // 访问当前节点

        x = x->right;  // 转入右子树
    }

    return result;
}

// ==================== 后序遍历（非递归） ====================
// 最复杂的非递归遍历
// 策略：用 lastVisited 记录上一次访问的节点，
//       判断当前节点的右子树是否已经访问过
vector<int> postorder_iterative(TreeNode* root) {
    vector<int> result;
    stack<TreeNode*> S;
    TreeNode* lastVisited = nullptr;

    TreeNode* x = root;
    while (x || !S.empty()) {
        // 沿左链深入
        while (x) {
            S.push(x);
            x = x->left;
        }

        TreeNode* peek = S.top();

        // 若右子树存在且未被访问过，转入右子树
        if (peek->right && peek->right != lastVisited) {
            x = peek->right;
        } else {
            // 否则访问当前节点
            result.push_back(peek->val);
            lastVisited = peek;
            S.pop();
        }
    }

    return result;
}

// ==================== 辅助函数 ====================
void printVector(const string& label, const vector<int>& v) {
    cout << "  " << label << ": ";
    for (size_t i = 0; i < v.size(); i++) {
        if (i > 0) cout << " ";
        cout << v[i];
    }
    cout << endl;
}

// ==================== 构建测试树 ====================
//
//         1
//       /   \
//      2     3
//     / \   / \
//    4   5 6   7
//
TreeNode* buildTree() {
    TreeNode* root = new TreeNode(1);
    root->left = new TreeNode(2);
    root->right = new TreeNode(3);
    root->left->left = new TreeNode(4);
    root->left->right = new TreeNode(5);
    root->right->left = new TreeNode(6);
    root->right->right = new TreeNode(7);
    return root;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二叉树非递归遍历                       " << endl;
    cout << "========================================" << endl << endl;

    TreeNode* root = buildTree();

    cout << "测试二叉树:" << endl;
    cout << "         1" << endl;
    cout << "       /   \\" << endl;
    cout << "      2     3" << endl;
    cout << "     / \\   / \\" << endl;
    cout << "    4   5 6   7" << endl << endl;

    cout << "[非递归遍历结果]" << endl;

    auto pre1 = preorder_iterative(root);
    printVector("先序 v1  ", pre1);

    auto pre2 = preorder_iterative_v2(root);
    printVector("先序 v2  ", pre2);

    auto in = inorder_iterative(root);
    printVector("中序     ", in);

    auto post = postorder_iterative(root);
    printVector("后序     ", post);

    // 验证一致性
    cout << endl << "[对比]" << endl;
    cout << "  先序: 根→左→右  → 1 2 4 5 3 6 7" << endl;
    cout << "  中序: 左→根→右  → 4 2 5 1 6 3 7" << endl;
    cout << "  后序: 左→右→根  → 4 5 2 6 7 3 1" << endl;
    cout << "  (请对照上方输出验证)" << endl;
    cout << endl;
    cout << "[复杂度]" << endl;
    cout << "  时间: O(n), 每个节点恰好入栈一次" << endl;
    cout << "  空间: O(h), h 为树的高度" << endl;

    // 清理
    delete root->left->left; delete root->left->right;
    delete root->right->left; delete root->right->right;
    delete root->left; delete root->right; delete root;

    return 0;
}
