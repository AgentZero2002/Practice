/**
 * AVL 平衡二叉搜索树
 *
 * 对应教材 7.3 节：AVL 树
 *
 * AVL 树是最早被提出的自平衡二叉搜索树。
 * 约束: 任意节点的左右子树高度差 (平衡因子) 不超过 1。
 *
 * 操作:
 *   - 插入: 最多 1 次旋转即可恢复平衡
 *   - 删除: 可能需要 O(log n) 次旋转
 *
 * 包含:
 *   - 四种旋转: LL (右旋), RR (左旋), LR (左-右双旋), RL (右-左双旋)
 *   - 3+4 重构 (邓俊辉教材精华)
 */

#include <iostream>
#include <algorithm>
using namespace std;

template <typename T>
struct AVLNode {
    T key;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(T k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

template <typename T>
class AVLTree {
private:
    AVLNode<T>* _root;

    int height(AVLNode<T>* node) {
        return node ? node->height : 0;
    }

    int balanceFactor(AVLNode<T>* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(AVLNode<T>* node) {
        if (node)
            node->height = 1 + max(height(node->left), height(node->right));
    }

    // ==================== 旋转操作 ====================
    // 右旋转 (LL 情况)
    AVLNode<T>* rotateRight(AVLNode<T>* y) {
        AVLNode<T>* x = y->left;
        AVLNode<T>* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;  // 新的根
    }

    // 左旋转 (RR 情况)
    AVLNode<T>* rotateLeft(AVLNode<T>* x) {
        AVLNode<T>* y = x->right;
        AVLNode<T>* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;  // 新的根
    }

    // ==================== 插入 ====================
    AVLNode<T>* insert(AVLNode<T>* node, T key) {
        // 1. 标准 BST 插入
        if (!node) return new AVLNode<T>(key);

        if (key < node->key)
            node->left = insert(node->left, key);
        else if (key > node->key)
            node->right = insert(node->right, key);
        else
            return node;  // 不插入重复键

        // 2. 更新高度
        updateHeight(node);

        // 3. 检查平衡并旋转
        int balance = balanceFactor(node);

        // LL 情况: 左子树过高，且插入在左子树的左侧
        if (balance > 1 && key < node->left->key)
            return rotateRight(node);

        // RR 情况: 右子树过高，且插入在右子树的右侧
        if (balance < -1 && key > node->right->key)
            return rotateLeft(node);

        // LR 情况: 左子树过高，但插入在左子树的右侧
        if (balance > 1 && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }

        // RL 情况: 右子树过高，但插入在右子树的左侧
        if (balance < -1 && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // ==================== 查找最小节点 ====================
    AVLNode<T>* minNode(AVLNode<T>* node) {
        AVLNode<T>* cur = node;
        while (cur && cur->left) cur = cur->left;
        return cur;
    }

    // ==================== 删除 ====================
    AVLNode<T>* remove(AVLNode<T>* node, T key) {
        if (!node) return nullptr;

        if (key < node->key)
            node->left = remove(node->left, key);
        else if (key > node->key)
            node->right = remove(node->right, key);
        else {
            // 单孩子或无孩子
            if (!node->left || !node->right) {
                AVLNode<T>* temp = node->left ? node->left : node->right;
                if (!temp) { temp = node; node = nullptr; }
                else *node = *temp;
                delete temp;
            } else {
                // 两个孩子: 用后继替代
                AVLNode<T>* temp = minNode(node->right);
                node->key = temp->key;
                node->right = remove(node->right, temp->key);
            }
        }

        if (!node) return nullptr;

        // 更新高度
        updateHeight(node);

        // 重新平衡
        int balance = balanceFactor(node);

        // LL
        if (balance > 1 && balanceFactor(node->left) >= 0)
            return rotateRight(node);
        // LR
        if (balance > 1 && balanceFactor(node->left) < 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        // RR
        if (balance < -1 && balanceFactor(node->right) <= 0)
            return rotateLeft(node);
        // RL
        if (balance < -1 && balanceFactor(node->right) > 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }

        return node;
    }

    // 中序遍历
    void inorder(AVLNode<T>* node) const {
        if (!node) return;
        inorder(node->left);
        cout << node->key << "(" << balanceFactor(node) << ") ";
        inorder(node->right);
    }

    void clear(AVLNode<T>* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    bool search(AVLNode<T>* node, T key) const {
        while (node) {
            if (key == node->key) return true;
            node = (key < node->key) ? node->left : node->right;
        }
        return false;
    }

public:
    AVLTree() : _root(nullptr) {}
    ~AVLTree() { clear(_root); }

    void insert(T key) { _root = insert(_root, key); }
    void remove(T key) { _root = remove(_root, key); }
    bool contains(T key) const { return search(_root, key); }
    int height() const { return _root ? _root->height : 0; }

    void printInorder() const {
        inorder(_root);
        cout << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  AVL 平衡二叉搜索树                     " << endl;
    cout << "========================================" << endl << endl;

    AVLTree<int> avl;

    // 依次插入导致 LL, RR, LR, RL 旋转
    cout << "[构建 AVL 树] 插入 50, 30, 70, 20, 40, 60, 80, 10, 25" << endl;
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 10, 25};
    for (int k : keys) avl.insert(k);

    cout << "  中序 (key(平衡因子)): ";
    avl.printInorder();
    cout << "  树高: " << avl.height() << " (n=9, 平衡高 max ≈ 1.44·log₂(n+2) ≈ 5)" << endl;

    // 查找
    cout << endl << "[查找]" << endl;
    cout << "  45: " << (avl.contains(45) ? "存在" : "不存在") << endl;
    cout << "  25: " << (avl.contains(25) ? "存在" : "不存在") << endl;

    // 删除
    cout << endl << "[删除 30]" << endl;
    avl.remove(30);
    cout << "  删除后: ";
    avl.printInorder();
    cout << "  树高: " << avl.height() << endl;

    // 旋转类型总结
    cout << endl << "[旋转类型总结]" << endl;
    cout << "  LL  → 右旋 (rotateRight)  — 平衡因子 > 1, 左子 BF ≥ 0" << endl;
    cout << "  RR  → 左旋 (rotateLeft)   — 平衡因子 < -1, 右子 BF ≤ 0" << endl;
    cout << "  LR  → 左旋+右旋               — 平衡因子 > 1, 左子 BF < 0" << endl;
    cout << "  RL  → 右旋+左旋               — 平衡因子 < -1, 右子 BF > 0" << endl;
    cout << endl;
    cout << "[3+4 重构] (邓俊辉教材)" << endl;
    cout << "  将上述四种旋转统一为对 3 个节点 + 4 棵子树的重新编排" << endl;

    return 0;
}
