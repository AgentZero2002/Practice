/**
 * 一元多项式运算
 *
 * 对应教材 3.4 节：列表的应用
 *
 * 使用链表存储稀疏多项式，支持:
 *   - 多项式加法
 *   - 多项式乘法
 *   - 多项式求值
 *
 * 每个节点存储：系数 (coeff) + 指数 (expon)
 * 链表按指数降序排列
 */

#include <iostream>
#include <list>
#include <cmath>
#include <iomanip>
using namespace std;

// 多项式项
struct Term {
    double coeff;
    int expon;

    Term(double c = 0, int e = 0) : coeff(c), expon(e) {}

    bool operator<(const Term& t) const { return expon > t.expon; }  // 降幂
    bool operator==(const Term& t) const { return expon == t.expon; }
};

typedef list<Term> Polynomial;

// ==================== 打印多项式 ====================
void printPoly(const Polynomial& poly, const string& label = "") {
    if (!label.empty()) cout << label;
    if (poly.empty()) { cout << "0" << endl; return; }

    bool first = true;
    for (const auto& t : poly) {
        if (t.coeff == 0) continue;

        if (!first && t.coeff > 0) cout << " + ";
        if (!first && t.coeff < 0) cout << " - ";
        first = false;

        double abs_coeff = fabs(t.coeff);
        if (t.expon == 0) {
            cout << abs_coeff;
        } else {
            if (abs_coeff != 1) cout << abs_coeff;
            cout << "x";
            if (t.expon != 1) cout << "^" << t.expon;
        }
    }
    if (first) cout << "0";  // 全零系数
    cout << endl;
}

// ==================== 多项式加法 ====================
Polynomial addPoly(const Polynomial& A, const Polynomial& B) {
    Polynomial result;
    auto pa = A.begin();
    auto pb = B.begin();

    while (pa != A.end() && pb != B.end()) {
        if (pa->expon == pb->expon) {
            double sum = pa->coeff + pb->coeff;
            if (fabs(sum) > 1e-10)  // 忽略零系数
                result.push_back({sum, pa->expon});
            ++pa; ++pb;
        } else if (pa->expon > pb->expon) {
            result.push_back(*pa);
            ++pa;
        } else {
            result.push_back(*pb);
            ++pb;
        }
    }

    // 追加剩余项
    while (pa != A.end()) { result.push_back(*pa); ++pa; }
    while (pb != B.end()) { result.push_back(*pb); ++pb; }

    return result;
}

// ==================== 多项式乘法 ====================
Polynomial mulPoly(const Polynomial& A, const Polynomial& B) {
    Polynomial result;

    for (const auto& ta : A) {
        Polynomial temp;
        for (const auto& tb : B) {
            double c = ta.coeff * tb.coeff;
            int e = ta.expon + tb.expon;
            temp.push_back({c, e});
        }
        result = addPoly(result, temp);
    }

    return result;
}

// ==================== 多项式求值 ====================
double evalPoly(const Polynomial& poly, double x) {
    double result = 0;
    for (const auto& t : poly)
        result += t.coeff * pow(x, t.expon);
    return result;
}

// ==================== 从系数数组构造多项式 ====================
// coeffs[i] 对应 x^i 的系数
Polynomial makePoly(const vector<double>& coeffs) {
    Polynomial poly;
    for (int i = 0; i < (int)coeffs.size(); i++)
        if (fabs(coeffs[i]) > 1e-10)
            poly.push_back({coeffs[i], i});
    // 降幂排列
    poly.sort([](const Term& a, const Term& b) { return a.expon > b.expon; });
    return poly;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  一元多项式运算                         " << endl;
    cout << "========================================" << endl << endl;

    // P(x) = 3x^4 + 2x^3 - x + 5
    Polynomial P = { {3, 4}, {2, 3}, {-1, 1}, {5, 0} };
    // Q(x) = 2x^3 + x^2 - 3x + 1
    Polynomial Q = { {2, 3}, {1, 2}, {-3, 1}, {1, 0} };

    cout << "P(x) = ";
    printPoly(P);
    cout << "Q(x) = ";
    printPoly(Q);

    // 加法
    Polynomial sum = addPoly(P, Q);
    cout << endl << "P+Q = ";
    printPoly(sum);

    // 乘法
    Polynomial prod = mulPoly(P, Q);
    cout << "P×Q = ";
    printPoly(prod);

    // 求值
    cout << endl << "[求值 x=2]" << endl;
    cout << "  P(2) = " << evalPoly(P, 2) << endl;
    cout << "  Q(2) = " << evalPoly(Q, 2) << endl;
    cout << "  (P+Q)(2) = " << evalPoly(sum, 2) << endl;
    cout << "  (P×Q)(2) = " << evalPoly(prod, 2) << endl;

    // 验证
    cout << endl << "[验证]" << endl;
    cout << "  P(2)+Q(2)  = " << evalPoly(P, 2) + evalPoly(Q, 2)
         << " == (P+Q)(2) = " << evalPoly(sum, 2) << endl;
    cout << "  P(2)×Q(2)  = " << evalPoly(P, 2) * evalPoly(Q, 2)
         << " == (P×Q)(2) = " << evalPoly(prod, 2) << endl;

    return 0;
}
