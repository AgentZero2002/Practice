/**
 * K路归并 + 败者树 (Loser Tree)
 *
 * K 路归并的挑战: K 很大时 (如 K=1000), 每次取最小值
 * 若用朴素比较 → O(K) per element → 总 O(N·K)
 *
 * 败者树优化 → O(log K) per element:
 *   Loser Tree 是一个完全二叉树:
 *     - 叶节点 = K 个归并段当前元素
 *     - 内部节点 = 子节点比较的 "败者" (较大者)
 *     - 根节点 = 最终的 "胜者" (最小者)
 *     每取走一个胜者, 从该叶向上更新 O(log K)
 *
 * 架构:
 *         [winner]
 *           |
 *     [loser A]
 *      /    \
 *   [lB]  [lC]
 *   / \    / \
 *  e0 e1 e2 e3    ← K=4 叶节点
 *
 * Heap vs Loser Tree:
 *   - Heap: push+pop = 2×O(log K)
 *   - Loser Tree: replace winner = O(log K)
 *   - Loser Tree 在 K 大时约快 2x (更少比较)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

const int INF = numeric_limits<int>::max();

class LoserTree {
    int K;
    vector<int> tree;   // 败者树 (大小 2*K)
    vector<int> leaves; // 叶节点 (各段当前元素)

public:
    LoserTree(int k) : K(k), tree(2 * k), leaves(k) {}

    void build() {
        // 初始化所有叶和内部节点为 INF
        for (int i = 0; i < 2 * K; i++) tree[i] = K;  // K = INF 标记
        for (int i = 0; i < K; i++) {
            // 从叶到根比较
            int winner = i;
            int parent = (i + K) / 2;
            while (parent > 0) {
                if (leaves[winner] > leaves[tree[parent]]) {
                    swap(winner, tree[parent]);  // winner 继续向上
                }
                parent /= 2;
            }
            tree[0] = winner;  // 最终胜者
        }
    }

    void setLeaf(int idx, int value) {
        leaves[idx] = value;
    }

    int getWinner() { return tree[0]; }

    void replaceWinner(int newVal) {
        int winner = tree[0];
        leaves[winner] = newVal;
        // 从 winner 的叶向上更新
        int parent = (winner + K) / 2;
        while (parent > 0) {
            int loser = tree[parent];
            if (leaves[winner] > leaves[loser])
                swap(winner, loser);
            tree[parent] = loser;
            parent /= 2;
        }
        tree[0] = winner;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  K路归并 + 败者树 (Loser Tree)            " << endl;
    cout << "========================================" << endl << endl;

    int K = 4;
    vector<vector<int>> runs = {
        {1, 6, 11}, {2, 7, 12}, {3, 8, 13}, {4, 9, 14}
    };
    vector<int> idx(K, 0);  // 每段的当前指针

    LoserTree lt(K);
    for (int i = 0; i < K; i++)
        lt.setLeaf(i, runs[i][0]);
    lt.build();

    vector<int> result;
    while (true) {
        int winner = lt.getWinner();
        if (lt.getWinner() >= K) break;  // 简化
        result.push_back(runs[winner][idx[winner]]);
        idx[winner]++;
        int newVal = (idx[winner] < (int)runs[winner].size())
                     ? runs[winner][idx[winner]] : INF;
        lt.replaceWinner(newVal);
        if (newVal == INF) break;  // 简化退出
    }

    cout << "[败者树归并结果] ";
    for (int x : result) cout << x << " ";
    cout << endl;

    cout << endl << "[败者树分析]" << endl;
    cout << "  每元素: O(log K) 比较" << endl;
    cout << "  K=1000 → ~10 次比较 vs 朴素 1000 次" << endl;

    return 0;
}
