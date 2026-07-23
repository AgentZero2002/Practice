/**
 * B+ 树 (B+ Tree) — 数据库索引核心
 *
 * B+ 树是 B-树的变体, 广泛用于数据库和文件系统的索引结构
 * (MySQL InnoDB, PostgreSQL, NTFS 等)。
 *
 * 与 B-树的区别:
 *   - 所有数据(键值)只存储在叶节点中
 *   - 内部节点只存储键(索引), 不存储数据
 *   - 叶节点之间用链表连接 → 支持高效范围查询
 *   - 叶节点存储 (key → value/record_ptr) 的映射
 *
 * 性质 (m 阶 B+ 树):
 *   - 根节点: 至少 1 个键 (若不为叶), 最多 m-1 个键
 *   - 内部节点: 至少 ⌈m/2⌉ 个子节点, 最多 m 个子节点
 *   - 叶节点: 至少 ⌈m/2⌉-1 个键值对, 最多 m-1 个
 *   - 所有叶节点在同一深度 (完美平衡)
 *
 * 核心操作:
 *   - 查找: O(log n) — 从根到叶二分查找
 *   - 插入: O(log n) — 可能需要分裂
 *   - 删除: O(log n) — 可能需要合并
 *   - 范围查询: O(log n + k) — 叶节点链表顺序扫描
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <string>
using namespace std;

const int ORDER = 4;  // 4阶B+树 (最多4个子节点, 3个键)

struct BPlusNode {
    bool isLeaf;
    vector<int> keys;                    // 键 (内部节点: 索引键, 叶节点: 数据键)
    vector<shared_ptr<BPlusNode>> children; // 子节点 (仅内部节点)
    vector<string> values;              // 值 (仅叶节点)
    shared_ptr<BPlusNode> next;         // 叶节点链表 (仅叶节点)

    BPlusNode(bool leaf) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
private:
    shared_ptr<BPlusNode> root;

    // 查找键应在的叶节点
    shared_ptr<BPlusNode> findLeaf(int key) {
        auto node = root;
        while (!node->isLeaf) {
            int i = upper_bound(node->keys.begin(), node->keys.end(), key)
                    - node->keys.begin();
            node = node->children[i];
        }
        return node;
    }

    // 分裂叶节点
    void splitLeaf(shared_ptr<BPlusNode> leaf, shared_ptr<BPlusNode> parent) {
        auto newLeaf = make_shared<BPlusNode>(true);
        int mid = leaf->keys.size() / 2;

        // 将后半移到新节点
        newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        newLeaf->values.assign(leaf->values.begin() + mid, leaf->values.end());
        leaf->keys.resize(mid);
        leaf->values.resize(mid);

        // 链表连接
        newLeaf->next = leaf->next;
        leaf->next = newLeaf;

        // 将新节点的第一个键提升到父节点
        if (parent) {
            insertInParent(leaf, newLeaf->keys[0], newLeaf);
        } else {
            // 创建新的根
            auto newRoot = make_shared<BPlusNode>(false);
            newRoot->keys.push_back(newLeaf->keys[0]);
            newRoot->children.push_back(leaf);
            newRoot->children.push_back(newLeaf);
            root = newRoot;
        }
    }

    // 分裂内部节点
    void splitInternal(shared_ptr<BPlusNode> node, shared_ptr<BPlusNode> parent) {
        auto newNode = make_shared<BPlusNode>(false);
        int mid = node->keys.size() / 2;
        int promoteKey = node->keys[mid];

        newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        newNode->children.assign(node->children.begin() + mid + 1, node->children.end());
        node->keys.resize(mid);
        node->children.resize(mid + 1);

        if (parent)
            insertInParent(node, promoteKey, newNode);
        else {
            auto newRoot = make_shared<BPlusNode>(false);
            newRoot->keys.push_back(promoteKey);
            newRoot->children.push_back(node);
            newRoot->children.push_back(newNode);
            root = newRoot;
        }
    }

    void insertInParent(shared_ptr<BPlusNode> left, int key,
                        shared_ptr<BPlusNode> right) {
        // 简化: 需要找到 left 的父节点并插入 key
        // 此处演示递归分裂时的框架
        // 实际工程中通过传递父节点引用实现
    }

public:
    BPlusTree() : root(make_shared<BPlusNode>(true)) {}

    // 插入
    void insert(int key, string value) {
        auto leaf = findLeaf(key);

        // 查找插入位置
        auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
        if (it != leaf->keys.end() && *it == key) {
            *it = key;  // 更新已存在的键
            leaf->values[it - leaf->keys.begin()] = value;
            return;
        }

        int pos = it - leaf->keys.begin();
        leaf->keys.insert(it, key);
        leaf->values.insert(leaf->values.begin() + pos, value);

        // 检查是否需要分裂
        if ((int)leaf->keys.size() >= ORDER) {
            splitLeaf(leaf, nullptr);  // 简化, 实际需传递父节点
        }
    }

    // 查找
    string search(int key) {
        auto leaf = findLeaf(key);
        auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
        if (it != leaf->keys.end() && *it == key)
            return leaf->values[it - leaf->keys.begin()];
        return "未找到";
    }

    // 范围查询
    vector<pair<int, string>> rangeQuery(int low, int high) {
        vector<pair<int, string>> result;
        auto leaf = findLeaf(low);
        while (leaf) {
            for (size_t i = 0; i < leaf->keys.size(); i++) {
                if (leaf->keys[i] > high) return result;
                if (leaf->keys[i] >= low)
                    result.push_back({leaf->keys[i], leaf->values[i]});
            }
            leaf = leaf->next;
        }
        return result;
    }

    void print() {
        cout << "  B+ 树结构 (BFS 示意):" << endl;
        printNode(root, 0);
    }

    void printNode(shared_ptr<BPlusNode> node, int depth) {
        if (!node) return;
        string indent(depth * 4, ' ');
        if (node->isLeaf) {
            cout << indent << "叶: ";
            for (size_t i = 0; i < node->keys.size(); i++)
                cout << node->keys[i] << "→" << node->values[i] << " ";
            cout << endl;
        } else {
            cout << indent << "内: ";
            for (int k : node->keys) cout << k << " ";
            cout << endl;
            for (auto& ch : node->children)
                printNode(ch, depth + 1);
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  B+ 树 (数据库索引核心)                  " << endl;
    cout << "========================================" << endl << endl;

    BPlusTree tree;
    cout << "[插入] 键值对" << endl;
    tree.insert(5, "Alice");
    tree.insert(8, "Bob");
    tree.insert(1, "Charlie");
    tree.insert(12, "David");
    tree.insert(3, "Eve");
    tree.insert(15, "Frank");

    tree.print();
    cout << endl;

    cout << "[查找]" << endl;
    cout << "  查找 5: " << tree.search(5) << endl;
    cout << "  查找 8: " << tree.search(8) << endl;
    cout << "  查找 99: " << tree.search(99) << endl;
    cout << endl;

    cout << "[范围查询] [3, 12]" << endl;
    auto results = tree.rangeQuery(3, 12);
    for (auto& [k, v] : results)
        cout << "    " << k << " → " << v << endl;
    cout << endl;

    cout << "[B+ 树 vs B 树]" << endl;
    cout << "  B+树: 数据全在叶节点, 叶节点有链表 → 范围查询快" << endl;
    cout << "  B 树: 数据可存于任何节点 → 点查可能更快" << endl;
    cout << "  数据库索引几乎全用 B+树 (MySQL/PostgreSQL)" << endl;

    return 0;
}
