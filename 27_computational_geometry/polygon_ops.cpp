/**
 * 多边形运算 (Polygon Operations)
 *
 * 平面简单多边形的核心操作:
 *
 * 1. 多边形面积 (Shoelace 公式):
 *    Area = ½ |Σ (xᵢ·yᵢ₊₁ - xᵢ₊₁·yᵢ)|
 *    顶点按逆时针排列时为正, 顺时针为负
 *
 * 2. 多边形重心:
 *    Cx = (1/(6A)) Σ (xᵢ+xᵢ₊₁)(xᵢ·yᵢ₊₁ - xᵢ₊₁·yᵢ)
 *    Cy = (1/(6A)) Σ (yᵢ+yᵢ₊₁)(xᵢ·yᵢ₊₁ - xᵢ₊₁·yᵢ)
 *
 * 3. 点在多边形内 (Ray Casting 射线法):
 *    从被测点向 x 轴正方向发射射线,
 *    统计与多边形边的交点数量:
 *      奇数 → 内部, 偶数 → 外部
 *
 * 4. 多边形是否为凸 (Convex Check):
 *    所有边的转向一致 (全为顺时针或全为逆时针)
 */

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

const double EPS = 1e-9;
int sign(double x) { return (x > EPS) - (x < -EPS); }

struct Point {
    double x, y;
    Point(double x=0, double y=0) : x(x), y(y) {}
    Point operator-(const Point& p) const { return Point(x-p.x, y-p.y); }
};

double cross(const Point& a, const Point& b) { return a.x*b.y - a.y*b.x; }
double cross(const Point& a, const Point& b, const Point& c) { return cross(b-a, c-a); }

// ==================== 1. 多边形面积 ====================
double polygonArea(const vector<Point>& poly) {
    int n = poly.size();
    double area = 0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += poly[i].x * poly[j].y;
        area -= poly[j].x * poly[i].y;
    }
    return fabs(area) / 2.0;
}

// ==================== 2. 多边形重心 ====================
Point polygonCentroid(const vector<Point>& poly) {
    int n = poly.size();
    double area = 0;
    Point centroid(0, 0);
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        double crossVal = poly[i].x * poly[j].y - poly[j].x * poly[i].y;
        area += crossVal;
        centroid.x += (poly[i].x + poly[j].x) * crossVal;
        centroid.y += (poly[i].y + poly[j].y) * crossVal;
    }
    area /= 2.0;
    centroid.x /= (6.0 * area);
    centroid.y /= (6.0 * area);
    return centroid;
}

// ==================== 3. 点在多边形内 (射线法) ====================
bool pointInPolygon(const Point& P, const vector<Point>& poly) {
    int n = poly.size();
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        Point A = poly[i], B = poly[(i + 1) % n];

        // 排除点在边上的退化情况
        if (sign(cross(A, B, P)) == 0 &&
            min(A.x, B.x) <= P.x && P.x <= max(A.x, B.x) &&
            min(A.y, B.y) <= P.y && P.y <= max(A.y, B.y))
            return true;  // 在边上 → 算作内部

        // 射线与边相交判定
        if ((A.y > P.y) != (B.y > P.y)) {
            double xIntersect = A.x + (P.y - A.y) * (B.x - A.x) / (B.y - A.y);
            if (P.x < xIntersect) cnt++;
        }
    }
    return cnt % 2 == 1;  // 奇数 → 内部
}

// ==================== 4. 判断多边形是否为凸 ====================
bool isConvex(const vector<Point>& poly) {
    int n = poly.size();
    if (n < 3) return true;

    bool hasPositive = false, hasNegative = false;
    for (int i = 0; i < n; i++) {
        double c = cross(poly[i], poly[(i+1)%n], poly[(i+2)%n]);
        if (sign(c) > 0) hasPositive = true;
        if (sign(c) < 0) hasNegative = true;
    }
    return !(hasPositive && hasNegative);  // 不能同时有左转和右转
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  多边形运算                              " << endl;
    cout << "========================================" << endl << endl;

    // 正方形 (逆时针)
    vector<Point> square = {{0,0}, {2,0}, {2,2}, {0,2}};
    cout << "[正方形] 顶点: (0,0)→(2,0)→(2,2)→(0,2)" << endl;
    cout << "  面积: " << polygonArea(square) << " (期望: 4)" << endl;
    auto cent = polygonCentroid(square);
    cout << "  重心: (" << cent.x << "," << cent.y << ") (期望: 1,1)" << endl;
    cout << "  凸多边形? " << (isConvex(square) ? "是 ✓" : "否") << endl;

    Point testP(1, 1), testQ(3, 3);
    cout << "  点(1,1)在内部? " << (pointInPolygon(testP, square) ? "是 ✓" : "否") << endl;
    cout << "  点(3,3)在内部? " << (pointInPolygon(testQ, square) ? "是" : "否 ✓") << endl;
    cout << endl;

    // 凹多边形
    vector<Point> concave = {{0,0}, {2,0}, {1,1}, {2,2}, {0,2}};
    cout << "[凹多边形] 星形" << endl;
    cout << "  面积: " << polygonArea(concave) << endl;
    cout << "  凸多边形? " << (isConvex(concave) ? "是" : "否 ✓") << endl;
    cout << endl;

    cout << "[常用公式]" << endl;
    cout << "  面积: ½|Σ(xᵢyᵢ₊₁ - xᵢ₊₁yᵢ)| (Shoelace)" << endl;
    cout << "  内部判定: 射线法 (Ray Casting), 检查交叉次数奇偶" << endl;
    cout << "  凸判定: 叉积符号全相同" << endl;

    return 0;
}
