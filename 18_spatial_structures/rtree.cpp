/**
 * R-Tree — 空间索引 (Spatial Index)
 *
 * R-Tree 是 B-Tree 在多维空间中的推广, 是最经典的空间索引结构。
 * 由 Antonin Guttman 于 1984 年提出。
 *
 * 核心思想:
 *   将空间对象组织成最小包围矩形 (MBR, Minimum Bounding Rectangle)
 *   的层次结构, 非叶节点存储子节点的 MBR 并集。
 *
 * 结构:
 *   叶节点: (MBR, object_id) 对
 *   内部节点: (MBR, child_ptr) 对
 *   MBR = (minX, minY, maxX, maxY)
 *
 * 操作:
 *   插入:
 *     1. 从根节点递归选择最优子树 (最小面积增长)
 *     2. 叶节点插入后向上调整 MBR
 *     3. 节点溢出 → 分裂
 *
 *   Guttman 分裂:
 *     1. PickSeeds: 挑两个"最远"的矩形作为种子
 *     2. 将剩余矩形逐个分配到面积增量更小的组
 *
 *   查询:
 *     1. 点查询: 沿 MBR 包含查询点的路径
 *     2. 范围查询: 沿 MBR 与查询矩形重叠的路径
 *     3. kNN: 优先队列 (Best-First Search)
 *
 * 变体:
 *   - R* Tree: 更优的插入策略 (最小重叠)
 *   - R+ Tree: 无重叠分区
 *   - Hilbert R-Tree: 基于 Hilbert 曲线排序
 *   - GiST (通用搜索树): PostgreSQL 的 R-Tree 实现
 *
 * 应用:
 *   - GIS (地理信息系统): 地图查询
 *   - 空间数据库: PostgreSQL PostGIS, Oracle Spatial
 *   - 碰撞检测: 游戏物理引擎
 *   - CAD: 电路布局
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

// MBR: Minimum Bounding Rectangle
struct MBR {
    double minX, minY, maxX, maxY;

    MBR(double x1=0, double y1=0, double x2=0, double y2=0)
        : minX(x1), minY(y1), maxX(x2), maxY(y2) {}

    double area() const {
        if (minX > maxX || minY > maxY) return 0;
        return (maxX - minX) * (maxY - minY);
    }

    bool contains(double x, double y) const {
        return x >= minX && x <= maxX && y >= minY && y <= maxY;
    }

    bool overlaps(const MBR& other) const {
        return !(minX > other.maxX || maxX < other.minX ||
                 minY > other.maxY || maxY < other.minY);
    }

    // 合并两个 MBR
    static MBR merge(const MBR& a, const MBR& b) {
        return MBR(min(a.minX, b.minX), min(a.minY, b.minY),
                   max(a.maxX, b.maxX), max(a.maxY, b.maxY));
    }

    // 合并后的面积增量
    double enlargementFor(const MBR& other) const {
        return merge(*this, other).area() - area();
    }
};

// R-Tree Node (简化版, 叶节点和内部节点用同一个结构)
struct RNode {
    bool isLeaf;
    MBR mbr;
    int pointId;                   // 叶节点: 数据点 ID
    vector<RNode*> children;      // 内部节点: 子节点

    RNode(bool leaf) : isLeaf(leaf), pointId(-1) {}

    void recalcMBR() {
        if (isLeaf) return;  // 叶节点的 MBR 由数据点决定
        for (auto* ch : children)
            mbr = MBR::merge(mbr, ch->mbr);
    }
};

// ==================== 简化版 R-Tree ====================
class RTree {
private:
    RNode* root;
    int maxChildren = 4;

    void searchRange(RNode* node, const MBR& query,
                     vector<int>& results) const {
        if (!node) return;
        if (!node->mbr.overlaps(query)) return;  // 剪枝

        if (node->isLeaf) {
            if (query.contains(node->mbr.minX, node->mbr.minY))
                results.push_back(node->pointId);
        } else {
            for (auto* ch : node->children)
                searchRange(ch, query, results);
        }
    }

public:
    RTree() : root(new RNode(false)) {}

    void rangeQuery(const MBR& query) const {
        vector<int> results;
        searchRange(root, query, results);
        cout << "  范围查询 [" << query.minX << "," << query.minY
             << "]→[" << query.maxX << "," << query.maxY
             << "]: 找到 " << results.size() << " 个点" << endl;
        for (int id : results) cout << "    点ID=" << id << " ";
        if (results.empty()) cout << "(空)";
        cout << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  R-Tree — 空间索引                       " << endl;
    cout << "========================================" << endl << endl;

    RTree tree;

    cout << "[R-Tree 核心概念]" << endl;
    cout << "  MBR = 最小包围矩形" << endl;
    cout << "  内部节点: 存储子节点 MBR 的并" << endl;
    cout << "  叶节点: 存储 (MBR, 数据指针)" << endl;
    cout << endl;

    cout << "[查询类型]" << endl;
    cout << "  1. 点查询: 检查 MBR 是否包含点" << endl;
    cout << "  2. 范围查询: 检查 MBR 是否重叠" << endl;
    cout << "  3. kNN: Best-First 优先队列" << endl;
    cout << endl;

    cout << "[剪枝原理]" << endl;
    cout << "  MBR 不相交 → 整颗子树可跳过" << endl;
    cout << "  类似 B-Tree 的键范围剪枝, 但推广到多维" << endl;
    cout << endl;

    cout << "[变体与应用]" << endl;
    cout << "  R* Tree: 最小重叠插入策略" << endl;
    cout << "  Hilbert R-Tree: Hilbert 曲线排序" << endl;
    cout << "  PostGIS GIST: PostgreSQL 空间索引" << endl;

    return 0;
}
