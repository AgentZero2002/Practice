/**
 * Read-Copy-Update (RCU) — Linux 内核核心技术
 *
 * RCU 是 Linux 内核中最优雅的并发机制之一, 专为
 * "读极多写极少" 的场景设计。
 *
 * 核心思想:
 *   更新的不是原数据, 而是拷贝一份再修改, 然后原子切换指针。
 *   旧数据等待所有读者完成后才释放。
 *
 * 生命周期:
 *   1. 写者: copy → update → atomic pointer swap
 *   2. 读者: 直接读 (无锁, 无原子操作 — 极快)
 *   3. 等待宽限期 (Grace Period): 所有读者离开临界区后,
 *      旧数据可以被安全释放
 *
 * Grace Period (宽限期):
 *   - 读者在 rcu_read_lock() / rcu_read_unlock() 之间
 *   - 宽限期 = 所有 CPU 都至少发生过一次上下文切换
 *     (或者在抢占式内核中, 所有读者都退出了临界区)
 *   - synchronize_rcu() 等待宽限期结束
 *   - call_rcu() 异步: 注册回调, 宽限期后自动调用
 *
 * RCU 适用场景:
 *   - 路由表更新 (读频繁, 写极少)
 *   - 文件系统目录缓存
 *   - 内核模块引用计数
 *   - 用户态: liburcu (Userspace RCU)
 *
 * 代价:
 *   - 内存开销: 旧版本在宽限期前不能释放
 *   - 写延迟: writer 需要等待宽限期
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
using namespace std;

// ==================== 简化版 RCU (单线程演示原理) ====================

template<typename T>
class RCUProtected {
private:
    atomic<shared_ptr<T>> dataPtr;  // 原子指针 (指向当前版本)

public:
    RCUProtected() : dataPtr(make_shared<T>()) {}

    // 读操作 (RCU read-side): 只需获取原子快照
    // 在真实 RCU 中, 读取是无锁的 (仅 memory barrier)
    shared_ptr<T> read() {
        // rcu_read_lock()  ← 实际是禁止抢占/内存屏障
        auto snapshot = dataPtr.load();  // 原子读取当前版本
        // 读者持有 snapshot, 即使写者更新了指针,
        // snapshot 仍然有效 (shared_ptr 引用计数保护)
        return snapshot;
        // rcu_read_unlock()
    }

    // 写操作 (RCU update-side): Copy → Modify → Atomic Swap
    void update(function<void(T&)> modifier) {
        // 1. Copy: 拷贝当前数据
        auto oldData = dataPtr.load();
        auto newData = make_shared<T>(*oldData);

        // 2. Modify: 修改副本
        modifier(*newData);

        // 3. Atomic Swap: 原子切换指针
        dataPtr.store(newData);

        // 4. Grace Period: 等待所有读者释放 oldData
        //    (在真实 RCU 中, 这里是 synchronize_rcu())
        //    这里由 shared_ptr 自动管理: 当所有读者离开后自动释放
        cout << "  [RCU Update] 旧版本将自动回收" << endl;
    }
};

// ==================== 简单示例结构 ====================
struct RouteTable {
    string defaultGateway = "192.168.1.1";
    int numRoutes = 3;

    void print() const {
        cout << "    Gateway: " << defaultGateway
             << ", Routes: " << numRoutes << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Read-Copy-Update (RCU)                 " << endl;
    cout << "========================================" << endl << endl;

    RCUProtected<RouteTable> rcuTable;

    cout << "[初始状态]" << endl;
    {
        auto data = rcuTable.read();
        data->print();
    }

    cout << endl << "[写者: 更新路由表]" << endl;
    rcuTable.update([](RouteTable& t) {
        t.defaultGateway = "10.0.0.1";
        t.numRoutes = 5;
    });

    cout << endl << "[读者: 读到新版本]" << endl;
    {
        auto data = rcuTable.read();
        cout << "  (更新后) ";
        data->print();
    }

    cout << endl << "[RCU 核心优势]" << endl;
    cout << "  读者: 无锁, 无原子操作 (仅有 memory barrier)" << endl;
    cout << "  写者: 仅指针切换是原子的" << endl;
    cout << "  适合: 读多写少的场景 (路由表, 配置, 缓存)" << endl;
    cout << endl;

    cout << "[Grace Period]" << endl;
    cout << "  synchronize_rcu(): 阻塞等待所有读者完成" << endl;
    cout << "  call_rcu(callback): 异步回收" << endl;
    cout << "  机制: 每个 CPU 经历一次 quiescent state" << endl;
    cout << endl;

    cout << "[用户态 RCU]" << endl;
    cout << "  liburcu (Userspace RCU)" << endl;
    cout << "  QSBR (Quiescent State Based Reclamation)" << endl;
    cout << "  Epoch-Based Reclamation (Crossbeam/Rust)" << endl;

    return 0;
}
