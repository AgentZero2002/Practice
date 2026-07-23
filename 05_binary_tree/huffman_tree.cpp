/**
 * Huffman 编码树
 *
 * 对应教材 5.5 节：Huffman 编码
 *
 * 给定若干字符及其出现频率，构造一棵最优无歧义编码树 (PFC, Prefix-Free Code)，
 * 使得编码的总长度（加权路径长度 WPL）达到最短。
 *
 * 算法（贪心策略）:
 *   1. 将所有字符视为独立的单节点树，放入优先队列（最小堆）
 *   2. 每次取出频率最小的两棵树 T1, T2 合并为一棵新树
 *   3. 新树的频率 = T1.freq + T2.freq
 *   4. 重复直到只剩一棵树，即为 Huffman 树
 *
 * 时间复杂度: O(n log n)
 */

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// ==================== Huffman 树节点 ====================
struct HuffNode {
    char ch;          // 字符（仅叶节点有效）
    int freq;         // 频率
    HuffNode* left;
    HuffNode* right;

    HuffNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffNode(int f, HuffNode* l, HuffNode* r)  // 内部节点
        : ch('\0'), freq(f), left(l), right(r) {}
};

// 优先队列比较器（小顶堆）
struct Compare {
    bool operator()(HuffNode* a, HuffNode* b) {
        return a->freq > b->freq;  // 频率小者优先
    }
};

// ==================== 构建 Huffman 树 ====================
HuffNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<HuffNode*, vector<HuffNode*>, Compare> pq;

    // 1. 所有字符作为独立节点入队列
    for (auto& p : freqMap)
        pq.push(new HuffNode(p.first, p.second));

    // 2. 不断取出最小的两棵树合并
    while (pq.size() > 1) {
        HuffNode* left = pq.top(); pq.pop();
        HuffNode* right = pq.top(); pq.pop();
        HuffNode* parent = new HuffNode(left->freq + right->freq, left, right);
        pq.push(parent);
    }

    return pq.empty() ? nullptr : pq.top();
}

// ==================== 生成编码表 ====================
void generateCodes(HuffNode* root, const string& prefix,
                   unordered_map<char, string>& codes) {
    if (!root) return;
    // 叶节点
    if (!root->left && !root->right) {
        codes[root->ch] = prefix.empty() ? "0" : prefix;  // 单字符情况
        return;
    }
    generateCodes(root->left, prefix + "0", codes);
    generateCodes(root->right, prefix + "1", codes);
}

// ==================== 计算 WPL（加权路径长度） ====================
int computeWPL(HuffNode* root, int depth) {
    if (!root) return 0;
    if (!root->left && !root->right)
        return root->freq * depth;  // 叶节点贡献
    return computeWPL(root->left, depth + 1) +
           computeWPL(root->right, depth + 1);
}

// ==================== 编码与解码 ====================
string encode(const string& text, const unordered_map<char, string>& codes) {
    string result;
    for (char ch : text)
        result += codes.at(ch);
    return result;
}

string decode(const string& encoded, HuffNode* root) {
    string result;
    HuffNode* node = root;
    for (char bit : encoded) {
        node = (bit == '0') ? node->left : node->right;
        if (!node->left && !node->right) {  // 到达叶节点
            result += node->ch;
            node = root;
        }
    }
    return result;
}

// ==================== 打印编码表 ====================
void printCodes(const unordered_map<char, string>& codes,
                const unordered_map<char, int>& freqMap) {
    cout << "  " << string(40, '-') << endl;
    cout << "  char | freq | code | bits" << endl;
    cout << "  " << string(40, '-') << endl;
    for (auto& p : codes) {
        char ch = p.first;
        cout << "  " << (ch == ' ' ? "SPC" : string(1, ch))
             << "    | " << setw(4) << freqMap.at(ch)
             << " | " << setw(8) << p.second
             << " | " << p.second.length() << endl;
    }
    cout << "  " << string(40, '-') << endl;
}

// ==================== 清理 Huffman 树 ====================
void deleteHuffTree(HuffNode* root) {
    if (!root) return;
    deleteHuffTree(root->left);
    deleteHuffTree(root->right);
    delete root;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Huffman 编码树                         " << endl;
    cout << "========================================" << endl << endl;

    // 输入: 字符及频率
    // "this is an example of a huffman tree"
    unordered_map<char, int> freqMap = {
        {'a', 4}, {'e', 4}, {'f', 3}, {'h', 2}, {'i', 2},
        {'l', 1}, {'m', 2}, {'n', 2}, {'o', 1}, {'p', 1},
        {'r', 1}, {'s', 2}, {'t', 2}, {'u', 1}, {'x', 1},
        {' ', 8}  // 空格也参与编码
    };

    cout << "[频率统计]" << endl;
    cout << "  ";
    for (auto& p : freqMap)
        cout << (p.first == ' ' ? "SPC" : string(1, p.first))
             << ":" << p.second << " ";
    cout << endl << endl;

    // 构建 Huffman 树
    HuffNode* root = buildHuffmanTree(freqMap);

    // 生成编码表
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);

    cout << "[Huffman 编码表]" << endl;
    printCodes(codes, freqMap);
    cout << endl;

    // WPL 计算
    int wpl = computeWPL(root, 0);
    cout << "[加权路径长度 WPL] = " << wpl << endl;
    cout << "  平均编码长度 = " << (double)wpl / 40 << " bits/char" << endl << endl;

    // 编码和解码测试
    string text = "a man a plan a canal ";
    cout << "[编码测试]" << endl;
    cout << "  原文: \"" << text << "\"" << endl;

    string encoded = encode(text, codes);
    cout << "  编码: " << encoded << endl;
    cout << "  压缩比: " << text.length() << " chars × 8 bits → "
         << encoded.length() << " bits ("
         << (double)encoded.length() / (text.length() * 8) * 100 << "% )" << endl;

    string decoded = decode(encoded, root);
    cout << "  解码: \"" << decoded << "\"" << endl;
    cout << "  匹配: " << (text == decoded ? "✓" : "✗") << endl;

    // 清理
    deleteHuffTree(root);

    return 0;
}
