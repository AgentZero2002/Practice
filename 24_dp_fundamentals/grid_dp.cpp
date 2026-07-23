/**
 * 网格 DP (Grid DP) + 记忆化搜索
 *
 * 网格 DP 是二维 DP 的直观形式, 也是记忆化搜索的最佳展示。
 *
 * 经典问题:
 *   1. 最小路径和 (Minimum Path Sum):
 *      dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1])
 *
 *   2. 不同路径数 (Unique Paths):
 *      dp[i][j] = dp[i-1][j] + dp[i][j-1]
 *
 *   3. 最大正方形 (Maximal Square):
 *      dp[i][j] = min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1
 *
 *   4. Cherry Pickup / Dungeon Game 等复杂网格DP
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== 1. 最小路径和 ====================
int minPathSum(const vector<vector<int>>& grid) {
    int n = grid.size(), m = grid[0].size();
    vector<vector<int>> dp(n, vector<int>(m, 0));
    dp[0][0] = grid[0][0];
    for (int i = 1; i < n; i++) dp[i][0] = dp[i-1][0] + grid[i][0];
    for (int j = 1; j < m; j++) dp[0][j] = dp[0][j-1] + grid[0][j];

    for (int i = 1; i < n; i++)
        for (int j = 1; j < m; j++)
            dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1]);
    return dp[n-1][m-1];
}

// ==================== 2. 最大正方形 ====================
int maximalSquare(const vector<vector<char>>& matrix) {
    int n = matrix.size(), m = matrix[0].size();
    vector<vector<int>> dp(n, vector<int>(m, 0));
    int maxSide = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] == '0') continue;
            if (i == 0 || j == 0) dp[i][j] = 1;
            else dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
            maxSide = max(maxSide, dp[i][j]);
        }
    return maxSide * maxSide;
}

// ==================== 3. 不同路径 (带障碍) ====================
int uniquePathsWithObstacles(const vector<vector<int>>& obstacleGrid) {
    int n = obstacleGrid.size(), m = obstacleGrid[0].size();
    vector<vector<long long>> dp(n, vector<long long>(m, 0));
    if (obstacleGrid[0][0] == 1) return 0;
    dp[0][0] = 1;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            if (obstacleGrid[i][j] == 1) { dp[i][j] = 0; continue; }
            if (i > 0) dp[i][j] += dp[i-1][j];
            if (j > 0) dp[i][j] += dp[i][j-1];
        }
    return dp[n-1][m-1];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  网格 DP (Grid DP)                       " << endl;
    cout << "========================================" << endl << endl;

    vector<vector<int>> grid = {{1,3,1},{1,5,1},{4,2,1}};
    cout << "[最小路径和] grid 3×3: "
         << minPathSum(grid) << " (期望: 7)" << endl;

    vector<vector<char>> matrix = {
        {'1','0','1','0'},
        {'1','1','1','1'},
        {'0','1','1','1'},
        {'1','0','1','0'}
    };
    cout << "[最大正方形] " << maximalSquare(matrix)
         << " (期望: 4, 2×2)" << endl;

    vector<vector<int>> obs = {{0,0,0},{0,1,0},{0,0,0}};
    cout << "[不同路径(障碍)] " << uniquePathsWithObstacles(obs)
         << " (期望: 2)" << endl;

    cout << endl << "[网格DP vs 图上DP]" << endl;
    cout << "  网格: 转移来自上一行/左一列 (DAG)" << endl;
    cout << "  图上DP: 必须先拓扑排序" << endl;

    return 0;
}
