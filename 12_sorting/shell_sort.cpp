/**
 * 希尔排序 (Shell Sort)
 *
 * 对应教材 12.3 节：希尔排序
 *
 * 希尔排序是插入排序的改进版：通过"递减增量"策略
 * 使元素每次大幅度跳跃，逐步接近最终位置。
 *
 * 核心思想:
 *   1. 使用一个递减的步长序列 (gap sequence)
 *   2. 对每个步长 h，对 h 个子序列分别做插入排序
 *   3. 最后 h=1 时等价于标准插入排序，但此时数组已"大致有序"
 *
 * 步长序列选择:
 *   Shell 原始: n/2, n/4, ..., 1          (最坏 O(n²))
 *   Hibbard:    1, 3, 7, 15, ..., 2^k-1  (最坏 O(n^(3/2)))
 *   Sedgewick:  1, 5, 19, 41, 109, ...   (最坏 O(n^(4/3)))
 *
 * 空间复杂度: O(1)，不稳定排序
 */

#include <iostream>
#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ==================== 希尔排序（Shell 步长序列） ====================
void shellSort(vector<int>& A) {
    int n = A.size();

    // Shell 原始步长: n/2, n/4, ..., 1
    for (int gap = n / 2; gap > 0; gap /= 2) {
        // 对每个子序列做插入排序
        for (int i = gap; i < n; i++) {
            int temp = A[i];
            int j;
            for (j = i; j >= gap && A[j - gap] > temp; j -= gap) {
                A[j] = A[j - gap];
            }
            A[j] = temp;
        }
    }
}

// ==================== 希尔排序（Hibbard 步长序列） ====================
void shellSortHibbard(vector<int>& A) {
    int n = A.size();

    // 生成 Hibbard 步长: 1, 3, 7, 15, ..., 2^k - 1
    int gap = 1;
    while (gap * 2 + 1 < n)
        gap = gap * 2 + 1;

    for (; gap > 0; gap = (gap - 1) / 2) {
        for (int i = gap; i < n; i++) {
            int temp = A[i];
            int j;
            for (j = i; j >= gap && A[j - gap] > temp; j -= gap)
                A[j] = A[j - gap];
            A[j] = temp;
        }
    }
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  希尔排序 (Shell Sort)                  " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {12, 34, 54, 2, 3, 7, 8, 90, 56, 45, 67, 89, 1};

    cout << "[希尔排序 Shell 步长序列]" << endl;
    cout << "  排序前: ";
    for (int x : arr) cout << x << " ";

    shellSort(arr);
    cout << endl << "  排序后: ";
    for (int x : arr) cout << x << " ";
    cout << endl << endl;

    // 演示步长序列
    cout << "[步长序列对比]" << endl;
    int n = 100;
    cout << "  n=" << n << " 时各序列步长:" << endl;

    cout << "  Shell:   ";
    for (int g = n/2; g > 0; g /= 2) cout << g << " ";
    cout << endl;

    cout << "  Hibbard: ";
    int g = 1;
    while (g * 2 + 1 < n) g = g * 2 + 1;
    for (; g > 0; g = (g - 1) / 2) cout << g << " ";
    cout << endl;
    cout << endl;

    // 性能对比
    cout << "[性能测试] 10,000 个随机数:" << endl;
    vector<int> data(10000);
    for (int i = 0; i < 10000; i++) data[i] = rand() % 10000;

    {
        vector<int> d = data;
        auto start = high_resolution_clock::now();
        shellSort(d);
        auto t = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
        cout << "  Shell:   " << t << " ms" << endl;
    }

    {
        vector<int> d = data;
        auto start = high_resolution_clock::now();
        shellSortHibbard(d);
        auto t = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
        cout << "  Hibbard: " << t << " ms" << endl;
    }

    return 0;
}
