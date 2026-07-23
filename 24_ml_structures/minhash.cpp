/**
 * MinHash — Jaccard 相似度估计 (LSH 的一种)
 *
 * MinHash 用于快速估计两个集合的 Jaccard 相似度,
 * 是 Google 早期网页去重、文档聚类等任务的核心技术。
 *
 * Jaccard 相似度:
 *   J(A, B) = |A ∩ B| / |A ∪ B|
 *
 * MinHash 原理:
 *   1. 对全集中的每个元素生成一个随机哈希值 h(x)
 *   2. 集合 S 的 MinHash = min{ h(x) | x ∈ S }
 *   3. 关键性质: Pr[MinHash(A) = MinHash(B)] = J(A, B)
 *
 * 证明 (简洁):
 *   考虑 A∪B 中哈希值最小的元素。
 *   - 如果该元素在 A∩B 中 → MinHash(A) = MinHash(B)
 *   - 如果该元素在 A\B 或 B\A 中 → MinHash(A) ≠ MinHash(B)
 *   所以 P(相等) = |A∩B| / |A∪B| = J(A, B) ∎
 *
 * 多个哈希函数 → k 维签名:
 *   用 k 个独立哈希函数, 得到 k 维 MinHash 签名。
 *   Jaccard ≈ (签名中位置相同的数量) / k
 *   k 越大, 估计越准 (误差 ∝ 1/√k)
 *
 * LSH 索引:
 *   签名分成 b 个 band, 每个 band 有 r 行 (k = b·r)。
 *   两个集合在某 band 中完全匹配 → 候选对 (candidate pair)。
 *   S 曲线: P(某band匹配) = 1 - (1 - J^r)^b
 *
 * 应用:
 *   - 网页去重 (Google News, 早期)
 *   - 文档聚类 (新闻聚合, LDA 预处理)
 *   - 协同过滤 (用户物品集合的相似度)
 *   - 抄袭检测
 */

#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <algorithm>
#include <limits>
using namespace std;

class MinHash {
    int numHashes;  // k
    vector<int> coeffA, coeffB;
    static const int PRIME = 2147483647;  // 2^31 - 1

public:
    MinHash(int k) : numHashes(k), coeffA(k), coeffB(k) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dist(1, PRIME - 1);
        for (int i = 0; i < k; i++) {
            coeffA[i] = dist(gen);
            coeffB[i] = dist(gen);
        }
    }

    // h(x) = (a*x + b) mod p
    vector<int> signature(const set<int>& elements) const {
        vector<int> sig(numHashes, numeric_limits<int>::max());
        for (int x : elements) {
            for (int i = 0; i < numHashes; i++) {
                int hashVal = (int)(((long long)coeffA[i] * x + coeffB[i]) % PRIME);
                sig[i] = min(sig[i], hashVal);
            }
        }
        return sig;
    }

    double estimateJaccard(const vector<int>& sigA,
                           const vector<int>& sigB) const {
        int matches = 0;
        for (int i = 0; i < numHashes; i++)
            if (sigA[i] == sigB[i]) matches++;
        return (double)matches / numHashes;
    }

    static double trueJaccard(const set<int>& a, const set<int>& b) {
        set<int> inter, uni;
        set_intersection(a.begin(), a.end(), b.begin(), b.end(),
                         inserter(inter, inter.begin()));
        set_union(a.begin(), a.end(), b.begin(), b.end(),
                  inserter(uni, uni.begin()));
        return uni.empty() ? 0 : (double)inter.size() / uni.size();
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  MinHash — Jaccard 相似度估计            " << endl;
    cout << "========================================" << endl << endl;

    // 模拟文档的 shingle 集合
    set<int> docA = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    set<int> docB = {1, 2, 3, 4, 5, 11, 12, 13, 14, 15};  // 50% 重叠
    set<int> docC = {6, 7, 8, 9, 10, 16, 17, 18, 19, 20}; // 50% 重叠(另一半)

    MinHash mh(256);  // 256 个哈希函数

    auto sigA = mh.signature(docA);
    auto sigB = mh.signature(docB);
    auto sigC = mh.signature(docC);

    cout << "[文档签名相似度]" << endl;
    cout << "  A vs B: 真实Jaccard=" << MinHash::trueJaccard(docA, docB)
         << ", MinHash估计=" << mh.estimateJaccard(sigA, sigB) << endl;
    cout << "  A vs C: 真实Jaccard=" << MinHash::trueJaccard(docA, docC)
         << ", MinHash估计=" << mh.estimateJaccard(sigA, sigC) << endl;
    cout << endl;

    cout << "[核心性质]" << endl;
    cout << "  Pr[MinHash(A)=MinHash(B)] = |A∩B|/|A∪B| = J(A,B)" << endl;
    cout << "  k 个哈希 → 估计误差 ∝ 1/√k" << endl;
    cout << endl;

    cout << "[LSH 索引 — S-Curve]" << endl;
    cout << "  签名分 b 个 band × r 行 (b·r = k)" << endl;
    cout << "  P(candidate) = 1 - (1 - J^r)^b" << endl;
    cout << "  b=20, r=5: J=0.8 → 96%找到, J=0.3 → 1%误报" << endl;
    cout << endl;

    cout << "[应用]" << endl;
    cout << "  - 网页去重 (Google News, 最早)" << endl;
    cout << "  - 文档聚类 (Datasketch 库)" << endl;
    cout << "  - 协同过滤 (LSH Forest)" << endl;

    return 0;
}
