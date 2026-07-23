/**
 * 莫队算法 (Mo's Algorithm)
 *
 * 莫队是处理离线区间查询的经典算法。
 *
 * 核心思想:
 *   1. 将所有查询按分块排序:
 *      先按左端点的块号排序，块内按右端点排序
 *   2. 维护当前区间 [L, R]，通过扩展/收缩 L 和 R 来回答每个查询
 *   3. 总移动次数: O((n+q) × sqrt(n))
 *
 * 时间复杂度: O(n√n) (或 O(n√q) 取决于实现)
 *
 * 适用条件: 离线查询 (所有查询预先已知)
 *          add/remove 操作是 O(1)
 *
 * 经典应用: 区间 distinct 值计数, 区间众数
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

// 查询结构
struct Query {
    int l, r, idx;
};

class MosAlgorithm {
private:
    int n;
    vector<int> arr;
    vector<int> freq;
    int uniqueCount;
    int blockSize;

    void add(int pos) {
        if (freq[arr[pos]] == 0) uniqueCount++;
        freq[arr[pos]]++;
    }

    void remove(int pos) {
        freq[arr[pos]]--;
        if (freq[arr[pos]] == 0) uniqueCount--;
    }

public:
    MosAlgorithm(const vector<int>& a, int maxVal)
        : n(a.size()), arr(a), freq(maxVal + 1, 0), uniqueCount(0) {
        blockSize = max(1, (int)sqrt(n));
    }

    // 回答所有离线查询
    vector<int> answer(vector<Query>& queries) {
        int q = queries.size();

        // 按块排序
        sort(queries.begin(), queries.end(), [this](const Query& a, const Query& b) {
            int blockA = a.l / blockSize;
            int blockB = b.l / blockSize;
            if (blockA != blockB) return blockA < blockB;
            // 奇数块 r 降序, 偶数块 r 升序 (优化)
            return (blockA % 2) ? (a.r > b.r) : (a.r < b.r);
        });

        vector<int> results(q);
        int curL = 0, curR = -1;

        for (const Query& qry : queries) {
            // 扩展/收缩到 [qry.l, qry.r]
            while (curL > qry.l) add(--curL);
            while (curR < qry.r) add(++curR);
            while (curL < qry.l) remove(curL++);
            while (curR > qry.r) remove(curR--);

            results[qry.idx] = uniqueCount;
        }

        return results;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  莫队算法 (Mo's Algorithm)              " << endl;
    cout << "  — 离线区间 distinct 值计数             " << endl;
    cout << "========================================" << endl << endl;

    vector<int> arr = {1, 1, 2, 1, 3, 4, 5, 2, 8};
    cout << "[数组] 1 1 2 1 3 4 5 2 8" << endl;

    vector<Query> queries = {
        {0, 3, 0},   // [1,1,2,1] → 2 个不同的值
        {2, 7, 1},   // [2,1,3,4,5,2] → 5 个
        {0, 8, 2},   // 全部 → 6 个
    };

    MosAlgorithm mo(arr, 10);
    auto results = mo.answer(queries);

    for (int i = 0; i < (int)queries.size(); i++) {
        cout << "  distinct[" << queries[i].l << ".." << queries[i].r
             << "] = " << results[i];
        // 手动验证
        unordered_set<int> s;
        for (int j = queries[i].l; j <= queries[i].r; j++) s.insert(arr[j]);
        cout << " (暴力验证: " << s.size() << ")" << endl;
    }

    cout << endl << "[莫队精髓]" << endl;
    cout << "  排序: 左端点分块 + 右端点奇偶优化" << endl;
    cout << "  指针移动: O(n√n) 总移动次数" << endl;
    cout << "  关键: add/remove 必须 O(1)" << endl;

    return 0;
}
