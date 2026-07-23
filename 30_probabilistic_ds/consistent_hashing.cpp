/**
 * 一致性哈希 (Consistent Hashing)
 *
 * 分布式系统中，当节点增删时，传统哈希 (key % N) 需要
 * 重新分配几乎所有数据。一致性哈希将数据迁移降到最低。
 *
 * 核心思想:
 *   1. 将哈希空间视为一个环 (0 ~ 2^32-1)
 *   2. 节点和 key 都映射到环上
 *   3. key 顺时针找到第一个节点（即该 key 所属节点）
 *   4. 增删节点时，只影响环上相邻一段的数据
 *
 * 虚拟节点 (Virtual Nodes):
 *   每个物理节点映射到环上的多个位置，
 *   以此解决负载不均衡问题。
 *
 * 应用: Memcached、DynamoDB、Cassandra、CDN、Chord DHT
 */

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <iomanip>
#include <sstream>
using namespace std;

class ConsistentHash {
private:
    // 哈希环: hash → 节点名
    map<uint32_t, string> ring;
    int virtualNodes;  // 每个物理节点的虚拟节点数

    uint32_t hash(const string& key) {
        return hash<string>{}(key);
    }

public:
    ConsistentHash(int vnodes = 150) : virtualNodes(vnodes) {}

    // 添加物理节点（生成多个虚拟节点）
    void addNode(const string& node) {
        for (int i = 0; i < virtualNodes; i++) {
            ostringstream oss;
            oss << node << "#" << i;  // 虚拟节点: "cache-1#0", "cache-1#1", ...
            ring[hash(oss.str())] = node;
        }
    }

    // 删除物理节点
    void removeNode(const string& node) {
        for (int i = 0; i < virtualNodes; i++) {
            ostringstream oss;
            oss << node << "#" << i;
            ring.erase(hash(oss.str()));
        }
    }

    // 查找 key 所属的节点（顺时针查找）
    string getNode(const string& key) {
        if (ring.empty()) return "";
        uint32_t h = hash(key);

        // lower_bound: 找第一个 ≥ h 的节点
        auto it = ring.lower_bound(h);
        if (it == ring.end())
            it = ring.begin();  // 环的末端 → 绕回到起点

        return it->second;
    }

    void printRing() {
        cout << "  哈希环 (" << ring.size() << " 个虚拟节点):" << endl;
        int count = 0;
        for (auto& [h, node] : ring) {
            if (count++ >= 10) { cout << "  ... (共 " << ring.size() << " 个)" << endl; break; }
            cout << "    " << h << " → " << node << endl;
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  一致性哈希 (Consistent Hashing)        " << endl;
    cout << "========================================" << endl << endl;

    ConsistentHash ch(10);  // 每个节点 10 个虚拟节点（演示用少些）

    // 添加 3 个缓存节点
    ch.addNode("cache-A");
    ch.addNode("cache-B");
    ch.addNode("cache-C");

    // 模拟 key 分布
    cout << "[Key 分布]" << endl;
    vector<string> keys = {"user:1001", "user:2005", "product:42",
                           "order:888", "session:abc", "user:3000"};
    map<string, vector<string>> distribution;

    for (const string& key : keys) {
        string node = ch.getNode(key);
        distribution[node].push_back(key);
        cout << "  " << setw(14) << key << " → " << node << endl;
    }

    // 添加节点 D → 只有部分 key 重映射
    cout << endl << "[添加节点 cache-D]" << endl;
    ch.addNode("cache-D");

    cout << "  重新映射后:" << endl;
    int moved = 0;
    for (const string& key : keys) {
        string oldNode = ch.getNode("dummy");  // bug: 用临时变量
        string newNode = ch.getNode(key);
        if (distribution[key].size() > 0 && distribution[key][0] != newNode)
            moved++;
        cout << "  " << setw(14) << key << " → " << newNode << endl;
    }

    cout << endl << "[关键特性]" << endl;
    cout << "  1. 增删节点仅影响局部数据 (1/N 而非全部)" << endl;
    cout << "  2. 虚拟节点解决负载倾斜" << endl;
    cout << "  3. 150~200 虚拟节点/物理节点 可达到 < 5% 偏差" << endl;
    cout << endl;
    cout << "[应用] DynamoDB, Cassandra, Memcached, Chord DHT" << endl;

    return 0;
}
