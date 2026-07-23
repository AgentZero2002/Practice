/**
 * KD-Tree (K-Dimensional Tree) — 多维搜索树
 *
 * KD-Tree 是二叉搜索树在 k 维空间中的推广, 每层按不同维度
 * 交替划分空间。广泛应用于最近邻搜索、范围查询等。
 *
 * 结构:
 *   每层选择一个维度 d 和一个切分值 v:
 *     - 左子树: 所有点在维度 d 上的值 ≤ v
 *     - 右子树: 所有点在维度 d 上的值 > v
 *   维度选择通常按深度轮流: dim = depth % k
 *
 * 构建 (Build):
 *   1. 选择当前维度的中位数作为切分点 (用 nth_element O(n))
 *   2. 递归构建左右子树
 *   3. 总复杂度: O(n log n)
 *
 * 最近邻搜索 (Nearest Neighbor Search):
 *   1. 递归下降到包含查询点的叶节点
 *   2. 回溯, 检查兄弟子树是否有更近的可能
 *      (用超球面与超平面的距离剪枝)
 *   3. 平均 O(log n), 最坏 O(n)
 *
 * 应用:
 *   - kNN 分类 / 回归
 *   - 范围搜索 (Range Search)
 *   - 碰撞检测 (游戏引擎)
 *   - 点云处理 (PCL 库)
 *
 * 局限:
 *   - 维数灾难: 高维时退化为 O(n) (维度 > 20 不推荐)
 *   - 对高维数据, 优先用近似算法 (LSH, Annoy, HNSW)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>
using namespace std;

struct Point2D {
    double x, y;
    Point2D(double x=0, double y=0) : x(x), y(y) {}
};

struct KDNode {
    Point2D point;
    KDNode* left;
    KDNode* right;
    KDNode(Point2D p) : point(p), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
    KDNode* root;
    int k = 2;  // 维度数

    // 按维度排序的比较器
    struct Compare {
        int dim;
        Compare(int d) : dim(d) {}
        bool operator()(const Point2D& a, const Point2D& b) const {
            if (dim == 0) return a.x < b.x;
            return a.y < b.y;
        }
    };

    KDNode* build(vector<Point2D>& pts, int depth, int l, int r) {
        if (l >= r) return nullptr;
        int dim = depth % k;
        int mid = (l + r) / 2;

        nth_element(pts.begin() + l, pts.begin() + mid, pts.begin() + r,
                     Compare(dim));

        KDNode* node = new KDNode(pts[mid]);
        node->left = build(pts, depth + 1, l, mid);
        node->right = build(pts, depth + 1, mid + 1, r);
        return node;
    }

    double dist2(const Point2D& a, const Point2D& b) const {
        return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
    }

    void nearest(KDNode* node, const Point2D& query, int depth,
                 Point2D& best, double& bestDist2) const {
        if (!node) return;
        double d2 = dist2(query, node->point);
        if (d2 < bestDist2) {
            bestDist2 = d2;
            best = node->point;
        }

        int dim = depth % k;
        double diff = (dim == 0) ? (query.x - node->point.x)
                                 : (query.y - node->point.y);

        KDNode* nearChild = (diff <= 0) ? node->left : node->right;
        KDNode* farChild  = (diff <= 0) ? node->right : node->left;

        nearest(nearChild, query, depth + 1, best, bestDist2);

        // 如果超球面可能与兄弟子树相交, 则检查
        if (diff * diff < bestDist2)
            nearest(farChild, query, depth + 1, best, bestDist2);
    }

public:
    KDTree(vector<Point2D>& pts) {
        root = build(pts, 0, 0, pts.size());
    }

    Point2D nearestNeighbor(const Point2D& query) const {
        Point2D best;
        double bestDist2 = numeric_limits<double>::max();
        nearest(root, query, 0, best, bestDist2);
        return best;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  KD-Tree (K 维搜索树)                    " << endl;
    cout << "========================================" << endl << endl;

    vector<Point2D> points = {
        {2, 3}, {5, 4}, {9, 6}, {4, 7},
        {8, 1}, {7, 2}, {3, 8}, {6, 5}
    };

    KDTree tree(points);

    cout << "[数据点] (共 " << points.size() << " 个)" << endl;
    for (auto& p : points)
        cout << "  (" << p.x << ", " << p.y << ")" << endl;

    Point2D query(5, 5);
    auto nn = tree.nearestNeighbor(query);
    cout << endl << "[最近邻查询]" << endl;
    cout << "  查询点: (" << query.x << ", " << query.y << ")" << endl;
    cout << "  最近邻: (" << nn.x << ", " << nn.y << ")" << endl;

    cout << endl << "[KD-Tree 特性]" << endl;
    cout << "  构建: O(n log n)" << endl;
    cout << "  查询: 平均 O(log n), 最坏 O(n)" << endl;
    cout << "  剪枝: 超球面与超平面的距离比较" << endl;
    cout << "  适用: 2~20 维低维空间" << endl;
    cout << "  替代: Ball Tree, R-Tree, HNSW" << endl;

    return 0;
}
