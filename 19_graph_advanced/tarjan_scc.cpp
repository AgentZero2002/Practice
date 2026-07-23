/**
 * Tarjan 强连通分量 (Strongly Connected Components)
 *
 * 强连通分量 (SCC): 有向图中的一个极大子图，
 * 其中任意两个顶点互相可达。
 *
 * Tarjan 算法 (单次 DFS):
 *   使用 dTime[] (发现时间) 和 low[] (可达的最小 dTime)
 *   当 dTime[v] == low[v] 时，v 是一个 SCC 的根
 *   将栈中的节点弹出直到 v，构成一个 SCC
 *
 * 时间复杂度: O(V + E)
 *
 * 应用: 2-SAT问题, 有向图缩点后变为DAG, 社交网络分析
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

class TarjanSCC {
private:
    int n;
    vector<vector<int>> graph;
    vector<int> dTime, low;
    vector<bool> onStack;
    stack<int> stk;
    int timer;
    vector<vector<int>> sccs;  // 所有 SCC

    void dfs(int v) {
        dTime[v] = low[v] = ++timer;
        stk.push(v);
        onStack[v] = true;

        for (int u : graph[v]) {
            if (dTime[u] == 0) {    // 树边
                dfs(u);
                low[v] = min(low[v], low[u]);
            } else if (onStack[u]) { // 后向边/交叉边 (仍在栈中)
                low[v] = min(low[v], dTime[u]);
            }
        }

        // 若 v 是 SCC 的根
        if (dTime[v] == low[v]) {
            vector<int> scc;
            int u;
            do {
                u = stk.top(); stk.pop();
                onStack[u] = false;
                scc.push_back(u);
            } while (u != v);
            sccs.push_back(scc);
        }
    }

public:
    TarjanSCC(int n, const vector<vector<int>>& g) : n(n), graph(g) {}

    vector<vector<int>> findSCCs() {
        dTime.assign(n, 0);
        low.assign(n, 0);
        onStack.assign(n, false);
        timer = 0;
        sccs.clear();

        for (int v = 0; v < n; v++)
            if (dTime[v] == 0)
                dfs(v);

        return sccs;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Tarjan 强连通分量 (SCC)                 " << endl;
    cout << "========================================" << endl << endl;

    // 示例有向图
    // 0→1→3→4→0 (SCC1: 0,1,3,4)
    //   ↓
    //   2 → 5 → 2    (SCC2: 2,5)
    int n = 6;
    vector<vector<int>> g(n);
    g[0].push_back(1);
    g[1].push_back(3);
    g[3].push_back(4);
    g[4].push_back(0);  // 形成环 0→1→3→4→0
    g[0].push_back(2);
    g[2].push_back(5);
    g[5].push_back(2);  // 形成环 2→5→2

    TarjanSCC ts(n, g);
    auto sccs = ts.findSCCs();

    cout << "[SCC 结果]" << endl;
    for (size_t i = 0; i < sccs.size(); i++) {
        cout << "  SCC #" << i + 1 << ": { ";
        for (int v : sccs[i]) cout << v << " ";
        cout << "}" << endl;
    }
    cout << "  共 " << sccs.size() << " 个强连通分量" << endl;

    cout << endl << "[算法关键]" << endl;
    cout << "  dTime: DFS 首次发现时间" << endl;
    cout << "  low:   通过树边 + 至多一次回边能到达的最小 dTime" << endl;
    cout << "  dTime[v] == low[v] ⟺ v 是此 SCC 的根" << endl;
    cout << endl;
    cout << "  [应用] 缩点后得到 DAG，可做拓扑排序+DP" << endl;

    return 0;
}
