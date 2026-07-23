/**
 * QuickSelect — O(n) 期望时间的选择算法
 *
 * QuickSelect 是快速排序的变体, 用于找第 k 小的元素。
 * 与快排不同, QuickSelect 每次只递归一侧。
 *
 * 算法:
 *   1. 随即选择一个 pivot
 *   2. 分区: < pivot 的元素在左, > pivot 的左右
 *   3. 如果 k < pivot位置, 递归左子数组
 *      如果 k > pivot位置, 递归右子数组
 *      否则 pivot 就是答案
 *
 * 期望时间复杂度:
 *   T(n) = n + T(n/2) = O(n)  (每次规模期望减半)
 *   最坏: O(n²) (但概率极低, 且可用 Median-of-Medians 优化)
 *
 * vs nth_element:
 *   C++ STL 的 nth_element 就是 QuickSelect 的工程实现
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

// Partition (Lomuto)
int partition(vector<int>& arr, int l, int r) {
    int pivot = arr[r];
    int i = l;
    for (int j = l; j < r; j++)
        if (arr[j] <= pivot)
            swap(arr[i++], arr[j]);
    swap(arr[i], arr[r]);
    return i;
}

// QuickSelect: 找第 k 小的元素 (k 为 0-indexed)
int quickSelect(vector<int>& arr, int l, int r, int k) {
    if (l == r) return arr[l];

    // 随机化 pivot
    int pi = l + rand() % (r - l + 1);
    swap(arr[pi], arr[r]);

    int pivotIdx = partition(arr, l, r);

    if (k == pivotIdx) return arr[pivotIdx];
    if (k < pivotIdx) return quickSelect(arr, l, pivotIdx - 1, k);
    return quickSelect(arr, pivotIdx + 1, r, k);
}

// 找中位数
double findMedian(vector<int>& arr) {
    int n = arr.size();
    if (n % 2 == 1)
        return quickSelect(arr, 0, n - 1, n / 2);
    else
        return (quickSelect(arr, 0, n - 1, n/2 - 1) +
                quickSelect(arr, 0, n - 1, n/2)) / 2.0;
}

// ==================== 主函数 ====================
int main() {
    srand(time(0));
    cout << "========================================" << endl;
    cout << "  QuickSelect — O(n) 随机选择              " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {3, 7, 1, 9, 2, 8, 5, 6, 4};
    cout << "[数组] ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    for (int k = 0; k < 5; k++) {
        auto copy = arr;
        cout << "  第 " << k << " 小 = "
             << quickSelect(copy, 0, copy.size()-1, k) << endl;
    }

    auto copy2 = arr;
    cout << "  中位数 ≈ " << findMedian(copy2) << " (期望: 5)" << endl;

    cout << endl << "[QuickSelect vs nth_element]" << endl;
    cout << "  QuickSelect: 期望 O(n), 最坏 O(n²)" << endl;
    cout << "  Median-of-Medians (BFPRT): 最坏 O(n)" << endl;
    cout << "  C++ nth_element: 工程实现 (IntroSelect)" << endl;

    return 0;
}
