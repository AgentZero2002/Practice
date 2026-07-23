/**
 * 向量去重算法（唯一化）
 *
 * 对应教材 2.4 节：有序向量的唯一化
 *
 * 演示:
 *   1. 无序向量去重：O(n²)，逐个检查前缀
 *   2. 有序向量唯一化（低效版）：O(n²)，逐元素删除
 *   3. 有序向量唯一化（高效版）：O(n)，双指针扫描
 */

#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;
using namespace std::chrono;

// ==================== 无序向量去重 O(n²) ====================
// 从头开始，检查当前元素是否在前缀中出现过
int deduplicate_unsorted(vector<int>& A) {
    int oldSize = A.size();
    int i = 1;
    while (i < (int)A.size()) {
        // 检查 A[i] 是否在 A[0..i) 中出现过
        bool found = false;
        for (int j = 0; j < i; j++) {
            if (A[j] == A[i]) { found = true; break; }
        }
        if (found)
            A.erase(A.begin() + i);  // 删除重复元素
        else
            i++;
    }
    return oldSize - A.size();
}

// ==================== 有序向量去重 — 低效版 O(n²) ====================
// 发现相邻重复就删除（每次删除 O(n)）
int uniquify_slow(vector<int>& A) {
    int oldSize = A.size();
    int i = 0;
    while (i < (int)A.size() - 1) {
        if (A[i] == A[i + 1])
            A.erase(A.begin() + i + 1);  // 每删除一个元素，O(n) 移动
        else
            i++;
    }
    return oldSize - A.size();
}

// ==================== 有序向量去重 — 高效版 O(n) ====================
// 双指针：i 指向已去重部分的末尾，j 扫描未处理元素
// 发现不同元素就向前搬移
int uniquify_fast(vector<int>& A) {
    int oldSize = A.size();
    if (A.empty()) return 0;
    int i = 0, j = 0;
    while (++j < (int)A.size())
        if (A[i] != A[j])          // 发现异于 A[i] 的新元素
            A[++i] = A[j];         // 向前搬移
    A.resize(++i);                 // 截断尾部
    return oldSize - i;
}

// ==================== 使用 Hash Set 去重 O(n) ====================
// 借助哈希集合，可处理无序，但需要额外 O(n) 空间
int deduplicate_hash(vector<int>& A) {
    unordered_set<int> seen;
    int i = 0;
    int oldSize = A.size();
    while (i < (int)A.size()) {
        if (seen.count(A[i]))
            A.erase(A.begin() + i);
        else {
            seen.insert(A[i]);
            i++;
        }
    }
    return oldSize - A.size();
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  向量去重算法 (Deduplication)           " << endl;
    cout << "========================================" << endl << endl;

    // 1. 无序去重
    {
        cout << "[无序向量去重]" << endl;
        vector<int> A = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9};
        cout << "  原始 (" << A.size() << " 项): ";
        for (int x : A) cout << x << " ";
        cout << endl;
        int cnt = deduplicate_unsorted(A);
        cout << "  去重 (" << A.size() << " 项, 移除 " << cnt << "): ";
        for (int x : A) cout << x << " ";
        cout << endl << endl;
    }

    // 2. 有序去重对比
    {
        cout << "[有序向量唯一化 — 两种实现对比]" << endl;
        vector<int> arr1 = {1, 1, 2, 2, 3, 3, 3, 5, 5, 8, 8, 8, 13};
        vector<int> arr2 = arr1;

        cout << "  原始 (" << arr1.size() << " 项): ";
        for (int x : arr1) cout << x << " ";
        cout << endl;

        int c1 = uniquify_slow(arr1);
        cout << "  低效版O(n²): " << arr1.size() << " 项, 移除 " << c1 << endl;

        int c2 = uniquify_fast(arr2);
        cout << "  高效版O(n):  " << arr2.size() << " 项, 移除 " << c2 << endl;
        cout << "  (结果一致: " << (arr1 == arr2 ? "✓" : "✗") << ")" << endl << endl;
    }

    // 3. Hash 去重
    {
        cout << "[Hash Set 去重 — O(n) 时间 + O(n) 空间]" << endl;
        vector<int> A = {7, 3, 8, 3, 6, 7, 9, 1, 6, 8};
        cout << "  原始 (" << A.size() << " 项): ";
        for (int x : A) cout << x << " ";
        cout << endl;
        int cnt = deduplicate_hash(A);
        cout << "  去重 (" << A.size() << " 项, 移除 " << cnt << "): ";
        for (int x : A) cout << x << " ";
        cout << endl << endl;
    }

    // 4. 性能对比
    cout << "[性能对比] 100,000 个有序元素去重:" << endl;
    vector<int> big(100000);
    for (int i = 0; i < 100000; i++) big[i] = i / 3;  // 大量重复

    {
        vector<int> A = big;
        auto start = high_resolution_clock::now();
        uniquify_fast(A);
        auto t = duration_cast<microseconds>(high_resolution_clock::now() - start).count();
        cout << "  高效版O(n):    " << t << " μs, 结果大小 " << A.size() << endl;
    }
    {
        vector<int> A;
        for (int i = 0; i < 100000; i++) A.push_back(i / 3);
        auto start = high_resolution_clock::now();
        uniquify_slow(A);
        auto t = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
        cout << "  低效版O(n²):  " << t << " ms, 结果大小 " << A.size() << endl;
    }

    return 0;
}
