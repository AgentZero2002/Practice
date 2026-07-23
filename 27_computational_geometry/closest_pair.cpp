/**
 * 最近点对 (Closest Pair of Points)
 *
 * 给定平面上 n 个点, 找到距离最近的一对点。
 *
 * 分治算法:
 *   1. 按 x 坐标排序所有点
 *   2. 用中线 x = midX 将点集分为左右两半
 *   3. 递归求解左右两半 → 得到 d = min(dLeft, dRight)
 *   4. 关键: 检查跨越中线的点对 (相距 < d)
 *      - 只考虑 x ∈ [midX-d, midX+d] 的带状区域
 *      - 带状区域内按 y 排序, 每个点只需检查上方最多 7 个点
 *   5. 合并结果
 *
 * 复杂度: T(n) = 2T(n/2) + O(n log n) → O(n log² n)
 *          优化: 全局按 y 排序 → O(n log n)
 *
 * 应用: 碰撞检测、聚类分析、分子动力学
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
using namespace std;

const double INF = 1e18;

struct Point {
    double x, y;
    Point(double x=0, double y=0) : x(x), y(y) {}
};

double dist2(const Point& a, const Point& b) {
    double dx = a.x - b.x, dy = a.y - b.y;
    return dx * dx + dy * dy;
}

double closestPair(vector<Point>& points) {
    int n = points.size();
    if (n <= 1) return INF;

    // 按 x 排序 (按 y 排序可以在带状区域中优化)
    sort(points.begin(), points.end(),
         [](const Point& a, const Point& b) { return a.x < b.x; });

    double d2 = INF;

    // 分治 + 带状检查
    function<void(int, int)> solve = [&](int l, int r) {
        if (l >= r) return;
        int mid = (l + r) / 2;
        double midX = points[mid].x;

        solve(l, mid);
        solve(mid + 1, r);

        // 带状区域的点 (按 y 排序)
        vector<Point> strip;
        for (int i = l; i <= r; i++)
            if (fabs(points[i].x - midX) * fabs(points[i].x - midX) < d2)
                strip.push_back(points[i]);

        sort(strip.begin(), strip.end(),
             [](const Point& a, const Point& b) { return a.y < b.y; });

        // 每个点只检查上方最多 7 个点
        for (size_t i = 0; i < strip.size(); i++)
            for (size_t j = i + 1; j < strip.size() &&
                 (strip[j].y - strip[i].y) * (strip[j].y - strip[i].y) < d2; j++)
                d2 = min(d2, dist2(strip[i], strip[j]));
    };

    solve(0, n - 1);
    return sqrt(d2);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  最近点对 (Closest Pair)                 " << endl;
    cout << "========================================" << endl << endl;

    vector<Point> points = {
        {0, 0}, {1, 1}, {2, 2}, {3, 3},
        {0, 10}, {1, 9}, {1.5, 1.5}
    };

    cout << "[点集] (共 " << points.size() << " 个点)" << endl;
    for (auto& p : points)
        cout << "  (" << p.x << ", " << p.y << ")" << endl;

    double d = closestPair(points);
    cout << endl << "最近距离: " << fixed << setprecision(4) << d << endl;
    cout << endl;

    cout << "[分治原理]" << endl;
    cout << "  1. 按 x 分成左右两半, 递归求解" << endl;
    cout << "  2. d = min(左最近距离, 右最近距离)" << endl;
    cout << "  3. 带状区 [midX-d, midX+d] 按 y 排序" << endl;
    cout << "  4. 每点只检查上方 ≤ 7 个点 (鸽巢原理)" << endl;
    cout << "  5. O(n log² n) 或 O(n log n)" << endl;

    return 0;
}
