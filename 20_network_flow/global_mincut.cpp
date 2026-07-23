/**
 * Stoer-Wagner 全局最小割算法
 *
 * 与 s-t 最小割 (Ford-Fulkerson/Dinic) 不同, 全局最小割
 * 是在整个图中找容量最小的割 (不指定源汇)。
 *
 * Stoer-Wagner 算法 (1995):
 *   通过反复找"最小割阶段" (Minimum Cut Phase) 合并顶点,
 *   在 O(VE + V² log V) 时间内找到全局最小割。
 *
 * 核心思想:
 *   Minimum Cut Phase:
 *     1. 从任意点开始, 每次选与已选集合连接最"强"的点加入
 *        (类似 Prim MST, 但以边权和为键)
 *     2. 最后加入的两个点 s, t: 它们之间的割 = 所有边权和
 *        (从 t 到已选集合的边权和)
 *     3. 这个割是 {t} 与其余点的割, 称为"阶段割"
 *
 *   Merge:
 *     全局最小割一定是某一次 Minimum Cut Phase 产生的阶段割!
 *     因此:
 *       1. 做一次 Phase, 记录阶段割值
 *       2. 将最后两个点 s, t 合并为一个点
 *       3. 重复 V-1 次
 *       4. 所有阶段割的最小值 = 全局最小割
 *
 * 复杂度: O(V³) 朴素, O(VE + V² log V) 用优先队列
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

const int INF = 1e9;

// Stoer-Wagner 全局最小割 O(V³)
int globalMinCut(vector<vector<int>> adj) {
    int n = adj.size();
    if (n <= 1) return 0;

    vector<int> verts(n);  // 合并后的顶点映射
    for (int i = 0; i < n; i++) verts[i] = i;

    int bestCut = INF;

    for (int phase = n; phase > 1; phase--) {
        vector<int> weight(phase, 0);  // 到已选集合的边权和
        vector<bool> selected(phase, false);
        int prev = -1, cur = 0;  // 最后加入的两个顶点

        for (int i = 0; i < phase; i++) {
            // 选 weight 最大的未选点
            int pick = -1;
            for (int j = 0; j < phase; j++) {
                if (selected[j]) continue;
                if (pick == -1 || weight[j] > weight[pick])
                    pick = j;
            }

            if (i == phase - 1) {
                // 最后一步: 阶段割 = weight[pick]
                bestCut = min(bestCut, weight[pick]);

                // 合并 prev 和 pick
                for (int j = 0; j < phase; j++) {
                    adj[pick][j] += adj[prev][j];
                    adj[j][pick] = adj[pick][j];
                }
                // 移除 prev (将其置换到最后)
                for (int j = 0; j < phase; j++) {
                    swap(adj[j][prev], adj[j][phase - 1]);
                }
                for (int j = 0; j < phase; j++) {
                    swap(adj[prev][j], adj[phase - 1][j]);
                }
            }

            prev = cur;
            cur = pick;
            selected[pick] = true;

            for (int j = 0; j < phase; j++)
                if (!selected[j])
                    weight[j] += adj[pick][j];
        }
    }
    return bestCut;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Stoer-Wagner 全局最小割                 " << endl;
    cout << "========================================" << endl << endl;

    // 无向图: 邻接矩阵
    vector<vector<int>> adj = {
        {0, 3, 1, 0, 0},
        {3, 0, 3, 2, 0},
        {1, 3, 0, 4, 3},
        {0, 2, 4, 0, 2},
        {0, 0, 3, 2, 0}
    };

    int minCut = globalMinCut(adj);
    cout << "[全局最小割] " << minCut << " (期望: 3)" << endl;

    cout << endl << "[算法对比]" << endl;
    cout << "  s-t最小割 (Dinic):  O(V²E), 需要指定源汇" << endl;
    cout << "  全局最小割 (Stoer): O(V³), 不指定源汇" << endl;
    cout << "  Karger 随机化算法: O(V² log³ V) 期望" << endl;

    return 0;
}
