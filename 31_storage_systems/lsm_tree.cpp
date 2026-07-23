/**
 * LSM Tree (Log-Structured Merge-Tree) — 写优化的存储引擎
 *
 * LSM-Tree 是现代高性能写密集型存储系统的核心结构,
 * 被 LevelDB, RocksDB, Cassandra, HBase, WiredTiger 等广泛采用。
 *
 * 核心思想:
 *   将随机写转化为顺序写, 以空间换写入性能。
 *
 * 两层结构:
 *   C0 (MemTable): 内存中的有序数据结构 (跳表/红黑树)
 *     - 所有写入先到 C0
 *     - 读操作先查 C0, 未命中再查 C1
 *
 *   C1 (SSTable): 磁盘上的不可变排序文件
 *     - C0 满了就 flush 到磁盘成为一个新的 SSTable (SST)
 *     - 多个 SSTable 通过 Compaction 合并
 *
 * Compaction 策略:
 *   1. Size-Tiered: 大小相近的 SST 合并 (Cassandra 风格)
 *   2. Leveled: 每层大小固定, 逐层下沉 (LevelDB/RocksDB 风格)
 *       - Level 0: ~10MB, Level 1: ~100MB, Level 2: ~1GB, ...
 *       - 每层内的 SST 之间 key 不重叠
 *
 * 写放大 vs 读放大 vs 空间放大:
 *   - 写放大 (WA): 一次写入最终写磁盘的次数
 *   - 读放大 (RA): 一次读取需要访问的 SST 数量
 *   - 空间放大 (SA): 过时数据占用的额外空间
 *   LSM 三角权衡: 这三个指标互相制约
 */

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <list>
using namespace std;

// ==================== 简化版 LSM Tree ====================
class SimpleLSMTree {
private:
    map<int, string> memTable;               // C0: MemTable
    vector<map<int, string>> sstables;       // C1: SSTable 列表
    int memtableLimit;

    // 将 MemTable 刷到磁盘 (生成新 SSTable)
    void flush() {
        if (memTable.empty()) return;
        sstables.push_back(memTable);
        memTable.clear();
        cout << "  [Flush] MemTable → SSTable #" << sstables.size() << endl;
    }

    // 简易 Compaction: 合并两个 SSTable
    map<int, string> compactTwo(const map<int, string>& a,
                                 const map<int, string>& b) {
        map<int, string> merged = a;
        for (auto& [k, v] : b) merged[k] = v;  // 新值覆盖旧值
        return merged;
    }

public:
    SimpleLSMTree(int limit = 5) : memtableLimit(limit) {}

    // 写入
    void put(int key, string value) {
        memTable[key] = value;
        if ((int)memTable.size() >= memtableLimit) {
            flush();
        }
    }

    // 读取 (先 MemTable, 再倒序查 SSTable)
    string get(int key) {
        // 1. 查 MemTable
        auto it = memTable.find(key);
        if (it != memTable.end()) return it->second;

        // 2. 倒序查 SSTable (新的在后, 新数据覆盖旧数据)
        for (int i = sstables.size() - 1; i >= 0; i--) {
            auto it2 = sstables[i].find(key);
            if (it2 != sstables[i].end()) return it2->second;
        }
        return "未找到";
    }

    // 范围扫描
    vector<pair<int, string>> scan(int low, int high) {
        map<int, string> merged;

        // 从老到新合并 (新的覆盖)
        for (auto& sst : sstables)
            for (auto& [k, v] : sst)
                if (k >= low && k <= high) merged[k] = v;
        for (auto& [k, v] : memTable)
            if (k >= low && k <= high) merged[k] = v;

        return vector<pair<int, string>>(merged.begin(), merged.end());
    }

    // 手动 Compaction (合并所有 SSTable)
    void compact() {
        if (sstables.size() <= 1) return;
        cout << "  [Compaction] 合并 " << sstables.size() << " 个 SSTable..." << endl;
        flush();  // 先把 MemTable 也刷下去
        map<int, string> merged;
        for (auto& sst : sstables)
            for (auto& [k, v] : sst) merged[k] = v;
        sstables.clear();
        sstables.push_back(merged);
        cout << "  [Compaction] 完成 → 1 个 SSTable" << endl;
    }

    void printStats() {
        cout << "  MemTable: " << memTable.size() << " 条" << endl;
        cout << "  SSTable数: " << sstables.size() << endl;
        int total = memTable.size();
        for (auto& sst : sstables) total += sst.size();
        cout << "  总数据: " << total << " 条" << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  LSM Tree — 写优化的存储引擎              " << endl;
    cout << "========================================" << endl << endl;

    SimpleLSMTree lsm(4);  // MemTable 阈值为 4

    cout << "[写入操作]" << endl;
    for (int i = 1; i <= 12; i++) {
        string val = "value" + to_string(i);
        lsm.put(i, val);
        cout << "  put(" << i << ", " << val << ")" << endl;
    }
    cout << endl;

    lsm.printStats();
    cout << endl;

    cout << "[读取]" << endl;
    cout << "  get(5): " << lsm.get(5) << endl;
    cout << "  get(1): " << lsm.get(1) << endl;
    cout << "  get(99): " << lsm.get(99) << endl;
    cout << endl;

    cout << "[Compaction]" << endl;
    lsm.compact();
    lsm.printStats();
    cout << endl;

    cout << "[LSM Tree 三放大]" << endl;
    cout << "  写放大 (WA): 1次写入 = 多次Compaction重写" << endl;
    cout << "  读放大 (RA): 1次读取 = 查MemTable + N个SST" << endl;
    cout << "  空间放大 (SA): 过时数据在Compaction前仍占空间" << endl;
    cout << "  优化: Bloom Filter 减少读放大, 分层减少 WA" << endl;

    return 0;
}
