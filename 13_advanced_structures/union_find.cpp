/**
 * 并查集 (Disjoint Set Union / Union-Find)
 *
 * 并查集维护一组不相交的集合，支持:
 *   - find(x): 查找元素 x 所属集合的代表元（根）
 *   - unite(x,y): 合并包含 x 和 y 的两个集合
 *   - same(x,y): 判断 x 和 y 是否在同一集合
 *
 * 两大优化:
 *   1. 路径压缩 (Path Compression):
 *      find 时沿路将节点直接连到根，使得后续 find 更快
 *   2. 按秩合并 (Union by Rank):
 *      合并时将矮树挂到高树下，保持树的高度为 O(log n)
 *
 * 结合两种优化后，每次操作的均摊复杂度 ≈ O(α(n))
 * α(n) 是 Ackermann 函数的反函数，对任何实际 n < 2^65536，
 * α(n) < 5，可视为常数时间。
 *
 * 经典应用:
 *   - Kruskal MST (已在 06_graph/mst_kruskal.cpp 中使用)
 *   - 连通分量统计
 *   - 动态连通性
 *   - 朋友圈/岛屿问题
 */

#include <iostream>
#include <vector>
using namespace std;

class UnionFind {
private:
    vector<int> parent;  // parent[i] = i 的父亲，若 parent[i]=i 则为根
    vector<int> rank;    // rank[i] = 以 i 为根的树的高度上界
    int _count;          // 连通分量数

public:
    UnionFind(int n) : parent(n), rank(n, 0), _count(n) {
        for (int i = 0; i < n; i++)
            parent[i] = i;  // 初始每个元素自成一个集合
    }

    // 查找（含路径压缩）
    int find(int x) {
        if (parent[x] != x)
            parent[x] = find(parent[x]);  // 路径压缩
        return parent[x];
    }

    // 合并（按秩合并）
    bool unite(int x, int y) {
        int rx = find(x), ry = find(y);
        if (rx == ry) return false;  // 已在同一集合

        // 将矮的树挂到高的树下
        if (rank[rx] < rank[ry])
            parent[rx] = ry;
        else if (rank[rx] > rank[ry])
            parent[ry] = rx;
        else {
            parent[ry] = rx;
            rank[rx]++;  // 等高时，新树高度+1
        }
        _count--;
        return true;
    }

    // 判断连通性
    bool same(int x, int y) {
        return find(x) == find(y);
    }

    // 连通分量数
    int count() const { return _count; }
};

// ==================== 应用: 朋友圈 (LeetCode 547) ====================
int findCircleNum(const vector<vector<int>>& M) {
    int n = M.size();
    UnionFind uf(n);

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (M[i][j] == 1)
                uf.unite(i, j);

    return uf.count();
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  并查集 (Union-Find / DSU)              " << endl;
    cout << "  — 路径压缩 + 按秩合并                   " << endl;
    cout << "========================================" << endl << endl;

    // 基本操作演示
    UnionFind uf(10);
    cout << "[基本操作]" << endl;

    uf.unite(0, 1); uf.unite(1, 2);
    uf.unite(3, 4); uf.unite(5, 6); uf.unite(6, 7);
    uf.unite(0, 3);  // 合并两个大集合

    cout << "  合并: {0,1,2} + {3,4} + {5,6,7}, 再合并 {0,3}" << endl;
    cout << "  find(2)=" << uf.find(2) << " find(4)=" << uf.find(4) << endl;
    cout << "  same(2,4): " << (uf.same(2, 4) ? "是" : "否") << endl;
    cout << "  same(2,5): " << (uf.same(2, 5) ? "是" : "否") << endl;
    cout << "  连通分量数: " << uf.count() << endl << endl;

    // 朋友圈
    cout << "[应用: 朋友圈问题]" << endl;
    vector<vector<int>> friends = {
        {1, 1, 0},
        {1, 1, 0},
        {0, 0, 1}
    };
    cout << "  (0-1 是朋友, 2 独立)" << endl;
    cout << "  朋友圈数: " << findCircleNum(friends) << " (期望: 2)" << endl;

    cout << endl << "[复杂度分析]" << endl;
    cout << "  路径压缩 + 按秩合并: O(α(n)) ≈ O(1) 均摊" << endl;
    cout << "  仅路径压缩: O(log n) 均摊" << endl;
    cout << "  无优化: O(n) 最坏 (退化为链)" << endl;

    return 0;
}
