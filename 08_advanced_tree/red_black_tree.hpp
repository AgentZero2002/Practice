/**
 * 红黑树 (Red-Black Tree)
 *
 * 对应教材 8.3 节：红黑树
 *
 * 红黑树是最广泛使用的平衡二叉搜索树之一。
 * STL map/set、Java TreeMap/TreeSet、Linux CFS 调度器等均使用红黑树。
 *
 * 五条性质:
 *   1. 每个节点要么是红的，要么是黑的
 *   2. 根节点是黑的
 *   3. 外部节点（nullptr）是黑的
 *   4. 红节点的孩子必须是黑的（没有两个连续的红节点）
 *   5. 从任何节点到其所有后代叶节点的路径含相同数量的黑节点
 *
 * 红黑树可以提升变换为 (2,4)-B-树:
 *   红节点与其黑父节点合并为一个 B-树节点
 *
 * 本实现为简化版，仅展示红黑树的结构规则和旋转着色。
 */

#include <iostream>
#include <iomanip>
using namespace std;

enum Color { RED, BLACK };

struct RBNode {
    int key;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(int k) : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}

    RBNode* sibling() const {
        if (!parent) return nullptr;
        return (this == parent->left) ? parent->right : parent->left;
    }

    RBNode* uncle() const {
        if (!parent || !parent->parent) return nullptr;
        return parent->sibling();
    }
};

class RedBlackTree {
private:
    RBNode* _root;

    void rotateLeft(RBNode* x) {
        RBNode* y = x->right;
        if (!y) return;

        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;

        if (!x->parent) _root = y;
        else if (x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void rotateRight(RBNode* y) {
        RBNode* x = y->left;
        if (!x) return;

        y->left = x->right;
        if (x->right) x->right->parent = y;
        x->parent = y->parent;

        if (!y->parent) _root = x;
        else if (y == y->parent->right) y->parent->right = x;
        else y->parent->left = x;

        x->right = y;
        y->parent = x;
    }

    // 插入后修复红黑性质
    void fixInsert(RBNode* z) {
        while (z->parent && z->parent->color == RED) {
            RBNode* uncle = z->uncle();

            if (uncle && uncle->color == RED) {
                // 情况1: 叔父为红 → 仅需重着色
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                // 情况2/3: 叔父为黑 → 旋转+重着色
                if (z->parent == z->parent->parent->left) {
                    if (z == z->parent->right) {
                        // 情况2: LR → 左旋转
                        z = z->parent;
                        rotateLeft(z);
                    }
                    // 情况3: LL → 右旋转
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateRight(z->parent->parent);
                } else {
                    // 对称情况
                    if (z == z->parent->left) {
                        // RL
                        z = z->parent;
                        rotateRight(z);
                    }
                    // RR
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rotateLeft(z->parent->parent);
                }
            }
        }
        _root->color = BLACK;
    }

    void insertBST(RBNode*& root, RBNode* parent, int key, RBNode*& newNode) {
        if (!root) {
            newNode = new RBNode(key);
            newNode->parent = parent;
            root = newNode;
            return;
        }
        if (key < root->key)
            insertBST(root->left, root, key, newNode);
        else if (key > root->key)
            insertBST(root->right, root, key, newNode);
        else
            return;  // 不重复
    }

    void inorder(RBNode* node) const {
        if (!node) return;
        inorder(node->left);
        cout << node->key << (node->color == RED ? "R" : "B") << " ";
        inorder(node->right);
    }

    bool verify(RBNode* node, int blackCount, int& pathBlack) const {
        if (!node) {
            if (pathBlack == -1) pathBlack = blackCount;
            return blackCount == pathBlack;
        }
        if (node->color == RED) {
            if ((node->left && node->left->color == RED) ||
                (node->right && node->right->color == RED))
                return false;  // 连续红色
        }
        int bc = blackCount + (node->color == BLACK ? 1 : 0);
        return verify(node->left, bc, pathBlack) &&
               verify(node->right, bc, pathBlack);
    }

    void clear(RBNode* node) {
        if (!node) return;
        clear(node->left); clear(node->right);
        delete node;
    }

public:
    RedBlackTree() : _root(nullptr) {}
    ~RedBlackTree() { clear(_root); }

    void insert(int key) {
        RBNode* newNode = nullptr;
        insertBST(_root, nullptr, key, newNode);
        if (newNode) fixInsert(newNode);
    }

    void printInorder() const {
        inorder(_root);
        cout << endl;
    }

    bool isValid() const {
        if (!_root) return true;
        if (_root->color != BLACK) return false;
        int pathBlack = -1;
        return verify(_root, 0, pathBlack);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  红黑树 (Red-Black Tree)                " << endl;
    cout << "========================================" << endl << endl;

    RedBlackTree rbt;

    cout << "[构建红黑树]" << endl;
    int keys[] = {10, 20, 30, 15, 25, 5, 1, 40, 35};
    cout << "  插入序列: ";
    for (int k : keys) { cout << k << " "; rbt.insert(k); }
    cout << endl << endl;

    cout << "[中序遍历] (R=红色, B=黑色)" << endl;
    cout << "  ";
    rbt.printInorder();

    cout << endl << "[红黑性质验证]" << endl;
    cout << "  " << (rbt.isValid() ? "✓ 满足5条红黑性质" : "✗ 违反红黑性质") << endl;

    cout << endl << "[红黑树 vs (2,4)-B-树]" << endl;
    cout << "  将每个红节点提升到与其黑父节点同层，" << endl;
    cout << "  即可得到一棵 (2,4)-B-树。" << endl;
    cout << "  所以红黑树本质上是用二叉树模拟的 4 阶 B-树。" << endl;

    cout << endl << "[插入修正案例]" << endl;
    cout << "  插入后违反\"不得连续红色\"规则:" << endl;
    cout << "  · 若叔父为红 → 重着色 (可能递归上溢)" << endl;
    cout << "  · 若叔父为黑 → 旋转+重着色" << endl;
    cout << "  至多 1 次旋转 + O(log n) 次重着色" << endl;

    return 0;
}
