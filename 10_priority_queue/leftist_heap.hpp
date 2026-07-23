/**
 * 左式堆 (Leftist Heap)
 *
 * 对应教材 10.4 节：左式堆
 *
 * 左式堆是优先级队列的一种可高效合并的实现。
 *
 * 核心概念:
 *   NPL (Null Path Length): 节点到最近空节点的距离
 *     - 空节点的 NPL = 0
 *     - 节点的 NPL = 1 + min(NPL(left), NPL(right))
 *
 * 左式性质: 对任意节点，NPL(左孩子) ≥ NPL(右孩子)
 *   → 可以证明，任何节点的右链长度 ≤ log(n+1)
 *
 * 合并操作 O(log n): 沿右链递归合并
 * 插入 = 与单节点堆合并
 * 删除 = 合并左右孩子
 *
 * 2025年826真题(2): 将任意二叉树左式堆化 (leftify)
 */

#include <iostream>
using namespace std;

struct LeftHeapNode {
    int key;
    LeftHeapNode* left;
    LeftHeapNode* right;
    int npl;  // 空节点路径长度

    LeftHeapNode(int k) : key(k), left(nullptr), right(nullptr), npl(0) {}
};

class LeftistHeap {
private:
    LeftHeapNode* _root;

    // 计算 NPL
    int npl(LeftHeapNode* node) const {
        return node ? node->npl : 0;
    }

    // 递归合并两个左式堆，返回合并后的根
    // 核心算法！O(log n)
    LeftHeapNode* merge(LeftHeapNode* h1, LeftHeapNode* h2) {
        if (!h1) return h2;
        if (!h2) return h1;

        // 确保 h1 的根较小（小顶堆）
        if (h1->key > h2->key)
            swap(h1, h2);

        // 递归：将 h1 的右孩子与 h2 合并
        h1->right = merge(h1->right, h2);

        // 维护左式性质: 若 NPL(左) < NPL(右)，交换左右孩子
        if (npl(h1->left) < npl(h1->right))
            swap(h1->left, h1->right);

        // 更新 NPL: 1 + NPL(右孩子)
        h1->npl = npl(h1->right) + 1;

        return h1;
    }

    void inorder(LeftHeapNode* node) const {
        if (!node) return;
        inorder(node->left);
        cout << node->key << "(" << node->npl << ") ";
        inorder(node->right);
    }

    void clear(LeftHeapNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    LeftistHeap() : _root(nullptr) {}
    ~LeftistHeap() { clear(_root); }

    bool empty() const { return !_root; }

    int top() const {
        if (!_root) throw runtime_error("Heap is empty");
        return _root->key;
    }

    // 插入 = 合并当前堆与新单节点堆
    void push(int key) {
        _root = merge(_root, new LeftHeapNode(key));
    }

    // 删除最小值 = 合并左右孩子
    int pop() {
        if (!_root) throw runtime_error("Heap is empty");
        int minKey = _root->key;
        LeftHeapNode* oldRoot = _root;
        _root = merge(_root->left, _root->right);
        delete oldRoot;
        return minKey;
    }

    // 原地合并另一个堆（会把另一个堆清空）
    void mergeWith(LeftistHeap& other) {
        _root = merge(_root, other._root);
        other._root = nullptr;
    }

    void print() const {
        cout << "  中序(key(NPL)): ";
        inorder(_root);
        cout << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  左式堆 (Leftist Heap)                  " << endl;
    cout << "========================================" << endl << endl;

    // 构建左式堆
    LeftistHeap h1;
    cout << "[堆1] 插入 3, 10, 8, 21, 14" << endl;
    h1.push(3); h1.push(10); h1.push(8); h1.push(21); h1.push(14);
    h1.print();

    LeftistHeap h2;
    cout << endl << "[堆2] 插入 6, 12, 7, 18" << endl;
    h2.push(6); h2.push(12); h2.push(7); h2.push(18);
    h2.print();

    // 合并
    cout << endl << "[合并 h1 + h2]" << endl;
    h1.mergeWith(h2);
    h1.print();
    cout << "  h2 变为: " << (h2.empty() ? "空" : "非空") << endl;

    // 依次弹出最小值
    cout << endl << "[依次弹出最小值]" << endl;
    cout << "  弹出顺序: ";
    while (!h1.empty()) {
        cout << h1.top() << " ";
        h1.pop();
    }
    cout << endl;

    cout << endl << "[左式堆特性]" << endl;
    cout << "  1. NPL(左) ≥ NPL(右) — 右链最短" << endl;
    cout << "  2. 合并 O(log n) — 沿右链递归" << endl;
    cout << "  3. 插入 O(log n) — 合并 + 单节点" << endl;
    cout << "  4. 删除 O(log n) — 合并左右孩子" << endl;
    cout << endl;
    cout << "[2025年826真题(2)]" << endl;
    cout << "  将任意二叉树左式堆化 (leftify):" << endl;
    cout << "  通过调整 NPL 和交换左右孩子，" << endl;
    cout << "  将一般二叉树转换为左式堆。" << endl;

    return 0;
}
