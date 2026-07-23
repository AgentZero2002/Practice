/**
 * Locality-Sensitive Hashing (LSH) — 近似最近邻搜索
 *
 * LSH 是一类哈希技术的总称, 核心思想是:
 *   "近邻的点被哈希到同一个桶的概率高, 远离的点碰撞概率低"
 *
 * 形式化定义:
 *   一个哈希族 H 是 (R, cR, p1, p2)-敏感的, 如果对任意两点 p, q:
 *     - 若 dist(p,q) ≤ R:   Pr[h(p)=h(q)] ≥ p1  (近邻→高碰撞)
 *     - 若 dist(p,q) ≥ cR:  Pr[h(p)=h(q)] ≤ p2  (远离→低碰撞)
 *     其中 c > 1, p1 > p2
 *
 * LSH 变体 (按距离度量):
 *
 * 1. Euclidean LSH (E2LSH, Datar et al.):
 *    用随机超平面投影: h(p) = ⌊(p·r + b) / w⌋
 *    即用随机方向 r 投影后分桶 (p-stable distribution)
 *
 * 2. Cosine LSH (随机超平面 / Random Projection):
 *    h(p) = sign(p · r)  (随机超平面法线 r, sign 给出 0/1)
 *    碰撞概率: Pr[h(p)=h(q)] = 1 - θ/π
 *    其中 θ = arccos(cos(p,q)), 与余弦相似度直接相关
 *
 * 3. Jaccard LSH (MinHash):
 *    h(S) = min{π(x) | x ∈ S}
 *    碰撞概率 = Jaccard(S, T)  (见 minhash.cpp)
 *
 * 查询策略 — AND-OR 复合:
 *   用 L 个哈希表, 每个表用 k 个哈希函数 AND 串联:
 *     g(p) = (h1(p), h2(p), ..., hk(p))  ← k 个哈希 AND
 *     查询时在 L 个表中查找 g(q), 合并候选集再精确排序
 *   参数: k ↑ → 准确率 ↑ (p2^k 小), L ↑ → 召回率 ↑ (1-(1-p1^k)^L 大)
 *
 * 应用:
 *   - 近似最近邻 (ANN) 搜索
 *   - 图像检索 (视觉词袋)
 *   - 推荐系统 (用户/物品向量相似)
 *   - 向量数据库 (Milvus, Faiss 的 IVF+PQ 常与 LSH 结合)
 */

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <bitset>
using namespace std;

// ==================== Cosine LSH (随机超平面) ====================

class CosineLSH {
    int dim;         // 向量维度
    int numBits;     // 签名位数 (即哈希表键长 k)
    vector<vector<double>> randomVectors;  // k 个随机超平面法线

public:
    CosineLSH(int dimension, int bits) : dim(dimension), numBits(bits) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<double> gauss(0.0, 1.0);

        randomVectors.resize(bits, vector<double>(dim));
        for (int i = 0; i < bits; i++)
            for (int d = 0; d < dim; d++)
                randomVectors[i][d] = gauss(gen);
    }

    // 计算 LSH 签名 (k 位)
    bitset<64> hash(const vector<double>& vec) const {
        bitset<64> sig;
        for (int i = 0; i < numBits; i++) {
            double dot = 0;
            for (int d = 0; d < dim; d++)
                dot += vec[d] * randomVectors[i][d];
            sig[i] = (dot >= 0);  // sign
        }
        return sig;
    }

    // 碰撞检测
    int hammingDist(const bitset<64>& a, const bitset<64>& b) const {
        return (a ^ b).count();
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  LSH — 局部敏感哈希 (近似最近邻)          " << endl;
    cout << "========================================" << endl << endl;

    int dim = 4;
    CosineLSH lsh(dim, 16);  // 4 维向量, 16 位签名

    // 三个向量: v1 ≈ v2, v3 远离
    vector<double> v1 = {0.5, 0.3, 0.2, 0.1};
    vector<double> v2 = {0.5, 0.35, 0.18, 0.12};  // 接近 v1
    vector<double> v3 = {-0.5, -0.3, -0.8, 0.7};  // 远离 v1

    auto sig1 = lsh.hash(v1);
    auto sig2 = lsh.hash(v2);
    auto sig3 = lsh.hash(v3);

    auto shortStr = [](const bitset<64>& bs) {
        string s;
        for (int i = 0; i < 16; i++) s += bs[i] ? '1' : '0';
        return s;
    };

    cout << "[LSH 签名] (16 位)" << endl;
    cout << "  v1: " << shortStr(sig1) << endl;
    cout << "  v2: " << shortStr(sig2) << " (≈v1)" << endl;
    cout << "  v3: " << shortStr(sig3) << " (≠v1)" << endl;
    cout << endl;

    cout << "[汉明距离]" << endl;
    cout << "  v1 vs v2: " << lsh.hammingDist(sig1, sig2) << " (接近 → 小距离)" << endl;
    cout << "  v1 vs v3: " << lsh.hammingDist(sig1, sig3) << " (远离 → 大距离)" << endl;
    cout << endl;

    cout << "[LSH 查询策略]" << endl;
    cout << "  碰撞概率 P = 1 - arccos(cos)/π" << endl;
    cout << "  AND: k 个哈希串联 → p^k (降低误报)" << endl;
    cout << "  OR:  L 个哈希表并联 → 1-(1-p^k)^L (提高召回)" << endl;
    cout << "  应用: Faiss, Annoy, NMSLIB, Milvus" << endl;

    return 0;
}
