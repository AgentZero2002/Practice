/**
 * Boyer-Moore (BM) 字符串匹配算法
 *
 * 对应教材 11.4 节：BM 算法
 *
 * BM 算法的核心创新:
 *   1. 从模式串的末尾向前比对 (逆向匹配)
 *   2. 利用坏字符 (BC) 和好后缀 (GS) 策略计算最大安全位移
 *
 * BC (Bad Character): 失配字符在模式中最靠右的出现位置
 * GS (Good Suffix): 已匹配后缀在模式中的另一次出现
 *
 * 在一般情况下 BM 是最快的串匹配算法之一，
 * 尤其是对于大字母表（如 ASCII/Unicode），具有良好的亚线性平均性能。
 *
 * 本实现重点展示 BC 策略。
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

const int ALPH_SIZE = 256;

// ==================== 构建 BC (坏字符) 表 ====================
// bc[c] = 字符 c 在模式串中最靠右的出现位置
//    若 c 不在模式中，则为 -1
vector<int> buildBC(const string& pattern) {
    vector<int> bc(ALPH_SIZE, -1);
    for (int i = 0; i < (int)pattern.length(); i++)
        bc[(unsigned char)pattern[i]] = i;
    return bc;
}

// ==================== 构建 GS (好后缀) 表 ====================
// gs[j] = 当 P[j] 失配时，模式串应右移的距离
// 与 KMP 的 next 表类似，但计算方向相反
vector<int> buildGS(const string& pattern) {
    int m = pattern.length();
    vector<int> gs(m, m);  // 默认右移 m 位

    // suffix[i] = 与 P[0..i] 匹配的最长后缀长度
    vector<int> suffix(m, 0);

    // 计算 suffix 数组
    for (int i = m - 2; i >= 0; i--) {
        int k = i;
        while (k >= 0 && pattern[k] == pattern[m - 1 - (i - k)])
            k--;
        suffix[i] = i - k;
    }

    // 情况1: 好后缀在模式某处再次出现
    for (int i = 0; i < m - 1; i++) {
        int j = m - 1 - suffix[i];
        if (j < m) gs[j] = m - 1 - i;
    }

    // 情况2: 好后缀的后缀是模式的前缀
    for (int i = m - 2; i >= 0; i--) {
        if (suffix[i] == i + 1) {  // 整个前缀 P[0..i] = 后缀
            for (int j = 0; j < m - 1 - i; j++)
                gs[j] = min(gs[j], m - 1 - i);
        }
    }

    return gs;
}

// ==================== BM 匹配 (BC + GS) ====================
int bmMatch(const string& text, const string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;
    if (n < m) return -1;

    vector<int> bc = buildBC(pattern);
    vector<int> gs = buildGS(pattern);

    int i = 0;  // 模式串在文本中的起始位置
    while (i <= n - m) {
        int j = m - 1;  // 从模式末尾开始比对

        while (j >= 0 && pattern[j] == text[i + j])
            j--;

        if (j < 0) return i;  // 完全匹配

        // 计算安全位移 = max(BC位移, GS位移)
        int bcShift = j - bc[(unsigned char)text[i + j]];
        int gsShift = gs[j];
        i += max(1, max(bcShift, gsShift));
    }

    return -1;
}

// ==================== BM 匹配 (仅 BC) ====================
int bmMatch_BConly(const string& text, const string& pattern) {
    int n = text.length(), m = pattern.length();
    if (m == 0) return 0;
    if (n < m) return -1;

    vector<int> bc = buildBC(pattern);
    int i = 0;

    while (i <= n - m) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[i + j])
            j--;
        if (j < 0) return i;

        // 仅用 BC 位移
        i += max(1, j - bc[(unsigned char)text[i + j]]);
    }

    return -1;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Boyer-Moore 字符串匹配算法             " << endl;
    cout << "========================================" << endl << endl;

    // BC 表示例
    string P = "EXAMPLE";
    cout << "[BC 坏字符表] P = \"" << P << "\"" << endl;
    vector<int> bc = buildBC(P);
    cout << "  字符: ";
    for (char c : P) cout << setw(3) << c;
    cout << endl;
    cout << "  BC表: ";
    for (char c : P) cout << setw(3) << bc[(unsigned char)c];
    cout << endl << endl;

    // GS 表示例
    cout << "[GS 好后缀表]" << endl;
    vector<int> gs = buildGS(P);
    cout << "  位置 j: ";
    for (int i = 0; i < (int)P.length(); i++)
        cout << setw(3) << i;
    cout << endl;
    cout << "  GS[j]:  ";
    for (int x : gs) cout << setw(3) << x;
    cout << endl << endl;

    // 匹配测试
    cout << "[BM 匹配测试]" << endl;
    string T = "HERE IS A SIMPLE EXAMPLE";
    cout << "  T = \"" << T << "\"" << endl;
    cout << "  P = \"" << P << "\"" << endl;

    int pos1 = bmMatch_BConly(T, P);
    int pos2 = bmMatch(T, P);
    cout << "  BM (仅BC): " << pos1 << endl;
    cout << "  BM (BC+GS): " << pos2 << endl << endl;

    // 算法特性
    cout << "[BM 算法特性]" << endl;
    cout << "  1. 逆向匹配：从模式末尾向前比对" << endl;
    cout << "  2. BC 策略：利用不匹配字符在模式中的位置跳跃" << endl;
    cout << "  3. GS 策略：类似 KMP 但方向相反" << endl;
    cout << "  4. 平均性能：亚线性 O(n/m)，大字母表时极快" << endl;
    cout << "  5. 最坏性能：O(n×m)（但实际中很少出现）" << endl;

    return 0;
}
