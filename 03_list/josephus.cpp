/**
 * 约瑟夫斯问题 (Josephus Problem)
 *
 * 对应教材 3.1 节习题 [3-19]
 *
 * 问题描述:
 *   n 个人围成一圈，从第 1 个人开始报数，
 *   每次数到第 k 个人时将其淘汰，并从下一个人重新报数，
 *   直到只剩一人。求淘汰顺序。
 *
 * 解法:
 *   1. 循环链表模拟 O(n*k)（本实现）
 *   2. 数学递推公式 (Josephus 递推) O(n):
 *      J(1) = 0
 *      J(i) = (J(i-1) + k) % i     (0-based 编号)
 */

#include <iostream>
#include <vector>
using namespace std;

// 循环链表节点
struct Node {
    int id;
    Node* next;
    Node(int i) : id(i), next(nullptr) {}
};

// ==================== 解法一：循环链表模拟 ====================
// 构建 n 个节点的循环链表，模拟淘汰过程
vector<int> josephus_list(int n, int k) {
    if (n <= 0) return {};

    // 1. 构建循环链表 1→2→...→n→1
    Node* head = new Node(1);
    Node* tail = head;
    for (int i = 2; i <= n; i++) {
        tail->next = new Node(i);
        tail = tail->next;
    }
    tail->next = head;  // 形成循环

    // 2. 模拟淘汰
    vector<int> result;
    Node* prev = tail;  // 指向当前节点的前驱
    Node* curr = head;

    while (n > 0) {
        // 走 k-1 步（prev 和 curr 同步前进）
        for (int step = 1; step < k; step++) {
            prev = curr;
            curr = curr->next;
        }

        // 淘汰 curr
        result.push_back(curr->id);
        prev->next = curr->next;
        Node* tmp = curr;
        curr = curr->next;
        if (curr == tmp) curr = curr->next;  // 处理只剩1个节点的情况
        delete tmp;
        n--;
    }

    return result;
}

// ==================== 解法二：数学递推（只求最终存活者） ====================
// n 个人，每次数到 k 淘汰，求最后存活者的编号（1-based）
// 时间复杂度 O(n)，空间复杂度 O(1)
int josephus_math(int n, int k) {
    if (n == 1) return 1;
    // J(i) = (J(i-1) + k) % i   (0-based)
    int survivor = 0;  // J(1) = 0 (0-based)
    for (int i = 2; i <= n; i++)
        survivor = (survivor + k) % i;
    return survivor + 1;  // 转为 1-based
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  约瑟夫斯问题 (Josephus Problem)       " << endl;
    cout << "========================================" << endl << endl;

    // 测试：n=10, k=3
    int n = 10, k = 3;
    cout << "[循环链表模拟] n=" << n << ", k=" << k << endl;

    vector<int> order = josephus_list(n, k);

    cout << "  淘汰顺序: ";
    for (int i = 0; i < (int)order.size(); i++) {
        if (i > 0) cout << " → ";
        if (i == (int)order.size() - 1)
            cout << "[" << order[i] << "]";  // 最终存活者
        else
            cout << order[i];
    }
    cout << endl;

    // 验证：用数学公式验证最终存活者
    int survivor = josephus_math(n, k);
    cout << "  最后存活者: " << order.back()
         << " (数学公式验证: " << survivor << ")" << endl << endl;

    // 多组测试
    cout << "[多组测试] 最后存活者:" << endl;
    cout << "  n=5,  k=2: " << josephus_math(5, 2) << " (应为 3)" << endl;
    cout << "  n=7,  k=3: " << josephus_math(7, 3) << " (应为 4)" << endl;
    cout << "  n=41, k=3: " << josephus_math(41, 3) << " (经典问题: 应为 31)" << endl;
    cout << "  n=10, k=2: " << josephus_math(10, 2) << " (应为 5)" << endl;

    // 经典故事
    cout << endl << "[历史典故]" << endl;
    cout << "  约瑟夫斯是 1 世纪犹太历史学家，" << endl;
    cout << "  他与 40 名士兵被罗马军队包围。" << endl;
    cout << "  他们决定以自杀代替被俘，" << endl;
    cout << "  41 个人围成一圈，从第 1 人开始，" << endl;
    cout << "  每逢第 3 人处决，直到只剩一人。" << endl;
    cout << "  约瑟夫斯将自己和朋友安排在第 16 和 31 位置，" << endl;
    cout << "  最终存活。" << endl;

    return 0;
}
