/**
 * 二维树状数组 (2D Fenwick Tree / 2D BIT)
 *
 * 将树状数组推广到二维，实现矩阵子区域的前缀和/单点更新。
 *
 * 操作:
 *   add(x, y, delta): 在 (x,y) 处加上 delta, O(log² n)
 *   sum(x, y): 查询 (1,1) 到 (x,y) 的子矩阵和, O(log² n)
 *   rangeSum(x1,y1, x2,y2): 任意子矩阵和 O(log² n)
 *
 * 经典应用:
 *   - 二维点更新 + 矩形区域和
 *   - 动态二维平面的逆序点对统计
 */

#include <iostream>
#include <vector>
using namespace std;

class Fenwick2D {
private:
    int n, m;
    vector<vector<long long>> tree;

public:
    Fenwick2D(int n, int m) : n(n), m(m), tree(n + 2, vector<long long>(m + 2, 0)) {}

    // 在 (x, y) 处加上 delta (1-indexed)
    void add(int x, int y, long long delta) {
        for (int i = x; i <= n; i += i & (-i))
            for (int j = y; j <= m; j += j & (-j))
                tree[i][j] += delta;
    }

    // 查询矩形 (1,1) → (x,y) 的和
    long long sum(int x, int y) {
        long long result = 0;
        for (int i = x; i > 0; i -= i & (-i))
            for (int j = y; j > 0; j -= j & (-j))
                result += tree[i][j];
        return result;
    }

    // 查询任意子矩形 (x1,y1) → (x2,y2) 的和
    long long rangeSum(int x1, int y1, int x2, int y2) {
        return sum(x2, y2) - sum(x1 - 1, y2)
             - sum(x2, y1 - 1) + sum(x1 - 1, y1 - 1);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二维树状数组 (2D BIT)                   " << endl;
    cout << "========================================" << endl << endl;

    int n = 4, m = 4;
    Fenwick2D bit(n, m);

    // 初始化矩阵
    int mat[4][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 10, 11, 12},
        {13, 14, 15, 16}
    };

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            bit.add(i + 1, j + 1, mat[i][j]);

    cout << "[矩阵]" << endl;
    for (int i = 0; i < n; i++) {
        cout << "  ";
        for (int j = 0; j < m; j++)
            cout << setw(3) << mat[i][j];
        cout << endl;
    }

    cout << endl << "[子矩阵和查询]" << endl;
    cout << "  sum(1,1)→(3,3) = " << bit.rangeSum(1, 1, 3, 3)
         << " (期望: 1+2+3+5+6+7+9+10+11=54)" << endl;
    cout << "  sum(2,2)→(4,4) = " << bit.rangeSum(2, 2, 4, 4)
         << " (期望: 6+7+8+10+11+12+14+15+16=99)" << endl;

    // 更新
    cout << endl << "[更新] mat[2][2] += 10" << endl;
    bit.add(2, 2, 10);
    cout << "  sum(1,1)→(3,3) = " << bit.rangeSum(1, 1, 3, 3)
         << " (期望: 54+10=64)" << endl;

    return 0;
}
