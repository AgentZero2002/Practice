/**
 * 虚树 (Virtual Tree / Auxiliary Tree)
 *
 * 虚树是处理"树上多次查询, 每次只涉及少量关键点"问题的技巧。
 * 它只保留关键点和它们的 LCA, 将规模从 O(n) 压缩至 O(k log k)。
 *
 * 应用场景:
 *   每次查询给定 k 个关键点 (Σk ≤ 3e5),
 *   需要对这些点做树上 DP。
 *   例: 每次查询给定一些城市, 求封锁这些城市的最小代价。
 *
 * 构建虚树 O(k log n):
 *   1. 将关键点按 DFS 序排序
 *   2. 维护一个栈, 从根到当前点的路径
 *   3. 对每个关键点:
 *      - lca = LCA(key, stack.top())
 *      - 弹出栈直到栈顶深度 ≤ lca 深度
 *      - 如果栈顶 != lca, 将 lca 入栈
 *      - key 入栈
 *   4. 栈中剩余点构成虚树的根→...链
 *
 * 关键性质:
 *   - 虚树的节点数 ≤ 2k (k为关键点数)
 *   - 虚树保留了关键点之间的祖先后代关系
 *   - 边权通常为原树两点间路径上的某些聚合值
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

// 原树
vector<vector<int>> g;
vector<int> depth, parent, inTime;
vector<vector<int>> up;  // 倍增 LCA
int timer = 0;

void dfsInit(int u, int p) {
    inTime[u] = timer++;
    up[u][0] = p;
    for (int i = 1; i < 20; i++)
        up[u][i] = up[up[u][i-1]][i-1];
    for (int v : g[u]) {
        if (v == p) continue;
        depth[v] = depth[u] + 1;
        dfsInit(v, u);
    }
}

int lca(int u, int v) {
    if (depth[u] < depth[v]) swap(u, v);
    int diff = depth[u] - depth[v];
    for (int i = 0; i < 20; i++)
        if (diff & (1 << i)) u = up[u][i];
    if (u == v) return u;
    for (int i = 19; i >= 0; i--)
        if (up[u][i] != up[v][i])
            u = up[u][i], v = up[v][i];
    return up[u][0];
}

// 构建虚树: 返回虚树的邻接表
// 关键点按 DFS 序给出
vector<vector<int>> buildVirtualTree(vector<int>& keys) {
    sort(keys.begin(), keys.end(), [&](int a, int b) {
        return inTime[a] < inTime[b];
    });

    vector<int> stk;  // 栈: 从根到当前节点的路径
    vector<vector<int>> vtree(keys.size() + 10);

    auto addEdge = [&](int u, int v) {
        vtree[u].push_back(v);
        vtree[v].push_back(u);
    };

    stk.push_back(keys[0]);
    for (size_t i = 1; i < keys.size(); i++) {
        int u = keys[i];
        int anc = lca(u, stk.back());

        if (anc != stk.back()) {
            while (stk.size() >= 2 && depth[stk[stk.size()-2]] >= depth[anc]) {
                addEdge(stk.back(), stk[stk.size()-2]);
                stk.pop_back();
            }
            if (stk.back() != anc) {
                addEdge(stk.back(), anc);
                stk.pop_back();
                stk.push_back(anc);
            }
        }
        stk.push_back(u);
    }
    while (stk.size() >= 2) {
        addEdge(stk.back(), stk[stk.size()-2]);
        stk.pop_back();
    }
    return vtree;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  虚树 (Virtual Tree)                     " << endl;
    cout << "========================================" << endl << endl;

    int n = 7;
    g.resize(n);
    depth.resize(n);
    inTime.resize(n);
    up.assign(n, vector<int>(20, 0));

    // 建树
    g[0] = {1, 2};
    g[1] = {0, 3, 4};
    g[2] = {0, 5, 6};
    depth[0] = 0;
    dfsInit(0, 0);

    // 关键点: 3, 4, 6
    vector<int> keys = {3, 4, 6};
    auto vtree = buildVirtualTree(keys);

    cout << "[关键点] 3, 4, 6" << endl;
    cout << "[虚树边] (含LCA)" << endl;
    for (int i = 0; i < (int)vtree.size(); i++)
        for (int j : vtree[i])
            if (i < j)
                cout << "  " << i << " — " << j << endl;

    cout << endl << "[虚树性质]" << endl;
    cout << "  节点数 ≤ 2k (k=3, 最多6个节点)" << endl;
    cout << "  保留关键点间祖先后代关系" << endl;
    cout << "  配合树形DP: O(k log k) vs O(n)" << endl;

    return 0;
}
