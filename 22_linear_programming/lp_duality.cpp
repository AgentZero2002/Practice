/**
 * LP 对偶性 (Linear Programming Duality)
 *
 * 每个 LP 问题 (原问题 Primal) 都有一个对偶问题 (Dual):
 *
 * 原问题 (Primal):        对偶问题 (Dual):
 *   max c^T x               min b^T y
 *   s.t. Ax ≤ b             s.t. A^T y ≥ c
 *        x ≥ 0                   y ≥ 0
 *
 * 弱对偶定理:
 *   c^T x ≤ b^T y  对任意可行 x, y 成立
 *   → 原问题值 ≤ 对偶问题值
 *
 * 强对偶定理:
 *   如果原问题有最优解 x*, 则对偶问题也有最优解 y*,
 *   且 c^T x* = b^T y*
 *
 * 互补松弛条件 (Complementary Slackness):
 *   yᵢ · (bᵢ - Aᵢx) = 0  对每条约束
 *   xⱼ · (A^T_yⱼ - cⱼ) = 0  对每个变量
 *
 * LP 对偶在数据结构中的应用:
 *   1. 最大流-最小割定理 = LP 对偶
 *   2. 二分图最大匹配 = 最小点覆盖 (König 定理) = LP 对偶
 *   3. 最短路径的 LP 对偶意义
 */

#include <iostream>
#include <vector>
using namespace std;

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  LP 对偶性 + 最大流-最小割               " << endl;
    cout << "========================================" << endl << endl;

    cout << "[原-对偶关系]" << endl;
    cout << "  Primal (max):     Dual (min):" << endl;
    cout << "  max c^Tx          min b^Ty" << endl;
    cout << "  s.t. Ax ≤ b       s.t. A^Ty ≥ c" << endl;
    cout << "       x ≥ 0              y ≥ 0" << endl;
    cout << endl;

    cout << "[最大流 LP 形式]" << endl;
    cout << "  变量: f_e (每条边的流量)" << endl;
    cout << "  目标: max Σ f_{(s,v)}" << endl;
    cout << "  约束: 0 ≤ f_e ≤ c_e" << endl;
    cout << "        Σ f_in(v) = Σ f_out(v)  (守恒)" << endl;
    cout << endl;

    cout << "[对偶 → 最小割]" << endl;
    cout << "  对偶变量 d_v ∈ {0,1} (到S或T)" << endl;
    cout << "  目标: min Σ c_e · d_e" << endl;
    cout << "  约束: d_s = 1, d_t = 0, d_u - d_v ≤ 1" << endl;
    cout << "  → 这就是最小割问题的 LP 形式!" << endl;
    cout << endl;

    cout << "[最大流-最小割定理]" << endl;
    cout << "  Max-Flow = Min-Cut" << endl;
    cout << "  = 强对偶定理的直接推论!" << endl;
    cout << endl;

    cout << "[更多应用]" << endl;
    cout << "  - 二分图匹配:  König 定理" << endl;
    cout << "  - 最小点覆盖 = 最大匹配" << endl;
    cout << "  - 二人零和博弈: von Neumann 最小最大定理" << endl;

    return 0;
}
