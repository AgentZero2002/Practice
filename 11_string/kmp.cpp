/**
 * KMP (Knuth-Morris-Pratt) 字符串匹配算法
 *
 * 对应教材 11.3 节：KMP 算法
 *
 * 核心思想: 利用已匹配的部分信息，避免文本指针回退。
 *
 * next 表 (next[]):
 *   next[j] = 在 P[0..j) 中，使得真前缀 = 真后缀的最大长度
 *   即当 P[j] 与文本失配时，将模式右移到 next[j] 继续匹配
 *
 * 时间复杂度: O(n+m)
 *   - 匹配过程: O(n) (i 只增不减)
 *   - 建表过程: O(m) (与匹配同理)
 *
 * 改进 next 表 (nextval[]):
 *   避免 P[j] == P[next[j]] 时的冗余比较
 */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ==================== 构建 next 表 ====================
// next[j] = P[0..j) 的最长公共前后缀长度
vector<int> buildNext(const string& pattern) {
    int m = pattern.length();
    vector<int> next(m, 0);

    int j = 0;    // 主串指针（正在填入 next 的位置）
    int t = -1;   // 模式指针（P[0..t] = P[j-t..j]）
    next[0] = -1; // 哨兵

    while (j < m - 1) {
        if (t < 0 || pattern[j] == pattern[t]) {
            j++; t++;
            next[j] = t;  // t = next[j]的最长公共前后缀长度
        } else {
            t = next[t];  // 关键：类比 KMP 自身匹配过程
        }
    }

    return next;
}

// ==================== 构建改进 next 表 (nextval) ====================
// 当 P[j] == P[next[j]] 时，nextval[j] = nextval[next[j]]
// 避免逐次比对同一个不匹配字符
vector<int> buildNextVal(const string& pattern) {
    int m = pattern.length();
    vector<int> nextval(m, 0);

    int j = 0, t = -1;
    nextval[0] = -1;

    while (j < m - 1) {
        if (t < 0 || pattern[j] == pattern[t]) {
            j++; t++;
            // 改进：若后继字符相同，则继承 nextval
            if (pattern[j] != pattern[t])
                nextval[j] = t;
            else
                nextval[j] = nextval[t];
        } else {
            t = nextval[t];
        }
    }

    return nextval;
}

// ==================== KMP 匹配 ====================
int kmpMatch(const string& text, const string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;
    if (n < m) return -1;

    vector<int> next = buildNext(pattern);

    int i = 0, j = 0;  // i: text指针, j: pattern指针
    while (i < n && j < m) {
        if (j < 0 || text[i] == pattern[j]) {
            i++; j++;
        } else {
            j = next[j];  // 关键：i不回退，仅调整j
        }
    }

    return (j == m) ? (i - j) : -1;
}

// ==================== 打印 next 表 ====================
void printNextTable(const string& pattern, const vector<int>& next,
                    const string& label) {
    cout << "  " << label << " 表:" << endl;
    cout << "  j  : ";
    for (int i = 0; i < (int)pattern.length(); i++)
        cout << setw(3) << i;
    cout << endl;
    cout << "  P  : ";
    for (char c : pattern) cout << setw(3) << c;
    cout << endl;
    cout << "  val: ";
    for (int x : next) cout << setw(3) << x;
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  KMP 字符串匹配算法                      " << endl;
    cout << "========================================" << endl << endl;

    // 示例 1
    string P1 = "abcabc";
    cout << "[next 表构造] P = \"" << P1 << "\"" << endl;

    vector<int> nxt = buildNext(P1);
    vector<int> nxtval = buildNextVal(P1);

    printNextTable(P1, nxt, "next");
    cout << endl;
    printNextTable(P1, nxtval, "nextval");
    cout << endl;

    // 解释 next 值的含义
    cout << "[next 表手动计算]" << endl;
    cout << "  next[j] = P[0..j) 中最大匹配的真前缀与真后缀长度" << endl;
    cout << "  next[0] = -1 (哨兵)" << endl;
    cout << "  next[1] = 0  (P[0..0]=\"a\", 无真前缀=真后缀)" << endl;
    cout << "  next[5] = 2  (P[0..4]=\"abcab\", \"ab\"=后缀\"ab\")" << endl << endl;

    // 匹配演示
    cout << "[KMP 匹配]" << endl;
    string T = "ababcabcacbab";
    string P = "abcac";
    cout << "  T = \"" << T << "\"" << endl;
    cout << "  P = \"" << P << "\"" << endl;

    vector<int> nextP = buildNext(P);
    cout << "  next 表: ";
    for (int x : nextP) cout << x << " ";
    cout << endl;

    int pos = kmpMatch(T, P);
    cout << "  匹配位置: " << pos << endl << endl;

    // 对比 BF
    cout << "[KMP vs BF]" << endl;
    cout << "  BF:  O(n×m) 最坏, 文本指针回退" << endl;
    cout << "  KMP: O(n+m), 文本指针永不回退" << endl;
    cout << "  关键: next 表的构造也是 KMP (模式自我匹配)" << endl;

    return 0;
}
