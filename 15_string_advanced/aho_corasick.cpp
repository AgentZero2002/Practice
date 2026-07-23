/**
 * AC 自动机 (Aho-Corasick Automaton)
 *
 * AC 自动机用于在文本中同时匹配多个模式串。
 * 它是 Trie + KMP 失败指针的结合体。
 *
 * 构建:
 *   1. 将所有模式串插入 Trie
 *   2. BFS 构建 fail 指针:
 *      fail[node] = 从根到 node 的最长真后缀，该后缀也是某个模式串的前缀
 *   3. 匹配: 沿 Trie 走字符，失败时跳转 fail
 *
 * 应用:
 *   - 敏感词过滤系统
 *   - 病毒特征码扫描
 *   - 多模式搜索引擎
 *
 * 时间复杂度: O(总长度 + 匹配数)
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
using namespace std;

class AhoCorasick {
private:
    struct Node {
        Node* children[26];
        Node* fail;           // 失败指针
        vector<int> output;   // 在此节点结束的模式串索引列表
        bool isEnd;

        Node() : fail(nullptr), isEnd(false) {
            fill(begin(children), end(children), nullptr);
        }
    };

    Node* root;
    vector<string> patterns;

public:
    AhoCorasick() : root(new Node()) {}

    void addPattern(const string& pattern) {
        patterns.push_back(pattern);
        Node* node = root;

        for (char ch : pattern) {
            int idx = ch - 'a';
            if (!node->children[idx])
                node->children[idx] = new Node();
            node = node->children[idx];
        }

        node->isEnd = true;
        node->output.push_back(patterns.size() - 1);
    }

    void build() {
        queue<Node*> q;
        root->fail = root;

        // 第一层: fail 都指向 root
        for (int i = 0; i < 26; i++) {
            if (root->children[i]) {
                root->children[i]->fail = root;
                q.push(root->children[i]);
            }
        }

        // BFS 构建 fail 指针
        while (!q.empty()) {
            Node* cur = q.front(); q.pop();

            for (int i = 0; i < 26; i++) {
                if (!cur->children[i]) continue;

                Node* child = cur->children[i];

                // 寻找孩子的 fail
                Node* f = cur->fail;
                while (f != root && !f->children[i])
                    f = f->fail;
                if (f->children[i])
                    child->fail = f->children[i];
                else
                    child->fail = root;

                // 继承 fail 的输出
                child->output.insert(child->output.end(),
                    child->fail->output.begin(),
                    child->fail->output.end());

                q.push(child);
            }
        }
    }

    // 在 text 中搜索所有模式串
    // 返回: {模式串索引, 在 text 中的位置}
    vector<pair<int, int>> search(const string& text) {
        vector<pair<int, int>> results;
        Node* node = root;

        for (int i = 0; i < (int)text.length(); i++) {
            int idx = text[i] - 'a';

            // 失败跳转
            while (node != root && !node->children[idx])
                node = node->fail;

            if (node->children[idx])
                node = node->children[idx];

            // 收集当前节点的所有输出
            for (int pid : node->output)
                results.push_back({pid, i - (int)patterns[pid].length() + 1});
        }

        return results;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  AC 自动机 (Aho-Corasick)               " << endl;
    cout << "  — 多模式匹配                           " << endl;
    cout << "========================================" << endl << endl;

    AhoCorasick ac;

    // 添加模式串 (敏感词)
    ac.addPattern("he");
    ac.addPattern("she");
    ac.addPattern("his");
    ac.addPattern("hers");

    ac.build();

    string text = "ushers";
    cout << "[多模式匹配]" << endl;
    cout << "  文本: \"" << text << "\"" << endl;
    cout << "  模式: \"he\", \"she\", \"his\", \"hers\"" << endl << endl;

    auto matches = ac.search(text);

    cout << "  匹配结果:" << endl;
    vector<string> pats = {"he", "she", "his", "hers"};
    for (auto [pid, pos] : matches) {
        cout << "    位置 " << pos << ": \"" << pats[pid] << "\"" << endl;
    }
    cout << "  期望: \"he\" at 1, \"she\" at 0, \"he\" at 4?, \"hers\" at 2" << endl;

    cout << endl << "[AC 自动机构建]" << endl;
    cout << "  Step 1: 所有模式串插入 Trie" << endl;
    cout << "  Step 2: BFS 构建 fail 指针" << endl;
    cout << "    fail[node] = node 的最长真后缀（也是某模式前缀）" << endl;
    cout << "  Step 3: 继承 fail 的 output 集合" << endl;
    cout << endl;
    cout << "[复杂度] 匹配 O(text_len + num_matches)" << endl;

    return 0;
}
