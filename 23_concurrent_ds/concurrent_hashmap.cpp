/**
 * 并发哈希表 (Concurrent Hash Map) — 分段锁
 *
 * 实现高并发哈希表的核心策略。
 *
 * 策略1: 分段锁 (Striped Locking / Java ConcurrentHashMap)
 *   将哈希表分为 N 个段 (segment), 每段有自己的锁。
 *   不同段的操作可完全并行, 同段内的操作才互斥。
 *   段数 N 决定了最大并发度。
 *
 * 策略2: 无锁 (Lock-Free Hash Map)
 *   每个桶的链表用 CAS 操作 (类似 lock-free stack 的思路)。
 *   实现更复杂但延迟最低。
 *   代表: Java ConcurrentHashMap (JDK 8+), Cliff Click 的
 *         high-scale-lib NonBlockingHashMap
 *
 * 策略3: Read-Write Lock Per Bucket
 *   读-读不互斥, 读-写互斥, 写-写互斥。
 *   适合读多写少的场景。
 *
 * 关键权衡:
 *   - 段数过少 → 竞争大
 *   - 段数过多 → 内存开销大, 且扩容复杂
 *   - JDK 7 ConcurrentHashMap: 默认 16 段
 *   - JDK 8+ 改为 CAS + synchronized (桶级别)
 */

#include <iostream>
#include <vector>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <functional>
using namespace std;

// ==================== 分段锁哈希表 ====================
template<typename K, typename V>
class StripedHashMap {
private:
    static const int NUM_SEGMENTS = 4;  // 段数

    struct Segment {
        vector<list<pair<K,V>>> buckets;
        mutable shared_mutex mtx;            // 读写锁

        Segment(int capacity) : buckets(capacity) {}
    };

    vector<Segment> segments;
    int capacity;

    int hash(const K& key) const {
        return std::hash<K>{}(key);
    }

    int segmentIndex(const K& key) const {
        return hash(key) % NUM_SEGMENTS;
    }

    int bucketIndex(const K& key) const {
        return hash(key) % capacity;
    }

public:
    StripedHashMap(int cap = 16) : capacity(cap) {
        for (int i = 0; i < NUM_SEGMENTS; i++)
            segments.emplace_back(capacity);
    }

    // 读操作: 共享锁
    bool get(const K& key, V& value) const {
        int segIdx = segmentIndex(key);
        int bktIdx = bucketIndex(key);

        shared_lock lock(segments[segIdx].mtx);  // 读锁 (多个线程可同时)
        for (auto& [k, v] : segments[segIdx].buckets[bktIdx])
            if (k == key) { value = v; return true; }
        return false;
    }

    // 写操作: 独占锁
    void put(const K& key, const V& value) {
        int segIdx = segmentIndex(key);
        int bktIdx = bucketIndex(key);

        unique_lock lock(segments[segIdx].mtx);  // 写锁 (互斥)
        auto& bucket = segments[segIdx].buckets[bktIdx];
        for (auto& [k, v] : bucket)
            if (k == key) { v = value; return; }
        bucket.push_back({key, value});
    }

    // 删除
    bool remove(const K& key) {
        int segIdx = segmentIndex(key);
        int bktIdx = bucketIndex(key);

        unique_lock lock(segments[segIdx].mtx);
        auto& bucket = segments[segIdx].buckets[bktIdx];
        for (auto it = bucket.begin(); it != bucket.end(); ++it)
            if (it->first == key) { bucket.erase(it); return true; }
        return false;
    }

    void printStats() const {
        cout << "  段数: " << NUM_SEGMENTS
             << ", 桶数/段: " << capacity << endl;
        for (int i = 0; i < NUM_SEGMENTS; i++) {
            int items = 0;
            for (auto& bkt : segments[i].buckets)
                items += bkt.size();
            cout << "  Segment " << i << ": " << items << " 条" << endl;
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  并发哈希表 (分段锁 / Striped Lock)       " << endl;
    cout << "========================================" << endl << endl;

    StripedHashMap<string, int> map(8);

    cout << "[写入]" << endl;
    map.put("Alice", 25);
    map.put("Bob", 30);
    map.put("Charlie", 35);
    map.put("David", 28);
    map.put("Eve", 22);
    cout << "  插入 5 条记录" << endl;
    map.printStats();

    cout << endl << "[并发场景模拟]" << endl;
    cout << "  线程1: 读 Alice   (shared_lock → 与线程2并行)" << endl;
    cout << "  线程2: 读 Bob     (shared_lock → 与线程1并行)" << endl;
    cout << "  线程3: 写 Frank   (unique_lock → 只锁段0)" << endl;
    cout << "  线程4: 写 Grace   (unique_lock → 只锁段2, 与线程3并行!)" << endl;

    cout << endl << "[分段锁要点]" << endl;
    cout << "  段数=" << 4 << ", 最大并发度=" << 4 << endl;
    cout << "  不同段的写操作完全并行" << endl;
    cout << "  同段的读操作共享锁, 可并行" << endl;
    cout << "  同段的写操作互斥" << endl;

    cout << endl << "[演进]" << endl;
    cout << "  Java 7: Segment[] + ReentrantLock" << endl;
    cout << "  Java 8+: CAS + synchronized (更高并发)" << endl;

    return 0;
}
