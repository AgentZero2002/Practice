/**
 * LRU 缓存 (Least Recently Used Cache)
 *
 * 经典题: LeetCode 146
 *
 * 设计一个遵循最近最少使用 (LRU) 淘汰策略的缓存。
 * 面试/工作中最常见的 DS 设计题之一！
 *
 * 核心: 哈希表 + 双向链表 O(1)
 *   - 哈希表: key → 链表节点的指针，O(1) 查找
 *   - 双向链表: 维护访问顺序，头部=最近使用，尾部=最久未用
 *
 * 操作:
 *   get(key): 若存在 → 移到链表头部，返回值
 *   put(key, value): 若存在 → 更新值并移到头部
 *                    若不存在 → 插入头部
 *                    若容量已满 → 删除链表尾部
 */

#include <iostream>
#include <unordered_map>
using namespace std;

class LRUCache {
private:
    struct Node {
        int key, value;
        Node* prev;
        Node* next;
        Node(int k, int v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    int capacity;
    unordered_map<int, Node*> cache;
    Node* head;  // 哨兵: 最近使用的在 head→next
    Node* tail;  // 哨兵: 最久未用的在 tail→prev

    // 将节点移到链表头部（标记为最近使用）
    void moveToHead(Node* node) {
        removeNode(node);
        addToHead(node);
    }

    // 从链表中移除节点（但不 delete）
    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // 插入到头部（head 之后）
    void addToHead(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    // 删除尾部节点（tail 之前）
    Node* removeTail() {
        Node* node = tail->prev;
        removeNode(node);
        return node;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }

    int get(int key) {
        if (cache.find(key) == cache.end())
            return -1;  // 未命中

        Node* node = cache[key];
        moveToHead(node);  // 标记为最近使用
        return node->value;
    }

    void put(int key, int value) {
        if (cache.find(key) != cache.end()) {
            // 键已存在，更新值并移到头部
            Node* node = cache[key];
            node->value = value;
            moveToHead(node);
        } else {
            // 键不存在，新建节点
            Node* newNode = new Node(key, value);
            cache[key] = newNode;
            addToHead(newNode);

            // 超出容量 → 淘汰最久未用的
            if ((int)cache.size() > capacity) {
                Node* lru = removeTail();
                cache.erase(lru->key);
                delete lru;
            }
        }
    }

    void print() {
        cout << "  [";
        for (Node* p = head->next; p != tail; p = p->next) {
            cout << "(" << p->key << ":" << p->value << ")";
            if (p->next != tail) cout << " → ";
        }
        cout << "]" << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  LRU 缓存 (Least Recently Used)         " << endl;
    cout << "  — 哈希表 + 双向链表 O(1)               " << endl;
    cout << "========================================" << endl << endl;

    LRUCache lru(3);  // 容量 = 3

    cout << "[操作序列]" << endl;
    cout << "  put(1, 10)" << endl; lru.put(1, 10); lru.print();
    cout << "  put(2, 20)" << endl; lru.put(2, 20); lru.print();
    cout << "  put(3, 30)" << endl; lru.put(3, 30); lru.print();
    cout << "  get(1)" << endl; cout << "  → " << lru.get(1) << " (命中, 移到头部)" << endl; lru.print();
    cout << "  put(4, 40) → 淘汰最久未用的 key=2" << endl; lru.put(4, 40); lru.print();
    cout << "  get(2)" << endl; cout << "  → " << lru.get(2) << " (已淘汰, 未命中)" << endl;

    cout << endl << "[O(1) 的秘密] 每个操作都是 O(1):" << endl;
    cout << "  · 哈希表 find: O(1)" << endl;
    cout << "  · 双向链表删除/插入: O(1) (已知节点位置)" << endl;
    cout << endl;
    cout << "[扩展] LFU (最不经常使用) 需要额外频次计数" << endl;

    return 0;
}
