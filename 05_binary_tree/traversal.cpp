/**
 * 二叉树四种遍历：先序/中序/后序 (递归) + 层次遍历 (BFS)
 *
 * 对应教材 5.3-5.4 节：二叉树的遍历
 *
 * 遍历是二叉树最核心的操作，几乎所有的树算法都建立在遍历之上。
 *
 *              A
 *            /   \
 *           B     C
 *          / \   / \
 *         D   E F   G
 *
 *  先序: A B D E C F G  (根→左→右)
 *  中序: D B E A F C G  (左→根→右)
 *  后序: D E B F G C A  (左→右→根)
 *  层次: A B C D E F G  (逐层从左到右)
 */

#include <iostream>
#include <queue>
#include <vector>
#include <string>
using namespace std;

// ==================== 二叉树节点 ====================
struct TreeNode {
    char val;
    TreeNode* left;
    TreeNode* right;
    TreeNode(char v) : val(v), left(nullptr), right(nullptr) {}
};

// ==================== 递归遍历 ====================
void preorder(TreeNode* root, vector<char>& result) {
    if (!root) return;
    result.push_back(root->val);      // 先访问根
    preorder(root->left, result);     // 再遍历左子树
    preorder(root->right, result);    // 最后遍历右子树
}

void inorder(TreeNode* root, vector<char>& result) {
    if (!root) return;
    inorder(root->left, result);      // 先遍历左子树
    result.push_back(root->val);      // 再访问根
    inorder(root->right, result);     // 最后遍历右子树
}

void postorder(TreeNode* root, vector<char>& result) {
    if (!root) return;
    postorder(root->left, result);    // 先遍历左子树
    postorder(root->right, result);   // 再遍历右子树
    result.push_back(root->val);      // 最后访问根
}

// ==================== 层次遍历 (BFS) ====================
vector<char> levelorder(TreeNode* root) {
    vector<char> result;
    if (!root) return result;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        TreeNode* node = q.front(); q.pop();
        result.push_back(node->val);
        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }

    return result;
}

// ==================== 带换行的层次遍历 ====================
void levelorderPretty(TreeNode* root) {
    if (!root) return;

    queue<TreeNode*> q;
    q.push(root);

    int level = 0;
    while (!q.empty()) {
        int sz = q.size();
        cout << "  第" << level << "层: ";
        for (int i = 0; i < sz; i++) {
            TreeNode* node = q.front(); q.pop();
            cout << node->val << " ";
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
        cout << endl;
        level++;
    }
}

// ==================== 打印遍历结果 ====================
void printTraversal(const string& name, const vector<char>& result) {
    cout << "  " << name << ": ";
    for (size_t i = 0; i < result.size(); i++) {
        if (i > 0) cout << " → ";
        cout << result[i];
    }
    cout << endl;
}

// ==================== 构建测试树 ====================
//
//              A
//            /   \
//           B     C
//          / \   / \
//         D   E F   G
//        /         / \
//       H         I   J
//
TreeNode* buildTestTree() {
    TreeNode* A = new TreeNode('A');
    TreeNode* B = new TreeNode('B'); A->left = B;
    TreeNode* C = new TreeNode('C'); A->right = C;
    TreeNode* D = new TreeNode('D'); B->left = D;
    TreeNode* E = new TreeNode('E'); B->right = E;
    TreeNode* F = new TreeNode('F'); C->left = F;
    TreeNode* G = new TreeNode('G'); C->right = G;
    TreeNode* H = new TreeNode('H'); D->left = H;
    TreeNode* I = new TreeNode('I'); G->left = I;
    TreeNode* J = new TreeNode('J'); G->right = J;
    return A;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二叉树遍历 (4种方式)                   " << endl;
    cout << "========================================" << endl << endl;

    TreeNode* root = buildTestTree();

    // 打印树结构
    cout << "[测试二叉树]" << endl;
    cout << "            A" << endl;
    cout << "          /   \\" << endl;
    cout << "         B     C" << endl;
    cout << "        / \\   / \\" << endl;
    cout << "       D   E F   G" << endl;
    cout << "      /         / \\" << endl;
    cout << "     H         I   J" << endl << endl;

    // 四种遍历
    cout << "[递归遍历结果]" << endl;
    vector<char> pre, in, post;
    preorder(root, pre);
    inorder(root, in);
    postorder(root, post);

    printTraversal("先序 (前序)", pre);
    printTraversal("中序       ", in);
    printTraversal("后序       ", post);

    vector<char> level = levelorder(root);
    printTraversal("层次 (BFS) ", level);
    cout << endl;

    // 漂亮的层次打印
    cout << "[层次遍历 (按层打印)]" << endl;
    levelorderPretty(root);

    // 遍历性质
    cout << endl << "[重要性质]" << endl;
    cout << "  1. 已知 先序+中序 或 后序+中序，可唯一确定二叉树" << endl;
    cout << "  2. 仅有 先序+后序 不能唯一确定一般二叉树" << endl;
    cout << "     （但可以唯一确定真二叉树 / 正则二叉树）" << endl;
    cout << "  3. 二叉搜索树的中序遍历是有序序列" << endl;
    cout << "  4. 叶子节点在各遍历中的相对顺序不变" << endl;

    // 清理
    // 简单清理（实际应使用递归delete）
    delete H; delete D; delete E; delete B;
    delete F; delete I; delete J; delete G; delete C;
    delete A;

    return 0;
}
