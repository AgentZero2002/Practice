/**
 * 无锁队列 (Lock-Free Queue) — Michael-Scott Queue
 *
 * Maged M. Michael 和 Michael L. Scott 于 1996 年提出,
 * 是经典的无锁 FIFO 队列算法, 被 Java ConcurrentLinkedQueue 采用。
 *
 * 核心结构:
 *   维护 head 和 tail 两个指针:
 *     head → 哨兵节点或首个有效节点
 *     tail → 最后一个节点
 *
 *   enqueue (入队):
 *     1. 创建新节点
 *     2. CAS(&tail->next, nullptr, newNode)   ← 链接
 *     3. CAS(&tail, oldTail, newNode)          ← 更新 tail (可帮助)
 *
 *   dequeue (出队):
 *     1. 读 head->next 获取第一个有效节点
 *     2. CAS(&head, oldHead, firstNode)        ← 移动 head
 *     3. 读取 firstNode->value, 释放 oldHead
 *
 * 关键设计 — tail 不总是精确的:
 *   - enqueue 时先链入新节点 (步骤2), 再更新 tail (步骤3)
 *   - 如果步骤3失败 (被其他线程抢先更新), 也没关系
 *   - 后续操作会"帮助"更新 tail
 *   - 这是无锁算法中常见的 "helping" 策略
 *
 * 与有锁队列对比:
 *   有锁: 简单正确, 但临界区互斥 → 并发度低
 *   无锁: 没有临界区, 多个线程可同时 CAS 竞争
 *
 * 性能: enqueue/dequeue 均为 O(1), 高并发下比 mutex 版本快数倍
 */

#include <iostream>
#include <atomic>
#include <memory>
using namespace std;

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T data;
        atomic<Node*> next;
        Node() : next(nullptr) {}
        Node(const T& d) : data(d), next(nullptr) {}
    };

    atomic<Node*> head;
    atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();  // 哨兵节点
        head.store(dummy);
        tail.store(dummy);
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        while (true) {
            Node* curTail = tail.load(memory_order_acquire);
            Node* next = curTail->next.load(memory_order_acquire);

            if (curTail == tail.load(memory_order_acquire)) {
                if (next == nullptr) {
                    // 步骤 2: 链接新节点
                    if (curTail->next.compare_exchange_weak(next, newNode,
                            memory_order_release, memory_order_acquire)) {
                        // 步骤 3: 更新 tail (失败也无妨, 他人会帮助)
                        tail.compare_exchange_strong(curTail, newNode,
                            memory_order_release, memory_order_acquire);
                        return;
                    }
                } else {
                    // 帮助更新 tail
                    tail.compare_exchange_strong(curTail, next,
                        memory_order_release, memory_order_acquire);
                }
            }
        }
    }

    bool dequeue(T& result) {
        while (true) {
            Node* curHead = head.load(memory_order_acquire);
            Node* curTail = tail.load(memory_order_acquire);
            Node* first = curHead->next.load(memory_order_acquire);

            if (curHead == head.load(memory_order_acquire)) {
                if (curHead == curTail) {
                    // 队列可能为空
                    if (first == nullptr) return false;
                    // tail 落后了, 帮助更新
                    tail.compare_exchange_strong(curTail, first,
                        memory_order_release, memory_order_acquire);
                } else {
                    result = first->data;
                    if (head.compare_exchange_weak(curHead, first,
                            memory_order_release, memory_order_acquire)) {
                        delete curHead;  // 安全回收 (实际需 hazard pointer)
                        return true;
                    }
                }
            }
        }
    }
};

// ==================== 演示 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  无锁队列 (Michael-Scott Queue)          " << endl;
    cout << "========================================" << endl << endl;

    LockFreeQueue<int> queue;

    cout << "[Enqueue]" << endl;
    for (int i = 1; i <= 5; i++) {
        queue.enqueue(i * 10);
        cout << "  enqueue(" << i * 10 << ")" << endl;
    }

    cout << endl << "[Dequeue]" << endl;
    int val;
    while (queue.dequeue(val)) {
        cout << "  dequeue() → " << val << endl;
    }
    cout << endl;

    cout << "[Michael-Scott 关键设计]" << endl;
    cout << "  1. 哨兵节点 dummy → 简化空队列处理" << endl;
    cout << "  2. tail 不一定精确 → 允许 help 机制" << endl;
    cout << "  3. two-phase enqueue: link + update tail" << endl;
    cout << "  4. CAS 失败 → 重试 (无锁的代价)" << endl;
    cout << endl;
    cout << "[实际应用]" << endl;
    cout << "  - Java: java.util.concurrent.ConcurrentLinkedQueue" << endl;
    cout << "  - C++: boost::lockfree::queue" << endl;
    cout << "  - Rust: crossbeam::queue::SegQueue" << endl;

    return 0;
}
