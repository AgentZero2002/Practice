/**
 * Ball Tree — 度量空间最近邻搜索
 *
 * Ball Tree 是 KD-Tree 在任意度量空间中的推广。
 * 与 KD-Tree 按超平面切分不同, Ball Tree 用超球面 (ball)
 * 组织数据点, 每个节点代表一个包含若干点的超球。
 *
 * 节点结构:
 *   每个 Ball Node 包含:
 *     - 球心 (centroid): 包含点的质心
 *     - 半径 (radius): 球心到最远点的距离
 *     - 左子球, 右子球
 *
 * 构建 (类似层次聚类):
 *   1. 找到当前点集中相距最远的两个点 (pivot1, pivot2)
 *   2. 将剩余点分配到离它更近的 pivot
 *   3. 递归构建子树
 *
 * 最近邻查询 (NN Search):
 *   query 在球心距离 + 半径之外 → 整颗子树可剪枝!
 *   剪枝条件: dist(query, centroid) - radius ≥ currentBest
 *
 * Ball Tree vs KD-Tree:
 *   - KD-Tree: 按轴对齐超平面切分, 适合低维欧氏空间
 *   - Ball Tree: 按超球组织, 适合任意度量空间和高维
 *   - 高维 (d > 20): Ball Tree > KD-Tree
 *   - 但两者在超高维 (d > 100) 都会退化
 *
 * 应用:
 *   - scikit-learn NearestNeighbors (默认 Ball Tree)
 *   - 度量学习
 *   - 蛋白质结构比对
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;

struct Point {
    double x, y;
    Point(double x=0, double y=0) : x(x), y(y) {}
};

double dist2(const Point& a, const Point& b) {
    return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

double dist(const Point& a, const Point& b) {
    return sqrt(dist2(a, b));
}

// Ball Tree Node
struct BallNode {
    Point centroid;      // 球心 (质心)
    double radius;       // 半径
    Point point;         // 叶节点存储的数据点
    BallNode* left;
    BallNode* right;
    bool isLeaf;

    BallNode(Point p) : centroid(p), radius(0), point(p),
                        left(nullptr), right(nullptr), isLeaf(true) {}
};

class BallTree {
private:
    BallNode* root;

    // 构建 Ball Tree
    BallNode* build(vector<Point>& pts) {
        if (pts.empty()) return nullptr;
        if (pts.size() == 1)
            return new BallNode(pts[0]);

        // 计算质心
        Point centroid(0, 0);
        for (auto& p : pts) { centroid.x += p.x; centroid.y += p.y; }
        centroid.x /= pts.size();
        centroid.y /= pts.size();

        // 计算半径
        double radius = 0;
        for (auto& p : pts)
            radius = max(radius, dist(centroid, p));

        // 找 pivot: 离质心最近和最远的点
        Point pivot1 = pts[0], pivot2 = pts[0];
        double minD = numeric_limits<double>::max(), maxD = 0;
        for (auto& p : pts) {
            double d = dist2(centroid, p);
            if (d < minD) { minD = d; pivot1 = p; }
            if (d > maxD) { maxD = d; pivot2 = p; }
        }

        // 分配点到两个子集
        vector<Point> leftPts, rightPts;
        for (auto& p : pts) {
            if (dist2(p, pivot1) <= dist2(p, pivot2))
                leftPts.push_back(p);
            else
                rightPts.push_back(p);
        }

        if (leftPts.empty() || rightPts.empty()) {
            // 如果不能分裂, 创建叶节点
            auto* node = new BallNode(centroid);
            node->point = centroid;
            node->radius = radius;
            return node;
        }

        BallNode* node = new BallNode(centroid);
        node->radius = radius;
        node->isLeaf = false;
        node->left = build(leftPts);
        node->right = build(rightPts);
        return node;
    }

    // 最近邻搜索 (含剪枝)
    void nearest(BallNode* node, const Point& query,
                 Point& best, double& bestDist) const {
        if (!node) return;

        // 剪枝: query 在球外部且距离球面 > bestDist
        double d = dist(query, node->centroid);
        if (d - node->radius >= bestDist) return;

        if (node->isLeaf) {
            double nd = dist(query, node->point);
            if (nd < bestDist) { bestDist = nd; best = node->point; }
            return;
        }

        // 先搜索近的子球
        double dLeft = node->left ? dist(query, node->left->centroid) : numeric_limits<double>::max();
        double dRight = node->right ? dist(query, node->right->centroid) : numeric_limits<double>::max();

        if (dLeft < dRight) {
            nearest(node->left, query, best, bestDist);
            nearest(node->right, query, best, bestDist);
        } else {
            nearest(node->right, query, best, bestDist);
            nearest(node->left, query, best, bestDist);
        }
    }

public:
    BallTree(vector<Point>& pts) {
        root = build(pts);
    }

    Point nearestNeighbor(const Point& query) const {
        Point best;
        double bestDist = numeric_limits<double>::max();
        nearest(root, query, best, bestDist);
        return best;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Ball Tree — 度量空间最近邻搜索          " << endl;
    cout << "========================================" << endl << endl;

    vector<Point> points = {
        {2, 3}, {5, 4}, {9, 6}, {4, 7},
        {8, 1}, {7, 2}, {3, 8}, {6, 5}
    };

    BallTree tree(points);

    Point query(5, 5);
    auto nn = tree.nearestNeighbor(query);

    cout << "[数据点] (共 " << points.size() << " 个)" << endl;
    cout << "[查询] (" << query.x << "," << query.y << ")" << endl;
    cout << "  最近邻: (" << nn.x << "," << nn.y << ")" << endl;
    cout << endl;

    cout << "[Ball Tree vs KD-Tree]" << endl;
    cout << "  Ball Tree: 超球切分, 适合任意度量空间" << endl;
    cout << "  KD-Tree:   超平面切分, 仅轴对齐" << endl;
    cout << "  剪枝: dist(center,query) - radius ≥ best → 跳过" << endl;
    cout << "  高维 (d>20): Ball Tree 通常更优" << endl;

    return 0;
}
