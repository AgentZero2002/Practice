/**
 * SLAB 分配器 (Slab Allocator)
 *
 * SLAB 分配器是 Linux 内核使用的内存分配器 (SLAB/SLUB/SLOB)。
 * 它是 Buddy Allocator 的上层, 管理相同大小的对象。
 *
 * 核心思想: 内存 → 缓存 (Cache) → Slab → Object
 *   每个 "缓存" (kmem_cache) 管理固定大小的对象
 *   Slab 是一块连续内存 (通常 1 页), 被切分等大的 object
 *
 * 三层架构:
 *   Cache: 一个 kmem_cache 对应一种对象大小
 *     ├── slabs_full:    所有对象都在使用中
 *     ├── slabs_partial: 部分对象在使用
 *     └── slabs_free:    所有对象空闲 (可回收)
 *
 *   Slab: 一个或多个连续物理页
 *     ├── free list: 指向第一个空闲对象
 *     └── objects:   每个对象的头部有 freelist 指针 (inline)
 *
 * 为什么需要 SLAB?
 *   - Buddy Allocator 最小分配单位是 1页(4KB), 对 ≤512B 的对象浪费大
 *   - SLAB 在已分配的页内做细粒度分配
 *   - 缓存预分配 + 对象复用 → 减少反复 buddy alloc/free
 *
 * SLUB (Unqueued Allocator):
 *   更现代的替代: 简化了 SLAB 的复杂队列管理
 *   每个 CPU 有自己的 slab 缓存, 减少锁竞争
 *
 * 对象着色 (Coloring):
 *   不同 slab 中, 对象起始位置错开 (按 cache line 偏移)
 *   减少 CPU 缓存冲突 (同一偏移的对象不会映射到同一 cache line)
 */

#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
using namespace std;

// ==================== 简化版 SLAB 分配器 ====================
class SlabAllocator {
public:
    struct Object {
        Object* next;  // freelist 指针 (inline, 对象空闲时复用)
        char data[0];  // 可变长度占位
    };

    struct Slab {
        vector<char> memory;  // slab 内存
        Object* freelist;     // 空闲对象链表
        int freeCount;
        int totalObjects;

        Slab(int objSize, int objectsPerSlab)
            : memory(objSize * objectsPerSlab, 0),
              freelist(nullptr),
              freeCount(objectsPerSlab),
              totalObjects(objectsPerSlab) {
            // 初始化 freelist: 所有对象链接在一起
            for (int i = 0; i < objectsPerSlab; i++) {
                auto* obj = reinterpret_cast<Object*>(memory.data() + i * objSize);
                obj->next = freelist;
                freelist = obj;
            }
        }

        bool isFull() const { return freeCount == 0; }
        bool isEmpty() const { return freeCount == totalObjects; }
    };

private:
    int objSize;
    int objectsPerSlab;
    list<Slab> slabsFull, slabsPartial, slabsFree;

    // 从 buddy allocator 获取页面 (简化: new)
    Slab* newSlab() {
        slabsFree.push_back(Slab(objSize, objectsPerSlab));
        auto* slab = &slabsFree.back();
        // 移到 partial
        slabsPartial.splice(slabsPartial.end(), slabsFree, --slabsFree.end());
        return slab;
    }

public:
    SlabAllocator(int objectSize, int objsPerSlab = 8)
        : objSize(objectSize), objectsPerSlab(objsPerSlab) {
        // 预分配一个 slab
        newSlab();
    }

    void* allocate() {
        // 从 partial slabs 中找有空闲对象的
        if (slabsPartial.empty()) {
            newSlab();
        }

        auto& slab = slabsPartial.front();
        Object* obj = slab.freelist;
        slab.freelist = obj->next;
        slab.freeCount--;

        if (slab.isFull()) {
            slabsFull.splice(slabsFull.end(), slabsPartial,
                             slabsPartial.begin());
        }

        return obj;
    }

    void deallocate(void* ptr) {
        auto* obj = static_cast<Object*>(ptr);

        // 查找 ptr 属于哪个 slab (简化: 线性搜索)
        // 实际 Linux 用 page→slab 反向映射
        for (auto it = slabsFull.begin(); it != slabsFull.end(); ++it) {
            char* start = it->memory.data();
            char* end = start + it->memory.size();
            if ((char*)ptr >= start && (char*)ptr < end) {
                // 归还到 freelist
                obj->next = it->freelist;
                it->freelist = obj;
                it->freeCount++;

                if (!it->isEmpty()) {
                    slabsPartial.splice(slabsPartial.end(), slabsFull, it);
                }
                return;
            }
        }
        // 同样搜索 slabsPartial
        for (auto it = slabsPartial.begin(); it != slabsPartial.end(); ++it) {
            char* start = it->memory.data();
            char* end = start + it->memory.size();
            if ((char*)ptr >= start && (char*)ptr < end) {
                obj->next = it->freelist;
                it->freelist = obj;
                it->freeCount++;
                if (it->isEmpty()) {
                    slabsFree.splice(slabsFree.end(), slabsPartial, it);
                }
                return;
            }
        }
    }

    void printStats() const {
        cout << "  SlabAllocator [objSize=" << objSize
             << ", objs/slab=" << objectsPerSlab << "]" << endl;
        cout << "    Full: " << slabsFull.size()
             << "  Partial: " << slabsPartial.size()
             << "  Free: " << slabsFree.size() << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  SLAB 分配器 (Linux 内核风格)              " << endl;
    cout << "========================================" << endl << endl;

    // 模拟分配和释放 fixed-size 对象
    SlabAllocator alloc(64, 4);  // 64字节对象, 每slab 4个

    cout << "[初始状态]" << endl;
    alloc.printStats();

    // 分配 6 个对象 (需要2个slab)
    vector<void*> ptrs;
    for (int i = 0; i < 6; i++) {
        ptrs.push_back(alloc.allocate());
        cout << "  分配 #" << i + 1 << ": " << ptrs[i] << endl;
    }

    cout << endl << "[分配6个对象后]" << endl;
    alloc.printStats();

    // 释放一半
    for (int i = 0; i < 3; i++) {
        alloc.deallocate(ptrs[i]);
        cout << "  释放 #" << i + 1 << endl;
    }

    cout << endl << "[释放3个后]" << endl;
    alloc.printStats();

    cout << endl << "[SLAB 架构]" << endl;
    cout << "  Buddy Allocator → 页分配 (4KB+)" << endl;
    cout << "  SLAB Allocator → 对象分配 (8B-8KB)" << endl;
    cout << "  对象着色 → 减少 CPU cache 冲突" << endl;
    cout << "  Per-CPU slab → 无锁分配 (快速路径)" << endl;

    return 0;
}
