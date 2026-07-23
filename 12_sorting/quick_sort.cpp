/**
 * 快速排序 (Quick Sort) — 含三路划分
 *
 * 对应教材 12.1 节：快速排序
 *
 * 算法核心: 分治策略
 *   1. 选择一个 pivot (轴点)
 *   2. 分区 (partition): 小于 pivot 的放左边，大于的放右边
 *   3. 递归排序左右两部分
 *
 * 平均: O(n log n)，最坏: O(n²)（已排序 + 固定 pivot）
 *
 * LGU 版分区（邓俊辉教材推荐）:
 *   将整个向量分为 ≤pivot(L) | ≥pivot(G) | 未处理(U) 三部分
 *
 * 三路划分 (Dutch National Flag):
 *   将向量分为 <pivot | =pivot | >pivot 三部分
 *   适合有大量重复元素的输入
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

// ==================== LGU 分区（邓俊辉版） ====================
int partition_LGU(vector<int>& A, int lo, int hi) {
    // 随机选 pivot 并交换到首位（防最坏情况）
    swap(A[lo], A[lo + rand() % (hi - lo)]);
    int pivot = A[lo];

    int mi = lo;  // 已处理区间 [lo+1, mi] 均 ≤ pivot
    for (int k = lo + 1; k < hi; k++) {
        if (A[k] < pivot)
            swap(A[++mi], A[k]);
    }
    swap(A[lo], A[mi]);  // 将 pivot 移到中间位置
    return mi;           // 返回 pivot 的最终秩
}

void quickSort(vector<int>& A, int lo, int hi) {
    if (hi - lo < 2) return;
    int mi = partition_LGU(A, lo, hi);
    quickSort(A, lo, mi);
    quickSort(A, mi + 1, hi);
}

// ==================== 三路划分 (3-Way / Dutch National Flag) ====================
// 高效处理含大量重复元素的数组
void quickSort3Way(vector<int>& A, int lo, int hi) {
    if (hi - lo < 2) return;

    swap(A[lo], A[lo + rand() % (hi - lo)]);
    int pivot = A[lo];

    int lt = lo;       // A[lo..lt) < pivot
    int gt = hi - 1;   // A(gt..hi) > pivot
    int i = lo + 1;    // A[lt..i) == pivot

    while (i <= gt) {
        if (A[i] < pivot)
            swap(A[lt++], A[i++]);
        else if (A[i] > pivot)
            swap(A[i], A[gt--]);
        else
            i++;
    }

    quickSort3Way(A, lo, lt);
    quickSort3Way(A, gt + 1, hi);
}

// ==================== 快速选择 (QuickSelect) ====================
// 在无序数组中找出第 k 小的元素 (0-based)
// 期望 O(n), 最坏 O(n²)（可通过 BFPRT 优化到 O(n) 最坏）
int quickSelect(vector<int>& A, int k) {
    int lo = 0, hi = A.size();
    while (lo < hi) {
        int mi = partition_LGU(A, lo, hi);
        if (k < mi)
            hi = mi;
        else if (k > mi)
            lo = mi + 1;
        else
            return A[mi];
    }
    return -1;  // 不应该到这里
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  快速排序 (Quick Sort)                  " << endl;
    cout << "========================================" << endl << endl;

    // 1. 标准快排
    cout << "[标准快排 LGU 分区]" << endl;
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10, 15, 56};
    cout << "  排序前: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    quickSort(arr, 0, arr.size());
    cout << "  排序后: ";
    for (int x : arr) cout << x << " ";
    cout << " | " << (is_sorted(arr.begin(), arr.end()) ? "✓" : "✗") << endl << endl;

    // 2. 三路划分
    cout << "[三路划分快排] 含大量重复元素" << endl;
    vector<int> arr2 = {4, 2, 4, 2, 4, 2, 1, 4, 3, 4, 1, 4};
    cout << "  排序前: ";
    for (int x : arr2) cout << x << " ";
    cout << endl;

    quickSort3Way(arr2, 0, arr2.size());
    cout << "  排序后: ";
    for (int x : arr2) cout << x << " ";
    cout << " | " << (is_sorted(arr2.begin(), arr2.end()) ? "✓" : "✗") << endl << endl;

    // 3. 快速选择
    cout << "[快速选择 QuickSelect]" << endl;
    vector<int> arr3 = {7, 10, 4, 3, 20, 15, 8, 12, 6};
    cout << "  数组: ";
    for (int x : arr3) cout << x << " ";
    cout << endl;

    for (int k : {0, 2, 4, 6, 8}) {
        vector<int> tmp = arr3;
        int val = quickSelect(tmp, k);
        cout << "  第 " << k << " 小 (0-based): " << val << endl;
    }

    cout << endl << "[快排 vs 归并]" << endl;
    cout << "  快排:    原位，不稳定，实际快（缓存友好）" << endl;
    cout << "  归并:    O(n)辅助空间，稳定" << endl;
    cout << "  堆排序:  原位，不稳定，缓存不友好" << endl;

    return 0;
}
