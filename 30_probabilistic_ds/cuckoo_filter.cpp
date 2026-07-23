/**
 * Cuckoo Filter — 改进版布隆过滤器
 *
 * Cuckoo Filter 是对 Bloom Filter 的改进:
 *   - 支持删除元素（Bloom 不支持）
 *   - 更低的假阳性率
 *   - 更高的空间效率（尤其是低假阳性率时）
 *
 * 原理:
 *   使用"布谷鸟哈希"(Cuckoo Hashing):
 *   每个元素有两个候选位置: h1(x) 和 h2(x) = h1(x) XOR fingerprint
 *   插入时若两个位置都被占用，随机踢出一个已有元素，
 *   被踢出的元素重新插入到其另一个候选位置。
 *   若踢出链过长，说明表已满，需要扩容。
 *
 * 应用: 数据库去重、网络包去重、缓存过滤
 */

#include <iostream>
#include <vector>
#include <random>
#include <cstring>
#include <algorithm>
using namespace std;

class CuckooFilter {
private:
    int capacity;
    int mask;
    vector<uint8_t> fingerprints;  // 指纹存储 (简化: 用 full byte)
    vector<bool> occupied;

    // 哈希函数
    uint32_t hash1(int x) {
        return (uint32_t)(x * 2654435761ULL) & mask;
    }

    uint32_t hash2(uint32_t fp, uint32_t idx1) {
        return (idx1 ^ (fp * 0x5bd1e995)) & mask;  // XOR 操作
    }

    uint8_t fingerprint(int x) {
        // 指纹: 取哈希的低8位，但至少为1（0表示空）
        return max((uint8_t)1, (uint8_t)(hash<int>{}(x) & 0xFF));
    }

public:
    CuckooFilter(int cap) : capacity(cap), mask(cap - 1) {
        fingerprints.resize(capacity, 0);
        occupied.resize(capacity, false);
    }

    bool insert(int x) {
        uint8_t fp = fingerprint(x);
        uint32_t idx1 = hash1(x);
        uint32_t idx2 = hash2(fp, idx1);

        // 尝试插入位置1
        if (!occupied[idx1]) {
            fingerprints[idx1] = fp;
            occupied[idx1] = true;
            return true;
        }

        // 尝试插入位置2
        if (!occupied[idx2]) {
            fingerprints[idx2] = fp;
            occupied[idx2] = true;
            return true;
        }

        // 随机踢出一个已有元素并重新安置
        uint32_t idx = (rand() % 2) ? idx1 : idx2;
        for (int kicks = 0; kicks < 500; kicks++) {
            // 踢出当前位置的元素
            uint8_t evictedFp = fingerprints[idx];
            fingerprints[idx] = fp;
            fp = evictedFp;

            // 计算被踢元素的另一个位置
            idx = hash2(fp, idx);

            // 若该位置为空，成功安置
            if (!occupied[idx]) {
                fingerprints[idx] = fp;
                occupied[idx] = true;
                return true;
            }
        }

        // 踢出链过长 → 表已满，需扩容
        return false;
    }

    bool mightContain(int x) {
        uint8_t fp = fingerprint(x);
        uint32_t idx1 = hash1(x);
        uint32_t idx2 = hash2(fp, idx1);

        return (occupied[idx1] && fingerprints[idx1] == fp) ||
               (occupied[idx2] && fingerprints[idx2] == fp);
    }

    bool remove(int x) {
        uint8_t fp = fingerprint(x);
        uint32_t idx1 = hash1(x);
        uint32_t idx2 = hash2(fp, idx1);

        if (occupied[idx1] && fingerprints[idx1] == fp) {
            occupied[idx1] = false;
            return true;
        }
        if (occupied[idx2] && fingerprints[idx2] == fp) {
            occupied[idx2] = false;
            return true;
        }
        return false;
    }

    int size() const {
        return count(occupied.begin(), occupied.end(), true);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Cuckoo Filter — 支持删除的过滤器       " << endl;
    cout << "========================================" << endl << endl;

    CuckooFilter cf(1024);  // capacity = 1024

    // 插入
    int keys[] = {42, 7, 100, 256, 42, 7, 512};
    for (int k : keys) {
        bool ok = cf.insert(k);
        if (k == 42 || k == 7)
            cout << "  插入 " << k << " (重复): " << (ok ? "OK" : "冲突") << endl;
    }
    cout << "  当前元素数: " << cf.size() << endl << endl;

    // 查询
    cout << "[查询]" << endl;
    for (int k : {42, 7, 100, 999}) {
        bool maybe = cf.mightContain(k);
        cout << "  " << k << ": " << (maybe ? "可能存在" : "一定不存在") << endl;
    }
    cout << endl;

    // 删除
    cout << "[删除 42]" << endl;
    cf.remove(42);
    cout << "  mightContain(42): " << (cf.mightContain(42) ? "仍存在" : "已删除") << endl;

    cout << endl << "[Cuckoo vs Bloom]" << endl;
    cout << "  Bloom:  不支持删除, 需计数版(空间×4)" << endl;
    cout << "  Cuckoo: 支持删除, 更低假阳性, 空间效率更高" << endl;

    return 0;
}
