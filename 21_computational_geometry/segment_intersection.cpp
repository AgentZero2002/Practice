/**
 * 线段相交 — 扫描线基础 (Segment Intersection)
 *
 * 线段相交是计算几何中最核心的问题之一, 广泛应用于:
 *   - GIS 地图线要素交叉检测
 *   - 计算机制图 (裁剪/渲染)
 *   - 碰撞检测
 *   - VLSI 设计规则检查
 *
 * 逐对检测 O(n²), 扫描线 (Bentley-Ottmann) 可达 O((n+k)log n),
 * 其中 k 为交点数量。
 *
 * 本文件先实现 O(n²) 的逐对检测 + 交点计算, 再介绍扫描线思想。
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
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
double dot(const Point& a, const Point& b) { return a.x*b.x + a.y*b.y; }

// ==================== 线段相交检测 ====================
struct Segment {
    Point A, B;
    Segment(Point A, Point B) : A(A), B(B) {}
};

bool onSegment(const Point& A, const Point& B, const Point& P) {
    return sign(cross(A, B, P)) == 0 && sign(dot(A-P, B-P)) <= 0;
}

bool intersect(const Segment& s1, const Segment& s2) {
    int s1c = sign(cross(s1.A, s1.B, s2.A));
    int s1d = sign(cross(s1.A, s1.B, s2.B));
    int s2a = sign(cross(s2.A, s2.B, s1.A));
    int s2b = sign(cross(s2.A, s2.B, s1.B));

    if (s1c * s1d < 0 && s2a * s2b < 0) return true;
    if (s1c == 0 && onSegment(s1.A, s1.B, s2.A)) return true;
    if (s1d == 0 && onSegment(s1.A, s1.B, s2.B)) return true;
    if (s2a == 0 && onSegment(s2.A, s2.B, s1.A)) return true;
    if (s2b == 0 && onSegment(s2.A, s2.B, s1.B)) return true;
    return false;
}

// ==================== 计算交点 ====================
// 假设两直线不平行, 求交点
Point intersection(const Segment& s1, const Segment& s2) {
    Point a = s1.A, b = s1.B, c = s2.A, d = s2.B;
    double t = cross(a, c, d) / cross(a, b, c - d);
    return a + (b - a) * t;
}

// ==================== 扫描线算法思想 ====================
// Bentley-Ottmann 算法简述:
//   1. 将所有线段端点按 y 坐标排序
//   2. 用一根水平线自下而上扫描
//   3. 维护"活动线段"集合 (与扫描线相交的线段, 按 x 排序)
//   4. 相邻活动线段可能相交 → 事件队列中加入新交点
//   5. 遇到交点时交换两条线段在活动集合中的位置
//   时间复杂度: O((n+k)log n), k = 交点数

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  线段相交 — 扫描线基础                   " << endl;
    cout << "========================================" << endl << endl;

    // 创建测试线段
    vector<Segment> segments = {
        Segment(Point(0,0), Point(2,2)),
        Segment(Point(0,2), Point(2,0)),
        Segment(Point(2,0), Point(4,0)),
        Segment(Point(1,1), Point(3,1)),
    };

    cout << "[逐对检测 O(n²)]" << endl;
    cout << "  线段数量: " << segments.size() << endl;

    vector<pair<int,int>> collisions;
    for (size_t i = 0; i < segments.size(); i++)
        for (size_t j = i + 1; j < segments.size(); j++)
            if (intersect(segments[i], segments[j])) {
                collisions.push_back({i, j});
                Point p = intersection(segments[i], segments[j]);
                cout << "  线段" << i << " 与 线段" << j
                     << " 相交于 (" << p.x << "," << p.y << ")" << endl;
            }

    cout << endl << "[扫描线进阶]" << endl;
    cout << "  Bentley-Ottmann 算法: O((n+k)log n)" << endl;
    cout << "  核心: 只检查相邻活动线段 → 大幅减少检测" << endl;
    cout << "  事件: 端点事件 + 交点事件" << endl;

    return 0;
}
