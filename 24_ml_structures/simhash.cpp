/**
 * SimHash — 近似余弦相似度 (Near-Duplicate Detection)
 *
 * SimHash 由 Moses Charikar 提出, 是 Google 爬虫用于网页去重的
 * 核心技术。它将高维向量映射为紧凑的二进制指纹 (fingerprint)。
 *
 * 核心思想:
 *   给定一个文档的 TF-IDF 特征向量 v ∈ R^d,
 *   生成一个 f 位的 0/1 签名:
 *     1. 初始化 f 维向量 S = 0
 *     2. 对每个特征 (维度 i, 权重 w_i):
 *        - 生成该维度的 f 位随机哈希 h(i) (每位的值是 ±1)
 *        - 如果特征权重 w_i > 0: S += w_i * h(i)
 *        - 如果特征权重 w_i < 0: S -= w_i * h(i)  (不要的)
 *     3. 最终签名: sign(S_j) → 正为 1, 负为 0
 *
 * 关键性质:
 *   两个向量的汉明距离 (Hamming Distance) 反比于它们的余弦相似度。
 *   具体: E[Hamming(sig(v1), sig(v2))] ∝ arccos(cosine(v1, v2))
 *
 * 与 MinHash 对比:
 *   MinHash:  适合 Jaccard 相似度 (集合)
 *   SimHash:  适合余弦相似度 (加权向量)
 *
 * 海量去重:
 *   1. 计算所有文档的 f 位 SimHash
 *   2. 对于汉明距离 ≤ 3 (几乎所有重复), 用分块查找:
 *      - 将 f 位分成 4 块, 每块 B 位
 *      - 对每块建哈希表 (块值 → 文档列表)
 *      - 汉明距离 ≤ 3 → 至少一块完全相同!
 *      - 只需检查同一桶内的文档对 → O(n) 而非 O(n²)
 *
 * 应用:
 *   - Google 爬虫网页去重
 *   - 新闻去重
 *   - 代码克隆检测
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <random>
#include <bitset>
#include <cmath>
#include <algorithm>
using namespace std;

const int FINGERPRINT_BITS = 64;  // 64 位指纹

// 生成随机哈希向量 (f 位, 每位的值 ±1)
using HashVec = bitset<FINGERPRINT_BITS>;

HashVec randomHash(int seed) {
    mt19937 gen(seed);
    HashVec hv;
    for (int i = 0; i < FINGERPRINT_BITS; i++)
        hv[i] = gen() % 2;
    return hv;
}

// 计算 SimHash 指纹
bitset<FINGERPRINT_BITS> computeSimHash(
    const map<int, double>& features) {  // <feature_id, weight>

    vector<long long> sums(FINGERPRINT_BITS, 0);

    for (auto& [featureID, weight] : features) {
        HashVec hv = randomHash(featureID);
        for (int j = 0; j < FINGERPRINT_BITS; j++) {
            if (hv[j])  // bit=1 → +weight
                sums[j] += (long long)(weight * 1000);  // 精度放大
            else        // bit=0 → -weight
                sums[j] -= (long long)(weight * 1000);
        }
    }

    bitset<FINGERPRINT_BITS> fingerprint;
    for (int j = 0; j < FINGERPRINT_BITS; j++)
        fingerprint[j] = (sums[j] >= 0);  // 正→1, 负→0
    return fingerprint;
}

// 汉明距离
int hammingDist(const bitset<FINGERPRINT_BITS>& a,
                const bitset<FINGERPRINT_BITS>& b) {
    return (a ^ b).count();
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  SimHash — 近似余弦/网页去重              " << endl;
    cout << "========================================" << endl << endl;

    // 模拟两个文档的 TF-IDF 特征
    map<int, double> doc1 = {{1, 3.5}, {2, 2.1}, {5, 1.0}, {10, 0.5}};
    map<int, double> doc2 = {{1, 3.5}, {2, 2.1}, {5, 1.0}, {10, 0.5}};  // 相同
    map<int, double> doc3 = {{1, 3.5}, {2, 2.1}, {7, 1.0}, {15, 0.5}};  // 部分不同
    map<int, double> doc4 = {{8, 3.0}, {9, 2.0}, {12, 1.5}, {20, 1.0}}; // 完全不同

    auto fp1 = computeSimHash(doc1);
    auto fp2 = computeSimHash(doc2);
    auto fp3 = computeSimHash(doc3);
    auto fp4 = computeSimHash(doc4);

    cout << "[SimHash 签名] (前16位)" << endl;
    auto shortStr = [](const bitset<FINGERPRINT_BITS>& bs) {
        string s;
        for (int i = 0; i < 16; i++) s += bs[i] ? '1' : '0';
        return s + "...";
    };

    cout << "  Doc1: " << shortStr(fp1) << endl;
    cout << "  Doc2: " << shortStr(fp2) << " (同Doc1)" << endl;
    cout << "  Doc3: " << shortStr(fp3) << " (部分不同)" << endl;
    cout << "  Doc4: " << shortStr(fp4) << " (完全不同)" << endl;
    cout << endl;

    cout << "[汉明距离]" << endl;
    cout << "  Doc1 vs Doc2: " << hammingDist(fp1, fp2)
         << " bit (相同 → ~0)" << endl;
    cout << "  Doc1 vs Doc3: " << hammingDist(fp1, fp3)
         << " bits (部分不同)" << endl;
    cout << "  Doc1 vs Doc4: " << hammingDist(fp1, fp4)
         << " bits (完全不同, 期望 ~32)" << endl;
    cout << endl;

    cout << "[海量去重 — 分块查找]" << endl;
    cout << "  • 64 位分 4 块 × 16 位" << endl;
    cout << "  • 汉明距离 ≤ 3 → 至少 1 块完全相同" << endl;
    cout << "  • 只需对每块建立哈希表 → O(n) 而非 O(n²)" << endl;
    cout << "  • Google 爬虫的去重核心技术" << endl;

    return 0;
}
