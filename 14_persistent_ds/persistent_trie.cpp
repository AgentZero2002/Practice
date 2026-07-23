/**
 * 可持久化 Trie (Persistent Trie / Binary Trie)
 *
 * 可持久化 Trie 是可持久化数据结构的另一个经典应用。
 * 与可持久化线段树类似, 每次修改(插入)只新建 O(log U) 个节点,
 * 旧版本保持不变, 所有历史版本均可访问。
 *
 * 核心思想:
 *   插入新字符串时, 沿路径复制节点:
 *     new_node = copy(old_node)
 *     修改 new_node 的对应子指针
 *     其余指针共享旧版本
 *
 * 应用:
 *   1. 区间异或最大值: 给定数组 A[1..n], 查询 max A[i] xor x, i ∈ [l,r]
 *      用可持久化 Trie 维护前缀版本, 版本 r - 版本 l-1 = 区间内的值
 *   2. 历史版本查询: 可回退到任意历史版本的 Trie
 *
 * 复杂度: 每次插入 O(log U), U 为值域大小 (通常 31 位)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int MAX_BITS = 31;

struct TrieNode {
    int child[2];  // 0 和 1
    int count;     // 以该节点为根的子树中的数字个数

    TrieNode() : child{-1, -1}, count(0) {}
};

class PersistentTrie {
private:
    vector<TrieNode> nodes;
    vector<int> roots;  // 每个版本的根节点编号

    int newNode() {
        nodes.push_back(TrieNode());
        return nodes.size() - 1;
    }

    int copyNode(int old) {
        nodes.push_back(nodes[old]);
        return nodes.size() - 1;
    }

public:
    PersistentTrie() {
        newNode();  // 节点 0 作为空节点
        roots.push_back(0);  // 版本 0: 空 Trie
    }

    // 插入一个值, 返回新版本号
    int insert(int prevVersion, int value) {
        int newRoot = copyNode(roots[prevVersion]);
        int cur = newRoot, old = roots[prevVersion];
        nodes[cur].count++;

        for (int bit = MAX_BITS - 1; bit >= 0; bit--) {
            int b = (value >> bit) & 1;
            int oldChild = nodes[old].child[b];

            int newChild = (oldChild == -1) ? newNode() : copyNode(oldChild);
            nodes[cur].child[b] = newChild;
            nodes[newChild].count++;

            cur = newChild;
            old = (oldChild == -1) ? 0 : oldChild;
        }

        roots.push_back(newRoot);
        return roots.size() - 1;
    }

    // 在版本区间 [lVer, rVer] 中查询与 x 异或最大的值
    int maxXor(int lVer, int rVer, int x) {
        int lNode = roots[lVer], rNode = roots[rVer];
        int result = 0;

        for (int bit = MAX_BITS - 1; bit >= 0; bit--) {
            int b = (x >> bit) & 1;
            int want = b ^ 1;  // 想要相反的位

            int lChild = nodes[lNode].child[want];
            int rChild = nodes[rNode].child[want];

            int cntL = (lChild == -1) ? 0 : nodes[lChild].count;
            int cntR = (rChild == -1) ? 0 : nodes[rChild].count;

            if (cntR - cntL > 0) {
                result |= (1 << bit);
                lNode = (lChild == -1) ? 0 : lChild;
                rNode = (rChild == -1) ? 0 : rChild;
            } else {
                lNode = nodes[lNode].child[b];
                rNode = nodes[rNode].child[b];
                if (lNode == -1) lNode = 0;
                if (rNode == -1) rNode = 0;
            }
        }
        return result;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  可持久化 Trie (Persistent Trie)         " << endl;
    cout << "========================================" << endl << endl;

    PersistentTrie pt;
    vector<int> versions = {0};  // 版本 0: 空

    // 插入数组 [2, 5, 3, 7, 1]
    int arr[] = {2, 5, 3, 7, 1};
    for (int x : arr) {
        int v = pt.insert(versions.back(), x);
        versions.push_back(v);
    }
    cout << "[数据] 2, 5, 3, 7, 1 (5个版本)" << endl;

    // 区间 [2, 4] (值: 5, 3, 7) 中与 6 异或最大
    int ans = pt.maxXor(1, 4, 6);
    cout << "[查询] 区间[2,4] 与 6 异或最大: " << ans
         << " (5^6=3, 3^6=5, 7^6=1 → 最大 5)" << endl;

    cout << endl << "[可持久化 Trie]" << endl;
    cout << "  每次插入 O(log U) 新建节点" << endl;
    cout << "  区间查询: version_r - version_{l-1}" << endl;
    cout << "  经典题: 区间最大异或和 (CF 665E / 276D)" << endl;

    return 0;
}
