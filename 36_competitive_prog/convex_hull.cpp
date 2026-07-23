/**
 * 凸包 (Convex Hull) — Graham Scan 算法
 *
 * 凸包是计算几何中的基础算法：
 *   给定平面上一组点，找出包含所有点的最小凸多边形。
 *
 * Graham Scan:
 *   1. 找到最左下角的点 p0（y最小，y相等时x最小）
 *   2. 按极角排序其余点（用叉积判断方向）
 *   3. 用栈维护凸包：检查最后三点是否构成"右转"，
 *      若是则弹出中间点
 *
 * 时间复杂度: O(n log n)（排序主导）
 *
 * 叉积 Cross Product:
 *   cross(a,b,c) = (b-a)×(c-a) = (b.x-a.x)(c.y-a.y) - (b.y-a.y)(c.x-a.x)
 *   > 0 → 左转 (CCW)  ← 凸包保留
 *   < 0 → 右转 (CW)   ← 弹出
 *   = 0 → 共线
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

struct Point {
    int x, y;
    Point(int x=0, int y=0) : x(x), y(y) {}
};

// 叉积: (b-a) × (c-a)
long long cross(const Point& a, const Point& b, const Point& c) {
    return (long long)(b.x - a.x) * (c.y - a.y)
         - (long long)(b.y - a.y) * (c.x - a.x);
}

// 距离平方
long long dist2(const Point& a, const Point& b) {
    long long dx = a.x - b.x;
    long long dy = a.y - b.y;
    return dx * dx + dy * dy;
}

// Graham Scan 凸包
vector<Point> convexHull(vector<Point> points) {
    int n = points.size();
    if (n <= 2) return points;

    // 1. 找最左下点
    int p0 = 0;
    for (int i = 1; i < n; i++)
        if (points[i].y < points[p0].y ||
            (points[i].y == points[p0].y && points[i].x < points[p0].x))
            p0 = i;
    swap(points[0], points[p0]);

    Point origin = points[0];

    // 2. 按极角排序
    sort(points.begin() + 1, points.end(), [&origin](const Point& a, const Point& b) {
        long long c = cross(origin, a, b);
        if (c != 0) return c > 0;  // CCW → 极角小
        return dist2(origin, a) < dist2(origin, b);  // 共线 → 近的在前
    });

    // 3. Graham Scan
    vector<Point> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);

    for (int i = 2; i < n; i++) {
        while (hull.size() >= 2 &&
               cross(hull[hull.size() - 2], hull.back(), points[i]) <= 0)
            hull.pop_back();  // 右转或共线 → 弹出
        hull.push_back(points[i]);
    }

    return hull;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  凸包 (Convex Hull) — Graham Scan       " << endl;
    cout << "========================================" << endl << endl;

    vector<Point> points = {
        {0, 0}, {1, 1}, {2, 0}, {2, 2},
        {0, 2}, {1, 0}, {3, 1}, {1, 3}
    };

    cout << "[点集] (共 " << points.size() << " 个点)" << endl;
    for (auto& p : points) cout << "  (" << p.x << "," << p.y << ")";
    cout << endl;

    auto hull = convexHull(points);

    cout << endl << "[凸包顶点] (逆时针顺序, 共 " << hull.size() << " 个)" << endl;
    for (auto& p : hull)
        cout << "  (" << p.x << "," << p.y << ")";
    cout << endl;

    cout << endl << "[叉积判向]" << endl;
    cout << "  cross > 0: 左转 (CCW) — 凸包保留" << endl;
    cout << "  cross < 0: 右转 (CW)  — 弹出" << endl;
    cout << "  cross = 0: 共线      — 弹出(取最远)" << endl;

    return 0;
}
