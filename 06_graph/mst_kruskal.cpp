/**
 * Kruskal 最小生成树算法
 *
 * 对应教材 6.8 节：最小支撑树
 *
 * Kruskal 算法思想:
 *   1. 将所有边按权重从小到大排序
 *   2. 依次检查每条边：若加入后不构成环，则加入 MST
 *   3. 直到 MST 有 n-1 条边
 *
 * 判环使用并查集 (Union-Find / Disjoint Set Union)
 *
 * 时间复杂度: O(E log E) ≈ O(E log V)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== 并查集 (Union-Find) ====================
class UnionFind {
private:
    vector<int> parent;
    vector<int> rank;

public:
    UnionFind(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; i++) parent[i] = i;
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
        if (rx == ry) return false;  // 已在同一集合 → 加入此边会成环

        if (rank[rx] < rank[ry])
            parent[rx] = ry;
        else if (rank[rx] > rank[ry])
            parent[ry] = rx;
        else {
            parent[ry] = rx;
            rank[rx]++;
        }
        return true;
    }
};

// ==================== Kruskal 算法 ====================
struct Edge {
    int u, v, weight;
    Edge(int u, int v, int w) : u(u), v(v), weight(w) {}

    bool operator<(const Edge& e) const {
        return weight < e.weight;
    }
};

// 返回 MST 的边和总权重
pair<vector<Edge>, int> kruskalMST(int n, vector<Edge>& edges) {
    // 1. 按权重排序
    sort(edges.begin(), edges.end());

    UnionFind uf(n);
    vector<Edge> mstEdges;
    int totalWeight = 0;

    // 2. 依次检查每条边
    for (const Edge& e : edges) {
        if (uf.unite(e.u, e.v)) {
            mstEdges.push_back(e);
            totalWeight += e.weight;
            if ((int)mstEdges.size() == n - 1) break;  // MST 已完成
        }
    }

    return {mstEdges, totalWeight};
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Kruskal 最小生成树算法                 " << endl;
    cout << "========================================" << endl << endl;

    int n = 7;
    vector<Edge> edges = {
        {0, 1, 7}, {0, 3, 5}, {1, 2, 8}, {1, 3, 9},
        {1, 4, 7}, {2, 4, 5}, {3, 4, 15}, {3, 5, 6},
        {4, 5, 8}, {4, 6, 9}, {5, 6, 11}
    };

    cout << "[无向带权图] 与 Prim 示例相同" << endl;
    cout << "  所有边 (按输入顺序):" << endl;
    for (auto& e : edges)
        cout << "    " << e.u << " — " << e.v << " : " << e.weight << endl;

    cout << endl << "[排序后]" << endl;
    sort(edges.begin(), edges.end());
    cout << "  ";
    for (auto& e : edges)
        cout << "(" << e.u << "-" << e.v << ":" << e.weight << ") ";
    cout << endl << endl;

    // 注意: 上面已经 sort 了一次，kruskalMST 内部又 sort 了一次
    // 为演示正确，重新初始化
    vector<Edge> edges2 = {
        {0, 1, 7}, {0, 3, 5}, {1, 2, 8}, {1, 3, 9},
        {1, 4, 7}, {2, 4, 5}, {3, 4, 15}, {3, 5, 6},
        {4, 5, 8}, {4, 6, 9}, {5, 6, 11}
    };

    auto [mstEdges, totalWeight] = kruskalMST(n, edges2);

    cout << "[Kruskal 最小生成树]" << endl;
    cout << "  选中的边:" << endl;
    for (auto& e : mstEdges)
        cout << "    " << e.u << " — " << e.v << " : " << e.weight << endl;
    cout << "  总权重: " << totalWeight << endl;

    cout << endl << "[Prim vs Kruskal]" << endl;
    cout << "  Prim:   从顶点出发，适合稠密图 (O(V²) 朴素版)" << endl;
    cout << "  Kruskal: 从边出发，适合稀疏图 (O(E log E))" << endl;
    cout << "  两者都是贪心算法，均可得到最优解" << endl;

    return 0;
}
