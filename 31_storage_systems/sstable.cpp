/**
 * SSTable / MemTable — LevelDB 核心组件
 *
 * SSTable (Sorted String Table) 是 LevelDB/RocksDB 的磁盘存储单元,
 * 与 MemTable 配合构成 LSM-Tree 的完整存储栈。
 *
 * 写入路径 (Write Path):
 *   1. 写入 WAL (持久化)
 *   2. 写入 MemTable (内存跳表)
 *   3. MemTable 满了 → 冻结为 Immutable MemTable
 *   4. 后台线程将 Immutable MemTable flush 成 SSTable
 *
 * SSTable 文件结构:
 *   ┌──────────────────────────────────────────────┐
 *   │ Data Blocks (若干, 4KB 对齐, 含压缩键值对)     │
 *   ├──────────────────────────────────────────────┤
 *   │ Filter Block (Bloom Filter, 加速查找)         │
 *   ├──────────────────────────────────────────────┤
 *   │ Meta Index Block                             │
 *   ├──────────────────────────────────────────────┤
 *   │ Index Block (每个 Data Block 的起始键 → 偏移)  │
 *   ├──────────────────────────────────────────────┤
 *   │ Footer (magic number + Index/Filter 偏移/大小) │
 *   └──────────────────────────────────────────────┘
 *
 * 查找一个 key (在单 SSTable 中):
 *   1. 查 Bloom Filter → 不在则直接返回 not found
 *   2. 二分查找 Index Block → 定位到具体 Data Block
 *   3. 在 Data Block 内二分查找 (块内键有序)
 *   4. 返回 value
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

// ==================== 简化版 Block ====================
struct DataBlock {
    map<string, string> entries;  // key → value (有序)
    string startKey, endKey;

    void finalize() {
        if (!entries.empty()) {
            startKey = entries.begin()->first;
            endKey = entries.rbegin()->first;
        }
    }

    int size() const { return entries.size(); }

    bool contains(const string& key) const {
        return entries.count(key) > 0;
    }

    string get(const string& key) const {
        auto it = entries.find(key);
        return (it != entries.end()) ? it->second : "";
    }
};

// ==================== 简化版 Bloom Filter ====================
class SimpleBloom {
    vector<bool> bits;
    int m;  // 位数组大小
public:
    SimpleBloom(int m = 128) : m(m), bits(m, false) {}

    void add(const string& key) {
        // 简单的多重哈希 (用 std::hash)
        size_t h1 = hash<string>{}(key);
        size_t h2 = hash<string>{}(key + "salt");
        bits[(h1 * 31) % m] = true;
        bits[(h2 * 17) % m] = true;
        bits[(h1 ^ h2) % m] = true;
    }

    bool mayContain(const string& key) const {
        size_t h1 = hash<string>{}(key);
        size_t h2 = hash<string>{}(key + "salt");
        return bits[(h1 * 31) % m] &&
               bits[(h2 * 17) % m] &&
               bits[(h1 ^ h2) % m];
    }
};

// ==================== SSTable ====================
class SSTable {
private:
    vector<DataBlock> dataBlocks;
    map<string, int> indexBlock;  // startKey → block index
    SimpleBloom bloom;
    int blockSizeLimit;

public:
    SSTable(int blockLimit = 3) : blockSizeLimit(blockLimit) {}

    // 从 MemTable (map) 构建 SSTable
    void buildFrom(const map<string, string>& memtable) {
        DataBlock currentBlock;
        for (auto& [k, v] : memtable) {
            if (currentBlock.size() >= blockSizeLimit) {
                currentBlock.finalize();
                dataBlocks.push_back(currentBlock);
                currentBlock = DataBlock();
            }
            currentBlock.entries[k] = v;
            bloom.add(k);
        }
        if (currentBlock.size() > 0) {
            currentBlock.finalize();
            dataBlocks.push_back(currentBlock);
        }

        // 构建 Index Block
        for (int i = 0; i < (int)dataBlocks.size(); i++) {
            indexBlock[dataBlocks[i].startKey] = i;
        }

        cout << "  [Build SSTable] " << dataBlocks.size() << " Data Blocks, "
             << memtable.size() << " keys" << endl;
    }

    // 查找
    string get(const string& key) const {
        // 1. Bloom Filter
        if (!bloom.mayContain(key))
            return "❌ (Bloom: 肯定不存在)";

        // 2. Index Block 定位
        auto it = indexBlock.upper_bound(key);
        if (it == indexBlock.begin())
            return "❌ (键小于最小键)";
        int blkIdx = prev(it)->second;

        // 3. Data Block 内查找
        const auto& blk = dataBlocks[blkIdx];
        if (blk.contains(key))
            return blk.get(key);

        return "❌ (Bloom 假阳性)";
    }

    void printStructure() {
        cout << endl << "  [SSTable 结构]" << endl;
        cout << "  Index Block:" << endl;
        for (auto& [startKey, idx] : indexBlock)
            cout << "    key≥\"" << startKey << "\" → Block#" << idx << endl;
        for (size_t i = 0; i < dataBlocks.size(); i++) {
            cout << "  Data Block #" << i << " ["
                 << dataBlocks[i].startKey << " .. "
                 << dataBlocks[i].endKey << "] : "
                 << dataBlocks[i].size() << " 条" << endl;
        }
    }
};

// ==================== MemTable ====================
class MemTable {
    map<string, string> table;  // 实际用跳表 (Skip List)
    int limit;
public:
    MemTable(int limit = 6) : limit(limit) {}

    void put(const string& key, const string& value) {
        table[key] = value;
    }

    string get(const string& key) const {
        auto it = table.find(key);
        return (it != table.end()) ? it->second : "";
    }

    bool isFull() const { return (int)table.size() >= limit; }

    const map<string, string>& data() const { return table; }

    int size() const { return table.size(); }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  SSTable / MemTable — LevelDB 核心      " << endl;
    cout << "========================================" << endl << endl;

    // 模拟 MemTable → SSTable 写入路径
    MemTable mem(6);
    cout << "[写入 MemTable]" << endl;
    vector<pair<string,string>> data = {
        {"apple", "red"}, {"banana", "yellow"}, {"cherry", "red"},
        {"date", "brown"}, {"elderberry", "purple"}, {"fig", "green"},
        {"grape", "purple"}
    };
    for (auto& [k, v] : data) {
        mem.put(k, v);
        cout << "  put(" << k << ", " << v << ")" << endl;
    }

    cout << endl << "[MemTable 满了 → Flush → SSTable]" << endl;
    SSTable sst;
    sst.buildFrom(mem.data());
    sst.printStructure();

    cout << endl << "[SSTable 查找]" << endl;
    for (string key : {"apple", "cherry", "grape", "zebra"}) {
        cout << "  get(\"" << key << "\") → " << sst.get(key) << endl;
    }

    cout << endl << "[SSTable 写入路径总结]" << endl;
    cout << "  1. WAL Append (持久性)" << endl;
    cout << "  2. MemTable Insert (内存跳表)" << endl;
    cout << "  3. MemTable 满 → Immutable MemTable" << endl;
    cout << "  4. Background Flush → SSTable" << endl;
    cout << "  5. Compaction: merge SSTables" << endl;

    return 0;
}
