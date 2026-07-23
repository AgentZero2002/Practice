/**
 * 二叉搜索树 (BST) 模板实现
 *
 * 对应教材 第7章：搜索树
 *
 * BST 性质: 对任意节点 v,
 *   - 左子树中所有节点的 key < v.key
 *   - 右子树中所有节点的 key > v.key
 *   - 中序遍历得到有序序列
 *
 * 操作: 查找 O(h), 插入 O(h), 删除 O(h)
 * 最坏情况下 h = n (退化为链表), 最好 h ≈ log n
 */

#ifndef BST_IMPL_HPP
#define BST_IMPL_HPP

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

template <typename T>
struct BSTNode {
    T key;
    BSTNode* left;
    BSTNode* right;

    BSTNode(T k) : key(k), left(nullptr), right(nullptr) {}
};

template <typename T>
class BST {
private:
    BSTNode<T>* _root;
    int _size;

    // 递归插入
    BSTNode<T>* insertRecur(BSTNode<T>* node, T key) {
        if (!node) { _size++; return new BSTNode<T>(key); }
        if (key < node->key)
            node->left = insertRecur(node->left, key);
        else if (key > node->key)
            node->right = insertRecur(node->right, key);
        // 若 key == node->key，不重复插入
        return node;
    }

    // 找最小值节点
    BSTNode<T>* findMin(BSTNode<T>* node) {
        while (node && node->left)
            node = node->left;
        return node;
    }

    // 递归删除
    BSTNode<T>* removeRecur(BSTNode<T>* node, T key) {
        if (!node) return nullptr;

        if (key < node->key) {
            node->left = removeRecur(node->left, key);
        } else if (key > node->key) {
            node->right = removeRecur(node->right, key);
        } else {
            // 找到目标节点 — 三种情况
            if (!node->left) {       // 情况1: 无左孩子
                BSTNode<T>* right = node->right;
                delete node; _size--; return right;
            } else if (!node->right) { // 情况2: 无右孩子
                BSTNode<T>* left = node->left;
                delete node; _size--; return left;
            } else {                 // 情况3: 有两个孩子
                // 用直接后继（右子树中的最小节点）替换
                BSTNode<T>* successor = findMin(node->right);
                node->key = successor->key;
                node->right = removeRecur(node->right, successor->key);
            }
        }
        return node;
    }

    // 中序遍历
    void inorder(BSTNode<T>* node, vector<T>& result) const {
        if (!node) return;
        inorder(node->left, result);
        result.push_back(node->key);
        inorder(node->right, result);
    }

    // 范围查询 (Range Query)
    void rangeQuery(BSTNode<T>* node, T lo, T hi, vector<T>& result) const {
        if (!node) return;
        if (lo < node->key)
            rangeQuery(node->left, lo, hi, result);
        if (lo <= node->key && node->key <= hi)
            result.push_back(node->key);
        if (node->key < hi)
            rangeQuery(node->right, lo, hi, result);
    }

    // 清理
    void clear(BSTNode<T>* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    BST() : _root(nullptr), _size(0) {}
    ~BST() { clear(_root); }

    int size() const { return _size; }
    bool empty() const { return _root == nullptr; }

    // 查找 O(h)
    BSTNode<T>* search(T key) const {
        BSTNode<T>* node = _root;
        while (node) {
            if (key < node->key)
                node = node->left;
            else if (key > node->key)
                node = node->right;
            else
                return node;  // 命中
        }
        return nullptr;  // 未找到
    }

    // 插入 O(h)
    void insert(T key) {
        _root = insertRecur(_root, key);
    }

    // 删除 O(h)
    void remove(T key) {
        _root = removeRecur(_root, key);
    }

    // 最小值
    T minimum() const {
        BSTNode<T>* node = findMin(_root);
        if (!node) throw runtime_error("BST is empty");
        return node->key;
    }

    // 最大值
    T maximum() const {
        BSTNode<T>* node = _root;
        while (node && node->right) node = node->right;
        if (!node) throw runtime_error("BST is empty");
        return node->key;
    }

    // 中序遍历 → 有序序列
    vector<T> toSortedVector() const {
        vector<T> result;
        inorder(_root, result);
        return result;
    }

    // ==================== 范围查询 (2021年826真题) ====================
    // 返回在 [lo, hi] 内的所有 key，时间复杂度 O(k + h)
    vector<T> rangeSearch(T lo, T hi) const {
        vector<T> result;
        rangeQuery(_root, lo, hi, result);
        return result;
    }

    // 判断是否为合法的 BST
    bool isValidBST() const {
        T prev;
        bool first = true;
        vector<T> sorted = toSortedVector();
        for (size_t i = 1; i < sorted.size(); i++)
            if (sorted[i] <= sorted[i-1]) return false;
        return true;
    }
};

#endif // BST_IMPL_HPP
