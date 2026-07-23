/**
 * 栈的经典应用
 *
 * 对应教材 4.1 节：栈的应用
 *
 * 1. 进制转换  — 利用栈的逆序输出特性
 * 2. 括号匹配  — 利用栈的嵌套匹配特性
 * 3. 表达式求值 — 中缀表达式计算（双栈法）
 * 4. 栈混洗判定 — 判断一个序列是否为合法栈混洗
 */

#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>
using namespace std;

// ==================== 1. 进制转换 ====================
// 将十进制数 n 转换为 base 进制 (base <= 16)
string convertBase(int n, int base) {
    static const char digits[] = "0123456789ABCDEF";
    stack<char> S;
    if (n == 0) return "0";

    while (n > 0) {
        S.push(digits[n % base]);
        n /= base;
    }

    string result;
    while (!S.empty()) {
        result += S.top();
        S.pop();
    }
    return result;
}

// ==================== 2. 括号匹配 ====================
// 检查三种括号 (), [], {} 是否正确匹配和嵌套
// 返回：true = 合法, false = 非法
// 若非法，通过 badPos 报告位置
bool matchBrackets(const string& expr, int& badPos) {
    stack<pair<char, int>> S;  // (字符, 位置)
    for (int i = 0; i < (int)expr.length(); i++) {
        char ch = expr[i];
        if (ch == '(' || ch == '[' || ch == '{') {
            S.push({ch, i});
        } else if (ch == ')' || ch == ']' || ch == '}') {
            if (S.empty()) { badPos = i; return false; }
            char open = S.top().first;
            if ((open == '(' && ch == ')') ||
                (open == '[' && ch == ']') ||
                (open == '{' && ch == '}')) {
                S.pop();
            } else {
                badPos = i; return false;
            }
        }
    }
    if (!S.empty()) { badPos = S.top().second; return false; }
    return true;
}

// ==================== 3. 中缀表达式求值 ====================
// 双栈法：操作数栈 + 运算符栈
// 支持 +, -, *, /, (, ) 以及多位数
class ExpressionEvaluator {
private:
    string expr;
    int pos;

    // 运算符优先级
    int precedence(char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;  // '('
    }

    int applyOp(int a, int b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return a / b;
            default: return 0;
        }
    }

public:
    ExpressionEvaluator(const string& e) : expr(e), pos(0) {}

    int evaluate() {
        stack<int> values;
        stack<char> ops;

        while (pos < (int)expr.length()) {
            char ch = expr[pos];

            if (ch == ' ') {
                pos++; continue;
            }

            // 数字：解析完整多位数
            if (isdigit(ch)) {
                int val = 0;
                while (pos < (int)expr.length() && isdigit(expr[pos])) {
                    val = val * 10 + (expr[pos] - '0');
                    pos++;
                }
                values.push(val);
                continue;
            }

            // 左括号
            if (ch == '(') {
                ops.push(ch);
                pos++;
                continue;
            }

            // 右括号：计算到匹配的左括号
            if (ch == ')') {
                while (!ops.empty() && ops.top() != '(') {
                    int b = values.top(); values.pop();
                    int a = values.top(); values.pop();
                    char op = ops.top(); ops.pop();
                    values.push(applyOp(a, b, op));
                }
                if (!ops.empty()) ops.pop();  // 弹出 '('
                pos++;
                continue;
            }

            // 运算符
            if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
                while (!ops.empty() && precedence(ops.top()) >= precedence(ch)) {
                    int b = values.top(); values.pop();
                    int a = values.top(); values.pop();
                    char op = ops.top(); ops.pop();
                    values.push(applyOp(a, b, op));
                }
                ops.push(ch);
                pos++;
                continue;
            }

            pos++;  // 跳过未知字符
        }

        // 处理剩余运算符
        while (!ops.empty()) {
            int b = values.top(); values.pop();
            int a = values.top(); values.pop();
            char op = ops.top(); ops.pop();
            values.push(applyOp(a, b, op));
        }

        return values.empty() ? 0 : values.top();
    }
};

// ==================== 4. 栈混洗判定 ====================
// 判断序列 B 是否为序列 A 的合法栈混洗
// A = {1, 2, ..., n}, 但此处支持任意 A
bool isStackPermutation(const vector<int>& A, const vector<int>& B) {
    if (A.size() != B.size()) return false;
    stack<int> S;
    int j = 0;
    for (int i = 0; i < (int)A.size(); i++) {
        S.push(A[i]);
        while (!S.empty() && j < (int)B.size() && S.top() == B[j]) {
            S.pop();
            j++;
        }
    }
    return S.empty();  // 栈空 = 合法栈混洗
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  栈的经典应用                           " << endl;
    cout << "========================================" << endl << endl;

    // 1. 进制转换
    cout << "[进制转换]" << endl;
    cout << "  255(10) = " << convertBase(255, 2) << "(2)" << endl;
    cout << "  255(10) = " << convertBase(255, 8) << "(8)" << endl;
    cout << "  255(10) = " << convertBase(255, 16) << "(16)" << endl;
    cout << endl;

    // 2. 括号匹配
    cout << "[括号匹配]" << endl;
    string tests[] = {
        "()", "()[]{}", "(]", "([)]", "{[]}", "((()))",
        "(()", "(()){[(])}"
    };
    for (const string& s : tests) {
        int badPos;
        bool ok = matchBrackets(s, badPos);
        cout << "  \"" << s << "\" → "
             << (ok ? "✓ 合法" : "✗ 非法 (位置 " + to_string(badPos) + ")")
             << endl;
    }
    cout << endl;

    // 3. 表达式求值
    cout << "[表达式求值]" << endl;
    vector<string> exprs = {
        "3+5*2",          // = 13
        "(1+2)*3",        // = 9
        "10+20*3",        // = 70
        "(15-7)*(8+2)/5"  // = 16
    };
    for (const string& s : exprs) {
        ExpressionEvaluator ev(s);
        cout << "  " << s << " = " << ev.evaluate() << endl;
    }
    cout << endl;

    // 4. 栈混洗
    cout << "[栈混洗判定] 入栈顺序 1,2,3,4,5:" << endl;
    vector<int> A = {1, 2, 3, 4, 5};
    vector<pair<vector<int>, bool>> tests2 = {
        {{3, 4, 5, 2, 1}, true},   // 合法
        {{1, 2, 3, 4, 5}, true},   // 合法（直接出栈）
        {{5, 4, 3, 2, 1}, true},   // 合法（先全部入栈）
        {{3, 1, 2, 4, 5}, false},  // 非法：1不能在2之前出栈
        {{4, 3, 5, 1, 2}, false},  // 非法
    };
    for (auto& [seq, expected] : tests2) {
        bool result = isStackPermutation(A, seq);
        cout << "  ";
        for (int x : seq) cout << x << " ";
        cout << "→ " << (result ? "合法" : "非法")
             << " (期望: " << (expected ? "合法" : "非法") << ")"
             << (result == expected ? " ✓" : " ✗") << endl;
    }

    return 0;
}
