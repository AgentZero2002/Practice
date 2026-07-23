/**
 * 列表（双向链表）ADT 模板实现
 *
 * 对应教材 第3章：列表
 *
 * 基于双向链表的列表实现:
 *   - 首尾哨兵节点（header/trailer）简化边界处理
 *   - 循位置访问（position-based access）, O(n) 查找
 *   - 插入/删除 O(1)（已知位置）
 *   - 选择排序、插入排序、归并排序
 *   - 有序/无序列表去重
 */

#ifndef LIST_IMPL_HPP
#define LIST_IMPL_HPP

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
struct ListNode {
    T data;
    ListNode<T>* pred;  // 前驱
    ListNode<T>* succ;  // 后继

    ListNode() : pred(nullptr), succ(nullptr) {}
    ListNode(T e, ListNode<T>* p = nullptr, ListNode<T>* s = nullptr)
        : data(e), pred(p), succ(s) {}

    // 在当前节点后插入
    ListNode<T>* insertAsSucc(T const& e) {
        ListNode<T>* x = new ListNode<T>(e, this, succ);
        succ->pred = x;
        succ = x;
        return x;
    }

    // 在当前节点前插入
    ListNode<T>* insertAsPred(T const& e) {
        ListNode<T>* x = new ListNode<T>(e, pred, this);
        pred->succ = x;
        pred = x;
        return x;
    }
};

template <typename T>
class List {
private:
    int _size;
    ListNode<T>* header;   // 头哨兵
    ListNode<T>* trailer;  // 尾哨兵

    // 初始化哨兵
    void init() {
        header = new ListNode<T>();
        trailer = new ListNode<T>();
        header->succ = trailer;
        header->pred = nullptr;
        trailer->pred = header;
        trailer->succ = nullptr;
        _size = 0;
    }

    // 清除所有节点
    int clear() {
        int oldSize = _size;
        while (_size > 0)
            remove(header->succ);
        return oldSize;
    }

public:
    // ==================== 构造函数 ====================
    List() { init(); }

    List(T const* A, int n) {
        init();
        for (int i = n - 1; i >= 0; i--)
            insertAsFirst(A[i]);  // 头插法保持顺序
    }

    ~List() {
        clear();
        delete header;
        delete trailer;
    }

    // ==================== 只读访问 ====================
    int size() const { return _size; }
    bool empty() const { return _size <= 0; }

    ListNode<T>* first() const { return header->succ; }
    ListNode<T>* last() const { return trailer->pred; }

    // 判断节点 p 是否合法
    bool valid(ListNode<T>* p) const {
        return p && p != header && p != trailer;
    }

    // 查找：在 p 的前 n 个真前驱中查找 e，O(n)
    ListNode<T>* find(T const& e, int n, ListNode<T>* p) const {
        while (0 < n--)
            if (e == (p = p->pred)->data)
                return p;
        return nullptr;  // 越界隐含未找到
    }

    // 在全列表中查找，O(n)
    ListNode<T>* find(T const& e) const {
        return find(e, _size, trailer);
    }

    // ==================== 插入操作 O(1) ====================
    ListNode<T>* insertAsFirst(T const& e) {
        _size++;
        return header->insertAsSucc(e);
    }

    ListNode<T>* insertAsLast(T const& e) {
        _size++;
        return trailer->insertAsPred(e);
    }

    ListNode<T>* insertAfter(ListNode<T>* p, T const& e) {
        _size++;
        return p->insertAsSucc(e);
    }

    ListNode<T>* insertBefore(ListNode<T>* p, T const& e) {
        _size++;
        return p->insertAsPred(e);
    }

    // ==================== 删除操作 O(1) ====================
    T remove(ListNode<T>* p) {
        T e = p->data;
        p->pred->succ = p->succ;
        p->succ->pred = p->pred;
        delete p;
        _size--;
        return e;
    }

    // ==================== 唯一化（去重） ====================
    // 无序列表去重 O(n²)
    int deduplicate() {
        if (_size < 2) return 0;
        int oldSize = _size;
        ListNode<T>* p = header->succ;
        int r = 0;
        while (p != trailer) {
            ListNode<T>* q = find(p->data, r, p);
            if (q) {
                ListNode<T>* d = p;
                p = p->succ;
                remove(d);
            } else {
                r++;
                p = p->succ;
            }
        }
        return oldSize - _size;
    }

    // 有序列表唯一化 O(n)
    int uniquify() {
        if (_size < 2) return 0;
        int oldSize = _size;
        ListNode<T>* p = first();
        ListNode<T>* q = p->succ;
        while (q != trailer) {
            if (p->data != q->data) {
                p = q;
                q = q->succ;
            } else {
                ListNode<T>* d = q;
                q = q->succ;
                remove(d);
            }
        }
        return oldSize - _size;
    }

    // ==================== 排序 ====================
    // 插入排序 O(n²) — 稳定，对近乎有序数据高效
    void insertionSort() {
        if (_size < 2) return;
        ListNode<T>* p = header->succ->succ;  // 从第二个元素开始
        while (p != trailer) {
            ListNode<T>* q = p->pred;
            T key = p->data;
            ListNode<T>* next = p->succ;
            remove(p);
            // 在前缀中找插入位置
            while (q != header && q->data > key)
                q = q->pred;
            insertAfter(q, key);
            p = next;
        }
    }

    // 选择排序 O(n²) — 不稳定
    void selectionSort() {
        if (_size < 2) return;
        ListNode<T>* head = header->succ;
        ListNode<T>* tail = trailer;
        while (head != tail) {
            // 在 head..tail 之间找最大值
            ListNode<T>* mx = head;
            ListNode<T>* p = head->succ;
            while (p != tail) {
                if (mx->data <= p->data) mx = p;
                p = p->succ;
            }
            // 将 mx 移到 tail 前
            T tmp = mx->data;
            mx->data = tail->pred->data;
            tail->pred->data = tmp;
            tail = tail->pred;  // tail 前移
        }
    }

    // ==================== 遍历 ====================
    template <typename VST>
    void traverse(VST& visit) const {
        for (ListNode<T>* p = header->succ; p != trailer; p = p->succ)
            visit(p->data);
    }

    // 翻转
    void reverse() {
        if (_size < 2) return;
        for (ListNode<T>* p = header; p; p = p->pred)
            swap(p->pred, p->succ);
        swap(header, trailer);
    }
};

#endif // LIST_IMPL_HPP
