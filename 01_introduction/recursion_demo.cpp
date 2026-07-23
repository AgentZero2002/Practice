/**
 * 递归与分治示例
 *
 * 对应教材 1.4 节：迭代与递归
 *
 * 经典示例:
 *   1. 汉诺塔（Tower of Hanoi）- 典型分治 O(2^n)
 *   2. 最大子序列和 - 四种解法对比：O(n³)/O(n²)/O(n log n)/O(n)
 *   3. 幂运算 - 快速幂 vs 普通递归
 */

#include <iostream>
#include <vector>
#include <climits>
#include <cmath>
using namespace std;

// ==================== 汉诺塔 ====================
// 将 n 个盘子从 src 借助 aux 移动到 dst
// 时间复杂度 O(2^n), 空间复杂度 O(n) 递归栈
// 这是最优解：n 个盘子的最少移动次数为 2^n - 1

void hanoi(int n, char src, char aux, char dst, int& moves) {
    if (n <= 0) return;
    hanoi(n - 1, src, dst, aux, moves);  // 将 n-1 个盘移到辅助柱
    cout << "  Step " << ++moves << ": 盘 " << n << "  "
         << src << " → " << dst << endl;
    hanoi(n - 1, aux, src, dst, moves);  // 将 n-1 个盘从辅助柱移到目标柱
}

// ==================== 最大子序列和 (Maximum Subarray Sum) ====================

// 解法一：暴力枚举 O(n³)
int max_subarray_bruteforce(const vector<int>& a) {
    int n = a.size(), maxSum = 0;
    for (int i = 0; i < n; i++)
        for (int j = i; j < n; j++) {
            int sum = 0;
            for (int k = i; k <= j; k++) sum += a[k];
            if (sum > maxSum) maxSum = sum;
        }
    return maxSum;
}

// 解法二：前缀和优化 O(n²)
int max_subarray_prefix(const vector<int>& a) {
    int n = a.size(), maxSum = 0;
    for (int i = 0; i < n; i++) {
        int sum = 0;
        for (int j = i; j < n; j++) {
            sum += a[j];
            if (sum > maxSum) maxSum = sum;
        }
    }
    return maxSum;
}

// 解法三：分治法 O(n log n)
int max_crossing(const vector<int>& a, int lo, int mi, int hi) {
    int leftSum = 0, rightSum = 0;
    for (int i = mi - 1, sum = 0; i >= lo; i--) {
        sum += a[i];
        if (sum > leftSum) leftSum = sum;
    }
    for (int i = mi, sum = 0; i < hi; i++) {
        sum += a[i];
        if (sum > rightSum) rightSum = sum;
    }
    return leftSum + rightSum;
}

int max_subarray_dc(const vector<int>& a, int lo, int hi) {
    if (hi - lo < 2) return max(0, a[lo]);  // 单个元素
    int mi = (lo + hi) / 2;
    int left = max_subarray_dc(a, lo, mi);
    int right = max_subarray_dc(a, mi, hi);
    int cross = max_crossing(a, lo, mi, hi);
    return max({left, right, cross});
}

int max_subarray_divide(const vector<int>& a) {
    return max_subarray_dc(a, 0, a.size());
}

// 解法四：贪心/在线算法 O(n) — Kadane's Algorithm
int max_subarray_kadane(const vector<int>& a) {
    int maxSum = 0, curSum = 0;
    for (int x : a) {
        curSum = max(0, curSum + x);
        maxSum = max(maxSum, curSum);
    }
    return maxSum;
}

// ==================== 快速幂 ====================
// 普通递归幂: O(n)
long long power_naive(long long base, int exp) {
    if (exp == 0) return 1;
    return base * power_naive(base, exp - 1);
}

// 快速幂(分治): O(log n)
// 核心思想: a^n = { (a^(n/2))^2        when n is even
//                 { a * (a^(n/2))^2    when n is odd
long long power_fast(long long base, int exp) {
    if (exp == 0) return 1;
    long long half = power_fast(base, exp / 2);
    return (exp % 2 == 0) ? half * half : half * half * base;
}

// 迭代版快速幂（更常用）
long long power_iterative(long long base, int exp) {
    long long result = 1;
    while (exp > 0) {
        if (exp & 1) result *= base;  // 当前二进制位为1
        base *= base;
        exp >>= 1;
    }
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  递归与分治示例                        " << endl;
    cout << "========================================" << endl << endl;

    // 1. 汉诺塔
    cout << "[汉诺塔] 3个盘子: A → C (辅助 B)" << endl;
    int moves = 0;
    hanoi(3, 'A', 'B', 'C', moves);
    cout << "  共 " << moves << " 步 (理论最少: 2^3 - 1 = 7)" << endl;
    cout << "  64个盘子理论最少步数: 2^64 - 1 ≈ 1.84 × 10^19" << endl;
    cout << "  （假如每秒搬一次，需要约 5800 亿年）" << endl << endl;

    // 2. 最大子序列和
    vector<int> arr = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    cout << "[最大子序列和] 序列: ";
    for (int x : arr) cout << x << " ";
    cout << endl << endl;
    cout << "  O(n³) 暴力:   " << max_subarray_bruteforce(arr) << endl;
    cout << "  O(n²) 前缀和: " << max_subarray_prefix(arr) << endl;
    cout << "  O(n log n) 分治: " << max_subarray_divide(arr) << endl;
    cout << "  O(n) Kadane:  " << max_subarray_kadane(arr) << endl;
    cout << "  正确答案: [4, -1, 2, 1] 和 = 6" << endl << endl;

    // 3. 快速幂
    cout << "[快速幂] 计算 2^10:" << endl;
    cout << "  普通递归: " << power_naive(2, 10) << endl;
    cout << "  快速幂:   " << power_fast(2, 10) << endl;
    cout << "  迭代版:   " << power_iterative(2, 10) << endl;
    cout << "  O(n) vs O(log n): 计算 2^1000 时差异巨大" << endl;

    return 0;
}
