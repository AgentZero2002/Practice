/**
 * CDQ 分治 (CDQ Divide & Conquer)
 *
 * CDQ 分治由陈丹琦提出, 是解决偏序统计和动态问题的强大工具。
 * 核心思想: 将"修改 → 查询"的时间维度用分治消除。
 *
 * 经典问题: 二维偏序 / 三维偏序计数
 *   给定 N 个三维点 (x,y,z), 对每个点 i 求
 *   count(j) where xⱼ ≤ xᵢ, yⱼ ≤ yᵢ, zⱼ ≤ zᵢ
 *
 * CDQ 分治:
 *   按 x 排序 (消除第一维)
 *   CDQ(l, r):
 *     1. mid = (l+r)/2
 *     2. CDQ(l, mid); CDQ(mid+1, r)  (递归)
 *     3. 统计跨越中线的贡献:
 *        - 左侧的 y 作为"修改" (BIT.add(y, 1))
 *        - 右侧的 y 作为"查询" (BIT.sum(y))
 *     4. 清除 BIT
 *   第三步消除了时间维度, 只剩 y, z 两维!
 *   z 维用 BIT (树状数组) 处理。
 *
 * 复杂度: O(n log² n)
 *
 * 其他 CDQ 应用:
 *   - 离线动态问题 (加边/删边)
 *   - 多维偏序
 *   - 斜率优化 DP (非单调斜率)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct Point3D {
    int x, y, z, id;
};

// 简化版 CDQ: 二维偏序计数 (x排序, y用BIT)
class BIT {
    vector<int> tree;
    int n;
public:
    BIT(int n) : n(n), tree(n + 2, 0) {}
    void add(int i, int delta) {
        for (i++; i <= n; i += i & (-i)) tree[i] += delta;
    }
    int sum(int i) {
        int s = 0;
        for (i++; i > 0; i -= i & (-i)) s += tree[i];
        return s;
    }
};

void cdq(vector<Point3D>& pts, int l, int r, vector<int>& ans, BIT& bit) {
    if (l >= r) return;
    int mid = (l + r) / 2;
    cdq(pts, l, mid, ans, bit);
    cdq(pts, mid + 1, r, ans, bit);

    // 合并: 统计左半对右半的贡献
    // 按 y 排序
    vector<Point3D> tmp;
    int i = l, j = mid + 1;

    // 扫描 + BIT
    while (i <= mid && j <= r) {
        if (pts[i].y <= pts[j].y) {
            bit.add(pts[i].z, 1);
            tmp.push_back(pts[i++]);
        } else {
            ans[pts[j].id] += bit.sum(pts[j].z);
            tmp.push_back(pts[j++]);
        }
    }
    while (i <= mid) { bit.add(pts[i].z, 1); tmp.push_back(pts[i++]); }
    while (j <= r) { ans[pts[j].id] += bit.sum(pts[j].z); tmp.push_back(pts[j++]); }

    // 清除 BIT + 拷贝回
    for (int k = l; k <= mid; k++) bit.add(pts[k].z, -1);
    for (int k = l; k <= r; k++) pts[k] = tmp[k - l];
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  CDQ 分治 (三维偏序计数)                  " << endl;
    cout << "========================================" << endl << endl;

    // 三维点: (x, y, z)
    vector<Point3D> pts = {
        {1, 1, 1, 0}, {2, 2, 2, 1}, {3, 3, 3, 2},
        {1, 2, 3, 3}, {2, 1, 1, 4}
    };

    // 按 x 排序
    sort(pts.begin(), pts.end(), [](auto& a, auto& b) {
        return a.x < b.x;
    });

    BIT bit(5);
    vector<int> ans(5, 0);
    cdq(pts, 0, pts.size() - 1, ans, bit);

    cout << "[三维偏序结果]" << endl;
    for (int i = 0; i < 5; i++)
        cout << "  点" << i << ": " << ans[i] << " 个点支配它" << endl;

    cout << endl << "[CDQ 分治套路]" << endl;
    cout << "  第一维: sort" << endl;
    cout << "  第二维: 分治过程中归并排序" << endl;
    cout << "  第三维: BIT 维护" << endl;
    cout << "  推广到 K 维需要嵌套 CDQ 或 K-D Tree" << endl;

    return 0;
}
