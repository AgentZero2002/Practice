/**
 * 垃圾回收 — 标记-清除 (Mark-Sweep GC)
 *
 * 标记-清除是最基本的 tracing garbage collection 算法, 用于
 * 自动回收不可达对象。它是 JVM、V8 等现代运行时 GC 的基础。
 *
 * 三色抽象 (Tri-Color Abstraction):
 *   白: 未被访问 (潜在 garbage)
 *   灰: 已访问, 但子节点未扫描 (worklist)
 *   黑: 已完成扫描 (survivor)
 *
 * Mark-Sweep 两阶段:
 *
 * Phase 1 — Mark (标记):
 *   从 GC roots (栈/寄存器/全局变量) 出发, DFS/BFS 标记所有可达对象。
 *   三色追踪:
 *     - 初始: 所有对象 = 白
 *     - 从 roots 入队列 (灰)
 *     - 从队列取对象, 将其引用的白对象标灰并入队
 *     - 处理完所有引用 → 标黑
 *
 * Phase 2 — Sweep (清除):
 *   遍历整个堆, 回收所有白色对象 (加入 freelist), 将黑色对象重置为白色。
 *
 * 改进:
 *   - Mark-Compact: 清除后压缩 → 消除碎片
 *   - Mark-Copy (Cheney semispace): 复制存活对象到新空间
 *   - Generational GC: 分代 (年轻代/老年代), 弱分代假说 (most objects die young)
 *   - Incremental GC: 增量标记, 避免 stop-the-world
 *   - Concurrent GC: 与应用线程并发, 写屏障 (write barrier)
 *
 * 本实现展示一个简化的 mark-sweep heap。
 */

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
using namespace std;

// ==================== 简化 Mark-Sweep GC 堆 ====================
enum Color { WHITE, GRAY, BLACK };

struct GCObject {
    Color color = WHITE;
    bool marked = false;  // 简化标记
    size_t size;
    vector<GCObject*> references;  // 引用的其他对象
    char data[0];  // 占位

    void addRef(GCObject* obj) {
        references.push_back(obj);
    }
};

class MiniGCHeap {
    vector<char*> blocks;       // 预分配的内存块
    vector<GCObject*> objects;  // 所有已分配对象
    vector<GCObject*> roots;    // GC roots
    size_t blockSize;

    void newBlock() {
        blocks.push_back(new char[blockSize]);
    }

public:
    MiniGCHeap(size_t bsize = 4096) : blockSize(bsize) {
        newBlock();
    }

    ~MiniGCHeap() {
        for (auto* block : blocks) delete[] block;
        // objects 只是指向 blocks 内的指针, 不需 delete
    }

    // 分配对象 (简化: 每次分配从新地址, 不考虑复用)
    GCObject* allocate(size_t dataSize) {
        size_t total = sizeof(GCObject) + dataSize;
        // 非常简化的 bump allocation
        static char* bumpPtr = blocks[0];
        static char* blockEnd = blocks[0] + blockSize;

        if (bumpPtr + total > blockEnd) {
            newBlock();
            bumpPtr = blocks.back();
            blockEnd = bumpPtr + blockSize;
        }

        auto* obj = reinterpret_cast<GCObject*>(bumpPtr);
        obj->color = WHITE;
        obj->marked = false;
        obj->size = dataSize;
        obj->references.clear();
        // Placement new 构造 vector (简化: 用 POD)
        new (&obj->references) vector<GCObject*>();

        bumpPtr += total;
        objects.push_back(obj);
        return obj;
    }

    void addRoot(GCObject* obj) {
        roots.push_back(obj);
    }

    // ==================== Mark 阶段 ====================
    void mark() {
        // 从 roots 做 DFS
        vector<GCObject*> worklist = roots;

        while (!worklist.empty()) {
            GCObject* obj = worklist.back();
            worklist.pop_back();

            if (obj->marked) continue;
            obj->marked = true;

            // 将其引用加入 worklist
            for (GCObject* ref : obj->references) {
                if (!ref->marked) {
                    worklist.push_back(ref);
                }
            }
        }
    }

    // ==================== Sweep 阶段 ====================
    int sweep() {
        int collected = 0;
        for (auto& obj : objects) {
            if (obj->marked) {
                obj->marked = false;  // 为下一次 GC 重置
            } else {
                // 回收: 调用析构, 放回 freelist (简化: 只计数)
                obj->references.clear();
                collected++;
            }
        }
        return collected;
    }

    // 完整 GC
    int collect() {
        mark();
        return sweep();
    }

    void stats() const {
        cout << "  Objects: " << objects.size()
             << " | Roots: " << roots.size()
             << " | Blocks: " << blocks.size() << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Mark-Sweep 垃圾回收 (GC)                 " << endl;
    cout << "========================================" << endl << endl;

    MiniGCHeap heap;

    // 创建对象的引用图:
    // root → A → B → C
    //         ↘ D (A引用D)
    //    E (无引用 → garbage!)
    //    F (无引用 → garbage!)

    auto* A = heap.allocate(16); strcpy(A->data, "A");
    auto* B = heap.allocate(16); strcpy(B->data, "B");
    auto* C = heap.allocate(16); strcpy(C->data, "C");
    auto* D = heap.allocate(16); strcpy(D->data, "D");
    auto* E = heap.allocate(16); strcpy(E->data, "E");
    auto* F = heap.allocate(16); strcpy(F->data, "F");

    // 建立引用关系
    heap.addRoot(A);
    A->addRef(B);
    A->addRef(D);
    B->addRef(C);
    // E, F 没有从 root 出发的引用 → garbage

    cout << "[初始状态]" << endl;
    heap.stats();

    cout << endl << "[GC 前]" << endl;
    cout << "  可达: A→B→{C, D}" << endl;
    cout << "  垃圾: E, F (无根引用)" << endl;

    int collected = heap.collect();

    cout << endl << "[GC 后]" << endl;
    cout << "  回收了 " << collected << " 个对象 (期望 2: E, F)" << endl;

    cout << endl << "[Mark-Sweep 竞争]" << endl;
    cout << "  Mark-Compact: + 压缩消除碎片" << endl;
    cout << "  Copying GC: 存活对象复制到新空间 (Cheney)" << endl;
    cout << "  Generational: 分代回收 (weak generational hypothesis)" << endl;
    cout << "  Concurrent: 写屏障 + 读屏障 减少 STW" << endl;

    return 0;
}
