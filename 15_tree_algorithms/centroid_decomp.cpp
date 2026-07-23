/**
 * 点分治 (Centroid Decomposition) — 树上路径统计
 *
 * 点分治是解决树上路径问题的经典分治框架。
 * 将树按重心分解, 递归处理经过重心的路径和子树内路径。
 *
 * 核心思想:
 *   1. 找树的重心 (Centroid): 删除该点后, 每棵子树大小 ≤ n/2
 *   2. 统计"经过重心"的路径
 *   3. 删除重心, 对剩余连通块递归
 *
 * 重心性质:
 *   - 任意树至少有一个重心 (可能两个)
 *   - 重心将树分成大小不超过 n/2 的若干子树
 *   - 递归深度 O(log n) (每次规模减半)
 *
 * 经典问题:
 *   1. 树上距离 = k 的点对数量
 *   2. 树上距离 ≤ k 的点对数量
 *   3. 树上是否存在距离 = k 的路径
 *
 * 整体复杂度: O(n log n) 或 O(n log² n), 取决于统计步骤
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

class CentroidDecomposition {
private:
    int n;
    vector<vector<pair<int,int>>> g;  // {to, weight}
    vector<bool> removed;
    vector<int> subtreeSize;

    // 计算子树大小
    void calcSize(int u, int parent) {
        subtreeSize[u] = 1;
        for (auto& [v, w] : g[u]) {
            if (v == parent || removed[v]) continue;
            calcSize(v, u);
            subtreeSize[u] += subtreeSize[v];
        }
    }

    // 找重心
    int findCentroid(int u, int parent, int total) {
        for (auto& [v, w] : g[u]) {
            if (v == parent || removed[v]) continue;
            if (subtreeSize[v] > total / 2)
                return findCentroid(v, u, total);
        }
        return u;
    }

    // 收集重心子树中的距离
    void collectDist(int u, int parent, int depth,
                     vector<int>& dists) {
        dists.push_back(depth);
        for (auto& [v, w] : g[u]) {
            if (v == parent || removed[v]) continue;
            collectDist(v, u, depth + w, dists);
        }
    }

    // 统计经过重心的路径中, 距离 = k 的个数
    int countPaths(int u, int k) {
        vector<int> allDists = {0};  // 所有子树距离

        int ans = 0;
        for (auto& [v, w] : g[u]) {
            if (removed[v]) continue;
            vector<int> subDists;
            collectDist(v, u, w, subDists);

            // 统计: subDists 中的距离与 allDists 中的距离之和 = k
            sort(subDists.begin(), subDists.end());
            sort(allDists.begin(), allDists.end());

            for (int d : subDists) {
                int need = k - d;
                ans += upper_bound(allDists.begin(), allDists.end(), need)
                     - lower_bound(allDists.begin(), allDists.end(), need);
            }

            // 合并到 allDists
            allDists.insert(allDists.end(), subDists.begin(), subDists.end());
        }
        return ans;
    }

    int decompose(int u, int k) {
        calcSize(u, -1);
        int centroid = findCentroid(u, -1, subtreeSize[u]);

        int ans = countPaths(centroid, k);
        removed[centroid] = true;

        for (auto& [v, w] : g[centroid]) {
            if (!removed[v])
                ans += decompose(v, k);
        }
        return ans;
    }

public:
    CentroidDecomposition(int n) : n(n), g(n), removed(n, false),
                                   subtreeSize(n, 0) {}

    void addEdge(int u, int v, int w) {
        g[u].push_back({v, w});
        g[v].push_back({u, w});
    }

    int countDistK(int k) {
        return decompose(0, k);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  点分治 (Centroid Decomposition)         " << endl;
    cout << "========================================" << endl << endl;

    int n = 7;
    CentroidDecomposition cd(n);
    // 构建一棵树
    cd.addEdge(0, 1, 1);
    cd.addEdge(0, 2, 2);
    cd.addEdge(1, 3, 3);
    cd.addEdge(1, 4, 1);
    cd.addEdge(2, 5, 2);
    cd.addEdge(2, 6, 1);

    int k = 3;
    cout << "[树上路径 = " << k << " 的点对] "
         << cd.countDistK(k) << " 对" << endl;

    cout << endl << "[点分治核心]" << endl;
    cout << "  1. 找重心 O(n)" << endl;
    cout << "  2. 统计经过重心的路径" << endl;
    cout << "  3. 递归分解, 深度 O(log n)" << endl;
    cout << "  总体 O(n log n) 或 O(n log² n)" << endl;

    return 0;
}
