/**
 * 二分查找的三种实现版本
 *
 * 对应教材 2.3.3 节：二分查找
 *
 * 版本 A: 三路分支（最直观，但比较次数多）
 * 版本 B: 两路分支（减少比较，但不能提前终止）
 * 版本 C: 两路分支 + 返回不大于 e 的最后一个元素（语义最清晰）
 *
 * 此外还包括 Fibonacci 查找
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
using namespace std;

// ==================== 版本 A: 三路分支 ====================
// 最直观的实现，每次迭代比较 2 次（若命中则 1 次）
// 成功时返回命中秩，失败返回 -1
int binSearch_A(const vector<int>& A, int e, int lo, int hi) {
    while (lo <= hi) {
        int mi = (lo + hi) >> 1;
        if (e < A[mi])
            hi = mi - 1;
        else if (A[mi] < e)
            lo = mi + 1;
        else
            return mi;  // 命中
    }
    return -1;  // 失败
}

// ==================== 版本 B: 两路分支 ====================
// 每次迭代仅比较 1 次，但不能提前终止
// 成功时返回命中秩，失败返回 -1
int binSearch_B(const vector<int>& A, int e, int lo, int hi) {
    while (1 < hi - lo) {    // 区间宽度大于1时继续
        int mi = (lo + hi) >> 1;
        e < A[mi] ? hi = mi : lo = mi;
    }  // 出口时 hi = lo + 1
    return (A[lo] == e) ? lo : -1;
}

// ==================== 版本 C: 两路分支（推荐） ====================
// 语意最清晰：返回不大于 e 的最后一个元素的位置
// 查找失败时返回 -1（若所有元素都大于 e）或合理的插入位置
int binSearch_C(const vector<int>& A, int e, int lo, int hi) {
    while (lo < hi) {
        int mi = (lo + hi) >> 1;
        e < A[mi] ? hi = mi : lo = mi + 1;
    }  // 出口时 lo = hi = 大于 e 的最小元素的秩
    return --lo;  // lo-1 即不大于 e 的最后一个元素
}

// ==================== Fibonacci 查找 ====================
// 当比较操作成本远高于加减法时，更优
// 通过黄金分割比 φ ≈ 1.618 来分割查找区间

class Fib {
    int f, g;  // f = fib(k-1), g = fib(k)
public:
    Fib(int n) {  // 初始化为不小于 n 的最小 Fibonacci 数
        f = 1; g = 0;
        while (g < n) next();
    }
    int get() { return g; }
    int next() { g = g + f; f = g - f; return g; }
    int prev() { f = g - f; g = g - f; return g; }  // fib(k-1)
};

int fibSearch(const vector<int>& A, int e, int lo, int hi) {
    Fib fib(hi - lo);
    while (lo < hi) {
        while (hi - lo < fib.get()) fib.prev();  // 用最近的 fib 数
        int mi = lo + fib.get() - 1;
        if (e < A[mi])
            hi = mi;
        else if (A[mi] < e)
            lo = mi + 1;
        else
            return mi;
    }
    return -1;
}

// ==================== 测试 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二分查找：三种版本 + Fibonacci 查找    " << endl;
    cout << "========================================" << endl << endl;

    // 准备有序向量
    vector<int> A = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    cout << "有序向量 A: ";
    for (int x : A) cout << x << " ";
    cout << endl << endl;

    // 测试查找存在的元素
    int targets[] = {7, 13, 23, 37, 1, 40, 15};
    cout << "查找测试:" << endl;
    cout << "  target | A版  | B版  | C版(≤) | Fib  | 期望" << endl;
    cout << "  " << string(55, '-') << endl;

    for (int e : targets) {
        int rA = binSearch_A(A, e, 0, A.size() - 1);
        int rB = binSearch_B(A, e, 0, A.size());
        int rC = binSearch_C(A, e, 0, A.size());
        int rF = fibSearch(A, e, 0, A.size());

        cout << "  " << setw(6) << e << " | "
             << setw(4) << rA << " | "
             << setw(4) << rB << " | "
             << setw(6) << rC << " | "
             << setw(4) << rF << " |";

        // C 版返回 "不大于 e 的最大秩"
        if (rC >= 0) cout << " A[" << rC << "]=" << A[rC];
        else cout << " (all > " << e << ")";
        cout << endl;
    }

    // 版本语义说明
    cout << endl << "[版本说明]" << endl;
    cout << "  版本A: 找到返回秩，否则返回 -1" << endl;
    cout << "  版本B: 找到返回秩，否则返回 -1" << endl;
    cout << "  版本C: 返回不大于 e 的最大秩（语义最清晰）" << endl;
    cout << "        当 e 小于所有元素时返回 -1" << endl;
    cout << "        这是 STL lower_bound 的前驱版本" << endl;

    return 0;
}
