/**
 * 半平面交 (Half-plane Intersection)
 *
 * 给定 n 个半平面 (每个由有向直线 ax+by+c ≤ 0 表示),
 * 求它们的交集。交集一定是凸多边形 (可能为空或无界)。
 *
 * 核心思想:
 *   每个半平面 → 一条有向直线 (左侧为有效区域)
 *   半平面交 = 所有"左半平面"的交集
 *   用双端队列维护凸多边形, 逐个加入切割线
 *
 * 算法 (Sort-and-Incremental, O(n log n)):
 *   1. 将所有直线按极角排序
 *   2. 去重 (平行线只保留最"左"的)
 *   3. 用 deque 维护凸多边形:
 *      - 新线从队尾加入, 检查队尾交点是否在右侧 → 弹出
 *      - 同理从队首检查
 *   4. 最后再检查队首队尾是否闭合
 *
 * 应用:
 *   - 线性规划 (2D): 求半平面交后找最值点
 *   - 可见性区域: 给定障碍物, 求可见范围
 *   - 凸多边形核 (Kernel): 星形多边形的内部区域
 *   - Voronoi 图 (通过半平面交构造)
 *
 * 时间复杂度: O(n log n)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <deque>
#include <iomanip>
using namespace std;

const double EPS = 1e-9;
int sign(double x) { return (x > EPS) - (x < -EPS); }

struct Point {
    double x, y;
    Point(double x=0, double y=0) : x(x), y(y) {}
    Point operator-(const Point& p) const { return Point(x-p.x, y-p.y); }
    Point operator+(const Point& p) const { return Point(x+p.x, y+p.y); }
    Point operator*(double k) const { return Point(x*k, y*k); }
};

double cross(const Point& a, const Point& b) { return a.x*b.y - a.y*b.x; }
double cross(const Point& a, const Point& b, const Point& c) { return cross(b-a, c-a); }

// 有向直线: ax + by + c ≤ 0 (左侧为有效区域)
struct Line {
    Point p, v;  // 过点 p, 方向向量 v
    double angle; // 极角

    Line(Point p=Point(), Point v=Point()) : p(p), v(v) {
        angle = atan2(v.y, v.x);
    }

    bool operator<(const Line& other) const {
        return angle < other.angle;
    }
};

// 两直线交点
Point lineIntersection(const Line& L1, const Line& L2) {
    Point u = L1.p - L2.p;
    double t = cross(L2.v, u) / cross(L1.v, L2.v);
    return L1.p + L1.v * t;
}

// 判断点 P 是否在直线 L 的左侧 (即满足不等式)
bool onLeft(const Line& L, const Point& P) {
    return cross(L.v, P - L.p) >= -EPS;
}

// 半平面交
vector<Point> halfplaneIntersection(vector<Line> lines) {
    sort(lines.begin(), lines.end());

    // 去重: 平行线只保留最内侧的
    vector<Line> unique;
    for (auto& L : lines) {
        if (!unique.empty() && sign(unique.back().angle - L.angle) == 0)
            continue;
        unique.push_back(L);
    }

    deque<Line> dq;
    deque<Point> pts;

    for (auto& L : unique) {
        // 从队尾弹出 (队尾交点在 L 右侧)
        while (pts.size() >= 1 && !onLeft(L, pts.back())) {
            dq.pop_back();
            pts.pop_back();
        }
        // 从队首弹出 (队首交点在 L 右侧)
        while (pts.size() >= 1 && !onLeft(L, pts.front())) {
            dq.pop_front();
            pts.pop_front();
        }
        // 加入新线
        dq.push_back(L);
        if (dq.size() >= 2)
            pts.push_back(lineIntersection(dq[dq.size()-2], dq.back()));
    }

    // 最后清理: 检查队首队尾
    while (pts.size() >= 1 && !onLeft(dq.front(), pts.back())) {
        dq.pop_back();
        pts.pop_back();
    }
    if (dq.size() >= 2)
        pts.push_back(lineIntersection(dq.front(), dq.back()));

    return vector<Point>(pts.begin(), pts.end());
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  半平面交 (Half-plane Intersection)      " << endl;
    cout << "========================================" << endl << endl;

    // 构造一个正方形区域 [-5,5] × [-5,5] 的半平面
    vector<Line> lines;
    // x ≥ -5 : -x - 5 ≤ 0 → 方向向上
    lines.push_back(Line(Point(-5, 0), Point(0, 1)));   // x ≥ -5
    lines.push_back(Line(Point(5, 0), Point(0, -1)));    // x ≤ 5
    lines.push_back(Line(Point(0, -5), Point(1, 0)));    // y ≥ -5
    lines.push_back(Line(Point(0, 5), Point(-1, 0)));    // y ≤ 5

    // 再加一斜线切掉一角: x + y ≤ 8
    lines.push_back(Line(Point(8, 0), Point(-1, 1)));

    auto poly = halfplaneIntersection(lines);

    cout << "[半平面交结果]" << endl;
    cout << "  5 个半平面 (4个边界 + 1条斜线)" << endl;
    cout << "  交集多边形顶点 (" << poly.size() << " 个):" << endl;
    for (auto& p : poly)
        cout << "    (" << fixed << setprecision(2)
             << p.x << ", " << p.y << ")" << endl;
    cout << endl;

    cout << "[算法步骤]" << endl;
    cout << "  1. 直线按极角排序 O(n log n)" << endl;
    cout << "  2. 平行线去重" << endl;
    cout << "  3. deque 维护凸多边形边界" << endl;
    cout << "  4. 新线加入时弹出在右侧的交点" << endl;
    cout << "  → 类似凸包 Graham Scan 的思想" << endl;

    return 0;
}
