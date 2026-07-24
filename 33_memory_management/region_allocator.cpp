/**
 * 区域分配器 (Region / Arena Allocator)
 *
 * Region Allocator (也称为 Arena / Bump Allocator)
 * 是最简单的内存分配策略之一。
 *
 * 核心思想:
 *   预分配一块大的连续内存 (region/arena),
 *   用一个 bump pointer 指向下一个可用地址,
 *   allocate 只需: return bumpPtr; bumpPtr += size;
 *   O(1) 分配, 无法单独释放 (只能整体释放整个 region)
 *
 * 优点:
 *   - 极快: 分配仅需一次指针加法 (比 malloc 快 10-100x)
 *   - 极简: 无碎片、无 freelist、无元数据 overhead
 *   - 缓存友好: 连续分配的对象在内存中也相邻
 *
 * 缺点:
 *   - 不能单独 free 单个对象
 *   - 内部碎片: 如果对象大小不统一
 *
 * 适用场景:
 *   - 游戏引擎: 每帧临时数据 (帧末全部释放)
 *   - 编译器: 解析阶段的 AST 节点 (编译完成后全部释放)
 *   - Web服务器: 请求级生命周期 (请求处理完全部释放)
 *   - 科学计算: 矩阵操作的临时缓冲区
 *
 * 变体:
 *   - Stack Allocator: 支持 LIFO 释放 (Mark & Rewind)
 *   - Pool Allocator: 同大小对象, 支持随机释放
 *   - Monotonic + Pool 混合: 大对象 bump, 小对象 pool
 *
 * 模式: Arena 分配器通常配合 RAII:
 *   Arena arena;
 *   {
 *     auto guard = arena.scope();  // 记住当前位置
 *     // 分配临时对象...
 *   }  // guard 析构 → arena.rewind() → 释放全部临时对象
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
using namespace std;

// ==================== Region Allocator 实现 ====================
class RegionAllocator {
    vector<char*> blocks;    // 预分配的大块内存
    char* bumpPtr;           // 当前分配位置
    char* blockEnd;          // 当前块结束地址
    size_t blockSize;
    size_t totalAllocated;

    void newBlock() {
        char* block = new char[blockSize];
        blocks.push_back(block);
        bumpPtr = block;
        blockEnd = block + blockSize;
    }

public:
    RegionAllocator(size_t initialBlockSize = 4096)
        : bumpPtr(nullptr), blockEnd(nullptr),
          blockSize(initialBlockSize), totalAllocated(0) {
        newBlock();
    }

    ~RegionAllocator() {
        for (char* block : blocks)
            delete[] block;
    }

    void* allocate(size_t size, size_t alignment = 8) {
        // 对齐 bump pointer
        uintptr_t addr = reinterpret_cast<uintptr_t>(bumpPtr);
        uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
        char* ptr = reinterpret_cast<char*>(aligned);

        // 当前块空间不足 → 分配新块
        if (ptr + size > blockEnd) {
            // 如果请求 > 块大小, 分配更大的块
            if (size > blockSize / 2) {
                char* bigBlock = new char[size];
                blocks.push_back(bigBlock);
                totalAllocated += size;
                return bigBlock;  // 大对象单独块, 不更新 bumpPtr
            }
            newBlock();
            ptr = bumpPtr;
        }

        bumpPtr = ptr + size;
        totalAllocated += size;
        return ptr;
    }

    // Mark & Rewind: LIFO 释放
    // 保存当前位置, 稍后恢复
    struct Mark {
        char* bumpPtr;
        size_t totalAllocated;
        size_t blockCount;
    };

    Mark mark() {
        return {bumpPtr, totalAllocated, blocks.size()};
    }

    void rewind(const Mark& m) {
        // 释放之后分配的块
        while (blocks.size() > m.blockCount) {
            delete[] blocks.back();
            blocks.pop_back();
        }
        bumpPtr = m.bumpPtr;
        totalAllocated = m.totalAllocated;
        // 恢复最后一块的结束位置
        if (!blocks.empty())
            blockEnd = blocks.back() + blockSize;
    }

    void reset() {
        for (size_t i = 1; i < blocks.size(); i++)
            delete[] blocks[i];
        blocks.resize(1);
        bumpPtr = blocks[0];
        blockEnd = bumpPtr + blockSize;
        totalAllocated = 0;
    }

    size_t allocated() const { return totalAllocated; }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  区域分配器 (Region/Arena Allocator)       " << endl;
    cout << "========================================" << endl << endl;

    RegionAllocator arena;

    // 模拟编译器: 分配多个 AST 节点
    struct ASTNode {
        int type;
        ASTNode* left;
        ASTNode* right;
        char name[16];
    };

    cout << "[分配 AST 节点]" << endl;
    auto mark1 = arena.mark();

    auto* root = static_cast<ASTNode*>(arena.allocate(sizeof(ASTNode)));
    auto* leftChild = static_cast<ASTNode*>(arena.allocate(sizeof(ASTNode)));
    auto* rightChild = static_cast<ASTNode*>(arena.allocate(sizeof(ASTNode)));

    root->type = 1; root->left = leftChild; root->right = rightChild;
    strcpy(root->name, "plus");
    leftChild->type = 2; strcpy(leftChild->name, "x");
    rightChild->type = 2; strcpy(rightChild->name, "y");

    cout << "  root 在 " << root << " (type=" << root->type << ")" << endl;
    cout << "  left 在 " << leftChild << endl;
    cout << "  right 在 " << rightChild << endl;
    cout << "  总分配: " << arena.allocated() << " 字节" << endl;

    // Rewind: 释放临时对象
    cout << endl << "[Rewind] 释放临时 AST..." << endl;
    arena.rewind(mark1);

    // 分配其他东西
    auto* buf = static_cast<char*>(arena.allocate(100));
    cout << "  新分配 100B 在 " << (void*)buf << endl;
    cout << "  总分配: " << arena.allocated() << " 字节 (复用之前的内存)" << endl;

    cout << endl << "[Region Allocator 特性]" << endl;
    cout << "  allocate: O(1) (bump pointer)" << endl;
    cout << "  deallocate: 不支持单独释放 (仅 rewind/reset)" << endl;
    cout << "  适用: 请求/帧/编译级生命周期" << endl;
    cout << "  参考: glibc obstack, Apache APR pools" << endl;

    return 0;
}
