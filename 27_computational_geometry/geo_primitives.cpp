/**
 * 计算几何基础原语 (2D Geometric Primitives)
 *
 * 所有计算几何算法的基石。所有复杂算法都建立在这些
 * 基础运算之上。
 *
 * 核心操作:
 *   1. 叉积 (Cross Product): 判断旋转方向
 *      cross(a,b) = a.x·b.y - a.y·b.x = |a||b|sin(θ)
 *      > 0 → b 在 a 的逆时针方向
 *      < 0 → b 在 a 的顺时针方向
 *      = 0 → 共线
 *
 *   2. 点积 (Dot Product): 判断夹角
 *      dot(a,b) = a.x·b.x + a.y·b.y = |a||b|cos(θ)
 *      > 0 → 锐角, = 0 → 直角, < 0 → 钝角
 *
 *   3. 方向判定: cross(b-a, c-a) = 0 → a,b,c 共线
 *                > 0 → a→b→c 左转 (CCW)
 *                < 0 → a→b→c 右转 (CW)
 *
 *   4. 点到线段距离、点在线段上判定
 */

#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

const double EPS = 1e-9;

struct Point {
    double x, y;
    Point(double x=0, double y=0) : x(x), y(y) {}

    Point operator+(const Point& p) const { return Point(x+p.x, y+p.y); }
    Point operator-(const Point& p) const { return Point(x-p.x, y-p.y); }
    Point operator*(double k) const { return Point(x*k, y*k); }

    double norm() const { return sqrt(x*x + y*y); }         // 模长
    double norm2() const { return x*x + y*y; }               // 模长平方
};

// 叉积: cross(a,b) = |a||b|sinθ
double cross(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

// 叉积: (b-a) × (c-a)
double cross(const Point& a, const Point& b, const Point& c) {
    return cross(b - a, c - a);
}

// 点积: dot(a,b) = |a||b|cosθ
double dot(const Point& a, const Point& b) {
    return a.x * b.x + a.y * b.y;
}

// 符号函数 (含 EPS)
int sign(double x) { return (x > EPS) - (x < -EPS); }

// 判断点 C 是否在线段 AB 上 (含端点)
bool onSegment(const Point& A, const Point& B, const Point& C) {
    return sign(cross(A, B, C)) == 0 &&
           sign(dot(A - C, B - C)) <= 0;  // C 在 A,B 之间
}

// 点到线段距离
double distToSegment(const Point& A, const Point& B, const Point& P) {
    if (sign(dot(B - A, P - A)) <= 0) return (P - A).norm();       // P 在 A 左侧
    if (sign(dot(A - B, P - B)) <= 0) return (P - B).norm();       // P 在 B 右侧
    return fabs(cross(A, B, P)) / (B - A).norm();                   // 垂直距离
}

// 两线段是否相交 (含端点)
bool segmentsIntersect(const Point& A, const Point& B,
                       const Point& C, const Point& D) {
    int s1 = sign(cross(A, B, C));
    int s2 = sign(cross(A, B, D));
    int s3 = sign(cross(C, D, A));
    int s4 = sign(cross(C, D, B));

    if (s1 * s2 < 0 && s3 * s4 < 0) return true;  // 严格相交

    // 端点在线段上
    if (s1 == 0 && onSegment(A, B, C)) return true;
    if (s2 == 0 && onSegment(A, B, D)) return true;
    if (s3 == 0 && onSegment(C, D, A)) return true;
    if (s4 == 0 && onSegment(C, D, B)) return true;

    return false;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  计算几何基础原语                        " << endl;
    cout << "========================================" << endl << endl;

    Point O(0,0), X(1,0), Y(0,1);
    cout << "[叉积]" << endl;
    cout << "  cross((1,0),(0,1)) = " << cross(X, Y)
         << " (>0, 逆时针)" << endl;
    cout << "  cross((0,1),(1,0)) = " << cross(Y, X)
         << " (<0, 顺时针)" << endl;
    cout << endl;

    cout << "[点积]" << endl;
    cout << "  dot((1,0),(0,1)) = " << dot(X, Y)
         << " (=0, 垂直)" << endl;
    cout << "  dot((1,0),(1,0)) = " << dot(X, X)
         << " (=1, 同方向)" << endl;
    cout << endl;

    // 线段相交
    cout << "[线段相交测试]" << endl;
    Point A(0,0), B(2,2), C(0,2), D(2,0);
    cout << "  AB(0,0→2,2) 与 CD(0,2→2,0): "
         << (segmentsIntersect(A,B,C,D) ? "相交 ✓" : "不相交") << endl;
    Point E(0,0), F(1,1), G(2,0), H(3,1);
    cout << "  EF(0,0→1,1) 与 GH(2,0→3,1): "
         << (segmentsIntersect(E,F,G,H) ? "相交" : "不相交 ✓") << endl;
    cout << endl;

    cout << "[关键宏]" << endl;
    cout << "  cross > 0: 逆时针 (CCW)" << endl;
    cout << "  cross < 0: 顺时针 (CW)" << endl;
    cout << "  dot > 0: 锐角, dot = 0: 直角" << endl;
    cout << "  共线判定: cross = 0" << endl;

    return 0;
}
