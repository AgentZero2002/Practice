/**
 * 布隆过滤器 (Bloom Filter)
 *
 * 对应教材 第9章：词典 — 扩展内容
 *
 * 布隆过滤器是一种空间效率极高的概率数据结构，
 * 用于快速判断一个元素是否"可能"在集合中。
 *
 * 特征:
 *   - 可能会产生假阳性 (False Positive): 报告存在但实际不存在
 *   - 不会产生假阴性 (False Negative): 若报告不存在，则一定不存在
 *   - 不能删除元素（计数布隆过滤器可解决）
 *
 * 使用 k 个独立的哈希函数，将元素映射到位数组的 k 个位置。
 */

#include <iostream>
#include <bitset>
#include <string>
#include <functional>
using namespace std;

const int BIT_SIZE = 256;   // 位数组大小
const int HASH_COUNT = 3;   // 哈希函数个数

// 简单的 Bloom Filter
class BloomFilter {
private:
    bitset<BIT_SIZE> bits;

    // 用不同的种子生成 k 个哈希值
    size_t hash(string s, int seed) const {
        size_t h = 0;
        // 使用简单的多项式哈希
        for (char c : s) {
            h = h * (31 + seed) + c;
        }
        return h % BIT_SIZE;
    }

public:
    // 添加元素
    void add(const string& s) {
        for (int i = 0; i < HASH_COUNT; i++) {
            bits.set(hash(s, i));
        }
    }

    // 检查元素是否可能存在
    // true = 可能存在, false = 一定不存在
    bool mightContain(const string& s) const {
        for (int i = 0; i < HASH_COUNT; i++) {
            if (!bits.test(hash(s, i)))
                return false;  // 有一个位置为 0，一定不存在
        }
        return true;  // 所有位置为 1，可能存在
    }

    // 估计的误判率
    double falsePositiveRate(int n) const {
        // FPR ≈ (1 - e^(-k*n/m))^k
        // 其中 m = BIT_SIZE, k = HASH_COUNT, n = 插入元素数
        double k = HASH_COUNT, m = BIT_SIZE;
        return pow(1 - pow(1 - 1.0/m, k*n), k);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  布隆过滤器 (Bloom Filter)               " << endl;
    cout << "========================================" << endl << endl;

    BloomFilter bf;

    // 添加元素
    vector<string> words = {"apple", "banana", "cherry", "date", "elderberry"};
    cout << "[插入] ";
    for (const string& w : words) {
        cout << w << " ";
        bf.add(w);
    }
    cout << endl;

    // 查询
    cout << endl << "[查询]" << endl;
    vector<string> queries = {"apple", "banana", "grape", "cherry", "kiwi"};
    for (const string& q : queries) {
        bool maybe = bf.mightContain(q);
        bool inSet = find(words.begin(), words.end(), q) != words.end();
        cout << "  " << setw(10) << q << ": "
             << (maybe ? "可能存在" : "一定不存在")
             << " (实际: " << (inSet ? "在集合中" : "不在集合中") << ")"
             << (maybe != inSet ? " ★误判!" : "") << endl;
    }

    // 误判率评估
    cout << endl << "[误判率估计]" << endl;
    cout << "  位数组大小 m = " << BIT_SIZE << endl;
    cout << "  哈希函数数 k = " << HASH_COUNT << endl;
    cout << "  已插入元素 n = " << words.size() << endl;
    cout << "  估计误判率 ≈ " << bf.falsePositiveRate(words.size()) * 100 << "%" << endl;

    cout << endl << "[应用场景]" << endl;
    cout << "  1. 缓存穿透防护 (Redis Bloom Filter)" << endl;
    cout << "  2. 垃圾邮件过滤" << endl;
    cout << "  3. 数据库查询优化 (BigTable, Cassandra)" << endl;
    cout << "  4. URL 去重" << endl;

    return 0;
}
