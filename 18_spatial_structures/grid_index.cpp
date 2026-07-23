/**
 * Grid Index (均匀网格索引) — 空间分桶
 *
 * Grid Index 是最简单的空间索引: 将空间均匀划分为网格,
 * 每个网格(cell)存储落入其中的点。
 *
 * 原理:
 *   给定点 (x, y) 和网格大小 Δ:
 *     cellX = ⌊x / Δ⌋
 *     cellY = ⌊y / Δ⌋
 *     点存入 map[(cellX, cellY)] 桶中
 *
 * 查询:
 *   范围查询: 暴力检查相交的网格单元
 *   kNN: 从查询点所在网格开始, 向外螺旋搜索
 *
 * 优点:
 *   - 构建 O(n), 查询 O(1 + k) where k = 网格内元素
 *   - 实现极简单
 *   - 均匀分布时性能极佳
 *
 * 缺点:
 *   - 数据非均匀分布时性能退化
 *   - 高维时网格数量爆炸
 *   - 网格大小需要调参
 *
 * 应用:
 *   - 游戏物理引擎 (碰撞检测)
 *   - GPS 地图 POI 查询 (简单场景)
 *   - 粒子系统 (SPH 流体模拟)
 */

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
using namespace std;

const double CELL_SIZE = 1.0;  // 网格大小

struct Point {
    double x, y;
    Point(double x=0, double y=0) : x(x), y(y) {}
};

class GridIndex {
    map<pair<int,int>, vector<Point>> grid;
    double cellSize;

public:
    GridIndex(double cs = CELL_SIZE) : cellSize(cs) {}

    void insert(const Point& p) {
        int cx = (int)floor(p.x / cellSize);
        int cy = (int)floor(p.y / cellSize);
        grid[{cx, cy}].push_back(p);
    }

    // 范围查询: 查询以 (cx,cy) 为中心, radius 个格子范围内的点
    vector<Point> rangeQuery(double x, double y, double radius) {
        vector<Point> result;
        int cx = (int)floor(x / cellSize);
        int cy = (int)floor(y / cellSize);
        int r = (int)ceil(radius / cellSize);

        for (int dx = -r; dx <= r; dx++)
            for (int dy = -r; dy <= r; dy++) {
                auto it = grid.find({cx + dx, cy + dy});
                if (it == grid.end()) continue;
                for (auto& p : it->second) {
                    double dist2 = (p.x-x)*(p.x-x) + (p.y-y)*(p.y-y);
                    if (dist2 <= radius * radius)
                        result.push_back(p);
                }
            }
        return result;
    }

    void printStats() {
        int totalCells = grid.size();
        int totalPoints = 0;
        int maxBucket = 0;
        for (auto& [cell, pts] : grid) {
            totalPoints += pts.size();
            maxBucket = max(maxBucket, (int)pts.size());
        }
        cout << "  网格数: " << totalCells
             << ", 点数: " << totalPoints
             << ", 最大桶: " << maxBucket << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Grid Index (均匀网格索引)                " << endl;
    cout << "========================================" << endl << endl;

    GridIndex gi(1.0);

    // 随机点
    vector<Point> pts = {{0.2,0.3}, {0.8,1.2}, {2.1,1.8}, {2.5,0.7},
                         {3.0,3.0}, {1.5,2.5}, {0.1,0.1}, {3.5,3.5}};
    for (auto& p : pts) gi.insert(p);

    gi.printStats();

    cout << endl << "[范围查询] 以 (1.5, 1.5) 为心, r=2.0" << endl;
    auto results = gi.rangeQuery(1.5, 1.5, 2.0);
    for (auto& p : results)
        cout << "  (" << p.x << "," << p.y << ")" << endl;

    cout << endl << "[Grid vs KD-Tree]" << endl;
    cout << "  Grid: O(1) 插入, 适合均匀分布" << endl;
    cout << "  KD-Tree: O(log n) 查询, 适合任意分布" << endl;
    cout << "  R-Tree: 适合磁盘存储的海量数据" << endl;

    return 0;
}
