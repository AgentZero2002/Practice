/**
 * 缓冲树 (Buffer Tree)
 *
 * Buffer Tree 是外部存储器 B-tree 的高效变体，通过延迟写优化 I/O。
 * 它是 LSM Tree 的前身之一，由 Arge 在 1995 年提出。
 *
 * 问题: 传统 B-tree 每个插入 O(log_B N) I/O，
 * 如果一次有 M 个插入 (M 很大)，总 I/O = O(M · log_B N)，
 * 远差于批量排序 + 建树。
 *
 * Buffer Tree 核心思想: 批处理 (Batching)
 *   每个内部节点附带一个"缓冲区" (buffer), 大小 Θ(M)。
 *   插入不立即下推到叶子，而是暂存在根节点的缓冲区。
 *   当缓冲区满了 → 一次性下推到子节点 (batch push)。
 *   下推时: 排序缓冲区中的操作, 合并, 分配到各子节点。
 *
 * 操作:
 *   INSERT(K, V): 加入根节点的缓冲区
 *   DELETE(K): 同上, 记录删除标记
 *   SEARCH(K): 从根到叶, 扫描路径上每个节点的缓冲区
 *     (缓冲区中的未下推操作可能包含该K)
 *
 * I/O 分析:
 *   批量 M 个插入: O(M/B · log_{M/B} N) I/O
 *     (每次下推花费 ~M/B 次 I/O 读写各子节点的缓冲区)
 *   相比传统 B-tree 的 O(M · log_B N): 当 M=Ω(B log_B N)时快 B 倍
 *
 * 与 LSM Tree 的关系:
 *   Buffer Tree: 树内缓冲 (internal node buffers)
 *   LSM Tree: 树外缓冲 (external log + leveled compaction)
 *   两者核心目标一致: 用批处理摊销写 I/O
 *
 * 应用:
 *   - 大规模图算法 (外存 BFS/SSSP)
 *   - 外部 kd-tree / R-tree
 *   - 数据库批量导入 (bulk load)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>
using namespace std;

// ==================== 简化版 Buffer Tree ====================
// 演示概念: 每个节点带有插入缓冲区, 批量下推

const int B = 4;  // 分支因子 (扇出)
const int BUFFER_SIZE = 8;  // 节点缓冲区大小

struct BufferNode {
    vector<int> keys;             // 路由键 (B-1个)
    vector<int> children;         // 子节点索引
    vector<pair<int,int>> buffer; // 缓冲区 (key, value) 暂存
    bool isLeaf;

    BufferNode(bool leaf = false) : isLeaf(leaf) {}
};

class BufferTree {
    vector<BufferNode> nodes;

    // 当节点缓冲区满时, 将缓冲区的元素下推到子节点
    void flushBuffer(int nodeIdx) {
        auto& node = nodes[nodeIdx];
        if ((int)node.buffer.size() < BUFFER_SIZE && nodeIdx != 0) return;
        // 根节点的缓冲区可以更大 (事实上它收集所有未下推内容)
        if (nodeIdx == 0 && (int)node.buffer.size() < BUFFER_SIZE) return;

        auto& buf = node.buffer;

        if (node.isLeaf) {
            // 叶节点: 合并到 keys (视为最终存储)
            for (auto& [k, v] : buf)
                node.keys.push_back(k);
            sort(node.keys.begin(), node.keys.end());
            node.keys.erase(unique(node.keys.begin(), node.keys.end()),
                            node.keys.end());
        } else {
            // 内部节点: 分发到子节点
            // 对缓冲区按 key 排序
            sort(buf.begin(), buf.end());

            // 分配给各子节点 (基于路由键范围)
            int childPtr = 0;
            for (auto& [k, v] : buf) {
                // 找到正确的子节点
                while (childPtr < (int)node.keys.size() &&
                       k > node.keys[childPtr]) {
                    childPtr++;
                }
                int childIdx = node.children[childPtr];
                nodes[childIdx].buffer.push_back({k, v});
            }
        }
        buf.clear();
    }

    // 递归下推 (从根到叶)
    void pushDown(int nodeIdx) {
        flushBuffer(nodeIdx);
        auto& node = nodes[nodeIdx];
        if (!node.isLeaf) {
            for (int child : node.children) {
                pushDown(child);
            }
        }
    }

public:
    BufferTree() {
        // 创建根节点
        nodes.push_back(BufferNode());
    }

    void insert(int key, int value) {
        nodes[0].buffer.push_back({key, value});
        // 根缓冲区溢出 → 可能触发分裂 (简化)
        if ((int)nodes[0].buffer.size() >= BUFFER_SIZE * 2) {
            // 简化:强制下推全部
            pushDown(0);
        }
    }

    bool search(int key) {
        // 需要搜索根→叶路径上所有缓冲区 (简化: 扫描全局)
        for (auto& node : nodes) {
            for (auto& [k, v] : node.buffer) {
                if (k == key) return true;
            }
            if (binary_search(node.keys.begin(), node.keys.end(), key))
                return true;
        }
        return false;
    }

    void flush() { pushDown(0); }

    void printStats() const {
        cout << "  Nodes: " << nodes.size() << endl;
        int totalBuf = 0, totalKeys = 0;
        for (auto& n : nodes) {
            totalBuf += n.buffer.size();
            totalKeys += n.keys.size();
        }
        cout << "  Buffered inserts: " << totalBuf << endl;
        cout << "  Stored keys: " << totalKeys << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  缓冲树 (Buffer Tree)                     " << endl;
    cout << "========================================" << endl << endl;

    BufferTree bt;

    cout << "[批量插入]" << endl;
    for (int i = 1; i <= 20; i++) {
        bt.insert(i * 5, i);
        cout << "  插入 " << i * 5;
        if (i % 5 == 0) cout << " (5的倍数, 批量触发)";
        cout << endl;
    }

    cout << endl << "[下推前]" << endl;
    bt.printStats();

    bt.flush();

    cout << endl << "[全量下推后]" << endl;
    bt.printStats();

    cout << endl << "[查找]" << endl;
    for (int k : {25, 33, 75}) {
        cout << "  Search(" << k << ") = "
             << (bt.search(k) ? "找到" : "未找到") << endl;
    }

    cout << endl << "[Buffer Tree I/O 分析]" << endl;
    cout << "  批量 M 插入: O(M/B · log_{M/B} N) I/O" << endl;
    cout << "  对比 B-Tree: O(M · log_B N) I/O" << endl;
    cout << "  当 M=Ω(B log_B N) 时, 快 B 倍" << endl;

    return 0;
}
