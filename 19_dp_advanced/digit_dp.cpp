/**
 * 数位 DP (Digit Dynamic Programming)
 *
 * 核心思想:
 *   按十进制位逐位构造数字, 用 DP 统计满足条件的数字个数。
 *   通常从高位到低位递归, 用 "tight" 标志控制上界限制。
 *
 * 状态设计:
 *   dp[pos][state][tight]:
 *     - pos: 当前处理的位 (从高位计数)
 *     - state: 携带的状态 (如: 前一位数字, 是否含某数字)
 *     - tight: 前缀是否与上界完全相同 (0=自由, 1=受限)
 *
 * 经典问题:
 *   1. 统计 [1, N] 中数字 d 出现的次数
 *   2. 不含前导零的数位年龄问题 (windy 数)
 *   3. 数字和 / 数字乘积 相关计数
 *
 * 时间复杂度: O(位数 × 状态数 × 10)
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;

// ==================== 1. 统计 [0, N] 中数字 d 出现的总次数 ====================
long long countDigit(long long N, int d) {
    if (N < 0) return 0;
    string s = to_string(N);
    int len = s.length();

    // dp[pos][cnt][tight][leadingZero]
    // 记忆化搜索
    vector<vector<vector<vector<long long>>>> memo(
        len, vector<vector<vector<long long>>>(
            len + 1, vector<vector<long long>>(
                2, vector<long long>(2, -1))));

    function<long long(int, int, bool, bool)> dfs =
        [&](int pos, int cnt, bool tight, bool leadingZero) -> long long {
        if (pos == len) return cnt;
        long long& res = memo[pos][cnt][tight][leadingZero];
        if (res != -1) return res;
        res = 0;
        int limit = tight ? (s[pos] - '0') : 9;
        for (int digit = 0; digit <= limit; digit++) {
            bool nxtTight = tight && (digit == limit);
            bool nxtZero = leadingZero && (digit == 0);
            int nxtCnt = cnt;
            if (!nxtZero && digit == d) nxtCnt++;
            res += dfs(pos + 1, nxtCnt, nxtTight, nxtZero);
        }
        return res;
    };

    return dfs(0, 0, true, true);
}

// ==================== 2. Windy 数 ====================
// 定义: 不含前导零, 且相邻两个数字之差至少为 2 的正整数
// 统计 [A, B] 中 Windy 数的个数
long long countWindy(long long N) {
    if (N <= 0) return 0;
    string s = to_string(N);
    int len = s.length();

    // dp[pos][prev][tight][started]
    vector<vector<vector<vector<long long>>>> memo(
        len, vector<vector<vector<long long>>>(
            11, vector<vector<long long>>(
                2, vector<long long>(2, -1))));

    function<long long(int, int, bool, bool)> dfs =
        [&](int pos, int prev, bool tight, bool started) -> long long {
        if (pos == len) return 1;  // 成功构造一个数
        long long& res = memo[pos][prev + 1][tight][started];
        if (res != -1) return res;
        res = 0;
        int limit = tight ? (s[pos] - '0') : 9;
        for (int digit = 0; digit <= limit; digit++) {
            if (!started) {
                // 第一个数字 (不能为 0, 但可以先跳过)
                if (digit == 0)
                    res += dfs(pos + 1, -1, tight && (digit == limit), false);
                else
                    res += dfs(pos + 1, digit, tight && (digit == limit), true);
            } else {
                if (abs(digit - prev) >= 2)
                    res += dfs(pos + 1, digit, tight && (digit == limit), true);
            }
        }
        return res;
    };

    return dfs(0, -1, true, false);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  数位 DP (Digit DP)                     " << endl;
    cout << "========================================" << endl << endl;

    // 1. 统计数字出现次数
    cout << "[统计数字出现次数]" << endl;
    long long N = 100;
    cout << "  [1, " << N << "] 中数字 1 出现: "
         << countDigit(N, 1) << " 次" << endl;
    cout << "  [1, " << N << "] 中数字 9 出现: "
         << countDigit(N, 9) << " 次" << endl;
    cout << endl;

    // 2. Windy 数
    cout << "[Windy 数] 相邻数字差 ≥ 2" << endl;
    long long A = 1, B = 100;
    cout << "  [1, 100] 中 Windy 数: "
         << countWindy(B) - countWindy(A - 1) << " 个" << endl;
    cout << "  [1, 200] 中 Windy 数: "
         << countWindy(200) - countWindy(A - 1) << " 个" << endl;
    cout << endl;

    cout << "[数位 DP 模板要点]" << endl;
    cout << "  1. 用 string 分解数字各位" << endl;
    cout << "  2. tight 控制上界: 前缀相同时受限, 否则[0,9]自由选" << endl;
    cout << "  3. leadingZero 处理前导零 (避免误统计 0)" << endl;
    cout << "  4. memo 记忆化: dp[pos][state][tight][lead]" << endl;

    return 0;
}
