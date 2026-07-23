/**
 * Floyd-Warshall 全源最短路径算法
 *
 * 对应教材 6.10 节：全源最短路径
 *
 * 计算所有顶点对之间的最短路径。
 * 基于动态规划: dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])
 *
 * 优点: 简单，可处理负权边（但不能有负权环）
 * 时间复杂度: O(V³)
 * 空间复杂度: O(V²)
 */

#include <iostream>
#include <vector>
#include <climits>
#include <iomanip>
using namespace std;

const int INF = INT_MAX / 2;  // 防溢出

// ==================== Floyd-Warshall 算法 ====================
// 输入: 邻接矩阵 (INF 表示无边)
// 输出: 最短距离矩阵 + 前驱矩阵（用于回溯路径）
void floydWarshall(vector<vector<int>>& dist, vector<vector<int>>& next) {
    int n = dist.size();

    // 初始化 next 矩阵
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (dist[i][j] != INF && i != j)
                next[i][j] = j;

    // 核心三重循环
    for (int k = 0; k < n; k++)           // 依次将顶点 k 作为中间节点
        for (int i = 0; i < n; i++)       // 源点
            for (int j = 0; j < n; j++)   // 目标
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];  // 走 k 的路径
                }
}

// ==================== 回溯路径 ====================
vector<int> getPath(int u, int v, const vector<vector<int>>& next) {
    if (next[u][v] == -1) return {};  // 不可达
    vector<int> path = {u};
    while (u != v) {
        u = next[u][v];
        path.push_back(u);
    }
    return path;
}

// ==================== 打印矩阵 ====================
void printMatrix(const vector<vector<int>>& mat, int n,
                 const string& title) {
    cout << "  " << title << ":" << endl;
    cout << "     ";
    for (int j = 0; j < n; j++) cout << setw(5) << j;
    cout << endl;
    for (int i = 0; i < n; i++) {
        cout << "  " << setw(2) << i << " ";
        for (int j = 0; j < n; j++) {
            if (mat[i][j] >= INF / 2)
                cout << setw(5) << "∞";
            else
                cout << setw(5) << mat[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Floyd-Warshall 全源最短路径            " << endl;
    cout << "========================================" << endl << endl;

    // 示例图
    int n = 4;
    vector<vector<int>> dist(n, vector<int>(n, INF));

    for (int i = 0; i < n; i++) dist[i][i] = 0;

    dist[0][1] = 3; dist[0][3] = 7;
    dist[1][2] = 2;
    dist[2][0] = 4; dist[2][3] = 1;
    dist[3][1] = 5;

    cout << "[初始邻接矩阵]" << endl;
    printMatrix(dist, n, "初始距离");

    vector<vector<int>> next(n, vector<int>(n, -1));
    floydWarshall(dist, next);

    cout << "[Floyd 结果]" << endl;
    printMatrix(dist, n, "最短距离");

    // 打印所有点对之间的最短路径
    cout << "[最短路径详情]" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            vector<int> path = getPath(i, j, next);
            if (path.empty()) continue;
            cout << "  " << i << " → " << j << ": ";
            for (size_t k = 0; k < path.size(); k++) {
                if (k > 0) cout << " → ";
                cout << path[k];
            }
            cout << " (距离 " << dist[i][j] << ")" << endl;
        }
    }

    // 负权环检测
    cout << endl << "[负权环检测]" << endl;
    bool hasNegCycle = false;
    for (int i = 0; i < n && !hasNegCycle; i++)
        if (dist[i][i] < 0) hasNegCycle = true;
    cout << "  负权环: " << (hasNegCycle ? "存在!" : "不存在 ✓") << endl;
    cout << "  若有负权环，对角线上会出现负值" << endl;

    return 0;
}
