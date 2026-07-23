/**
 * 迷宫求解（BFS 广度优先搜索）
 *
 * 对应教材 4.3.1 节：试探回溯法（迷宫寻径）
 *
 * 两种模式:
 *   1. DFS (栈) — 找到任意一条路径（不一定最短）
 *   2. BFS (队列) — 找到最短路径
 *
 * 迷宫表示:
 *   0 = 可通行, 1 = 墙壁
 *   S = 起点 (Start), E = 终点 (End)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <iomanip>
using namespace std;

const int WALL = 1;
const int PATH = 0;
const int VISITED = 2;
const int SOLUTION = 3;

// 方向数组: 上, 下, 左, 右
const int dx[] = {0, 0, -1, 1};
const int dy[] = {-1, 1, 0, 0};
const char dirChar[] = {'↑', '↓', '←', '→'};

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& p) const { return x == p.x && y == p.y; }
};

// ==================== BFS 最短路径 ====================
// 返回最短路径，若不存在返回空 vector
vector<Point> mazeBFS(const vector<vector<int>>& maze, Point start, Point end) {
    int rows = maze.size(), cols = maze[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<vector<Point>> parent(rows, vector<Point>(cols, {-1, -1}));

    queue<Point> q;
    q.push(start);
    visited[start.y][start.x] = true;

    bool found = false;
    while (!q.empty() && !found) {
        Point p = q.front(); q.pop();
        for (int d = 0; d < 4; d++) {
            int nx = p.x + dx[d], ny = p.y + dy[d];
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
                !visited[ny][nx] && maze[ny][nx] != WALL) {
                visited[ny][nx] = true;
                parent[ny][nx] = p;
                q.push({nx, ny});
                if (Point(nx, ny) == end) { found = true; break; }
            }
        }
    }

    if (!found) return {};

    // 回溯路径
    vector<Point> path;
    Point cur = end;
    while (!(cur == start)) {
        path.push_back(cur);
        cur = parent[cur.y][cur.x];
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

// ==================== DFS 路径（不一定最短） ====================
bool mazeDFS_recursive(const vector<vector<int>>& maze,
                        vector<vector<bool>>& visited,
                        vector<Point>& path,
                        Point cur, Point end) {
    if (cur == end) { path.push_back(cur); return true; }

    int rows = maze.size(), cols = maze[0].size();
    visited[cur.y][cur.x] = true;
    path.push_back(cur);

    for (int d = 0; d < 4; d++) {
        int nx = cur.x + dx[d], ny = cur.y + dy[d];
        if (nx >= 0 && nx < cols && ny >= 0 && ny < rows &&
            !visited[ny][nx] && maze[ny][nx] != WALL) {
            if (mazeDFS_recursive(maze, visited, path, {nx, ny}, end))
                return true;
        }
    }

    path.pop_back();  // 回溯
    return false;
}

vector<Point> mazeDFS(const vector<vector<int>>& maze, Point start, Point end) {
    int rows = maze.size(), cols = maze[0].size();
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    vector<Point> path;
    mazeDFS_recursive(maze, visited, path, start, end);
    return path;
}

// ==================== 打印迷宫 ====================
void printMaze(const vector<vector<int>>& maze, const vector<Point>& path = {},
               Point start = {0,0}, Point end = {0,0}) {
    int rows = maze.size(), cols = maze[0].size();

    // 标记路径
    vector<vector<char>> display(rows, vector<char>(cols));
    for (int y = 0; y < rows; y++)
        for (int x = 0; x < cols; x++)
            display[y][x] = maze[y][x] == WALL ? '█' : ' ';

    for (const Point& p : path) {
        if (!(p == start) && !(p == end))
            display[p.y][p.x] = '·';
    }
    display[start.y][start.x] = 'S';
    display[end.y][end.x] = 'E';

    cout << "  +" << string(cols * 2 + 1, '-') << "+" << endl;
    for (int y = 0; y < rows; y++) {
        cout << "  | ";
        for (int x = 0; x < cols; x++)
            cout << display[y][x] << " ";
        cout << "|" << endl;
    }
    cout << "  +" << string(cols * 2 + 1, '-') << "+" << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  迷宫求解 (BFS 最短路径)                " << endl;
    cout << "========================================" << endl << endl;

    //             0  1  2  3  4  5  6  7  8  9
    vector<vector<int>> maze = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},  // 0
        {0, 1, 0, 1, 0, 1, 1, 1, 1, 0},  // 1
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // 2
        {0, 1, 1, 1, 1, 1, 0, 1, 1, 0},  // 3
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 4
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0},  // 5
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},  // 6
        {0, 1, 1, 1, 1, 1, 0, 1, 1, 0},  // 7
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},  // 8
    };

    Point start(0, 0);
    Point end(9, 8);

    cout << "迷宫 (S=起点, E=终点, █=墙壁):" << endl;
    printMaze(maze, {}, start, end);

    // BFS 最短路径
    cout << "[BFS 最短路径]" << endl;
    vector<Point> bfsPath = mazeBFS(maze, start, end);
    cout << "  路径长度: " << bfsPath.size()
         << (bfsPath.empty() ? " (无法到达)" : "") << endl;
    if (!bfsPath.empty()) {
        cout << "  路径: ";
        for (size_t i = 0; i < bfsPath.size(); i++) {
            if (i > 0) cout << " → ";
            cout << "(" << bfsPath[i].x << "," << bfsPath[i].y << ")";
        }
        cout << endl;
        printMaze(maze, bfsPath, start, end);
    }

    // DFS 路径
    cout << "[DFS 路径 (不一定最短)]" << endl;
    vector<Point> dfsPath = mazeDFS(maze, start, end);
    cout << "  路径长度: " << dfsPath.size() << endl;
    if (!dfsPath.empty())
        printMaze(maze, dfsPath, start, end);

    // 对比
    cout << "[对比]" << endl;
    cout << "  BFS 路径长度: " << bfsPath.size() << " (最短路径)" << endl;
    cout << "  DFS 路径长度: " << dfsPath.size() << " (任意路径)" << endl;
    cout << "  结论: 求最短路径请用 BFS!" << endl;

    return 0;
}
