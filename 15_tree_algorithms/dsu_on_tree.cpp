/**
 * DSU on Tree (树上启发式合并 / Sack)
 *
 * 又名 "dsu on tree" 或 "Sack 算法", 是解决子树查询问题的
 * 高效技巧, 由阿里土创团队命名。
 *
 * 核心思想:
 *   对每个节点, 保留其重儿子 (size 最大的子节点) 的贡献,
 *   轻儿子的贡献暴力计算后丢弃, 从而每个节点最多被计算
 *   O(log n) 次 (因为每次合并轻儿子, 集合大小至少翻倍)。
 *
 * 典型问题:
 *   1. 每棵子树中不同颜色/值的个数
 *   2. 每棵子树中出现次数最多的颜色
 *   3. 子树中的众数和
 *
 * 算法步骤:
 *   1. 预处理: DFS 计算每个结点的 size 和重儿子 (bigChild)
 *   2. 处理过程 (dfs2):
 *      - 递归处理所有轻儿子, 每次处理完清除贡献
 *      - 递归处理重儿子, 保留贡献
 *      - 暴力加入轻儿子的所有结点
 *      - 记录当前结点的答案
 *      - 如果当前结点是父亲的轻儿子, 清除所有贡献
 *
 * 复杂度: O(n log n), 简单且常数小
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class DSUonTree {
private:
    int n;
    vector<vector<int>> g;
    vector<int> color;        // 每个结点的颜色
    vector<int> size, bigChild;
    vector<int> cnt;          // 颜色频率
    vector<int> answer;       // 每个结点的答案
    int maxFreq = 0;          // 当前最多出现次数
    int sumMaxFreq = 0;       // 最大频率颜色的编号和

    // 预处理: 计算 size 和重儿子
    void dfsSize(int u, int parent) {
        size[u] = 1;
        bigChild[u] = -1;
        int maxSz = 0;

        for (int v : g[u]) {
            if (v == parent) continue;
            dfsSize(v, u);
            size[u] += size[v];
            if (size[v] > maxSz) {
                maxSz = size[v];
                bigChild[u] = v;
            }
        }
    }

    // 暴力加入/移除某结点的子树
    void add(int u, int parent, int delta) {
        int c = color[u];
        cnt[c] += delta;

        if (delta == 1 && cnt[c] >= maxFreq) {
            if (cnt[c] > maxFreq) {
                maxFreq = cnt[c];
                sumMaxFreq = 0;
            }
            sumMaxFreq += c;
        }

        for (int v : g[u]) {
            if (v == parent) continue;
            add(v, u, delta);
        }
    }

    // DSU on Tree
    void dfs(int u, int parent, bool keep) {
        // 1. 先处理轻儿子 (不保留)
        for (int v : g[u]) {
            if (v == parent || v == bigChild[u]) continue;
            dfs(v, u, false);
        }

        // 2. 处理重儿子 (保留贡献)
        if (bigChild[u] != -1)
            dfs(bigChild[u], u, true);

        // 3. 暴力加入轻儿子
        for (int v : g[u]) {
            if (v == parent || v == bigChild[u]) continue;
            add(v, u, 1);
        }

        // 4. 加入当前结点
        int c = color[u];
        cnt[c]++;
        if (cnt[c] >= maxFreq) {
            if (cnt[c] > maxFreq) {
                maxFreq = cnt[c];
                sumMaxFreq = 0;
            }
            sumMaxFreq += c;
        }

        // 5. 记录答案
        answer[u] = sumMaxFreq;

        // 6. 如果是轻儿子, 清除贡献
        if (!keep) {
            add(u, parent, -1);
            maxFreq = 0;
            sumMaxFreq = 0;
        }
    }

public:
    DSUonTree(int n) : n(n), g(n), color(n), size(n), bigChild(n),
                       cnt(100, 0), answer(n, 0) {}

    void addEdge(int u, int v) {
        g[u].push_back(v);
        g[v].push_back(u);
    }

    void setColor(int node, int c) { color[node] = c; }

    vector<int> solve() {
        dfsSize(0, -1);
        dfs(0, -1, true);
        return answer;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  DSU on Tree (树上启发式合并)             " << endl;
    cout << "========================================" << endl << endl;

    int n = 6;
    DSUonTree ds(n);

    // 树结构
    ds.addEdge(0, 1); ds.addEdge(0, 2);
    ds.addEdge(1, 3); ds.addEdge(1, 4);
    ds.addEdge(2, 5);

    // 颜色: 1, 2, 1, 2, 3, 1
    ds.setColor(0, 1);
    ds.setColor(1, 2);
    ds.setColor(2, 1);
    ds.setColor(3, 2);
    ds.setColor(4, 3);
    ds.setColor(5, 1);

    auto ans = ds.solve();
    cout << "[每棵子树众数和]" << endl;
    for (int i = 0; i < n; i++)
        cout << "  结点" << i << ": " << ans[i] << endl;

    cout << endl << "[DSU on Tree 关键点]" << endl;
    cout << "  重儿子保留, 轻儿子清空" << endl;
    cout << "  每个结点最多被加入 O(log n) 次" << endl;
    cout << "  复杂度: O(n log n), 常数很小" << endl;

    return 0;
}
