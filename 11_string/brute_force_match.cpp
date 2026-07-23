/**
 * 蛮力 (Brute Force) 模式匹配
 *
 * 对应教材 11.1-11.2 节：串匹配
 *
 * 算法: 从文本串 T 的每个位置开始，逐一比对模式串 P
 * 时间复杂度: O(n*m) 最坏情况
 *   如: T = "aaaaaaaaab", P = "aaaab"
 */

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ==================== BF 版本1 ====================
// 最直观的实现
int bf_match_v1(const string& text, const string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;

    int i = 0, j = 0;
    while (i < n && j < m) {
        if (text[i] == pattern[j]) {
            i++; j++;
        } else {
            i = i - j + 1;  // 回退到下一个位置
            j = 0;
        }
    }
    return (j == m) ? (i - j) : -1;
}

// ==================== BF 版本2 (更清晰) ====================
int bf_match_v2(const string& text, const string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++)
            if (text[i + j] != pattern[j])
                break;
        if (j == m) return i;  // 完全匹配
    }
    return -1;
}

// ==================== 查找所有匹配位置 ====================
vector<int> bf_findAll(const string& text, const string& pattern) {
    vector<int> positions;
    int n = text.length(), m = pattern.length();
    if (m == 0) return positions;

    for (int i = 0; i <= n - m; i++) {
        bool match = true;
        for (int j = 0; j < m; j++) {
            if (text[i + j] != pattern[j]) {
                match = false; break;
            }
        }
        if (match) positions.push_back(i);
    }
    return positions;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  BF 蛮力模式匹配                        " << endl;
    cout << "========================================" << endl << endl;

    string text = "ababcabcacbab";
    string pattern = "abcac";

    cout << "[基本匹配]" << endl;
    cout << "  文本 T: \"" << text << "\"" << endl;
    cout << "  模式 P: \"" << pattern << "\"" << endl;

    int pos1 = bf_match_v1(text, pattern);
    int pos2 = bf_match_v2(text, pattern);
    cout << "  v1 首次匹配位置: " << pos1 << " (T[" << pos1
         << ".." << pos1 + pattern.length() - 1 << "])" << endl;
    cout << "  v2 首次匹配位置: " << pos2 << endl << endl;

    // 查找全部
    cout << "[查找所有匹配] P=\"ab\" in T" << endl;
    auto positions = bf_findAll(text, "ab");
    cout << "  匹配位置: ";
    for (int p : positions) cout << p << " ";
    cout << endl << endl;

    // 最坏情况演示
    cout << "[最坏情况分析]" << endl;
    string worstT(100000, 'a');
    worstT += 'b';
    string worstP = "aaaab";

    auto start = high_resolution_clock::now();
    int result = bf_match_v1(worstT, worstP);
    auto t = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();

    cout << "  T = 100000个'a' + 'b', P = \"aaaab\"" << endl;
    cout << "  匹配位置: " << result << " (应为 99995)" << endl;
    cout << "  耗时: " << t << " ms" << endl;
    cout << "  分析: 每次失配在 P 的最后一位，" << endl;
    cout << "       一步一移，总比较数 ≈ n×m ≈ 5×10^5" << endl;
    cout << endl;
    cout << "[改进方向]" << endl;
    cout << "  KMP:  利用失配信息，O(n+m)" << endl;
    cout << "  BM:   从后往前匹配，亚线性平均性能" << endl;

    return 0;
}
