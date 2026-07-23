/**
 * LRU-K 缓存淘汰算法
 *
 * LRU (Least Recently Used) 的问题是容易被"一次性大量访问"污染。
 * LRU-K 改进: 考虑最近 K 次访问的历史，而不仅是最新一次。
 *
 * LRU-2 原理:
 *   维护两个队列:
 *     - 访问历史 (History): 记录仅访问过 1 次的数据
 *     - 缓存 (Cache): 存储访问过 ≥2 次的数据
 *   淘汰时从缓存队尾淘汰 (真正的"最近使用"而非刚加入的)
 *
 * 应用:
 *   - PostgreSQL buffer manager
 *   - 数据库索引缓存
 *
 * 本实现: 简化版 LRU-2，用 STL 容器模拟
 */

#include <iostream>
#include <list>
#include <unordered_map>
using namespace std;

class LRUKCache {
private:
    int capacity;  // 缓存容量
    int K;         // 考虑 K 次访问
    int accessCountThreshold;  // 达到此阈值才进入正式缓存

    struct Entry {
        int key, value;
        int accessCount;
        list<int>::iterator pos;  // 在对应链表中的位置
    };

    // 只访问过 1 次的"候选"链表
    list<int> history;
    // 访问过 ≥K 次的正式缓存链表
    list<int> cache;

    unordered_map<int, Entry> entries;

    void promoteToCache(int key) {
        auto& entry = entries[key];
        history.erase(entry.pos);

        // 加入缓存头部
        cache.push_front(key);
        entry.pos = cache.begin();

        // 若缓存已满，淘汰队尾
        if ((int)cache.size() > capacity) {
            int victim = cache.back();
            cache.pop_back();
            entries.erase(victim);
            cout << "  (淘汰 key=" << victim << ")" << endl;
        }
    }

public:
    LRUKCache(int cap, int k = 2) : capacity(cap), K(k), accessCountThreshold(k) {}

    int get(int key) {
        if (entries.find(key) == entries.end()) return -1;

        auto& entry = entries[key];
        entry.accessCount++;

        if (entry.accessCount >= accessCountThreshold) {
            // 移到缓存头部
            cache.erase(entry.pos);
            cache.push_front(key);
            entry.pos = cache.begin();
        }

        return entry.value;
    }

    void put(int key, int value) {
        if (entries.find(key) != entries.end()) {
            entries[key].value = value;
            get(key);  // 更新访问信息
            return;
        }

        // 新数据
        Entry entry{key, value, 1, history.end()};

        if (entry.accessCount >= accessCountThreshold) {
            // 直接进缓存
            cache.push_front(key);
            entry.pos = cache.begin();

            if ((int)cache.size() > capacity) {
                int victim = cache.back();
                cache.pop_back();
                entries.erase(victim);
            }
        } else {
            // 先进历史队列
            history.push_front(key);
            entry.pos = history.begin();
        }

        entries[key] = entry;
    }

    void printState() {
        cout << "  Cache (≥" << accessCountThreshold << "次访问): ";
        for (int k : cache) cout << k << ":" << entries[k].accessCount << "t ";
        cout << endl;
        cout << "  History (1次访问): ";
        for (int k : history) cout << k << " ";
        cout << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  LRU-K 缓存淘汰算法                     " << endl;
    cout << "========================================" << endl << endl;

    LRUKCache cache(3, 2);  // 容量3, K=2

    cout << "[操作序列]" << endl;
    cout << "  put(1,10)" << endl; cache.put(1, 10);
    cout << "  put(2,20)" << endl; cache.put(2, 20);
    cout << "  put(3,30)" << endl; cache.put(3, 30);
    cache.printState();
    cout << endl;

    // 再次访问 1 → 1 从 history 进入 cache
    cout << "  get(1) → " << cache.get(1) << " (第2次访问, 进入正式缓存)" << endl;

    // 大量访问 4 → 不会污染缓存
    for (int i = 0; i < 5; i++) {
        cout << "  put(4, 40)" << endl;
        cache.put(4, 40);
    }

    cout << endl << "[LRU vs LRU-2]" << endl;
    cout << "  LRU:   一次性大量访问4会淘汰1/2/3" << endl;
    cout << "  LRU-2: 4只访问1次 → 留在history, 不污染缓存" << endl;
    cout << "  应用: 数据库buffer pool, 抗扫描污染" << endl;

    return 0;
}
