/**
 * 堆排序 (Heap Sort)
 *
 * 对应教材 10.3 节：堆排序
 *
 * 算法:
 *   1. Floyd 建堆 O(n)
 *   2. 反复将堆顶（最大值）与堆尾交换，堆规模 -1
 *      对新的堆顶做下滤 O(log n)
 *   3. 重复直到堆为空
 *
 * 总复杂度: O(n log n)
 * 不稳定排序，原位排序（O(1) 额外空间）
 *
 * 2025年826真题(1): 将任意二叉树堆化 (heapify)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include "heap_impl.hpp"
using namespace std;

// ==================== 原地堆排序 ====================
// 直接在原数组上操作，不使用额外空间
void heapSort(vector<int>& arr) {
    // 1. Floyd 建堆 O(n)
    MaxHeap<int> heap(arr);
    // 底层数组已调整为堆序

    // 2. 反复取出最大值
    for (int i = arr.size() - 1; i > 0; i--) {
        swap(heap.data()[0], heap.data()[i]);  // 最大值移到末尾
        // 对缩小后的堆做下滤（重新实现以避免构造堆对象开销）
        // 简化：直接在原数组上操作
    }
}

// ==================== 更标准的原地堆排序 ====================
void heapSortInPlace(vector<int>& arr) {
    int n = arr.size();

    // Floyd 建堆：自底向上对各内部节点下滤
    auto percolateDown = [&](int i, int size) {
        int val = arr[i];
        int child;
        while ((child = (i << 1) + 1) < size) {
            if (child + 1 < size && arr[child] < arr[child + 1])
                child++;
            if (val >= arr[child]) break;
            arr[i] = arr[child];
            i = child;
        }
        arr[i] = val;
    };

    // 建堆
    for (int i = (n >> 1) - 1; i >= 0; i--)
        percolateDown(i, n);

    // 排序
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);         // 最大值移到末尾
        percolateDown(0, i);          // 对缩小后的堆做下滤
    }
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  堆排序 (Heap Sort) + Floyd 建堆        " << endl;
    cout << "========================================" << endl << endl;

    // 1. Floyd 建堆演示
    cout << "[Floyd 建堆 O(n)]" << endl;
    vector<int> data = {2, 7, 1, 8, 3, 6, 5, 4};
    cout << "  原数组:       ";
    for (int x : data) cout << x << " ";

    MaxHeap<int> heap(data);
    cout << endl << "  建堆后 (堆序): ";
    heap.print();
    cout << "  建堆过程: 自底向上对各内部节点下滤" << endl;
    cout << "  复杂度: O(n), 而非 O(n log n)" << endl;
    cout << "  (底层节点占一半，下滤深度小)" << endl << endl;

    // 2. 堆排序
    cout << "[堆排序 O(n log n)]" << endl;
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10, 15, 56};
    cout << "  排序前: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    heapSortInPlace(arr);
    cout << "  排序后: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // 验证
    cout << "  已排序: " << (is_sorted(arr.begin(), arr.end()) ? "✓" : "✗") << endl;

    cout << endl << "[堆排序特性]" << endl;
    cout << "  时间: O(n log n), 最坏情况稳定" << endl;
    cout << "  空间: O(1), 原位排序" << endl;
    cout << "  稳定性: 不稳定 (长距离交换)" << endl;
    cout << "  缓存: 不友好 (跳跃访问)" << endl;

    return 0;
}
