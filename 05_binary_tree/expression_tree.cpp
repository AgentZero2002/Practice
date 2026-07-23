/**
 * 表达式树
 *
 * 对应教材 5.5 节：二叉树的应用
 *
 * 表达式树是一棵二叉树，其中:
 *   - 叶节点存储操作数 (operand)
 *   - 内部节点存储运算符 (operator)
 *   - 后序遍历 = 后缀表达式 (RPN)
 *   - 中序遍历 = 中缀表达式（需加括号体现优先级）
 *
 * 本程序从后缀表达式（RPN）构建表达式树并求值。
 */

#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <sstream>
#include <vector>
using namespace std;

// ==================== 表达式树节点 ====================
struct ExprNode {
    string val;     // 操作数或运算符
    ExprNode* left;
    ExprNode* right;

    ExprNode(const string& v) : val(v), left(nullptr), right(nullptr) {}

    bool isOperator() const {
        return val == "+" || val == "-" || val == "*" || val == "/";
    }

    bool isOperand() const { return !isOperator(); }
};

// ==================== 后缀表达式 → 表达式树 ====================
// 扫描后缀表达式，操作数压入栈，运算符从栈中弹出两个操作数作为孩子
ExprNode* buildExprTree(const vector<string>& postfix) {
    stack<ExprNode*> S;

    for (const string& token : postfix) {
        ExprNode* node = new ExprNode(token);

        if (node->isOperator()) {
            // 注意弹出顺序：先右后左
            if (S.size() < 2) {
                cerr << "无效的后缀表达式" << endl;
                delete node;
                return nullptr;
            }
            node->right = S.top(); S.pop();
            node->left = S.top(); S.pop();
        }

        S.push(node);
    }

    if (S.size() != 1) {
        cerr << "无效的后缀表达式（多余操作数）" << endl;
        return nullptr;
    }

    return S.top();
}

// ==================== 求值表达式树 ====================
double evaluate(ExprNode* root) {
    if (!root) return 0;

    if (root->isOperand())
        return stod(root->val);

    double leftVal = evaluate(root->left);
    double rightVal = evaluate(root->right);

    if (root->val == "+") return leftVal + rightVal;
    if (root->val == "-") return leftVal - rightVal;
    if (root->val == "*") return leftVal * rightVal;
    if (root->val == "/") return leftVal / rightVal;

    return 0;
}

// ==================== 中序遍历 → 中缀表达式（带括号） ====================
void toInfix(ExprNode* root, int precedence, string& result) {
    if (!root) return;

    bool needParen = false;
    int myPrec = 0;

    if (root->isOperator()) {
        if (root->val == "+" || root->val == "-") myPrec = 1;
        else if (root->val == "*" || root->val == "/") myPrec = 2;

        needParen = (myPrec < precedence);
        if (needParen) result += "(";
    }

    if (root->left) toInfix(root->left, max(myPrec, root->left->isOperator() ? myPrec : 0), result);
    result += root->val;
    if (root->right) toInfix(root->right, max(myPrec, root->right->isOperator() ? myPrec : 0), result);

    if (needParen) result += ")";
}

// 后序遍历 → 后缀表达式
void toPostfix(ExprNode* root, string& result) {
    if (!root) return;
    toPostfix(root->left, result);
    toPostfix(root->right, result);
    if (!result.empty()) result += " ";
    result += root->val;
}

// ==================== 中缀 → 后缀 ====================
// 经典调度场算法 (Shunting-yard Algorithm)
vector<string> infixToPostfix(const string& expr) {
    vector<string> output;
    stack<string> ops;
    istringstream iss(expr);
    string token;

    auto precedence = [](const string& op) -> int {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        return 0;
    };

    while (iss >> token) {
        if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
            output.push_back(token);
        } else if (token == "(") {
            ops.push(token);
        } else if (token == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top()); ops.pop();
            }
            ops.pop();  // 丢弃 '('
        } else {  // 运算符
            while (!ops.empty() && precedence(ops.top()) >= precedence(token)) {
                output.push_back(ops.top()); ops.pop();
            }
            ops.push(token);
        }
    }

    while (!ops.empty()) {
        output.push_back(ops.top()); ops.pop();
    }

    return output;
}

// ==================== 清理 ====================
void deleteExprTree(ExprNode* root) {
    if (!root) return;
    deleteExprTree(root->left);
    deleteExprTree(root->right);
    delete root;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  表达式树 (Expression Tree)            " << endl;
    cout << "========================================" << endl << endl;

    // 后缀表达式: 3 4 + 5 2 - *
    // 等价中缀: (3 + 4) * (5 - 2) = 21
    vector<string> postfix = {"3", "4", "+", "5", "2", "-", "*"};
    cout << "[后缀表达式] ";
    for (const string& s : postfix) cout << s << " ";
    cout << endl;

    ExprNode* root = buildExprTree(postfix);
    if (!root) return 1;

    // 求值
    cout << "  求值: " << evaluate(root) << endl;

    // 还原中缀
    string infixResult;
    toInfix(root, 0, infixResult);
    cout << "  中缀: " << infixResult << endl;

    // 还原后缀
    string postfixResult;
    toPostfix(root, postfixResult);
    cout << "  后缀: " << postfixResult << endl;

    deleteExprTree(root);

    // 测试2：中缀 → 后缀变换
    cout << endl << "[中缀→后缀 变换]" << endl;
    vector<pair<string, double>> tests = {
        {"3 + 4 * 5", 23},
        {"( 1 + 2 ) * 3", 9},
        {"15 - 7 + 3", 11}
    };

    for (auto& [expr, expected] : tests) {
        cout << "  中缀: " << expr << endl;
        vector<string> pf = infixToPostfix(expr);
        cout << "  后缀: ";
        for (const string& s : pf) cout << s << " ";
        ExprNode* r = buildExprTree(pf);
        cout << "→ 求值: " << evaluate(r) << " (期望: " << expected << ")" << endl;
        deleteExprTree(r);
    }

    return 0;
}
