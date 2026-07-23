/**
 * Write-Ahead Log (WAL) — 数据库崩溃恢复核心
 *
 * WAL 是数据库系统保证持久性 (Durability) 的核心机制。
 *
 * 核心原则:
 *   在将数据写入数据文件之前, 先将操作记录到 WAL (日志文件)。
 *   如果系统崩溃, 可通过重放 (replay) WAL 恢复数据。
 *
 * 为什么需要 WAL?
 *   - 内存中的数据在崩溃时会丢失
 *   - 直接写磁盘数据文件可能在写入中途崩溃 (半页写入)
 *   - WAL 是顺序追加写 → 极快
 *   - 崩溃恢复时按顺序重放已提交的事务
 *
 * WAL 记录格式:
 *   ┌────────────┬──────────┬────────┬──────────────────┐
 *   │ LSN (LogSeq)│ TxnID   │ OpCode │ Key, Value, ...  │
 *   └────────────┴──────────┴────────┴──────────────────┘
 *
 * Checkpoint (检查点):
 *   - 定期将内存数据刷入磁盘, 标记 checkpoint LSN
 *   - 恢复时只需从 checkpoint 之后的日志重放
 *   - 减少恢复时间
 *
 * 应用: PostgreSQL, SQLite, MySQL InnoDB, LevelDB 等几乎所有数据库
 */

#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

// ==================== WAL 实现 ====================

enum OpCode { INSERT_OP, UPDATE_OP, DELETE_OP };

struct WALRecord {
    int lsn;        // Log Sequence Number
    int txnId;      // 事务 ID
    OpCode op;      // 操作类型
    int key;
    string value;

    string toString() const {
        string opStr = (op == INSERT_OP) ? "INSERT" :
                       (op == UPDATE_OP) ? "UPDATE" : "DELETE";
        return "LSN=" + to_string(lsn) + " Txn=" + to_string(txnId)
             + " " + opStr + " key=" + to_string(key) + " val=" + value;
    }
};

class WAL {
private:
    deque<WALRecord> log;       // 内存中的日志缓冲 (实际系统为磁盘文件)
    map<int, string> data;      // 模拟数据存储 (通常为 B+Tree)
    int nextLSN;
    int checkpointLSN;

public:
    WAL() : nextLSN(1), checkpointLSN(0) {}

    // ====== 写操作 ======

    void insert(int txnId, int key, string value) {
        // 1. 先写日志
        WALRecord rec = {nextLSN++, txnId, INSERT_OP, key, value};
        log.push_back(rec);
        cout << "  [WAL] " << rec.toString() << endl;

        // 2. 再写数据 (可延迟到 checkpoint)
        data[key] = value;
    }

    void remove(int txnId, int key) {
        WALRecord rec = {nextLSN++, txnId, DELETE_OP, key, ""};
        log.push_back(rec);
        cout << "  [WAL] " << rec.toString() << endl;
        data.erase(key);
    }

    void update(int txnId, int key, string value) {
        WALRecord rec = {nextLSN++, txnId, UPDATE_OP, key, value};
        log.push_back(rec);
        cout << "  [WAL] " << rec.toString() << endl;
        data[key] = value;
    }

    // ====== 读操作 ======

    string get(int key) const {
        auto it = data.find(key);
        return (it != data.end()) ? it->second : "未找到";
    }

    // ====== Checkpoint ======

    void checkpoint() {
        // 在实际系统中, 此操作将内存数据批量写入磁盘
        // 然后标记 checkpoint 位置
        checkpointLSN = nextLSN - 1;
        cout << endl << "[Checkpoint] LSN=" << checkpointLSN
             << ", 数据已安全落盘, 可截断之前的日志" << endl;

        // 截断 checkpoint 之前的日志 (实际系统是截断 WAL 文件)
        while (!log.empty() && log.front().lsn <= checkpointLSN)
            log.pop_front();
    }

    // ====== 崩溃恢复 ======

    void crashAndRecover() {
        cout << endl << "!!! 系统崩溃 !!!" << endl;
        cout << "  内存数据丢失, 但 WAL 文件安全 (顺序写入磁盘)" << endl;

        // 模拟: 清空内存数据, 从 WAL 重放
        data.clear();
        int recoverFrom = checkpointLSN + 1;

        cout << "[Recovery] 从 checkpoint LSN=" << checkpointLSN
             << " 之后重放:" << endl;

        for (auto& rec : log) {
            if (rec.lsn > checkpointLSN) {
                cout << "  Replay: " << rec.toString() << endl;
                if (rec.op == INSERT_OP || rec.op == UPDATE_OP)
                    data[rec.key] = rec.value;
                else if (rec.op == DELETE_OP)
                    data.erase(rec.key);
            }
        }

        cout << "[Recovery] 完成! 数据已恢复" << endl;
    }

    // ====== 打印 ======

    void printData() const {
        cout << endl << "[当前数据] (key → value)" << endl;
        for (auto& [k, v] : data)
            cout << "  " << k << " → " << v << endl;
    }

    void printWAL() const {
        cout << endl << "[WAL 内容] (" << log.size() << " 条尚未截断)" << endl;
        for (auto& rec : log)
            cout << "  " << rec.toString() << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Write-Ahead Log (WAL)                  " << endl;
    cout << "========================================" << endl << endl;

    WAL db;

    cout << "=== 事务1: 插入数据 ===" << endl;
    db.insert(1, 100, "Alice");
    db.insert(1, 200, "Bob");
    db.update(1, 100, "AliceUpdated");
    db.printData();

    cout << endl << "=== Checkpoint ===" << endl;
    db.checkpoint();

    cout << endl << "=== 事务2: 更多操作 ===" << endl;
    db.insert(2, 300, "Charlie");
    db.remove(2, 200);
    db.printData();
    db.printWAL();

    // 崩溃恢复!
    db.crashAndRecover();
    db.printData();

    cout << endl << "[WAL 核心原则]" << endl;
    cout << "  1. 日志先写 (Write Ahead)" << endl;
    cout << "  2. 数据后写 (可延迟)" << endl;
    cout << "  3. 崩溃恢复 = 重放日志" << endl;
    cout << "  4. Checkpoint = 截断日志 + 减少恢复时间" << endl;

    return 0;
}
