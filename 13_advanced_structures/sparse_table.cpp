/**
 * Sparse Table (ST表) — 静态 RMQ (Range Minimum Query)
 *
 * ST 表利用倍增思想，在 O(n log n) 预处理后，
 * 可实现 O(1) 的区间最小值（或最大值/GCD等）查询。
 *
 * 核心思想:
 *   st[i][k] = min(range[i .. i+2^k-1])
 *   即从位置 i 开始、长度为 2^k 的区间的最小值
 *
 * 递推: st[i][k] = min(st[i][k-1], st[i+2^(k-1)][k-1])
 * 查询: min(A[l..r]) = min(st[l][k], st[r-2^k+1][k])
 *       其中 k = floor(log₂(r-l+1))
 *
 * 限制: 不支持修改（静态结构），需要修改请用线段树
 *
 * 应用:
 *   - LCA (加上欧拉环游序 + 深度)
 *   - 区间 GCD 查询
 *   - 任意可重复贡献操作的静态区间查询
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

class SparseTable {
private:
    vector<vector<int>> st;    // st[i][k]
    vector<int> log2;           // 预计算 log2 表

public:
    SparseTable(const vector<int>& arr) {
        int n = arr.size();

        // 预计算 log2[1..n]
        log2.resize(n + 1);
        log2[1] = 0;
        for (int i = 2; i <= n; i++)
            log2[i] = log2[i / 2] + 1;

        int K = log2[n] + 1;  // 最大幂次
        st.assign(n, vector<int>(K));

        // k=0: 长度 2^0 = 1 的区间就是元素本身
        for (int i = 0; i < n; i++)
            st[i][0] = arr[i];

        // k>0: 合并两个长度为 2^(k-1) 的区间
        for (int k = 1; k < K; k++) {
            for (int i = 0; i + (1 << k) <= n; i++) {
                st[i][k] = min(st[i][k - 1],
                               st[i + (1 << (k - 1))][k - 1]);
            }
        }
    }

    // O(1) 查询区间 [l, r] 的最小值
    int queryMin(int l, int r) {
        int k = log2[r - l + 1];
        // 用两个可能有重叠的区间覆盖 [l, r]
        return min(st[l][k], st[r - (1 << k) + 1][k]);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Sparse Table (ST表) — 静态 RMQ        " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {7, 2, 3, 0, 5, 8, 1, 4, 6, 3};
    cout << "[测试数组] ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    SparseTable st(arr);

    cout << endl << "[区间最小值查询 (O(1))]" << endl;
    vector<pair<int, int>> queries = {{0, 3}, {2, 6}, {4, 9}, {0, 9}};
    for (auto [l, r] : queries) {
        int val = st.queryMin(l, r);
        cout << "  min[" << l << ".." << r << "] = " << val;
        // 用暴力验证
        int brute = *min_element(arr.begin() + l, arr.begin() + r + 1);
        cout << (val == brute ? " ✓" : " ✗") << endl;
    }

    cout << endl << "[ST 表特性]" << endl;
    cout << "  预处理: O(n log n)" << endl;
    cout << "  查询:   O(1)" << endl;
    cout << "  空间:   O(n log n)" << endl;
    cout << "  限制:   不支持修改 (静态)" << endl;
    cout << "  适用:   任意满足 f(a,a)=a 的幂等操作 " << endl;
    cout << "         (min, max, gcd, 按位与/或等)" << endl;

    return 0;
}
