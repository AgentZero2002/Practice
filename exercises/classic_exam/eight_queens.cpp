/**
 * 八皇后问题 (Eight Queens)
 *
 * 对应教材 4.3.1 节：试探回溯法
 * 经典 DFS 回溯问题
 *
 * 在 8×8 的棋盘上放置 8 个皇后，
 * 使得它们互不攻击（不在同行、同列、同对角线）。
 *
 * 共有 92 组解 (去除旋转和反射的独立解有 12 组)。
 *
 * 算法: 逐行试探 + 列/对角线可行性剪枝
 */

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

// ==================== N-Queens 回溯 ====================
class NQueens {
private:
    int N;
    vector<vector<string>> solutions;
    vector<string> board;
    vector<bool> cols;         // 列占用标记
    vector<bool> diag1;        // 主对角线 / (row-col 为常量)
    vector<bool> diag2;        // 副对角线 \ (row+col 为常量)

    void backtrack(int row) {
        if (row == N) {
            solutions.push_back(board);
            return;
        }

        for (int col = 0; col < N; col++) {
            int d1 = row - col + N - 1;  // 映射到 [0, 2N-2]
            int d2 = row + col;

            if (cols[col] || diag1[d1] || diag2[d2])
                continue;  // 冲突，剪枝

            // 放置皇后
            board[row][col] = 'Q';
            cols[col] = diag1[d1] = diag2[d2] = true;

            backtrack(row + 1);

            // 撤销
            board[row][col] = '.';
            cols[col] = diag1[d1] = diag2[d2] = false;
        }
    }

public:
    NQueens(int n) : N(n), board(n, string(n, '.')),
        cols(n, false), diag1(2 * n - 1, false), diag2(2 * n - 1, false) {}

    vector<vector<string>> solve() {
        solutions.clear();
        backtrack(0);
        return solutions;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  八皇后问题 (N-Queens) — 试探回溯法      " << endl;
    cout << "========================================" << endl << endl;

    int n = 8;

    NQueens solver(n);
    auto solutions = solver.solve();

    cout << "N=" << n << " 时共有 " << solutions.size() << " 组解" << endl;
    cout << "  (独立解 12 组，其余为旋转/反射)" << endl << endl;

    // 打印前 3 组解
    cout << "[前 3 组解]" << endl;
    for (int s = 0; s < min(3, (int)solutions.size()); s++) {
        cout << "  解 #" << s + 1 << ":" << endl;
        for (int row = 0; row < n; row++) {
            cout << "    ";
            for (int col = 0; col < n; col++) {
                cout << solutions[s][row][col] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

    // N-Queens 规模增长
    cout << "[N-Queens 解的数量]" << endl;
    for (int i = 1; i <= 8; i++) {
        NQueens qs(i);
        int count = qs.solve().size();
        cout << "  N=" << i << ": " << count << " 组解" << endl;
    }

    cout << endl << "[回溯法要点]" << endl;
    cout << "  1. 逐行放置，行内自然不冲突" << endl;
    cout << "  2. 列冲突: col[] 数组判断" << endl;
    cout << "  3. 对角线冲突: row±col 为常量" << endl;
    cout << "  4. 剪枝: 不满足约束时立即回溯" << endl;
    cout << "  5. 时间复杂度: O(N!) — 但剪枝使实际远小于此" << endl;

    return 0;
}
