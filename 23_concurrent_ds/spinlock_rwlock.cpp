/**
 * 自旋锁 + 读写锁 (Spinlock & Read-Write Lock)
 *
 * 自旋锁 (Spinlock):
 *   与 mutex 不同, spinlock 在等待期间不挂起线程,
 *   而是忙等待 (busy-waiting, "自旋"), 不断轮询锁状态。
 *
 *   优点: 避免上下文切换开销 (适用于临界区极短的情况)
 *   缺点: 浪费 CPU 周期 (临界区长时更糟)
 *   应用: Linux 内核中断上下文, 短临界区保护
 *
 *   C++ 实现: std::atomic_flag 的 TAS (Test-And-Set)
 *
 * 读写锁 (Read-Write Lock / Shared Mutex):
 *   区分读操作和写操作:
 *     - 多个读者可同时持有锁 (共享)
 *     - 写者独占 (排他)
 *     - 读→写升级需小心死锁
 *
 *   公平性策略:
 *     1. 读优先: 新读者不等待写者 → 写者可能饿死
 *     2. 写优先: 新读者等待已有写请求 → 读者吞吐下降
 *     3. 公平: 按请求顺序服务 (FIFO)
 *
 *   C++ 实现: std::shared_mutex (C++17)
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <vector>
#include <chrono>
using namespace std;

// ==================== 1. 简化自旋锁实现 ====================
class SimpleSpinlock {
    atomic_flag flag = ATOMIC_FLAG_INIT;

public:
    void lock() {
        // TAS (Test-And-Set): 循环直到成功获取锁
        while (flag.test_and_set(memory_order_acquire)) {
            // 自旋等待 (在 x86 上可用 _mm_pause() 降低功耗)
            // 或使用 C++20: flag.wait(true)
        }
    }

    void unlock() {
        flag.clear(memory_order_release);
        // C++20: flag.notify_one()
    }

    bool try_lock() {
        return !flag.test_and_set(memory_order_acquire);
    }
};

// ==================== 2. 读写锁使用 (C++17 shared_mutex) ====================
class ReadWriteCounter {
    mutable shared_mutex mtx;
    int value = 0;

public:
    int read() const {
        shared_lock lock(mtx);  // 共享锁: 多个读者可同时持有
        return value;
    }

    void write(int newVal) {
        unique_lock lock(mtx);  // 排他锁: 写者独占
        value = newVal;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  自旋锁 + 读写锁                        " << endl;
    cout << "========================================" << endl << endl;

    // 1. Spinlock Demo
    SimpleSpinlock spin;
    int counter = 0;

    cout << "[自旋锁] 10 次递增 (模拟)" << endl;
    for (int i = 0; i < 10; i++) {
        spin.lock();
        counter++;  // 极短临界区
        spin.unlock();
    }
    cout << "  counter = " << counter << endl;
    cout << endl;

    // 2. Read-Write Lock Demo
    ReadWriteCounter rwc;
    rwc.write(42);

    cout << "[读写锁]" << endl;
    // 两个读者同时读
    auto reader1 = [&]() {
        int v = rwc.read();
        cout << "  Reader1 读到: " << v << endl;
    };
    auto reader2 = [&]() {
        int v = rwc.read();
        cout << "  Reader2 读到: " << v << " (与 Reader1 可并行!)" << endl;
    };

    reader1();
    reader2();

    cout << endl << "[锁的对比]" << endl;
    cout << "  mutex:     阻塞等待, OS 调度" << endl;
    cout << "  spinlock:  忙等待, CPU 自旋" << endl;
    cout << "  shared_mutex: 读共享, 写排他" << endl;
    cout << "  rwlock:    同 shared_mutex" << endl;
    cout << endl;

    cout << "[选择建议]" << endl;
    cout << "  临界区 < 1μs:       spinlock" << endl;
    cout << "  临界区 > 10μs:      mutex (避免浪费 CPU)" << endl;
    cout << "  读多写少 (10:1+):   shared_mutex" << endl;
    cout << "  一般场景:           mutex (最简单)" << endl;

    return 0;
}
