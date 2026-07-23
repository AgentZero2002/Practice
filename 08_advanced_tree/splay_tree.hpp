/**
 * 伸展树 (Splay Tree)
 *
 * 对应教材 8.1 节：伸展树
 *
 * 伸展树是一种无需记录平衡因子的自适应二叉搜索树。
 * 每次访问一个节点后，通过"伸展"将该节点旋转到根。
 *
 * 伸展操作的四种情况:
 *   - zig (单旋):    父为根，则单旋
 *   - zig-zig (同侧): 节点与父分别为同侧孩子 → 先旋父再旋子
 *   - zig-zag (异侧): 节点与父分别为异侧孩子 → 两次旋转子
 *
 * 摊销复杂度 O(log n)（均摊分析）
 *
 * 2020年826真题：将任意BST通过伸展操作调整为完全二叉树
 */

#include <iostream>
#include <vector>
using namespace std;

struct SplayNode {
    int key;
    SplayNode* left;
    SplayNode* right;
    SplayNode* parent;

    SplayNode(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr) {}
};

class SplayTree {
private:
    SplayNode* _root;

    // 右旋转
    void rotateRight(SplayNode* x) {
        SplayNode* y = x->parent;
        if (!y) return;

        // 连接 y->left = x->right
        y->left = x->right;
        if (x->right) x->right->parent = y;

        // 连接 x->right = y
        x->right = y;
        x->parent = y->parent;
        y->parent = x;

        // 更新祖先
        if (x->parent) {
            if (x->parent->left == y)
                x->parent->left = x;
            else
                x->parent->right = x;
        }

        _root = x;  // x 成为新的根
        while (_root->parent) _root = _root->parent;
    }

    // 左旋转
    void rotateLeft(SplayNode* x) {
        SplayNode* y = x->parent;
        if (!y) return;

        y->right = x->left;
        if (x->left) x->left->parent = y;

        x->left = y;
        x->parent = y->parent;
        y->parent = x;

        if (x->parent) {
            if (x->parent->left == y)
                x->parent->left = x;
            else
                x->parent->right = x;
        }

        _root = x;
        while (_root->parent) _root = _root->parent;
    }

    // 伸展操作：将 x 旋转到根
    void splay(SplayNode* x) {
        if (!x) return;

        while (x->parent) {
            SplayNode* p = x->parent;
            SplayNode* g = p->parent;

            if (!g) {
                // zig: 父为根
                if (p->left == x)
                    rotateRight(x);       // x 是左孩子 → 右旋 x
                else
                    rotateLeft(x);        // x 是右孩子 → 左旋 x
            } else if (p->left == x && g->left == p) {
                // zig-zig (LL): 先旋父再旋子
                rotateRight(p);
                rotateRight(x);
            } else if (p->right == x && g->right == p) {
                // zig-zig (RR): 先旋父再旋子
                rotateLeft(p);
                rotateLeft(x);
            } else if (p->left == x && g->right == p) {
                // zig-zag (RL): 两次旋转 x
                rotateRight(x);
                rotateLeft(x);
            } else {
                // zig-zag (LR): 两次旋转 x
                rotateLeft(x);
                rotateRight(x);
            }
        }
    }

    // BST 查找
    SplayNode* searchNode(SplayNode* node, int key) {
        while (node) {
            if (key == node->key) return node;
            if (key < node->key && node->left)
                node = node->left;
            else if (key > node->key && node->right)
                node = node->right;
            else
                break;  // 返回最近访问的节点
        }
        return node;
    }

    void inorder(SplayNode* node, vector<int>& result) const {
        if (!node) return;
        inorder(node->left, result);
        result.push_back(node->key);
        inorder(node->right, result);
    }

    void clear(SplayNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    SplayTree() : _root(nullptr) {}
    ~SplayTree() { clear(_root); }

    // 查找并伸展（核心操作）
    bool search(int key) {
        if (!_root) return false;
        SplayNode* node = searchNode(_root, key);
        if (node) splay(node);
        _root = node;
        return _root && _root->key == key;
    }

    // 插入
    void insert(int key) {
        if (!_root) {
            _root = new SplayNode(key);
            return;
        }

        SplayNode* node = _root;
        SplayNode* parent = nullptr;

        while (node) {
            parent = node;
            if (key < node->key)
                node = node->left;
            else if (key > node->key)
                node = node->right;
            else {
                splay(node);  // 已存在，伸展
                _root = node;
                return;
            }
        }

        // 创建新节点
        SplayNode* newNode = new SplayNode(key);
        newNode->parent = parent;
        if (key < parent->key)
            parent->left = newNode;
        else
            parent->right = newNode;

        splay(newNode);
        _root = newNode;
    }

    // 获取中序序列
    vector<int> toSortedVector() const {
        vector<int> result;
        inorder(_root, result);
        return result;
    }

    // 获取根的值
    int rootKey() const { return _root ? _root->key : -1; }

    // 打印树结构（简化版）
    void printStructure() const {
        vector<int> sorted = toSortedVector();
        cout << "  中序: ";
        for (int x : sorted) cout << x << " ";
        cout << "| 根: " << rootKey() << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  伸展树 (Splay Tree)                    " << endl;
    cout << "========================================" << endl << endl;

    SplayTree st;

    // 插入
    cout << "[构建伸展树] 插入 50, 30, 70, 20, 40, 60, 80" << endl;
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    for (int k : keys) st.insert(k);
    st.printStructure();

    // 查找（触发伸展）
    cout << endl << "[伸展操作] 每次查找后，被查找节点旋转到根" << endl;

    int queries[] = {20, 70, 40};
    for (int q : queries) {
        cout << "  查找 " << q << " → ";
        st.search(q);
        st.printStructure();
    }

    // 2020真题概念
    cout << endl << "[2020年826真题]" << endl;
    cout << "  题目: 将任意 BST 通过 splay 操作调整为完全二叉树" << endl;
    cout << "  关键: 选择合适的 splay 目标序列来重新调整树形" << endl;
    cout << endl;

    cout << "[伸展树特性]" << endl;
    cout << "  1. 自适应: 经常访问的节点靠近根" << endl;
    cout << "  2. 平均复杂度: O(log n)，含均摊分析" << endl;
    cout << "  3. 不支持多线程（树形会因读而变）" << endl;
    cout << "  4. zig-zig 双层伸展优于逐层伸展" << endl;

    return 0;
}
