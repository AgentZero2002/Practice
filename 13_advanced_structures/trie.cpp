/**
 * 字典树 (Trie / Prefix Tree)
 *
 * Trie 是一种树形数据结构，用于高效存储和检索字符串集合。
 * 每个节点代表一个前缀，边代表字符。
 *
 * 核心操作 (均 O(L)，L 为字符串长度):
 *   - insert(word): 插入单词
 *   - search(word): 精确查找单词
 *   - startsWith(prefix): 查找以 prefix 为前缀的单词数
 *   - remove(word): 删除单词
 *
 * 经典应用:
 *   - 自动补全 / 拼写检查
 *   - IP 路由最长前缀匹配
 *   - 最大异或对 (Max XOR Pair)
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
using namespace std;

class Trie {
private:
    struct Node {
        Node* children[26];
        int count;    // 经过此节点的单词数
        int endCount; // 以此节点结尾的单词数

        Node() : count(0), endCount(0) {
            fill(begin(children), end(children), nullptr);
        }
    };

    Node* root;

public:
    Trie() : root(new Node()) {}

    // 插入单词
    void insert(const string& word) {
        Node* node = root;
        for (char ch : word) {
            int idx = ch - 'a';
            if (!node->children[idx])
                node->children[idx] = new Node();
            node = node->children[idx];
            node->count++;
        }
        node->endCount++;
    }

    // 精确查找
    bool search(const string& word) {
        Node* node = root;
        for (char ch : word) {
            int idx = ch - 'a';
            if (!node->children[idx]) return false;
            node = node->children[idx];
        }
        return node->endCount > 0;
    }

    // 统计以 prefix 为前缀的单词数
    int countPrefix(const string& prefix) {
        Node* node = root;
        for (char ch : prefix) {
            int idx = ch - 'a';
            if (!node->children[idx]) return 0;
            node = node->children[idx];
        }
        return node->count;
    }

    // 删除单词
    bool remove(const string& word) {
        if (!search(word)) return false;
        Node* node = root;
        for (char ch : word) {
            int idx = ch - 'a';
            Node* child = node->children[idx];
            child->count--;
            if (child->count == 0) {
                // 无其他单词经过此节点，可安全删除
                clearNode(child);
                node->children[idx] = nullptr;
                return true;
            }
            node = child;
        }
        node->endCount--;
        return true;
    }

private:
    void clearNode(Node* node) {
        for (int i = 0; i < 26; i++)
            if (node->children[i])
                clearNode(node->children[i]);
        delete node;
    }
};

// ==================== 应用: 最大异或对 (Max XOR Pair) ====================
// 在数组中找两个数的最大异或值 — LeetCode 421
class BinaryTrie {
private:
    struct Node {
        Node* children[2] = {nullptr, nullptr};
    };
    Node* root;

public:
    BinaryTrie() : root(new Node()) {}

    void insert(int num) {
        Node* node = root;
        for (int i = 31; i >= 0; i--) {
            int bit = (num >> i) & 1;
            if (!node->children[bit])
                node->children[bit] = new Node();
            node = node->children[bit];
        }
    }

    // 找到与 num 异或值最大的数
    int maxXor(int num) {
        Node* node = root;
        int result = 0;
        for (int i = 31; i >= 0; i--) {
            int bit = (num >> i) & 1;
            int want = 1 - bit;  // 想要相反的位以最大化异或
            if (node->children[want]) {
                result |= (1 << i);
                node = node->children[want];
            } else {
                node = node->children[bit];
            }
        }
        return result;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  字典树 (Trie / Prefix Tree)            " << endl;
    cout << "========================================" << endl << endl;

    // 1. 字符串 Trie
    cout << "[字符串 Trie]" << endl;
    Trie trie;
    trie.insert("apple");
    trie.insert("app");
    trie.insert("application");
    trie.insert("banana");
    trie.insert("band");

    cout << "  插入: apple, app, application, banana, band" << endl;
    cout << "  search(\"app\"): " << (trie.search("app") ? "存在" : "不存在") << endl;
    cout << "  search(\"appl\"): " << (trie.search("appl") ? "存在" : "不存在") << endl;
    cout << "  startsWith(\"app\"): " << trie.countPrefix("app") << " 个单词" << endl;
    cout << "  startsWith(\"ba\"): " << trie.countPrefix("ba") << " 个单词" << endl;
    cout << endl;

    // 删除
    trie.remove("app");
    cout << "  删除 \"app\" 后:" << endl;
    cout << "  search(\"app\"): " << (trie.search("app") ? "存在" : "不存在") << endl;
    cout << "  search(\"apple\"): " << (trie.search("apple") ? "存在" : "存在") << endl;
    cout << "  startsWith(\"app\"): " << trie.countPrefix("app") << " 个" << endl;
    cout << endl;

    // 2. 二进制 Trie — 最大异或对
    cout << "[二进制 Trie — 最大异或对]" << endl;
    vector<int> nums = {3, 10, 5, 25, 2, 8};
    cout << "  数组: ";
    for (int x : nums) cout << x << " ";

    BinaryTrie bt;
    int maxXor = 0;
    for (int x : nums) {
        bt.insert(x);
        maxXor = max(maxXor, bt.maxXor(x));
    }
    cout << endl << "  最大异或值: " << maxXor << " (5 XOR 25 = 28)" << endl;

    cout << endl << "[Trie 特性]" << endl;
    cout << "  时间: O(L) 插入/查找/删除, L=字符串长度" << endl;
    cout << "  空间: O(N × L × 26) 最坏, 实际远小于此" << endl;
    cout << "  适用: 字符串集合操作, 前缀相关查询" << endl;

    return 0;
}
