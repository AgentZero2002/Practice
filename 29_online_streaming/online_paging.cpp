/**
 * 在线算法 — Page Replacement (在线分页 / 缓存替换)
 *
 * 在线算法 (Online Algorithm): 输入逐个到达, 必须在看到未来输入
 * 之前做出决策。竞争分析 (Competitive Analysis) 是评估在线算法
 * 的核心工具。
 *
 * Paging Problem:
 *   缓存容量为 k, 请求页面序列 p₁, p₂, ..., pₙ:
 *     - 命中 (Hit): 页面已在缓存中, 无代价
 *     - 缺失 (Miss): 需从磁盘加载, 代价=1, 需驱逐一个缓存页面
 *
 * 离线最优: Belady's MIN (驱逐未来最远使用的页面)
 *   需要知道完整请求序列 → 不可在在线场景使用
 *
 * 在线策略:
 *   1. LRU (Least Recently Used): 驱逐最久未使用的
 *   2. FIFO (First In First Out): 驱逐最早加载的
 *   3. LFU (Least Frequently Used): 驱逐使用次数最少的
 *   4. Clock (Second Chance): LRU 的近似, Linux 页替换
 *
 * 竞争比 (Competitive Ratio):
 *   CR(ALG) = max{ ALG(σ) / OPT(σ) } 对所有请求序列 σ
 *   - LRU: k-competitive
 *   - FIFO: k-competitive
 *   - 任何确定性在线算法 ≥ k-competitive (下界)
 *   - 随机化可以做到 O(log k)-competitive (Marking Algorithm)
 */

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
using namespace std;

// LRU 缓存模拟
int lruPaging(const vector<int>& requests, int capacity) {
    list<int> cache;  // 前端=最近使用
    unordered_map<int, list<int>::iterator> pos;
    int misses = 0;

    for (int page : requests) {
        if (pos.count(page)) {
            // 命中: 移到前端
            cache.erase(pos[page]);
            cache.push_front(page);
            pos[page] = cache.begin();
        } else {
            misses++;
            if ((int)cache.size() >= capacity) {
                // 驱逐最后 (最久未使用)
                int evicted = cache.back();
                cache.pop_back();
                pos.erase(evicted);
            }
            cache.push_front(page);
            pos[page] = cache.begin();
        }
    }
    return misses;
}

// Belady's MIN (离线最优) — 简化实现
int optimalPaging(const vector<int>& requests, int capacity) {
    int n = requests.size();
    int misses = 0;
    vector<int> cache;

    for (int i = 0; i < n; i++) {
        if (find(cache.begin(), cache.end(), requests[i]) != cache.end())
            continue;  // 命中
        misses++;
        if ((int)cache.size() < capacity) {
            cache.push_back(requests[i]);
        } else {
            // 驱逐未来最远使用的
            int farthest = -1, toEvict = -1;
            for (int j = 0; j < (int)cache.size(); j++) {
                int nextUse = n + 1;
                for (int k = i + 1; k < n; k++)
                    if (requests[k] == cache[j]) { nextUse = k; break; }
                if (nextUse > farthest) {
                    farthest = nextUse;
                    toEvict = j;
                }
            }
            cache[toEvict] = requests[i];
        }
    }
    return misses;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  在线算法 — Page Replacement              " << endl;
    cout << "========================================" << endl << endl;

    vector<int> pages = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    int cap = 3;

    cout << "[页面请求] cap=" << cap << endl;
    for (int p : pages) cout << "  " << p;
    cout << endl;

    int lruMiss = lruPaging(pages, cap);
    int optMiss = optimalPaging(pages, cap);
    cout << "  LRU Miss: " << lruMiss << endl;
    cout << "  OPT Miss: " << optMiss << " (离线最优)" << endl;
    cout << "  竞争比 ≈ " << (double)lruMiss / optMiss << endl;

    cout << endl << "[在线算法竞争比]" << endl;
    cout << "  确定性下界: k-competitive" << endl;
    cout << "  随机化: O(log k)-competitive" << endl;

    return 0;
}
