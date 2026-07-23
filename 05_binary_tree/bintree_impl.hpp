/**
 * 二叉树 ADT 模板实现
 *
 * 对应教材 第5章：二叉树
 *
 * 实现功能:
 *   - 二叉树节点类 (BinNode)
 *   - 二叉树类 (BinTree)，支持:
 *     · 更新树的高度
 *     · 节点插入（左孩子/右孩子）
 *     · 四种遍历（先序/中序/后序/层次）
 *     · 树的大小统计
 *
 * 基于父子链接法 + 孩子指针
 */

#ifndef BINTREE_IMPL_HPP
#define BINTREE_IMPL_HPP

#include <iostream>
#include <queue>
#include <stack>
#include <algorithm>
using namespace std;

// 节点状态（用于遍历等操作）
enum RBColor { RB_RED, RB_BLACK };

template <typename T>
struct BinNode {
    T data;
    BinNode<T>* parent;
    BinNode<T>* left;
    BinNode<T>* right;
    int height;
    int npl;        // 空节点路径长度 (NULL PATH LENGTH, for leftist heap)
    RBColor color;  // 红黑树着色

    BinNode() : parent(nullptr), left(nullptr), right(nullptr),
                height(0), npl(0), color(RB_RED) {}

    BinNode(T e, BinNode<T>* p = nullptr, BinNode<T>* l = nullptr,
            BinNode<T>* r = nullptr)
        : data(e), parent(p), left(l), right(r),
          height(0), npl(0), color(RB_RED) {}

    // 作为当前节点的左孩子插入
    BinNode<T>* insertAsLeft(T const& e) {
        return left = new BinNode<T>(e, this);
    }

    // 作为当前节点的右孩子插入
    BinNode<T>* insertAsRight(T const& e) {
        return right = new BinNode<T>(e, this);
    }

    // 当前节点的后代总数（子树规模）
    int size() const {
        int s = 1;
        if (left) s += left->size();
        if (right) s += right->size();
        return s;
    }

    // 是否为根
    bool isRoot() const { return parent == nullptr; }

    // 是否为左孩子
    bool isLeftChild() const {
        return parent && this == parent->left;
    }

    // 是否为右孩子
    bool isRightChild() const {
        return parent && this == parent->right;
    }

    // 兄弟节点
    BinNode<T>* sibling() {
        if (!parent) return nullptr;
        return isLeftChild() ? parent->right : parent->left;
    }
};

template <typename T>
class BinTree {
protected:
    int _size;
    BinNode<T>* _root;

    // 更新节点高度（基于左右孩子高度）
    virtual int updateHeight(BinNode<T>* x) {
        if (!x) return -1;
        int lh = x->left ? x->left->height : -1;
        int rh = x->right ? x->right->height : -1;
        return x->height = 1 + max(lh, rh);
    }

    // 更新 x 及其历代祖先的高度
    void updateHeightAbove(BinNode<T>* x) {
        while (x) {
            updateHeight(x);
            x = x->parent;
        }
    }

    // 删除以 x 为根的子树
    int removeAt(BinNode<T>* x) {
        if (!x) return 0;
        int n = 1 + removeAt(x->left) + removeAt(x->right);
        delete x;
        return n;
    }

public:
    BinTree() : _size(0), _root(nullptr) {}

    ~BinTree() {
        if (_root) removeAt(_root);
    }

    int size() const { return _size; }
    bool empty() const { return !_root; }
    BinNode<T>* root() const { return _root; }

    // 将 e 作为根节点插入（原树为空时）
    BinNode<T>* insertAsRoot(T const& e) {
        _size = 1;
        return _root = new BinNode<T>(e);
    }

    // 将 e 作为 x 的左孩子插入
    BinNode<T>* insertAsLeft(BinNode<T>* x, T const& e) {
        _size++;
        x->insertAsLeft(e);
        updateHeightAbove(x);
        return x->left;
    }

    // 将 e 作为 x 的右孩子插入
    BinNode<T>* insertAsRight(BinNode<T>* x, T const& e) {
        _size++;
        x->insertAsRight(e);
        updateHeightAbove(x);
        return x->right;
    }

    // ==================== 遍历接口 ====================
    // 先序遍历 (递归)
    void traversePre(BinNode<T>* x, void (*visit)(T&)) {
        if (!x) return;
        visit(x->data);
        traversePre(x->left, visit);
        traversePre(x->right, visit);
    }

    // 中序遍历 (递归)
    void traverseIn(BinNode<T>* x, void (*visit)(T&)) {
        if (!x) return;
        traverseIn(x->left, visit);
        visit(x->data);
        traverseIn(x->right, visit);
    }

    // 后序遍历 (递归)
    void traversePost(BinNode<T>* x, void (*visit)(T&)) {
        if (!x) return;
        traversePost(x->left, visit);
        traversePost(x->right, visit);
        visit(x->data);
    }

    // 层次遍历 (BFS)
    void traverseLevel(BinNode<T>* x, void (*visit)(T&)) {
        if (!x) return;
        queue<BinNode<T>*> Q;
        Q.push(x);
        while (!Q.empty()) {
            BinNode<T>* node = Q.front(); Q.pop();
            visit(node->data);
            if (node->left) Q.push(node->left);
            if (node->right) Q.push(node->right);
        }
    }
};

#endif // BINTREE_IMPL_HPP
