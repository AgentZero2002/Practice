/**
 * Memory Pool (对象池 / 内存池) — 高频分配优化
 *
 * Memory Pool 是一次分配一大块内存, 然后自行管理的策略。
 * 相比每次 new/malloc 都有系统调用 + 元数据开销,
 * Pool 只需在 Pool 内移动指针 → O(1) 分配, 无碎片。
 *
 * 核心思想:
 *   1. 预先分配一块大内存 (chunk)
 *   2. 每个 chunk 切分为固定大小的 slot
 *   3. 分配: 从空闲链表 (free list) 取一个 slot O(1)
 *   4. 释放: 将 slot 归还到空闲链表 O(1)
 *
 * 设计选择:
 *   - 固定大小 vs 可变大小 (可变大小难, 常用 buddy)
 *   - 线程安全: 每线程一个 Pool (Thread-Local Cache)
 *   - 回收: 所有对象释放后统一归还 OS
 *
 * 实际应用:
 *   - C++ STL allocator: std::allocator 可用 memory pool 替换
 *   - Nginx: 连接池 + 内存池
 *   - 游戏引擎: 每帧临时对象用完即弃
 *   - jemalloc / tcmalloc: 多线程优化的 Pool Allocator
 */

#include <iostream>
#include <vector>
using namespace std;

class MemoryPool {
private:
    struct Block {
        Block* next;
    };
    Block* freeList = nullptr;
    vector<char*> chunks;  // 所有大块
    size_t slotSize;
    int slotsPerChunk;
    int allocated = 0;

    void allocateChunk() {
        char* chunk = new char[slotSize * slotsPerChunk];
        chunks.push_back(chunk);

        // 将 chunk 中的 slot 链接成空闲链表
        for (int i = 0; i < slotsPerChunk; i++) {
            Block* slot = reinterpret_cast<Block*>(chunk + i * slotSize);
            slot->next = freeList;
            freeList = slot;
        }
    }

public:
    MemoryPool(size_t size, int perChunk = 64)
        : slotSize(max(size, sizeof(Block))), slotsPerChunk(perChunk) {}

    void* allocate() {
        if (!freeList) allocateChunk();
        void* ptr = freeList;
        freeList = freeList->next;
        allocated++;
        return ptr;
    }

    void deallocate(void* ptr) {
        Block* slot = static_cast<Block*>(ptr);
        slot->next = freeList;
        freeList = slot;
        allocated--;
    }

    void printStats() {
        cout << "  Slot大小: " << slotSize
             << "B, 每chunk: " << slotsPerChunk
             << " slots" << endl;
        cout << "  Chunks: " << chunks.size()
             << ", 已分配: " << allocated << endl;
    }

    ~MemoryPool() {
        for (char* chunk : chunks) delete[] chunk;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Memory Pool (对象池/内存池)              " << endl;
    cout << "========================================" << endl << endl;

    MemoryPool pool(32, 4);  // 32B 对象, 每 chunk 4 个
    pool.printStats();

    cout << endl << "[分配 10 个对象]" << endl;
    vector<void*> ptrs;
    for (int i = 0; i < 10; i++) ptrs.push_back(pool.allocate());
    pool.printStats();

    cout << endl << "[释放一半]" << endl;
    for (int i = 0; i < 5; i++) pool.deallocate(ptrs[i]);
    pool.printStats();

    cout << endl << "[Pool vs Heap]" << endl;
    cout << "  直接 malloc: ~50ns (系统调用, 锁, 元数据)" << endl;
    cout << "  Memory Pool: ~5ns  (指针移动, O(1))" << endl;
    cout << "  jemalloc/tcmalloc: 线程缓存 + Pool" << endl;

    return 0;
}
