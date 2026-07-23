/**
 * 无锁环形缓冲区 (Lock-Free Ring Buffer / Circular Buffer)
 *
 * 生产者-消费者模型的经典实现, 广泛应用于:
 *   - 日志系统 (spdlog 异步日志)
 *   - 网络包处理 (DPDK, Netmap)
 *   - 音频/视频流缓冲
 *   - LMAX Disruptor (高性能事件队列)
 *   - Linux perf 事件缓冲区
 *
 * 核心思想:
 *   1. 固定大小的循环数组
 *   2. 两个指针: head (读位置) 和 tail (写位置)
 *   3. 单生产者单消费者 (SPSC): 最简单的无锁版本
 *      - 只有生产者写 tail, 只有消费者写 head
 *      - 不需要 CAS, 只需 memory barrier
 *
 * SPSC (Single Producer Single Consumer) 实现:
 *   write(data):
 *     1. 检查是否满 (tail + 1 == head → 满)
 *     2. buffer[tail] = data
 *     3. tail = (tail + 1) % capacity
 *
 *   read():
 *     1. 检查是否空 (head == tail → 空)
 *     2. data = buffer[head]
 *     3. head = (head + 1) % capacity
 *
 * 多生产者多消费者 (MPMC):
 *   需要对 head/tail 做 CAS, 或使用多个 slot 的状态标记。
 *
 * 关键优化:
 *   - 缓存行对齐 (避免 false sharing)
 *   - 容量为 2 的幂 (取模 → 位运算)
 *   - batch read/write (减少 cache miss)
 */

#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
using namespace std;

// ==================== SPSC 无锁环形缓冲区 ====================
template<typename T, int Capacity = 8>
class SPSCRingBuffer {
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be power of 2");

    T buffer[Capacity];
    alignas(64) atomic<int> head{0};  // 缓存行对齐 → 避免 false sharing
    alignas(64) atomic<int> tail{0};

public:
    // 生产者: 写入 (单生产者, 无锁)
    bool write(const T& data) {
        int curTail = tail.load(memory_order_relaxed);
        int nextTail = (curTail + 1) & (Capacity - 1);  // mod Capacity

        if (nextTail == head.load(memory_order_acquire))
            return false;  // 缓冲区满

        buffer[curTail] = data;
        tail.store(nextTail, memory_order_release);
        return true;
    }

    // 消费者: 读取 (单消费者, 无锁)
    bool read(T& data) {
        int curHead = head.load(memory_order_relaxed);

        if (curHead == tail.load(memory_order_acquire))
            return false;  // 缓冲区空

        data = buffer[curHead];
        head.store((curHead + 1) & (Capacity - 1), memory_order_release);
        return true;
    }

    int size() const {
        int t = tail.load(memory_order_acquire);
        int h = head.load(memory_order_acquire);
        return (t - h) & (Capacity - 1);
    }

    bool isEmpty() const { return head.load() == tail.load(); }
    bool isFull() const {
        return ((tail.load() + 1) & (Capacity - 1)) == head.load();
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  无锁环形缓冲区 (Ring Buffer)            " << endl;
    cout << "========================================" << endl << endl;

    SPSCRingBuffer<int, 8> ring;

    cout << "[生产者: 写入 7 个元素]" << endl;
    for (int i = 1; i <= 7; i++) {
        bool ok = ring.write(i * 10);
        cout << "  write(" << i * 10 << ") → "
             << (ok ? "成功" : "失败(满)") << endl;
    }

    cout << "  当前大小: " << ring.size() << endl;

    // 再写一个 → 满
    cout << "  write(80) → " << (ring.write(80) ? "成功" : "失败(满)")
         << " (满, 预留1槽用于区分空/满)" << endl;
    cout << endl;

    cout << "[消费者: 读取全部]" << endl;
    int val;
    while (ring.read(val)) {
        cout << "  read() → " << val << endl;
    }
    cout << "  当前大小: " << ring.size() << " (空)" << endl;
    cout << endl;

    cout << "[Ring Buffer 设计要点]" << endl;
    cout << "  capacity = 2^k → mod 变为位运算 &(cap-1)" << endl;
    cout << "  预留 1 槽 → 区分空(head=tail)和满(tail+1=head)" << endl;
    cout << "  SPSC: head 仅消费者写, tail 仅生产者写 → 无 CAS" << endl;
    cout << "  MPMC: head/tail CAS + 每个slot的状态" << endl;
    cout << "  Cache line padding → 防止 false sharing" << endl;
    cout << endl;

    cout << "[实际项目]" << endl;
    cout << "  - LMAX Disruptor: 单机 600 万 TPS" << endl;
    cout << "  - spdlog: C++ 异步日志" << endl;
    cout << "  - DPDK rte_ring: 网络包处理" << endl;
    cout << "  - Linux kfifo: 内核环形缓冲区" << endl;

    return 0;
}
