/**
 * 树链剖分 (Heavy-Light Decomposition, HLD)
 *
 * HLD 将树分解为若干条"重链"，使得任意节点到根的路径
 * 被分解为 O(log n) 段连续的链。
 *
 * 配合线段树，可实现:
 *   - 树上路径点权更新/查询 O(log² n)
 *   - 子树更新/查询 O(log n)
 *
 * 核心概念:
 *   - 重儿子 (heavy child): 子树 size 最大的孩子
 *   - 轻儿子 (light child): 其余孩子
 *   - 重链: 由重儿子连接而成的链
 *   - 关键性质: 任意点到根路径上轻边数 ≤ log n
 *
 * 应用: 树剖+LCA, 树剖+线段树(树路径修改查询)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class HLD {
private:
    int n;
    vector<vector<int>> tree;

    vector<int> parent, depth, size;   // DFS1
    vector<int> heavy;                  // heavy[v] = v 的重儿子
    vector<int> head, pos;             // DFS2: 链头, DFS序位置
    int curPos;

    // 第一遍 DFS: 计算 parent, depth, size, heavy
    int dfs1(int v, int p, int d) {
        parent[v] = p;
        depth[v] = d;
        size[v] = 1;
        int maxSize = 0;

        for (int u : tree[v]) {
            if (u == p) continue;
            int subSize = dfs1(u, v, d + 1);
            size[v] += subSize;
            if (subSize > maxSize) {
                maxSize = subSize;
                heavy[v] = u;
            }
        }
        return size[v];
    }

    // 第二遍 DFS: 分配 DFS 序和链头
    void dfs2(int v, int h) {
        head[v] = h;
        pos[v] = curPos++;
        cout << "    pos[" << v << "]=" << pos[v] << ", head=" << h << endl;

        if (heavy[v] != -1)
            dfs2(heavy[v], h);  // 重儿子继续同一条链

        for (int u : tree[v]) {
            if (u == parent[v] || u == heavy[v]) continue;
            dfs2(u, u);  // 轻儿子开启新链
        }
    }

public:
    HLD(int n) : n(n), tree(n), parent(n), depth(n), size(n),
                 heavy(n, -1), head(n), pos(n), curPos(0) {}

    void addEdge(int u, int v) {
        tree[u].push_back(v);
        tree[v].push_back(u);
    }

    void build(int root = 0) {
        dfs1(root, -1, 0);
        dfs2(root, root);
    }

    // 查询 u 到 v 路径 (配合线段树使用)
    // 返回路径被分解成的若干区间 [pos[l], pos[r]]
    vector<pair<int, int>> queryPath(int u, int v) {
        vector<pair<int, int>> segments;
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            segments.push_back({pos[head[u]], pos[u]});  // 跳轻链
            u = parent[head[u]];
        }
        if (depth[u] > depth[v]) swap(u, v);
        segments.push_back({pos[u], pos[v]});  // 同一条链
        return segments;
    }

    // LCA via HLD
    int lca(int u, int v) {
        while (head[u] != head[v]) {
            if (depth[head[u]] < depth[head[v]]) swap(u, v);
            u = parent[head[u]];
        }
        return depth[u] < depth[v] ? u : v;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  树链剖分 (Heavy-Light Decomposition)   " << endl;
    cout << "========================================" << endl << endl;

    // 示例树
    //     0
    //    / \
    //   1   2
    //  /|\
    // 3 4 5
    int n = 6;
    HLD hld(n);
    hld.addEdge(0, 1); hld.addEdge(0, 2);
    hld.addEdge(1, 3); hld.addEdge(1, 4); hld.addEdge(1, 5);

    cout << "[DFS 序分配]" << endl;
    hld.build(0);

    cout << endl << "[路径分解]" << endl;
    auto segs = hld.queryPath(3, 2);
    cout << "  路径 3→2 被分解为:" << endl;
    for (auto [l, r] : segs)
        cout << "    区间 [" << l << ", " << r << "]" << endl;

    cout << endl << "[LCA via HLD]" << endl;
    cout << "  LCA(3,4) = " << hld.lca(3, 4) << " (期望: 1)" << endl;
    cout << "  LCA(3,2) = " << hld.lca(3, 2) << " (期望: 0)" << endl;

    cout << endl << "[关键复杂度]" << endl;
    cout << "  任意点到根: O(log n) 条链" << endl;
    cout << "  路径查询:   O(log² n) 配合线段树" << endl;

    return 0;
}
