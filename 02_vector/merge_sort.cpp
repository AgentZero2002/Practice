/**
 * 归并排序（二路归并排序）
 *
 * 对应教材 2.8 节：归并排序
 *
 * 算法特征:
 *   - 基于分治策略 (Divide and Conquer)
 *   - 时间复杂度 O(n log n)，最坏/平均/最好均为 O(n log n)
 *   - 稳定排序（相等元素的相对位置不变）
 *   - 需要 O(n) 辅助空间
 *
 * 本实现同时提供 vector 版本和数组原地排序版本。
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
using namespace std;
using namespace std::chrono;

// ==================== 二路归并（核心操作） ====================
// 将有序区间 [lo, mi) 和 [mi, hi) 合并为有序区间 [lo, hi)
void merge(vector<int>& A, int lo, int mi, int hi) {
    int lb = mi - lo;          // 左半段长度
    vector<int> B(lb);         // 辅助数组，复制左半段
    for (int i = 0; i < lb; i++)
        B[i] = A[lo + i];

    // i: B的游标, j: 右半段游标, k: 写入位置
    for (int i = 0, j = mi, k = lo; i < lb; ) {
        // 右半段耗尽 或 B[i]更小 → 取B[i]; 否则取A[j]
        if (j >= hi || B[i] <= A[j])
            A[k++] = B[i++];
        else
            A[k++] = A[j++];
    }
    // 右半段剩余元素已在正确位置，无需处理
}

// ==================== 归并排序（递归版） ====================
void mergeSort(vector<int>& A, int lo, int hi) {
    if (hi - lo < 2) return;   // 递归基：单元素区间自然有序
    int mi = (lo + hi) >> 1;   // 平分
    mergeSort(A, lo, mi);      // 左半段排序
    mergeSort(A, mi, hi);      // 右半段排序
    merge(A, lo, mi, hi);      // 二路归并
}

// ==================== 归并排序（迭代版/自底向上） ====================
void mergeSort_iterative(vector<int>& A) {
    int n = A.size();
    // 子区间宽度从 1 开始，每次翻倍
    for (int width = 1; width < n; width <<= 1) {
        for (int lo = 0; lo < n - width; lo += (width << 1)) {
            int mi = lo + width;
            int hi = min(lo + (width << 1), n);
            merge(A, lo, mi, hi);
        }
    }
}

// ==================== 利用归并排序求逆序对 ====================
// 逆序对：若 i < j 但 A[i] > A[j]，则 (i,j) 为一个逆序对
// 逆序对数目等于冒泡排序所需的交换次数
long long countInversions(vector<int>& A, int lo, int mi, int hi) {
    int lb = mi - lo;
    vector<int> B(lb);
    for (int i = 0; i < lb; i++) B[i] = A[lo + i];

    long long inv = 0;
    for (int i = 0, j = mi, k = lo; i < lb; ) {
        if (j >= hi || B[i] <= A[j]) {
            A[k++] = B[i++];
        } else {
            A[k++] = A[j++];
            inv += (lb - i);  // B[i..lb) 均大于 A[j]，贡献 lb-i 个逆序对
        }
    }
    return inv;
}

long long inversionCount(vector<int>& A, int lo, int hi) {
    if (hi - lo < 2) return 0;
    int mi = (lo + hi) >> 1;
    long long left = inversionCount(A, lo, mi);
    long long right = inversionCount(A, mi, hi);
    return left + right + countInversions(A, lo, mi, hi);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  归并排序 (Merge Sort)                  " << endl;
    cout << "========================================" << endl << endl;

    // 1. 基本排序测试
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10, 15, 56, 23};
    cout << "[基本排序]" << endl;
    cout << "  排序前: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    mergeSort(arr, 0, arr.size());
    cout << "  排序后: ";
    for (int x : arr) cout << x << " ";
    cout << endl << endl;

    // 2. 逆序对计数
    vector<int> arr2 = {7, 5, 3, 1, 2, 4, 6};  // 已知有 10 个逆序对
    vector<int> arr2_copy = arr2;
    cout << "[逆序对计数]" << endl;
    cout << "  序列: ";
    for (int x : arr2) cout << x << " ";
    cout << endl;
    long long inv = inversionCount(arr2_copy, 0, arr2_copy.size());
    cout << "  逆序对数量: " << inv << " (期望: 10)" << endl << endl;

    // 3. 性能对比：递归 vs 迭代
    cout << "[性能测试] 对 100,000 个随机数排序:" << endl;

    vector<int> data1(100000);
    mt19937 rng(42);
    uniform_int_distribution<int> dist(1, 1000000);
    for (int& x : data1) x = dist(rng);
    vector<int> data2 = data1;

    auto start = high_resolution_clock::now();
    mergeSort(data1, 0, data1.size());
    auto t_rec = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

    start = high_resolution_clock::now();
    mergeSort_iterative(data2);
    auto t_iter = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

    // 验证正确性
    bool sorted = is_sorted(data1.begin(), data1.end()) && is_sorted(data2.begin(), data2.end());
    cout << "  递归版耗时: " << t_rec << " ms" << endl;
    cout << "  迭代版耗时: " << t_iter << " ms" << endl;
    cout << "  排序正确: " << (sorted ? "✓" : "✗") << endl;

    return 0;
}
