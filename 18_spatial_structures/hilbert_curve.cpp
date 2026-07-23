/**
 * Hilbert 曲线 (Hilbert Curve) — 空间填充曲线
 *
 * Hilbert 曲线是将多维空间映射到一维的连续分形曲线,
 * 具有极好的空间局部性: 在曲线上相邻的点在多维空间中也相邻。
 *
 * 关键性质:
 *   1. 空间填充 (Space-Filling): 足够高阶时经过所有格子
 *   2. 局部性保持 (Locality-Preserving):
 *      欧式距离相近的点, Hilbert 距离也相近 (反之不总成立)
 *   3. 递归构造: 用 4 个旋转/反射的 N-1 阶曲线拼接
 *
 * 构造 (2D, N阶):
 *   1阶:  ┌─┐  (基本 U 型)
 *        └─┘
 *   2阶: 将 1 阶的 4 个副本旋转后连接
 *
 * 应用:
 *   - 希尔伯特 R-Tree (Hilbert R-Tree):
 *     按 Hilbert 值对空间对象排序 → 更好的聚类效果
 *   - 数据库页分配: 相邻页在磁盘上相邻
 *   - 图像抖动 (Dithering)
 *   - 负载均衡 (相邻任务分配给相邻节点)
 *
 * Hilbert 值计算 (2D):
 *   给定坐标 (x,y) 和阶数 n, 用递归位交织计算
 *   位置在 Hilbert 曲线上的排列序号
 */

#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

// 将 (x, y) 坐标映射为 Hilbert 序数值 (阶数 order)
// 使用经典的 bit-manipulation 方法
long long xy2hilbert(int x, int y, int order) {
    long long d = 0;
    int n = 1 << order;

    for (int s = n / 2; s > 0; s >>= 1) {
        int rx = (x & s) ? 1 : 0;
        int ry = (y & s) ? 1 : 0;

        d += (long long)s * s * ((3 * rx) ^ ry);

        // 旋转坐标
        if (ry == 0) {
            if (rx == 1) {
                x = s - 1 - x;
                y = s - 1 - y;
            }
            swap(x, y);
        }
    }
    return d;
}

// 从 Hilbert 值恢复坐标
pair<int,int> hilbert2xy(long long d, int order) {
    int x = 0, y = 0;
    int n = 1 << order;
    long long t = d;

    for (int s = 1; s < n; s <<= 1) {
        int rx = 1 & (t / 2);
        int ry = 1 & (t ^ rx);

        // 旋转
        if (ry == 0) {
            if (rx == 1) { x = s - 1 - x; y = s - 1 - y; }
            swap(x, y);
        }
        x += s * rx;
        y += s * ry;
        t /= 4;
    }
    return {x, y};
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Hilbert 曲线 (空间填充曲线)              " << endl;
    cout << "========================================" << endl << endl;

    int order = 3;  // 2^3 = 8×8 网格
    int n = 1 << order;

    cout << "[Hilbert " << order << "阶] " << n << "×" << n << " 网格" << endl;
    cout << "  (0,0) →  hilbert = " << xy2hilbert(0, 0, order) << endl;
    cout << "  (7,0) →  hilbert = " << xy2hilbert(7, 0, order) << endl;
    cout << "  (7,7) →  hilbert = " << xy2hilbert(7, 7, order) << endl;

    // 验证可逆性
    cout << endl << "[可逆性验证]" << endl;
    auto [x1, y1] = hilbert2xy(xy2hilbert(3, 5, order), order);
    cout << "  (3,5) → Hilbert → (" << x1 << "," << y1 << ") "
         << ((x1==3 && y1==5) ? "✓" : "✗") << endl;

    cout << endl << "[Hilbert R-Tree]" << endl;
    cout << "  对空间对象按 Hilbert 值排序" << endl;
    cout << "  → 空间相邻的对象在存储上也相邻" << endl;
    cout << "  → 减少磁盘 I/O (PostGIS 采用的方案之一)" << endl;

    return 0;
}
