/**
 * SG 定理 (Sprague-Grundy Theorem) — 博弈论
 *
 * SG 定理是公平组合游戏 (Impartial Combinatorial Game) 的核心理论。
 *
 * 公平组合游戏两大条件:
 *   1. 双方可用操作相同
 *   2. 没有随机因素, 完全信息
 *
 * SG 函数定义:
 *   SG(x) = mex{ SG(y) | y 是从 x 可达的状态 }
 *   mex = 最小未出现的非负整数
 *
 * 核心定理:
 *   - SG(x)=0 ⟺ 必败态 (P-position)
 *   - SG(x)>0 ⟺ 必胜态 (N-position)
 *   - 多个独立游戏的 SG = SG(G1) ⊕ SG(G2) ⊕ ... ⊕ SG(Gn)
 *
 * 经典游戏:
 *   1. Nim 游戏: n 堆石子，每次取任意颗 → SG = a1⊕a2⊕...⊕an
 *   2. 巴什博奕 (Bash Game): 1 堆，每次取 1~m 个
 *   3. 取石子变形
 */

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

// ==================== SG 函数计算 (通用模板) ====================
// 状态转移由 getMoves 函数定义
vector<int> computeSG(int maxState,
                      function<vector<int>(int)> getMoves) {
    vector<int> sg(maxState + 1, 0);

    for (int state = 1; state <= maxState; state++) {
        set<int> reachable;
        for (int next : getMoves(state))
            reachable.insert(sg[next]);

        // mex
        int mexVal = 0;
        while (reachable.count(mexVal)) mexVal++;
        sg[state] = mexVal;
    }

    return sg;
}

// ==================== Nim 游戏判断 ====================
bool nimIsWinning(const vector<int>& piles) {
    int xorsum = 0;
    for (int p : piles) xorsum ^= p;
    return xorsum != 0;  // 非零 → 先手必胜
}

// ==================== 巴什博奕 (Bash Game) ====================
// n 个石子，每次可取 1~m 个，取完最后一个的获胜
bool bashGameWinning(int n, int m) {
    return n % (m + 1) != 0;
}

// ==================== 自定义游戏 SG 示例 ====================
// 有一堆石子，每次可取 1, 2 或 3 个
vector<int> movesExample(int state) {
    vector<int> result;
    for (int take = 1; take <= 3 && take <= state; take++)
        result.push_back(state - take);
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  SG 定理 — 博弈论                       " << endl;
    cout << "========================================" << endl << endl;

    // 1. Nim
    cout << "[Nim 游戏]" << endl;
    vector<int> piles = {3, 4, 5};
    cout << "  石堆: 3, 4, 5 → XOR=" << (3^4^5)
         << " → " << (nimIsWinning(piles) ? "先手必胜 ✓" : "后手必胜") << endl;
    cout << endl;

    // 2. Bash
    cout << "[巴什博奕]" << endl;
    cout << "  n=10, m=3 → "
         << (bashGameWinning(10, 3) ? "先手必胜" : "后手必胜") << endl;
    cout << endl;

    // 3. SG 表
    cout << "[SG 表] 一堆石子, 可取1/2/3个:" << endl;
    auto sg = computeSG(15, movesExample);
    cout << "  state: ";
    for (int s = 0; s <= 15; s++) cout << setw(2) << s << " ";
    cout << endl << "  SG:    ";
    for (int s = 0; s <= 15; s++) cout << setw(2) << sg[s] << " ";
    cout << endl << "  规律: SG(s) = s mod 4, 周期为 4" << endl;

    cout << endl << "[SG 定理核心]" << endl;
    cout << "  单个游戏: SG=0 必败, SG>0 必胜" << endl;
    cout << "  多个游戏: 总 SG = SG1 ⊕ SG2 ⊕ ..." << endl;
    cout << "  mex: Minimal EXcluded — 最小未出现非负整数" << endl;

    return 0;
}
