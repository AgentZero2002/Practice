/**
 * B-树实现
 *
 * 对应教材 8.2 节：B-树
 *
 * B-树是一种平衡的多路搜索树，专为外存（磁盘）设计。
 * 每个节点可以包含多个关键字和多个孩子分支。
 *
 * m 阶 B-树的性质 (m ≥ 3):
 *   - 每个内部节点最多有 m 个孩子，最少有 ceil(m/2) 个孩子（根节点除外）
 *   - 每个节点最多有 m-1 个关键字
 *   - 所有叶节点在同一层
 *
 * 本实现演示 5 阶 B-树的插入分裂过程。
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int ORDER = 5;                        // B-树的阶
const int MAX_KEYS = ORDER - 1;             // 每个节点最多 4 个关键字
const int MIN_KEYS = (ORDER + 1) / 2 - 1;   // 每个节点最少 2 个关键字（根除外）

struct BTreeNode {
    vector<int> keys;                  // 关键字
    vector<BTreeNode*> children;       // 孩子指针
    bool isLeaf;

    BTreeNode(bool leaf = true) : isLeaf(leaf) {}

    // 在非叶节点中查找关键字应属于哪个孩子子树
    int childIndex(int key) const {
        int idx = 0;
        while (idx < (int)keys.size() && keys[idx] < key)
            idx++;
        return idx;
    }

    // 在节点中插入关键字（假定节点未满）
    void insertKey(int key) {
        keys.insert(
            lower_bound(keys.begin(), keys.end(), key), key);
    }

    // 从节点中删除关键字
    void removeKey(int key) {
        auto it = find(keys.begin(), keys.end(), key);
        if (it != keys.end()) keys.erase(it);
    }
};

class BTree {
private:
    BTreeNode* _root;

    // 分裂满节点（上溢处理）
    void splitChild(BTreeNode* parent, int index) {
        BTreeNode* full = parent->children[index];
        BTreeNode* newNode = new BTreeNode(full->isLeaf);

        int mid = MAX_KEYS / 2;  // 中间关键字索引 (5阶: mid=2, 即第3个位置)
        int midKey = full->keys[mid];

        // 将 mid 之后的关键字移到新节点
        for (int i = mid + 1; i < (int)full->keys.size(); i++)
            newNode->keys.push_back(full->keys[i]);

        // 如果不是叶节点，也将孩子分一半
        if (!full->isLeaf) {
            for (int i = mid + 1; i < (int)full->children.size(); i++)
                newNode->children.push_back(full->children[i]);
        }

        // 将 mid 之前的保留在原节点
        full->keys.resize(mid);
        if (!full->isLeaf)
            full->children.resize(mid + 1);

        // 将中间关键字提升到父节点
        parent->keys.insert(parent->keys.begin() + index, midKey);
        parent->children.insert(parent->children.begin() + index + 1, newNode);
    }

    // 非满节点的插入
    void insertNonFull(BTreeNode* node, int key) {
        int i = node->keys.size() - 1;

        if (node->isLeaf) {
            // 叶节点直接插入
            node->insertKey(key);
        } else {
            // 找到应插入的孩子子树
            int idx = node->childIndex(key);

            // 如果孩子已满，先分裂
            if ((int)node->children[idx]->keys.size() == MAX_KEYS) {
                splitChild(node, idx);
                if (key > node->keys[idx])
                    idx++;  // 中间关键字被提升，重新判断走哪个孩子
            }

            insertNonFull(node->children[idx], key);
        }
    }

    // 遍历（中序）
    void traverse(BTreeNode* node, int depth = 0) const {
        if (!node) return;
        for (int i = 0; i < (int)node->keys.size(); i++) {
            if (!node->isLeaf)
                traverse(node->children[i], depth + 1);
            cout << node->keys[i] << " ";
        }
        if (!node->isLeaf)
            traverse(node->children[node->keys.size()], depth + 1);
    }

    // 打印树结构
    void printTree(BTreeNode* node, int depth = 0) const {
        if (!node) return;
        cout << string(depth * 4, ' ');
        cout << "[";
        for (size_t i = 0; i < node->keys.size(); i++) {
            if (i > 0) cout << " ";
            cout << node->keys[i];
        }
        cout << "] ";
        cout << (node->isLeaf ? "(叶)" : "(内)") << endl;
        if (!node->isLeaf)
            for (auto child : node->children)
                printTree(child, depth + 1);
    }

    void clear(BTreeNode* node) {
        if (!node) return;
        if (!node->isLeaf)
            for (auto child : node->children)
                clear(child);
        delete node;
    }

    // 检查是否包含
    bool contains(BTreeNode* node, int key) const {
        if (!node) return false;
        int idx = node->childIndex(key);
        if (idx < (int)node->keys.size() && node->keys[idx] == key)
            return true;
        if (node->isLeaf) return false;
        return contains(node->children[idx], key);
    }

public:
    BTree() : _root(nullptr) {}
    ~BTree() { clear(_root); }

    void insert(int key) {
        if (!_root) {
            _root = new BTreeNode(true);
            _root->keys.push_back(key);
            return;
        }

        // 如果根已满，需要分裂根
        if ((int)_root->keys.size() == MAX_KEYS) {
            BTreeNode* newRoot = new BTreeNode(false);
            newRoot->children.push_back(_root);
            splitChild(newRoot, 0);
            _root = newRoot;
        }

        insertNonFull(_root, key);
    }

    void traverse() const {
        traverse(_root);
        cout << endl;
    }

    void printStructure() const {
        cout << "[B-树结构 (" << ORDER << " 阶)]" << endl;
        if (_root)
            printTree(_root);
        else
            cout << "  (空树)" << endl;
    }

    bool contains(int key) const { return contains(_root, key); }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  B-树 (5阶) — 插入与分裂演示            " << endl;
    cout << "========================================" << endl << endl;

    BTree bt;

    // 逐步插入，观察分裂过程
    cout << "[逐步插入]" << endl;
    int seq[] = {10, 20, 30, 40, 15, 25, 35, 45,
                 50, 5, 12, 18, 22, 28, 32, 38, 42, 48};

    for (int i = 0; i < (int)(sizeof(seq)/sizeof(seq[0])); i++) {
        bt.insert(seq[i]);
        if (i < 5 || i % 5 == 4) {  // 关键步骤展示
            cout << endl << "  插入 " << seq[i] << " 后:" << endl;
            bt.printStructure();
        }
    }

    cout << endl << "[最终 B-树]" << endl;
    bt.printStructure();

    cout << endl << "[中序遍历] ";
    bt.traverse();

    // B-树 VS 内存树
    cout << endl << "[B-树的关键特性]" << endl;
    cout << "  1. 节点可存储多个关键字，" << ORDER << " 阶 max = " << MAX_KEYS << " 个" << endl;
    cout << "  2. 通过增加扇出(fan-out)来降低树高" << endl;
    cout << "  3. 每次磁盘 I/O 读取一个完整节点" << endl;
    cout << "  4. 节点满时分裂，向上提升中间关键字" << endl;
    cout << "  5. 删除时若下溢，从兄弟节点借或合并" << endl;

    return 0;
}
