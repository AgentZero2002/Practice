/**
 * 汉诺塔非递归解法
 *
 * 对应教材 4.3.2 节：栈与递归
 *
 * 递归解法虽然简洁，但递归调用本质上是系统栈的隐式使用。
 * 非递归版本显式使用栈，模拟递归调用过程。
 *
 * 算法: 用栈模拟递归 + 迭代解法（交替法）
 */

#include <iostream>
#include <stack>
#include <cmath>
using namespace std;

// ==================== 方法一：用栈显式模拟递归 ====================
// 将每个递归调用的参数和状态压入栈中
struct HanoiFrame {
    int n;
    char src, aux, dst;
    int step;  // 0 = 待处理, 1 = 已处理递归, 2 = 可输出
};

void hanoi_stack(int n, char src, char aux, char dst) {
    stack<HanoiFrame> S;
    S.push({n, src, aux, dst, 0});
    int move = 0;

    while (!S.empty()) {
        HanoiFrame f = S.top(); S.pop();

        if (f.n == 0) continue;

        if (f.step == 2) {
            // 直接移动（n=1 或 已完成递归的中间步骤）
            cout << "  Step " << ++move << ": 盘 " << f.n
                 << " " << f.src << " → " << f.dst << endl;
        } else {
            // 按逆序入栈（模拟递归的三步）
            // 3. 将 n-1 从 aux 移到 dst
            S.push({f.n - 1, f.aux, f.src, f.dst, 0});
            // 2. 移动第 n 个盘
            S.push({f.n, f.src, f.aux, f.dst, 2});
            // 1. 将 n-1 从 src 移到 aux
            S.push({f.n - 1, f.src, f.dst, f.aux, 0});
        }
    }
    cout << "  共 " << move << " 步" << endl;
}

// ==================== 方法二：交替法（仅适用于3柱汉诺塔） ====================
// 算法思想：
//   对偶数 n：在 A↔B（不含 C）之间交替移动最小盘和合法操作
//   对奇数 n：在 A↔C（不含 B）之间交替移动最小盘和合法操作
// 时间复杂度 O(2^n)，空间 O(1)
void hanoi_iterative(int n) {
    // 用三个栈模拟塔柱
    stack<int> pegs[3];
    // 从大到小压入源柱
    for (int i = n; i >= 1; i--)
        pegs[0].push(i);

    // 确定移动方向
    int src = 0, dst = 2, aux = 1;  // A=0, B=1, C=2
    if (n % 2 == 0) swap(dst, aux);

    int totalMoves = (1 << n) - 1;
    for (int i = 1; i <= totalMoves; i++) {
        if (i % 3 == 1) {
            // 在 src 和 dst 之间移动最小盘
            int from = pegs[src].empty() ? dst :
                       (pegs[dst].empty() ? src :
                        (pegs[src].top() < pegs[dst].top() ? src : dst));
            int to = (from == src) ? dst : src;
            int disk = pegs[from].top(); pegs[from].pop();
            pegs[to].push(disk);
            cout << "  Step " << i << ": 盘 " << disk
                 << " " << char('A' + from) << " → " << char('A' + to) << endl;
        } else if (i % 3 == 2) {
            // 在 src 和 aux 之间移动
            int from = pegs[src].empty() ? aux :
                       (pegs[aux].empty() ? src :
                        (pegs[src].top() < pegs[aux].top() ? src : aux));
            int to = (from == src) ? aux : src;
            int disk = pegs[from].top(); pegs[from].pop();
            pegs[to].push(disk);
            cout << "  Step " << i << ": 盘 " << disk
                 << " " << char('A' + from) << " → " << char('A' + to) << endl;
        } else {
            // 在 aux 和 dst 之间移动
            int from = pegs[aux].empty() ? dst :
                       (pegs[dst].empty() ? aux :
                        (pegs[aux].top() < pegs[dst].top() ? aux : dst));
            int to = (from == aux) ? dst : aux;
            int disk = pegs[from].top(); pegs[from].pop();
            pegs[to].push(disk);
            cout << "  Step " << i << ": 盘 " << disk
                 << " " << char('A' + from) << " → " << char('A' + to) << endl;
        }
    }
}

// ==================== 递归版（用于对比） ====================
void hanoi_recursive(int n, char src, char aux, char dst, int& moves) {
    if (n <= 0) return;
    hanoi_recursive(n - 1, src, dst, aux, moves);
    cout << "  Step " << ++moves << ": 盘 " << n
         << " " << src << " → " << dst << endl;
    hanoi_recursive(n - 1, aux, src, dst, moves);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  汉诺塔非递归解法                       " << endl;
    cout << "========================================" << endl << endl;

    int n = 3;

    // 递归版
    cout << "[递归版] n=" << n << ":" << endl;
    int moves = 0;
    hanoi_recursive(n, 'A', 'B', 'C', moves);

    // 栈模拟版
    cout << endl << "[栈模拟递归] n=" << n << ":" << endl;
    hanoi_stack(n, 'A', 'B', 'C');

    // 交替法
    cout << endl << "[迭代交替法] n=" << n << ":" << endl;
    hanoi_iterative(n);

    // 最少步数验证
    cout << endl << "[理论分析]" << endl;
    for (int i = 1; i <= 5; i++)
        cout << "  n=" << i << ": 最少步数 = 2^" << i << "-1 = "
             << (1 << i) - 1 << endl;
    cout << "  n=64: 最少步数 ≈ 1.84 × 10^19（如每秒1步需5800亿年）" << endl;

    return 0;
}
