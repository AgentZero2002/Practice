/**
 * 锦标赛排序 (Tournament Sort) + 排序对比
 *
 * 对应教材 12 章：排序
 *
 * 锦标赛排序（胜者树）:
 *   - 构建一棵完全二叉树，叶节点为待排序元素
 *   - 两两比较，胜者晋级，直到根为最小值
 *   - 类似体育竞赛淘汰制
 *   - 时间复杂度: O(n log n)
 *
 * 本文件同时提供多个排序算法的性能对比。
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <climits>
using namespace std;
using namespace std::chrono;

// ==================== 锦标赛排序 ====================
// 使用胜者树 (Winner Tree) 实现
class TournamentSort {
private:
    vector<int> tree;  // 胜者树（2*leaves - 1 个节点）
    int n;             // 叶子节点数

    // 比赛：比较两个叶子，记录胜者
    int play(int left, int right) {
        if (tree[left] <= tree[right])
            return tree[left];
        return tree[right];
    }

public:
    void sort(vector<int>& arr) {
        int m = arr.size();
        // 扩展到 2 的幂
        n = 1;
        while (n < m) n <<= 1;

        // 构建胜者树
        tree.assign(2 * n, INT_MAX);
        for (int i = 0; i < m; i++)
            tree[n + i] = arr[i];

        // 从叶子到根进行比赛
        for (int i = n - 1; i > 0; i--)
            tree[i] = min(tree[2 * i], tree[2 * i + 1]);

        // 循环输出最小值
        for (int i = 0; i < m; i++) {
            arr[i] = tree[1];  // 当前最小值
            // 找到最小值的叶子位置
            int idx = 1;
            while (idx < n)
                idx = 2 * idx + (tree[2 * idx + 1] == tree[1] ? 1 : 0);

            tree[idx] = INT_MAX;  // 移除该元素

            // 从该叶子上溯更新路径
            while (idx > 1) {
                idx >>= 1;
                tree[idx] = min(tree[2 * idx], tree[2 * idx + 1]);
            }
        }
    }
};

// ==================== 各排序实现 ====================
void bubbleSort(vector<int>& A) {
    int n = A.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; j++) {
            if (A[j] > A[j + 1]) {
                swap(A[j], A[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void insertionSort(vector<int>& A) {
    for (int i = 1; i < (int)A.size(); i++) {
        int key = A[i], j = i - 1;
        while (j >= 0 && A[j] > key) {
            A[j + 1] = A[j];
            j--;
        }
        A[j + 1] = key;
    }
}

void selectionSort(vector<int>& A) {
    for (int i = 0; i < (int)A.size() - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < (int)A.size(); j++)
            if (A[j] < A[minIdx]) minIdx = j;
        swap(A[i], A[minIdx]);
    }
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  排序算法性能对比                       " << endl;
    cout << "========================================" << endl << endl;

    // 锦标赛排序演示
    cout << "[锦标赛排序]" << endl;
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10, 15};
    cout << "  原始: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    TournamentSort ts;
    ts.sort(arr);
    cout << "  排序: ";
    for (int x : arr) cout << x << " ";
    cout << " | " << (is_sorted(arr.begin(), arr.end()) ? "✓" : "✗") << endl << endl;

    // ==================== 综合性能对比 ====================
    cout << "[性能对比] 对 N=5000 个随机数排序:" << endl;

    const int N = 5000;
    vector<int> original(N);
    mt19937 rng(42);
    uniform_int_distribution<int> dist(1, 10000);
    for (int& x : original) x = dist(rng);

    // STL sort as baseline
    vector<int> data = original;
    auto start = high_resolution_clock::now();
    sort(data.begin(), data.end());
    auto baseline = duration_cast<microseconds>(high_resolution_clock::now() - start).count();

    struct SortEntry {
        string name;
        function<void(vector<int>&)> sortFn;
    };

    vector<SortEntry> sorts = {
        {"std::sort",       [](auto& v) { sort(v.begin(), v.end()); }},
        {"std::stable_sort",[](auto& v) { stable_sort(v.begin(), v.end()); }},
        {"冒泡排序",        bubbleSort},
        {"选择排序",        selectionSort},
        {"插入排序",        insertionSort},
        {"锦标赛排序",      [&ts](auto& v) { ts.sort(v); }},
    };

    cout << "  " << left << setw(18) << "算法"
         << setw(12) << right << "耗时"
         << setw(10) << "相对std"
         << "  验证" << endl;
    cout << "  " << string(50, '-') << endl;

    for (auto& entry : sorts) {
        vector<int> d = original;
        auto t0 = high_resolution_clock::now();
        entry.sortFn(d);
        auto t = duration_cast<microseconds>(high_resolution_clock::now() - t0).count();
        bool ok = is_sorted(d.begin(), d.end());

        cout << "  " << left << setw(18) << entry.name
             << setw(10) << right << t << " μs"
             << setw(8) << right << fixed << setprecision(1)
             << (double)t / baseline << "x"
             << "  " << (ok ? "✓" : "✗") << endl;
    }

    // 排序总结表
    cout << endl << "[排序算法总结]" << endl;
    cout << "  算法         | 平均      | 最坏      | 空间  | 稳定" << endl;
    cout << "  " << string(55, '-') << endl;
    cout << "  冒泡排序     | O(n²)     | O(n²)     | O(1)  | ✓" << endl;
    cout << "  选择排序     | O(n²)     | O(n²)     | O(1)  | ✗" << endl;
    cout << "  插入排序     | O(n²)     | O(n²)     | O(1)  | ✓" << endl;
    cout << "  归并排序     | O(n log n)| O(n log n)| O(n)  | ✓" << endl;
    cout << "  快速排序     | O(n log n)| O(n²)     | O(log n)| ✗" << endl;
    cout << "  堆排序       | O(n log n)| O(n log n)| O(1)  | ✗" << endl;
    cout << "  希尔排序     | ~O(n^1.3) | O(n²)     | O(1)  | ✗" << endl;
    cout << "  基数排序     | O(d·n)    | O(d·n)    | O(n+k)| ✓" << endl;
    cout << "  锦标赛排序   | O(n log n)| O(n log n)| O(n)  | ✗" << endl;

    return 0;
}
