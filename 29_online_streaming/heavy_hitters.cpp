/**
 * Heavy Hitters — Misra-Gries & Count-Sketch 算法
 *
 * 在流式数据中找出频率最高的 k 个元素 (Heavy Hitters),
 * 是流式算法的经典问题。
 *
 * Misra-Gries (1982):
 *   维护 k-1 个 (元素, 计数) 对。
 *   新元素:
 *     - 如果已在计数器中 → 计数器+1
 *     - 如果计数器 < k-1 → 新增计数器
 *     - 否则所有计数器-1, 丢弃变为0的
 *
 *   结论: 任何频率 > N/k 的元素必然被保留。
 *   空间: O(k) 个计数器。
 *
 * Count-Sketch (Charikar et al. 2002):
 *   类似 Count-Min Sketch 但加上了符号:
 *     h_j(x): 映射到 [0, w-1]
 *     s_j(x): 随机符号 ±1
 *     sketch[h_j(x)] += s_j(x) × c
 *
 *   查询: median{ sketch[j][h_j(x)] × s_j(x) }
 *   与 Count-Min 的区别: 含符号 → 无偏估计
 *
 * Counter-based vs Sketch-based:
 *   MG/Lossy Counting: 基于计数器, 确定性, 小空间
 *   Count-Min/Count-Sketch: 基于哈希, 概率性, 可并行
 */

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <random>
using namespace std;

// ==================== Misra-Gries ====================
vector<pair<string,int>> misraGries(const vector<string>& stream, int k) {
    map<string, int> counters;

    for (const string& item : stream) {
        if (counters.count(item)) {
            counters[item]++;
        } else if ((int)counters.size() < k - 1) {
            counters[item] = 1;
        } else {
            // 所有计数器减 1, 移除变为 0 的
            auto it = counters.begin();
            while (it != counters.end()) {
                it->second--;
                if (it->second == 0)
                    it = counters.erase(it);
                else
                    ++it;
            }
        }
    }

    vector<pair<string,int>> result(counters.begin(), counters.end());
    sort(result.begin(), result.end(),
         [](auto& a, auto& b) { return a.second > b.second; });
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Heavy Hitters — Misra-Gries            " << endl;
    cout << "========================================" << endl << endl;

    // 模拟流数据: "A" 出现 10 次, "B" 8 次, 其他少
    vector<string> stream;
    for (int i = 0; i < 10; i++) stream.push_back("A");
    for (int i = 0; i < 8; i++) stream.push_back("B");
    for (int i = 0; i < 3; i++) stream.push_back("C");
    for (int i = 0; i < 2; i++) stream.push_back("D");
    stream.push_back("E");

    int k = 3;  // Top-2 heavy hitters
    auto result = misraGries(stream, k);

    cout << "[流数据] A×10, B×8, C×3, D×2, E×1" << endl;
    cout << "[Misra-Gries k=" << k << "] 找到的 Heavy Hitters:" << endl;
    for (auto& [key, cnt] : result)
        cout << "  " << key << ": 估计~" << cnt << " 次" << endl;

    cout << endl << "[理论保证]" << endl;
    cout << "  任何频率 > N/k 的元素必被保留" << endl;
    cout << "  空间: O(k) 计数器, 与 N 无关!" << endl;

    return 0;
}
