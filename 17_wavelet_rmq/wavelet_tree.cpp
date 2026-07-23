/**
 * Wavelet Tree — 序列上的快速 rank/select 查询
 *
 * Wavelet Tree 是一种简洁数据结构 (Succinct Data Structure),
 * 将一个序列按值域递归二分, 形成平衡二叉树。
 *
 * 结构:
 *   根节点: 按值域中点 mid 将序列分为左右两部分
 *     - 左子: 值 ≤ mid 的元素 (按原序)
 *     - 右子: 值 > mid 的元素 (按原序)
 *     用位图记录每个位置的归属 (0=左, 1=右)
 *   递归构建直到叶结点 (值域大小为 1)
 *
 * 操作 (均 O(log σ), σ = 值域大小):
 *   1. access(i): 查询第 i 个位置的值
 *      - 从根开始, 若 bitmap[i]=0 则走左子树, 否则走右子树
 *      - 下标映射: i' = rank_0(i) 或 rank_1(i)
 *
 *   2. rank(val, i): 值 val 在 [0, i] 范围出现的次数
 *      - 从根到 val 对应的叶的路径上累计
 *
 *   3. kthSmallest(l, r, k): 区间 [l, r] 的第 k 小值
 *      - 在左右子树间根据 rank 决定走哪边
 *
 * 应用:
 *   - 区间第 K 小 (替代主席树)
 *   - 区间不同值数量
 *   - 序列的逆序对 (动态)
 *
 * 空间: n log σ bits + O(n) words
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

class WaveletTree {
private:
    struct Node {
        vector<int> prefixBits;  // 前缀和: prefixBits[i] = 前 i 位中 0 的个数
        int low, high;           // 值域 [low, high]
        Node* left;              // 值 ≤ mid
        Node* right;             // 值 > mid
        Node(int l, int h) : low(l), high(h), left(nullptr), right(nullptr) {}
    };

    Node* build(vector<int>& arr, int l, int r, int low, int high) {
        if (low > high || l > r) return nullptr;
        Node* node = new Node(low, high);

        if (low == high) {
            // 叶节点: 不需要位图, 该范围内所有值都等于 low
            return node;
        }

        int mid = (low + high) / 2;
        node->prefixBits.reserve(r - l + 2);
        node->prefixBits.push_back(0);

        vector<int> leftArr, rightArr;
        for (int i = l; i <= r; i++) {
            if (arr[i] <= mid) {
                leftArr.push_back(arr[i]);
                node->prefixBits.push_back(node->prefixBits.back() + 1);
            } else {
                rightArr.push_back(arr[i]);
                node->prefixBits.push_back(node->prefixBits.back());
            }
        }

        node->left = build(leftArr, 0, (int)leftArr.size() - 1, low, mid);
        node->right = build(rightArr, 0, (int)rightArr.size() - 1, mid + 1, high);
        return node;
    }

    Node* root;

public:
    WaveletTree(vector<int>& arr) {
        int maxVal = *max_element(arr.begin(), arr.end());
        int minVal = *min_element(arr.begin(), arr.end());
        root = build(arr, 0, arr.size() - 1, minVal, maxVal);
    }

    // 区间 [l, r] 中第 k 小的值 (0-indexed k)
    int kthSmallest(int l, int r, int k) {
        Node* node = root;
        while (node->low != node->high) {
            int zerosL = (l == 0) ? 0 : node->prefixBits[l];
            int zerosR = node->prefixBits[r + 1];
            int zeros = zerosR - zerosL;  // 左子树中的元素个数

            if (k < zeros) {
                // 答案在左子树
                l = zerosL;
                r = zerosR - 1;
                node = node->left;
            } else {
                // 答案在右子树
                k -= zeros;
                l = l - zerosL;
                r = r - zerosR + 1;  // r - zerosR = (r+1 - zerosR) - 1... wait let me fix
                // Actually: r位置前有 zerosR 个0, 所以 r 之前有 (r+1-zerosR) 个1
                // l 之前有 (l - zerosL) 个1
                int onesL = l - zerosL;
                int onesR = (r + 1) - zerosR;
                l = onesL;
                r = onesR - 1;
                node = node->right;
            }
        }
        return node->low;
    }

    void printInfo() {
        cout << "  Wavelet Tree 是简洁数据结构的代表" << endl;
        cout << "  空间 ~ n log σ bits" << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Wavelet Tree — 简洁序列数据结构           " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6};
    WaveletTree wt(arr);

    cout << "[序列] ";
    for (int x : arr) cout << x << " ";
    cout << endl << endl;

    cout << "[区间第 K 小查询]" << endl;
    auto test = [&](int l, int r, int k) {
        cout << "  [" << l << "," << r << "] 第 " << k
             << " 小 = " << wt.kthSmallest(l, r, k) << endl;
    };
    test(0, 7, 0);  // 第 1 小
    test(0, 7, 4);  // 第 5 小
    test(2, 5, 1);  // [4,1,5,9] 中第 2 小

    wt.printInfo();

    cout << endl << "[Wavelet Tree 应用]" << endl;
    cout << "  - 区间第K小: 替代主席树, 更省空间" << endl;
    cout << "  - 区间不同值: rank(val, r) - rank(val, l-1)" << endl;
    cout << "  - 动态逆序对: 支持更灵活的范围查询" << endl;

    return 0;
}
