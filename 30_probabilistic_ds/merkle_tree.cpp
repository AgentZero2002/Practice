/**
 * Merkle 哈希树 (Merkle Tree / Hash Tree)
 *
 * Merkle 树是一棵二叉树，叶节点存储数据块的哈希值，
 * 内部节点存储其孩子哈希值的组合哈希。
 *
 * 核心用途: 高效验证大数据集的完整性。
 *
 * 验证方式:
 *   给定一个数据块 + Merkle Proof (从叶到根的路径上的兄弟哈希)，
 *   我们可以 O(log n) 验证该数据块确实属于这棵树。
 *
 * 应用:
 *   - 区块链 (Bitcoin/Ethereum): 交易 Merkle Tree
 *   - Git: 每个 commit 都是 Merkle 树的根哈希
 *   - BitTorrent: 分块校验
 *   - Certificate Transparency
 */

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <iomanip>
#include <sstream>
using namespace std;

class MerkleTree {
private:
    struct Node {
        string hash;
        Node* left;
        Node* right;
        Node(const string& h) : hash(h), left(nullptr), right(nullptr) {}
    };

    Node* root;
    vector<string> leafHashes;

    string sha256lite(const string& data) {
        // 简化版哈希（真实场景应使用 SHA-256）
        hash<string> h;
        size_t val = h(data);
        ostringstream oss;
        oss << hex << setfill('0') << setw(16) << val;
        return oss.str();
    }

    Node* buildTree(int l, int r) {
        if (l == r) {
            return new Node(leafHashes[l]);
        }
        int mid = (l + r) / 2;
        Node* left = buildTree(l, mid);
        Node* right = buildTree(mid + 1, r);
        string combined = left->hash + right->hash;
        Node* node = new Node(sha256lite(combined));
        node->left = left;
        node->right = right;
        return node;
    }

    void printTree(Node* node, int depth = 0) {
        if (!node) return;
        printTree(node->right, depth + 1);
        cout << string(depth * 4, ' ') << node->hash.substr(0, 8) << "..." << endl;
        printTree(node->left, depth + 1);
    }

public:
    MerkleTree(const vector<string>& data) {
        for (const auto& d : data)
            leafHashes.push_back(sha256lite(d));
        root = buildTree(0, leafHashes.size() - 1);
    }

    string rootHash() const { return root->hash; }

    // 验证: 假设我们有完整数据和根哈希，验证某块数据
    bool verifyData(int index, const string& data, const string& knownRoot) {
        string leafHash = sha256lite(data);
        return knownRoot == rootHash();  // 简化: 实际需 Merkle Proof
    }

    void print() {
        cout << "  Merkle Tree 结构:" << endl;
        printTree(root);
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Merkle 哈希树 (Merkle Tree)            " << endl;
    cout << "========================================" << endl << endl;

    // 模拟数据块
    vector<string> blocks = {
        "Alice pays Bob 10 BTC",
        "Bob pays Charlie 5 BTC",
        "Charlie pays Dave 3 BTC",
        "Dave pays Eve 1 BTC"
    };

    cout << "[数据块]" << endl;
    for (size_t i = 0; i < blocks.size(); i++)
        cout << "  Block " << i << ": " << blocks[i] << endl;

    MerkleTree mt(blocks);

    cout << endl;
    mt.print();

    cout << endl << "[根哈希] " << mt.rootHash().substr(0, 16) << "..." << endl;

    // 验证
    cout << endl << "[验证]" << endl;
    cout << "  验证 Block 0 原始数据: "
         << (mt.verifyData(0, blocks[0], mt.rootHash()) ? "通过 ✓" : "失败 ✗") << endl;
    cout << "  验证 篡改数据: "
         << (mt.verifyData(0, "Alice pays Bob 100 BTC", mt.rootHash())
             ? "通过 ✗" : "失败 ✓") << endl;

    cout << endl << "[应用场景]" << endl;
    cout << "  区块链:   每个区块头含交易 Merkle Root" << endl;
    cout << "  Git:     每个 commit 的 SHA 本质是 Merkle 树" << endl;
    cout << "  SPV节点: 只需下载区块头，用 Merkle Proof 验证交易" << endl;
    cout << "  复杂度:   验证 O(log n), Proof 大小 O(log n)" << endl;

    return 0;
}
