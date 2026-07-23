/**
 * 滑动窗口 + 字符串 — 面试必考模式
 *
 * 滑动窗口维护一个可变大小的窗口，通过左右边界移动
 * 来查找满足条件的子串。
 *
 * 经典题:
 *   1. 无重复字符的最长子串           (LeetCode 3)
 *   2. 最小覆盖子串                  (LeetCode 76) ★Hard
 *   3. 找到字符串中所有字母异位词     (LeetCode 438)
 *
 * 滑动窗口模板:
 *   右边界扩展窗口直到满足条件
 *   左边界收缩窗口直到不满足条件
 *   记录最优解
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;

// ==================== 1. 无重复字符的最长子串 ====================
int lengthOfLongestSubstring(const string& s) {
    unordered_set<char> window;
    int left = 0, maxLen = 0;

    for (int right = 0; right < (int)s.length(); right++) {
        // 收缩: 直到窗口内没有重复
        while (window.count(s[right])) {
            window.erase(s[left]);
            left++;
        }
        window.insert(s[right]);
        maxLen = max(maxLen, right - left + 1);
    }
    return maxLen;
}

// ==================== 2. 最小覆盖子串 ★ ====================
string minWindow(const string& s, const string& t) {
    unordered_map<char, int> need, window;
    for (char c : t) need[c]++;

    int left = 0, matched = 0;
    int minStart = 0, minLen = INT_MAX;

    for (int right = 0; right < (int)s.length(); right++) {
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) matched++;
        }

        // 窗口已包含所有需要的字符 → 尝试收缩左边界
        while (matched == (int)need.size()) {
            if (right - left + 1 < minLen) {
                minStart = left;
                minLen = right - left + 1;
            }

            char d = s[left];
            if (need.count(d)) {
                if (window[d] == need[d]) matched--;
                window[d]--;
            }
            left++;
        }
    }

    return minLen == INT_MAX ? "" : s.substr(minStart, minLen);
}

// ==================== 3. 字母异位词找到所有位置 ====================
vector<int> findAnagrams(const string& s, const string& p) {
    vector<int> result;
    unordered_map<char, int> need, window;
    for (char c : p) need[c]++;

    int left = 0, matched = 0;
    for (int right = 0; right < (int)s.length(); right++) {
        char c = s[right];
        if (need.count(c)) {
            window[c]++;
            if (window[c] == need[c]) matched++;
        }

        // 窗口大小固定为 p.length()
        if (right - left + 1 == (int)p.length()) {
            if (matched == (int)need.size())
                result.push_back(left);

            char d = s[left];
            if (need.count(d)) {
                if (window[d] == need[d]) matched--;
                window[d]--;
            }
            left++;
        }
    }
    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  滑动窗口 + 字符串                      " << endl;
    cout << "========================================" << endl << endl;

    // 1. 最长无重复
    string s1 = "abcabcbb";
    cout << "[最长无重复子串]" << endl;
    cout << "  s=\"" << s1 << "\" → "
         << lengthOfLongestSubstring(s1) << " (期望: 3, \"abc\")" << endl << endl;

    // 2. 最小覆盖子串
    string s2 = "ADOBECODEBANC", t2 = "ABC";
    cout << "[最小覆盖子串]" << endl;
    cout << "  s=\"" << s2 << "\", t=\"" << t2 << "\"" << endl;
    cout << "  → \"" << minWindow(s2, t2) << "\" (期望: \"BANC\")" << endl << endl;

    // 3. 字母异位词
    string s3 = "cbaebabacd", p3 = "abc";
    cout << "[字母异位词]" << endl;
    cout << "  s=\"" << s3 << "\", p=\"" << p3 << "\"" << endl;
    auto ans = findAnagrams(s3, p3);
    cout << "  起始位置: ";
    for (int x : ans) cout << x << " ";
    cout << "(期望: 0, 6)" << endl;

    cout << endl << "[滑动窗口模板口诀]" << endl;
    cout << "  右扩: 加入新字符, 更新状态" << endl;
    cout << "  左缩: 窗口条件不满足时收缩" << endl;
    cout << "  记录: 在收缩循环后记录最优解" << endl;

    return 0;
}
