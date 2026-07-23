/**
 * 向量 ADT 模板实现
 *
 * 对应教材 第2章：向量
 *
 * 基于数组的可动态扩容向量，支持:
 *   - 循秩访问 O(1)
 *   - 插入/删除 O(n)（移动元素）
 *   - 扩容策略：加倍式（分摊 O(1)）
 *   - 无序/有序元素去重
 *   - 有序向量二分查找
 *   - 起泡排序、归并排序
 */

#ifndef VECTOR_IMPL_HPP
#define VECTOR_IMPL_HPP

#include <iostream>
#include <algorithm>
#include <stdexcept>
using namespace std;

template <typename T>
class Vector {
private:
    static const int DEFAULT_CAPACITY = 3;
    int _size, _capacity;
    T* _elem;

    // 扩容：加倍策略，分摊 O(1)
    void expand() {
        if (_size < _capacity) return;
        _capacity = max(_capacity, DEFAULT_CAPACITY);
        T* old = _elem;
        _elem = new T[_capacity <<= 1];  // 容量加倍
        for (int i = 0; i < _size; i++)
            _elem[i] = old[i];
        delete[] old;
    }

    // 缩容：当负载率低于 25% 时容量减半
    void shrink() {
        if (_capacity < DEFAULT_CAPACITY << 1) return;
        if (_size << 2 > _capacity) return;  // 负载率 > 25%
        T* old = _elem;
        _elem = new T[_capacity >>= 1];
        for (int i = 0; i < _size; i++)
            _elem[i] = old[i];
        delete[] old;
    }

    // 归并排序：对区间 [lo, hi) 排序
    void mergeSort(int lo, int hi) {
        if (hi - lo < 2) return;
        int mi = (lo + hi) >> 1;
        mergeSort(lo, mi);
        mergeSort(mi, hi);
        merge(lo, mi, hi);
    }

    // 二路归并
    void merge(int lo, int mi, int hi) {
        int lb = mi - lo;
        T* B = new T[lb];
        for (int i = 0; i < lb; i++) B[i] = _elem[lo + i];  // 复制前半段
        for (int i = 0, j = mi, k = lo; i < lb; )
            if (j >= hi || B[i] <= _elem[j])
                _elem[k++] = B[i++];
            else
                _elem[k++] = _elem[j++];
        delete[] B;
    }

public:
    // ==================== 构造函数 ====================
    Vector(int c = DEFAULT_CAPACITY) : _size(0), _capacity(c) {
        _elem = new T[_capacity];
    }

    Vector(T const* A, int n) : _size(n), _capacity(max(n, DEFAULT_CAPACITY)) {
        _elem = new T[_capacity];
        for (int i = 0; i < n; i++) _elem[i] = A[i];
    }

    // 拷贝构造
    Vector(Vector<T> const& V) : _size(V._size), _capacity(V._capacity) {
        _elem = new T[_capacity];
        for (int i = 0; i < _size; i++) _elem[i] = V._elem[i];
    }

    ~Vector() { delete[] _elem; }

    // ==================== 基本操作 ====================
    int size() const { return _size; }
    int capacity() const { return _capacity; }
    bool empty() const { return _size == 0; }

    // 循秩访问 O(1)
    T& operator[](int r) {
        if (r < 0 || r >= _size) throw out_of_range("Vector: index out of range");
        return _elem[r];
    }

    const T& operator[](int r) const {
        if (r < 0 || r >= _size) throw out_of_range("Vector: index out of range");
        return _elem[r];
    }

    // 插入：在秩 r 处插入元素 e O(n)
    int insert(int r, T const& e) {
        expand();
        for (int i = _size; i > r; i--)
            _elem[i] = _elem[i - 1];
        _elem[r] = e;
        _size++;
        return r;
    }

    // 尾部追加 O(1) 分摊
    int push_back(T const& e) {
        expand();
        _elem[_size++] = e;
        return _size - 1;
    }

    // 删除：移除秩 r 处的元素 O(n)
    T remove(int r) {
        T e = _elem[r];
        for (int i = r; i < _size - 1; i++)
            _elem[i] = _elem[i + 1];
        _size--;
        shrink();
        return e;
    }

    // ==================== 查找 ====================
    // 无序向量：顺序查找 O(n)，返回秩，不存在返回 -1
    int find(T const& e, int lo = 0, int hi = -1) const {
        if (hi < 0) hi = _size;
        while (lo < hi-- && _elem[hi] != e);
        return hi;  // hi < lo 时返回 lo - 1 = -1
    }

    // 有序向量：二分查找（版本C，语意最清晰）O(log n)
    // 返回不大于 e 的最后一个元素的秩
    int search(T const& e, int lo = 0, int hi = -1) const {
        if (hi < 0) hi = _size;
        while (lo < hi) {
            int mi = (lo + hi) >> 1;
            e < _elem[mi] ? hi = mi : lo = mi + 1;
        }
        return --lo;  // lo 为大于 e 的最小元素的秩
    }

    // ==================== 唯一化（去重） ====================
    // 无序向量去重 O(n²)
    int deduplicate() {
        int oldSize = _size;
        int i = 1;
        while (i < _size)
            if (find(_elem[i], 0, i) < 0)  // 在前缀中未找到
                i++;
            else
                remove(i);
        return oldSize - _size;
    }

    // 有序向量唯一化 O(n)
    int uniquify() {
        int i = 0, j = 0;
        while (++j < _size)
            if (_elem[i] != _elem[j])
                _elem[++i] = _elem[j];  // 发现不同元素，向前搬移
        _size = ++i;
        shrink();
        return j - i;
    }

    // ==================== 排序 ====================
    // 起泡排序 O(n²) — 含提前终止优化
    void bubbleSort(int lo = 0, int hi = -1) {
        if (hi < 0) hi = _size;
        while (lo < --hi) {
            bool sorted = true;
            for (int i = lo; i < hi; i++)
                if (_elem[i] > _elem[i + 1]) {
                    swap(_elem[i], _elem[i + 1]);
                    sorted = false;
                }
            if (sorted) break;  // 已有序，提前终止
        }
    }

    // 归并排序 O(n log n)
    void mergeSort() { mergeSort(0, _size); }

    // ==================== 遍历 ====================
    void traverse(void (*visit)(T&)) {
        for (int i = 0; i < _size; i++) visit(_elem[i]);
    }

    template <typename VST>
    void traverse(VST& visit) {
        for (int i = 0; i < _size; i++) visit(_elem[i]);
    }
};

#endif // VECTOR_IMPL_HPP
