/**
 * HyperLogLog — 基数估计 (Cardinality Estimation)
 *
 * HyperLogLog 用于估计数据流中不重复元素的数量（基数）。
 * 使用极小的内存（约12KB）即可达到 ≈2% 的标准误差！
 *
 * 原理:
 *   1. 对每个元素计算哈希值 h(x)
 *   2. 记录 h(x) 的前导零数量 ρ = leading_zeros(h(x))
 *   3. 用调和平均数消除离群值的影响:
 *      E = α_m × m² / (Σ 2^{-ρ_i})
 *      其中 α_m 为偏差修正系数，m 为寄存器数量
 *
 * 直觉: 一个随机哈希值的前导零个数分布为:
 *   P(ρ=v) = 2^{-(v+1)}, v ≥ 0
 *   见到 ρ≥k 需要约 2^k 个不同元素
 *
 * Redis 实现: PFADD / PFCOUNT 命令
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <functional>
#include <algorithm>
using namespace std;

class HyperLogLog {
private:
    int m;              // 寄存器数量 (2^p)
    int p;              // m = 2^p
    vector<int> regs;   // 寄存器, regs[i] = max ρ for bucket i
    hash<int> hasher;

    // 修正系数 α_m
    double alpha() const {
        if (m == 16) return 0.673;
        if (m == 32) return 0.697;
        if (m == 64) return 0.709;
        return 0.7213 / (1.0 + 1.079 / m);
    }

    // 前导零个数 (ρ)
    int rho(uint32_t hashVal) {
        // 低 p 位用于选择桶，高位用于统计 ρ
        uint32_t val = hashVal >> p;  // 去掉桶选择位
        if (val == 0) return 32 - p;
        return __builtin_clz(val) + 1;  // GCC/Clang 内置函数
    }

    int bucket(uint32_t hashVal) {
        return hashVal & (m - 1);  // 低 p 位
    }

public:
    // p: 精度参数，m = 2^p
    // p=14 → 16KB, 误差 ≈ 0.81%
    HyperLogLog(int p = 14) : p(p), m(1 << p), regs(m, 0) {}

    void add(int x) {
        uint32_t h = hasher(x);
        int b = bucket(h);
        regs[b] = max(regs[b], rho(h));
    }

    // 基数估计
    long long estimate() {
        double sum = 0;
        int zeroRegs = 0;

        for (int r : regs) {
            sum += pow(2.0, -r);
            if (r == 0) zeroRegs++;
        }

        double rawEst = alpha() * m * m / sum;

        // 小基数修正
        if (rawEst <= 2.5 * m && zeroRegs > 0)
            rawEst = m * log((double)m / zeroRegs);

        return (long long)round(rawEst);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  HyperLogLog — 基数估计                 " << endl;
    cout << "========================================" << endl << endl;

    mt19937 rng(42);

    for (int p : {8, 10, 12}) {  // p=8→256B, p=12→4KB
        HyperLogLog hll(p);
        int N = 10000;
        uniform_int_distribution<int> dist(1, 5000);  // 约 5000 个不同值

        for (int i = 0; i < N; i++)
            hll.add(dist(rng));

        long long est = hll.estimate();
        cout << "  p=" << p << " (m=" << (1<<p)
             << "), N=" << N << " 个元素" << endl;
        cout << "    估计基数: " << est
             << "  误差: " << abs(est - 5000.0) / 5000 * 100 << "%" << endl;
    }

    cout << endl << "[对比]" << endl;
    cout << "  精确计数:  O(n) 空间 → 大数据下不可行" << endl;
    cout << "  Bitmap:    O(unique_count) → 可能很大" << endl;
    cout << "  HLL:       固定 O(2^p) → 12KB即可达标准误2%" << endl;
    cout << endl;
    cout << "[应用] Redis PFCOUNT, Google BigQuery, Presto" << endl;

    return 0;
}
