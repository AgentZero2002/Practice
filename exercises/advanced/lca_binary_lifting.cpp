/**
 * 倍增法求 LCA (Binary Lifting)
 *
 * 给定一棵树，多次查询任意两个节点的最近公共祖先 (LCA)。
 *
 * 倍增法 (Binary Lifting):
 *   1. DFS 预处理: up[v][i] = 节点 v 向上 2^i 步到达的节点
 *   2. 查询: 先将较深的节点跳到与较浅节点同一深度
 *           然后两节点一起向上跳跃
 *   3. up[root][i] = root (作为哨兵)
 *
 * 预处理: O(n log n)
 * 单次查询: O(log n)
 *
 * vs 标准 LCA 方法:
 *   - 递归法: O(n) 单次查询
 *   - 倍增法: O(log n) 查询, 适合多次查询
 *   - Tarjan 离线: O(n+q) 总时间
 *   - RMQ(ST表): O(n log n) 预处理, O(1) 查询
 */

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class LCA {
private:
    int n, LOG;
    vector<vector<int>> up;   // up[v][i]
    vector<int> depth;
    vector<vector<int>> tree;

    void dfs(int v, int parent) {
        up[v][0] = (parent == -1 ? v : parent);

        for (int i = 1; i < LOG; i++)
            up[v][i] = up[up[v][i - 1]][i - 1];

        for (int u : tree[v]) {
            if (u == parent) continue;
            depth[u] = depth[v] + 1;
            dfs(u, v);
        }
    }

public:
    LCA(int n) : n(n) {
        LOG = ceil(log2(n)) + 1;
        up.assign(n, vector<int>(LOG));
        depth.assign(n, 0);
        tree.resize(n);
    }

    void addEdge(int u, int v) {
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    void build(int root = 0) {
        dfs(root, -1);
    }

    int query(int u, int v) {
        // 确保 depth[u] >= depth[v]
        if (depth[u] < depth[v]) swap(u, v);

        // 将 u 提升到与 v 同一深度
        int diff = depth[u] - depth[v];
        for (int i = 0; i < LOG; i++)
            if (diff & (1 << i))
                u = up[u][i];

        if (u == v) return u;

        // 两节点一起向上跳跃
        for (int i = LOG - 1; i >= 0; i--) {
            if (up[u][i] != up[v][i]) {
                u = up[u][i];
                v = up[v][i];
            }
        }

        return up[u][0];  // 父节点即为 LCA
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  倍增法求 LCA (Binary Lifting)          " << endl;
    cout << "========================================" << endl << endl;

    // 构建测试树
    //         0
    //       /   \
    //      1     2
    //     / \   / \
    //    3   4 5   6
    //   /
    //  7
    int n = 8;
    LCA lca(n);
    lca.addEdge(0, 1); lca.addEdge(0, 2);
    lca.addEdge(1, 3); lca.addEdge(1, 4);
    lca.addEdge(2, 5); lca.addEdge(2, 6);
    lca.addEdge(3, 7);

    lca.build(0);

    cout << "[测试树]" << endl;
    cout << "         0" << endl;
    cout << "       /   \\" << endl;
    cout << "      1     2" << endl;
    cout << "     / \\   / \\" << endl;
    cout << "    3   4 5   6" << endl;
    cout << "   /" << endl;
    cout << "  7" << endl << endl;

    cout << "[LCA 查询]" << endl;
    vector<tuple<int, int, int>> queries = {
        {3, 4, 1},  // LCA(3,4)=1
        {3, 5, 0},  // LCA(3,5)=0
        {7, 4, 1},  // LCA(7,4)=1
        {5, 6, 2},  // LCA(5,6)=2
        {7, 7, 7},  // LCA(7,7)=7
    };

    cout << "  u | v | LCA | 期望" << endl;
    cout << "  " << string(25, '-') << endl;
    for (auto [u, v, expected] : queries) {
        int result = lca.query(u, v);
        cout << "  " << u << " | " << v << " | "
             << setw(3) << result << " | " << setw(3) << expected
             << (result == expected ? " ✓" : " ✗") << endl;
    }

    cout << endl << "[方法对比]" << endl;
    cout << "  递归法:     O(n)/查询, 适合单次查询" << endl;
    cout << "  倍增法:     O(log n)/查询, 适合多次查询" << endl;
    cout << "  Tarjan离线: O(n+q), 查询需预先知道" << endl;
    cout << "  ST表+欧拉序: O(1)/查询, O(n log n) 预处理" << endl;

    return 0;
}
