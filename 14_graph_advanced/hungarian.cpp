/**
 * 匈牙利算法 (Hungarian Algorithm)
 * — 二分图最大匹配
 *
 * 给定二分图 G = (U, V, E)，求最大匹配（选取尽可能多的边，
 * 使得任意两条边没有公共顶点）。
 *
 * 匈牙利算法核心思想:
 *   交替路 (Alternating Path): 交替经过匹配边和非匹配边
 *   增广路 (Augmenting Path): 首尾都是未匹配点的交替路
 *
 *   找到增广路 → 翻转路径上所有边的匹配状态 → 匹配数 +1
 *
 * 时间复杂度: O(VE)，即 O(n·m) 其中 n = |U|, m = |E|
 *
 * 转化: 二分图最大匹配 → 最大流 (加超级源汇)
 *       Dinic 复杂度 O(E√V)，对单位容量网络更优
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Hungarian {
private:
    int nL, nR;                          // 左右两边大小
    vector<vector<int>> graph;           // 左→右的边
    vector<int> matchR;                  // 右部节点匹配的左部节点 (-1=未匹配)
    vector<bool> visited;                // DFS 访问标记

    // 尝试为左部节点 v 寻找增广路
    bool dfs(int v) {
        for (int u : graph[v]) {
            if (visited[u]) continue;
            visited[u] = true;

            // u 未匹配 或 u 的当前匹配者可以换到别的点
            if (matchR[u] == -1 || dfs(matchR[u])) {
                matchR[u] = v;  // 建立匹配 v ↔ u
                return true;
            }
        }
        return false;
    }

public:
    Hungarian(int leftSize, int rightSize)
        : nL(leftSize), nR(rightSize), graph(leftSize) {}

    void addEdge(int left, int right) {
        graph[left].push_back(right);
    }

    int maxMatching() {
        matchR.assign(nR, -1);
        int matches = 0;

        for (int v = 0; v < nL; v++) {
            visited.assign(nR, false);
            if (dfs(v)) matches++;
        }

        return matches;
    }

    // 返回具体的匹配: {左节点, 右节点}
    vector<pair<int, int>> getMatches() const {
        vector<pair<int, int>> result;
        for (int u = 0; u < nR; u++)
            if (matchR[u] != -1)
                result.push_back({matchR[u], u});
        return result;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  匈牙利算法 — 二分图最大匹配            " << endl;
    cout << "========================================" << endl << endl;

    // 经典问题: 工人分配任务
    // 工人 {0,1,2,3} (左), 任务 {0,1,2,3} (右)
    Hungarian h(4, 4);
    h.addEdge(0, 0); h.addEdge(0, 1);
    h.addEdge(1, 0); h.addEdge(1, 2);
    h.addEdge(2, 1); h.addEdge(2, 2); h.addEdge(2, 3);
    h.addEdge(3, 2);

    cout << "[工人-任务匹配]" << endl;
    cout << "  工人0 可做: 任务0, 任务1" << endl;
    cout << "  工人1 可做: 任务0, 任务2" << endl;
    cout << "  工人2 可做: 任务1, 任务2, 任务3" << endl;
    cout << "  工人3 可做: 任务2" << endl;
    cout << endl;

    int matches = h.maxMatching();
    cout << "[结果] 最大匹配数: " << matches << " (期望: 4)" << endl;

    auto matchList = h.getMatches();
    cout << "  匹配: ";
    for (auto [w, t] : matchList)
        cout << "工人" << w << "→任务" << t << "  ";
    cout << endl;

    cout << endl << "[Kőnig 定理]" << endl;
    cout << "  二分图的最大匹配数 = 最小顶点覆盖数" << endl;
    cout << "  这不等式是组合优化中的经典对偶关系" << endl;

    return 0;
}
