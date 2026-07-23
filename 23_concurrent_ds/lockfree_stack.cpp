/**
 * 无锁栈 (Lock-Free Stack) — Treiber Stack
 *
 * 使用 CAS (Compare-And-Swap) 实现的无锁并发栈,
 * 由 R. Kent Treiber 于 1986 年提出。
 *
 * 核心原理:
 *   栈的 push/pop 都只修改 head 指针,
 *   用 CAS 循环 (lock-free 模式) 保证原子性:
 *     push: 循环尝试 CAS(&head, oldHead, newNode)
 *     pop:  循环尝试 CAS(&head, oldHead, newHead)
 *
 * C++ 实现 (C++11 atomic):
 *   std::atomic<Node*> head;
 *   compare_exchange_weak / compare_exchange_strong
 *
 * ABA 问题:
 *   - 线程A读到 head=A→B, 准备 pop A
 *   - 线程B pop A, pop B, push A (前)
 *   - 线程A CAS 成功但 head 实际已变 (B 丢失!)
 *   - 解决: 使用带 tag 的指针 (tagged pointer) 或 hazard pointer
 *
 * 性能特性:
 *   - push/pop: O(1) 无锁
 *   - 无系统调用, 适合低延迟场景
 *   - 高竞争下 CAS 可能自旋多次
 */

#include <iostream>
#include <atomic>
#include <memory>
#include <thread>
#include <vector>
using namespace std;

// 简化版无锁栈 (单线程演示原理)
template<typename T>
class LockFreeStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    atomic<Node*> head;

public:
    LockFreeStack() : head(nullptr) {}

    void push(const T& value) {
        Node* newNode = new Node(value);
        // CAS 循环: 不断尝试直到成功
        while (true) {
            Node* oldHead = head.load(memory_order_acquire);
            newNode->next = oldHead;
            // 如果 head 还是 oldHead, 则改为 newNode
            if (head.compare_exchange_weak(oldHead, newNode,
                    memory_order_release, memory_order_acquire))
                return;  // 成功!
            // CAS 失败 (其他线程改了 head) → 重试
        }
    }

    bool pop(T& result) {
        while (true) {
            Node* oldHead = head.load(memory_order_acquire);
            if (oldHead == nullptr) return false;  // 栈空
            Node* newHead = oldHead->next;
            if (head.compare_exchange_weak(oldHead, newHead,
                    memory_order_release, memory_order_acquire)) {
                result = oldHead->data;
                delete oldHead;  // 注意: 其他线程可能还持有引用!
                return true;
            }
        }
    }

    bool isEmpty() const {
        return head.load() == nullptr;
    }

    // Hazard Pointer / Epoch-Based GC 在实际系统中解决 safe reclamation
};

// ==================== 演示 (单线程, 展示 CAS 机制) ====================
int main() {
    cout << "========================================" << endl;
    cout << "  无锁栈 (Lock-Free Stack)                " << endl;
    cout << "========================================" << endl << endl;

    LockFreeStack<int> stack;

    cout << "[Push 操作]" << endl;
    for (int i = 1; i <= 5; i++) {
        stack.push(i);
        cout << "  push(" << i << ")" << endl;
    }
    cout << endl;

    cout << "[Pop 操作]" << endl;
    int val;
    while (stack.pop(val)) {
        cout << "  pop() → " << val << endl;
    }
    cout << endl;

    cout << "[无锁编程核心]" << endl;
    cout << "  CAS(addr, expected, desired):" << endl;
    cout << "    if *addr == expected:" << endl;
    cout << "      *addr = desired; return true" << endl;
    cout << "    else: return false" << endl;
    cout << endl;
    cout << "[ABA 问题]" << endl;
    cout << "  解决: Tagged Pointer (指针+版本号)" << endl;
    cout << "        Hazard Pointer (延迟回收)" << endl;
    cout << "        Epoch-Based GC (Crossbeam/Rust)" << endl;
    cout << endl;
    cout << "[实际项目]" << endl;
    cout << "  - Java ConcurrentLinkedDeque" << endl;
    cout << "  - Rust crossbeam::TreiberStack" << endl;
    cout << "  - C++ boost::lockfree::stack" << endl;

    return 0;
}
