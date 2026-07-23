/**
 * Buddy Allocator (伙伴分配器) — Linux 内核物理页分配
 *
 * Buddy Allocator 是 Linux 内核用于管理物理内存页的分配器。
 * 它以 2 的幂次大小分配内存, 通过分裂和合并伙伴块工作。
 *
 * 核心概念:
 *   伙伴 (Buddy): 两个大小相同、地址相邻的空闲块互为伙伴。
 *   设块大小为 2^k:
 *     - 伙伴块 A 的地址: A ^ (1 << k)  (异或翻转第 k 位)
 *     - 只有两个 Buddy 都空闲时才可合并为 2^(k+1) 的块
 *
 * 操作:
 *   alloc(size):
 *     1. 向上取整 size 到最近的 2^k
 *     2. 如果空闲链表 [k] 非空, 直接分配
 *     3. 否则从更大的块分裂 (递归)
 *
 *   free(ptr, size):
 *     1. 检查伙伴是否空闲
 *     2. 如果空闲 → 合并, 递归检查更大的伙伴
 *     3. 如果伙伴忙 → 加入空闲链表
 *
 * 优点:
 *   - 外部碎片少 (伙伴合并)
 *   - 分配/释放 O(log M), M = 最大块大小
 *   - 适合物理页分配 (块大小是 2 的幂)
 *
 * 缺点:
 *   - 内部碎片: 分配 3KB → 实际分 4KB (浪费 25%)
 *   - 不适合小于一页的分配 (→ slab allocator)
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;

class BuddyAllocator {
private:
    int maxOrder;  // 最大阶数 (2^maxOrder = 总内存)
    vector<vector<int>> freeLists;  // freeLists[order] = 空闲块起始地址列表

    int buddyOf(int addr, int order) {
        return addr ^ (1 << order);  // 异或翻转对应位
    }

public:
    BuddyAllocator(int totalSize) {
        maxOrder = 0;
        while ((1 << maxOrder) < totalSize) maxOrder++;
        freeLists.resize(maxOrder + 1);
        freeLists[maxOrder].push_back(0);  // 初始: 一整块内存
    }

    // 分配 2^order 大小的块
    int alloc(int order) {
        if (order > maxOrder) return -1;

        // 在当前阶或更高阶找空闲块
        int o = order;
        while (o <= maxOrder && freeLists[o].empty()) o++;
        if (o > maxOrder) return -1;  // 内存不足

        // 取出一块
        int addr = freeLists[o].back();
        freeLists[o].pop_back();

        // 递归分裂到目标大小
        while (o > order) {
            o--;
            int buddy = addr + (1 << o);
            freeLists[o].push_back(buddy);  // Buddy 加入空闲链表
        }
        return addr;
    }

    // 释放地址 addr, 阶数为 order 的块
    void free(int addr, int order) {
        while (order < maxOrder) {
            int buddy = buddyOf(addr, order);
            auto& fl = freeLists[order];
            auto it = find(fl.begin(), fl.end(), buddy);

            if (it != fl.end()) {
                // 伙伴空闲 → 合并
                fl.erase(it);
                addr = min(addr, buddy);
                order++;
            } else {
                break;  // 伙伴忙 → 停止合并
            }
        }
        freeLists[order].push_back(addr);
    }

    void printStats() {
        cout << "  [空闲链表]" << endl;
        for (int o = 0; o <= maxOrder; o++) {
            cout << "  order " << o << " (" << (1<<o) << "B): "
                 << freeLists[o].size() << " 块";
            if (!freeLists[o].empty())
                cout << " [addr:";
                for (int a : freeLists[o]) cout << " " << a;
                cout << "]";
            cout << endl;
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Buddy Allocator (伙伴分配器)             " << endl;
    cout << "========================================" << endl << endl;

    BuddyAllocator ba(64);  // 64 字节总空间
    cout << "[初始状态]" << endl;
    ba.printStats();

    cout << endl << "[alloc order=2 (4B)]" << endl;
    int a1 = ba.alloc(2);
    cout << "  → addr=" << a1 << endl;
    ba.printStats();

    cout << endl << "[alloc order=1 (2B)]" << endl;
    int a2 = ba.alloc(1);
    cout << "  → addr=" << a2 << endl;

    cout << endl << "[free addr=" << a1 << " order=2]" << endl;
    ba.free(a1, 2);
    ba.printStats();

    cout << endl << "[Linux 内核]" << endl;
    cout << "  Buddy: 管理物理页 (4KB)" << endl;
    cout << "  Slab:  管理内核对象 (< 1页)" << endl;
    cout << "  互补: Buddy + Slab = 完整内存管理" << endl;

    return 0;
}
