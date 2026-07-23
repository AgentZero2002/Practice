/**
 * 跳表 (Skip List)
 *
 * 对应教材 第9章：词典 — 扩展实现
 *
 * 跳表是一种基于概率的平衡数据结构，可以看作是
 * "分层链表"。每一层是下一层的"快速通道"。
 *
 * 核心思想:
 *   底层 (level 0) 包含所有元素的普通有序链表
 *   上层是下层的稀疏子集，像"快速通道"
 *   查找时：从顶层向右走，下不去时向下一层
 *
 * 复杂度 (期望):
 *   查找/插入/删除: O(log n)
 *   空间: O(n) 期望（每元素大约出现在 2 层）
 *
 * 应用:
 *   - Redis Sorted Set 的底层实现之一
 *   - LevelDB 的 memtable
 */

#include <iostream>
#include <vector>
#include <random>
#include <climits>
using namespace std;

const int MAX_LEVEL = 16;  // 最高层级
const double P = 0.5;       // 每层的晋升概率

struct SkipNode {
    int key;
    int value;
    vector<SkipNode*> forward;  // 各层的前驱指针

    SkipNode(int k, int v, int level)
        : key(k), value(v), forward(level + 1, nullptr) {}
};

class SkipList {
private:
    int _level;           // 当前最高层
    SkipNode* _header;    // 头哨兵
    mt19937 _rng;
    uniform_real_distribution<double> _dist;

    // 随机生成层级 (几何分布)
    int randomLevel() {
        int level = 0;
        while (_dist(_rng) < P && level < MAX_LEVEL)
            level++;
        return level;
    }

public:
    SkipList() : _level(0), _rng(42), _dist(0.0, 1.0) {
        _header = new SkipNode(INT_MIN, 0, MAX_LEVEL);
    }

    // 查找 O(log n) 期望
    bool search(int key, int& value) {
        SkipNode* cur = _header;

        // 从最高层开始，逐层下降到最底层
        for (int i = _level; i >= 0; i--) {
            while (cur->forward[i] && cur->forward[i]->key < key)
                cur = cur->forward[i];
        }

        cur = cur->forward[0];  // 最底层的下一个
        if (cur && cur->key == key) {
            value = cur->value;
            return true;
        }
        return false;
    }

    // 插入 O(log n) 期望
    void insert(int key, int value) {
        vector<SkipNode*> update(MAX_LEVEL + 1);
        SkipNode* cur = _header;

        // 从高到低找每层的插入位置
        for (int i = _level; i >= 0; i--) {
            while (cur->forward[i] && cur->forward[i]->key < key)
                cur = cur->forward[i];
            update[i] = cur;  // 记录每层的前驱
        }

        cur = cur->forward[0];

        // 键已存在 → 更新值
        if (cur && cur->key == key) {
            cur->value = value;
            return;
        }

        // 随机生成新节点层级
        int newLevel = randomLevel();

        // 如果新层级超过当前最高层
        if (newLevel > _level) {
            for (int i = _level + 1; i <= newLevel; i++)
                update[i] = _header;
            _level = newLevel;
        }

        // 创建新节点
        SkipNode* newNode = new SkipNode(key, value, newLevel);

        // 在各层插入新节点（类似链表插入）
        for (int i = 0; i <= newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    // 删除 O(log n) 期望
    bool remove(int key) {
        vector<SkipNode*> update(MAX_LEVEL + 1);
        SkipNode* cur = _header;

        for (int i = _level; i >= 0; i--) {
            while (cur->forward[i] && cur->forward[i]->key < key)
                cur = cur->forward[i];
            update[i] = cur;
        }

        cur = cur->forward[0];
        if (!cur || cur->key != key) return false;

        // 从各层删除
        for (int i = 0; i <= _level; i++) {
            if (update[i]->forward[i] != cur) break;
            update[i]->forward[i] = cur->forward[i];
        }

        delete cur;

        // 更新最高层
        while (_level > 0 && !_header->forward[_level])
            _level--;

        return true;
    }

    // 打印跳表结构
    void print() {
        cout << "  SkipList (level " << _level << "):" << endl;
        for (int i = _level; i >= 0; i--) {
            cout << "  L" << i << ": HEAD → ";
            SkipNode* node = _header->forward[i];
            while (node) {
                cout << node->key << " → ";
                node = node->forward[i];
            }
            cout << "NIL" << endl;
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  跳表 (Skip List)                       " << endl;
    cout << "  — 概率平衡的分层链表                    " << endl;
    cout << "========================================" << endl << endl;

    SkipList sl;

    // 插入
    int keys[] = {3, 6, 7, 9, 12, 19, 17, 26, 21, 25};
    cout << "[插入] ";
    for (int k : keys) { cout << k << " "; sl.insert(k, k * 10); }
    cout << endl;
    sl.print();
    cout << endl;

    // 查找
    cout << "[查找]" << endl;
    for (int k : {12, 20, 25}) {
        int val;
        if (sl.search(k, val))
            cout << "  " << k << " → " << val << endl;
        else
            cout << "  " << k << " → 未找到" << endl;
    }

    // 删除
    cout << endl << "[删除 19]" << endl;
    sl.remove(19);
    sl.print();

    cout << endl << "[跳表 vs 平衡树]" << endl;
    cout << "  跳表:    实现简单，概率平衡，Redis 使用" << endl;
    cout << "  红黑树:   严格平衡，STL map 使用" << endl;
    cout << "  SkipList: 层级高度期望 = log_{1/p}(n)" << endl;
    cout << "            p=0.5 时 ≈ log₂(n) 层" << endl;

    return 0;
}
