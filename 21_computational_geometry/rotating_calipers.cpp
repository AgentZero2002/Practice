/**
 * 旋转卡壳 (Rotating Calipers)
 *
 * 在已知凸包的基础上, 用 O(n) 时间求解以下问题:
 *   1. 凸包直径 (最远点对)
 *   2. 最小宽度 (两平行线夹住凸包的最小距离)
 *   3. 两凸包间最近/最远距离
 *
 * 核心思想:
 *   用一对平行线夹住凸包, 旋转这对平行线。
 *   利用凸包上的点对距离是单峰函数 (单峰 = 先增后减) 的性质,
 *   只需维护两个指针 (calipers), 每个指针总计旋转一圈。
 *
 * 这就是所谓的 "旋转卡壳" (像卡尺一样旋转):
 *   对于凸包的一条边 (AB), 与之距离最远的顶点 C 构成三角形(ABC)
 *   面积最大。随着 AB 绕凸包旋转一周, C 也跟随着旋转,
 *   且每个方向都只前进不后退。
 *
 * 复杂度: O(n) (基于已有凸包)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

struct Point {
    long long x, y;
    Point(long long x=0, long long y=0) : x(x), y(y) {}
    Point operator-(const Point& p) const { return Point(x-p.x, y-p.y); }
};

long long cross(const Point& a, const Point& b) { return a.x*b.y - a.y*b.x; }
long long cross(const Point& a, const Point& b, const Point& c) { return cross(b-a, c-a); }
long long dist2(const Point& a, const Point& b) {
    return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

// Graham Scan 凸包 (从第 18 章简化)
vector<Point> convexHull(vector<Point> pts) {
    int n = pts.size();
    if (n <= 2) return pts;
    sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    });
    vector<Point> hull;
    for (int phase = 0; phase < 2; phase++) {
        int start = hull.size();
        for (auto& p : pts) {
            while (hull.size() >= start + 2 &&
                   cross(hull[hull.size()-2], hull.back(), p) <= 0)
                hull.pop_back();
            hull.push_back(p);
        }
        hull.pop_back();
        reverse(pts.begin(), pts.end());
    }
    return hull;
}

// 凸包直径 (最远点对) via 旋转卡壳
long long convexDiameter(const vector<Point>& hull) {
    int n = hull.size();
    if (n == 1) return 0;
    if (n == 2) return dist2(hull[0], hull[1]);

    int j = 1;
    long long maxDist2 = 0;

    for (int i = 0; i < n; i++) {
        int ni = (i + 1) % n;
        // 旋转卡壳: 找边 hull[i]→hull[ni] 对应的最远点
        while (true) {
            int nj = (j + 1) % n;
            long long curArea = abs(cross(hull[i], hull[ni], hull[j]));
            long long nxtArea = abs(cross(hull[i], hull[ni], hull[nj]));
            if (nxtArea > curArea)
                j = nj;
            else
                break;
        }
        maxDist2 = max(maxDist2, dist2(hull[i], hull[j]));
        maxDist2 = max(maxDist2, dist2(hull[ni], hull[j]));
    }
    return maxDist2;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  旋转卡壳 (Rotating Calipers)           " << endl;
    cout << "========================================" << endl << endl;

    vector<Point> points = {
        {0, 0}, {3, 0}, {3, 3}, {0, 3},
        {1, 1}, {2, 2}, {2, 1}, {1, 2}
    };

    auto hull = convexHull(points);
    cout << "[凸包顶点] 共 " << hull.size() << " 个" << endl;
    for (auto& p : hull)
        cout << "  (" << p.x << "," << p.y << ")";
    cout << endl;

    long long diam2 = convexDiameter(hull);
    cout << endl << "[凸包直径²] " << diam2
         << " (期望: 18, 对角线(0,0)→(3,3))" << endl;
    cout << "  直径 = " << sqrt(diam2) << " ≈ 4.243" << endl;
    cout << endl;

    cout << "[旋转卡壳原理]" << endl;
    cout << "  维护两平行线夹凸包旋转" << endl;
    cout << "  对每条边, 对应的最远点单调移动" << endl;
    cout << "  每个指针总共只旋转一圈 → O(n)" << endl;
    cout << "  比 O(n²) 枚举点对快得多" << endl;

    return 0;
}
