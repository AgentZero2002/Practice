/**
 * 完全二叉堆 (Complete Binary Heap) 模板实现
 *
 * 对应教材 第10章：优先级队列
 *
 * 堆的性质:
 *   - 结构性质: 是一棵完全二叉树
 *   - 堆序性质: 任意节点 ≥ (或 ≤) 其孩子
 *
 * 由于结构性质，可用向量（数组）紧凑存储:
 *   节点 i 的父亲: (i-1)/2
 *   节点 i 的左孩子: 2*i+1
 *   节点 i 的右孩子: 2*i+2
 *
 * 操作复杂度:
 *   - getMax()      O(1)
 *   - insert()      O(log n)  (上滤 percolate up)
 *   - delMax()      O(log n)  (下滤 percolate down)
 *   - buildHeap()   O(n)     (Floyd 算法)
 */

#ifndef HEAP_IMPL_HPP
#define HEAP_IMPL_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

template <typename T>
class MaxHeap {
private:
    vector<T> _data;

    // 上滤：将位置 i 的元素向上移动到正确位置
    void percolateUp(int i) {
        T val = _data[i];
        while (i > 0) {
            int parent = (i - 1) >> 1;
            if (_data[parent] >= val) break;
            _data[i] = _data[parent];  // 父下移
            i = parent;
        }
        _data[i] = val;
    }

    // 下滤：将位置 i 的元素向下移动到正确位置
    void percolateDown(int i) {
        T val = _data[i];
        int n = _data.size();
        int child;
        while ((child = (i << 1) + 1) < n) {  // 左孩子存在
            // 选择较大的孩子
            if (child + 1 < n && _data[child] < _data[child + 1])
                child++;
            if (val >= _data[child]) break;
            _data[i] = _data[child];  // 孩子上移
            i = child;
        }
        _data[i] = val;
    }

public:
    MaxHeap() {}
    MaxHeap(const vector<T>& data) : _data(data) {
        buildHeap();
    }

    bool empty() const { return _data.empty(); }
    int size() const { return _data.size(); }

    // 获取最大值 O(1)
    T getMax() const {
        if (empty()) throw runtime_error("Heap is empty");
        return _data[0];
    }

    // 插入 O(log n)
    void insert(T val) {
        _data.push_back(val);
        percolateUp(_data.size() - 1);
    }

    // 删除最大值 O(log n)
    T delMax() {
        if (empty()) throw runtime_error("Heap is empty");
        T maxVal = _data[0];
        _data[0] = _data.back();
        _data.pop_back();
        if (!empty()) percolateDown(0);
        return maxVal;
    }

    // ==================== Floyd 建堆 O(n) ====================
    // 自底向上对各内部节点做下滤
    // 复杂度分析: Σ(h-i) × 2^i = O(n)
    void buildHeap() {
        if (_data.size() < 2) return;
        // 从最后一个内部节点开始，自底向上
        for (int i = (_data.size() >> 1) - 1; i >= 0; i--)
            percolateDown(i);
    }

    // 打印堆（数组表示）
    void print() const {
        for (size_t i = 0; i < _data.size(); i++) {
            cout << _data[i] << " ";
        }
        cout << endl;
    }

    // 返回堆的底层数组（用于堆排序）
    vector<T>& data() { return _data; }
};

#endif // HEAP_IMPL_HPP
