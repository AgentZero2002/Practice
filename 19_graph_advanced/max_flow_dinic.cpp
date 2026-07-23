/**
 * Dinic 最大流算法
 *
 * 网络流是图论中的经典问题：在一个带容量的有向图中，
 * 从源点 s 到汇点 t 的最大流量是多少？
 *
 * Dinic 算法 = BFS分层 + DFS增广 + 当前弧优化
 * 时间复杂度: O(V²E)，实际远快于此
 *
 * 核心步骤:
 *   1. BFS 构建层次图 (level graph)
 *   2. DFS 在层次图中找增广路 (blocking flow)
 *   3. 重复以上直到 s 无法到达 t
 *
 * 相关应用:
 *   - 二分图最大匹配 → 转化为最大流
 *   - 最小割 = 最大流 (Max-Flow Min-Cut 定理)
 *   - 点容量 → 拆点
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstring>
using namespace std;

class Dinic {
private:
    struct Edge {
        int to, rev;      // 目标节点, 反向边在邻接表中的索引
        long long cap;    // 容量
    };

    int n;
    vector<vector<Edge>> graph;
    vector<int> level;     // BFS 层次
    vector<int> iter;      // DFS 当前弧优化

    // BFS: 构建层次图
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (const Edge& e : graph[v]) {
                if (e.cap > 0 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] >= 0;  // 是否能到达 t
    }

    // DFS: 在层次图中找增广路
    long long dfs(int v, int t, long long f) {
        if (v == t) return f;

        for (int& i = iter[v]; i < (int)graph[v].size(); i++) {
            Edge& e = graph[v][i];
            if (e.cap > 0 && level[v] < level[e.to]) {
                long long d = dfs(e.to, t, min(f, e.cap));
                if (d > 0) {
                    e.cap -= d;                        // 正向边减容量
                    graph[e.to][e.rev].cap += d;       // 反向边加容量
                    return d;
                }
            }
        }
        return 0;
    }

public:
    Dinic(int n) : n(n), graph(n), level(n), iter(n) {}

    void addEdge(int from, int to, long long cap) {
        graph[from].push_back({to, (int)graph[to].size(), cap});
        graph[to].push_back({from, (int)graph[from].size() - 1, 0});
    }

    // 计算从 s 到 t 的最大流
    long long maxFlow(int s, int t) {
        long long flow = 0;
        while (bfs(s, t)) {
            fill(iter.begin(), iter.end(), 0);
            long long f;
            while ((f = dfs(s, t, LLONG_MAX)) > 0)
                flow += f;
        }
        return flow;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Dinic 最大流 (Max Flow)                " << endl;
    cout << "========================================" << endl << endl;

    // 经典网络流示例
    //   s→a [10], s→b [10]
    //   a→b [2], a→c [4], a→d [8]
    //   b→d [9]
    //   c→t [10]
    //   d→c [6], d→t [10]
    Dinic dinic(6);  // 0=s, 1=a, 2=b, 3=c, 4=d, 5=t

    dinic.addEdge(0, 1, 10); dinic.addEdge(0, 2, 10);
    dinic.addEdge(1, 2, 2);  dinic.addEdge(1, 3, 4);
    dinic.addEdge(1, 4, 8);  dinic.addEdge(2, 4, 9);
    dinic.addEdge(3, 5, 10); dinic.addEdge(4, 3, 6);
    dinic.addEdge(4, 5, 10);

    cout << "[网络流]" << endl;
    cout << "  s → a(10), s → b(10)" << endl;
    cout << "  a → b(2), a → c(4), a → d(8)" << endl;
    cout << "  b → d(9)" << endl;
    cout << "  c → t(10), d → c(6), d → t(10)" << endl;
    cout << endl;

    long long flow = dinic.maxFlow(0, 5);
    cout << "[结果] 最大流 = " << flow << " (期望: 24)" << endl;

    cout << endl << "[Max-Flow Min-Cut 定理]" << endl;
    cout << "  最大流 = 最小割的容量" << endl;
    cout << "  这是图论中最深刻的定理之一" << endl;
    cout << endl;
    cout << "[Dinic 算法要素]" << endl;
    cout << "  1. BFS 分层: 确保增广沿最短路径" << endl;
    cout << "  2. DFS 增广: 一次找多条增广路" << endl;
    cout << "  3. 当前弧优化: 避免重复检查已满的边" << endl;

    return 0;
}
