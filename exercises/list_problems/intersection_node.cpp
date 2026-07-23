/**
 * 寻找两个链表的相交节点
 *
 * 经典题: LeetCode 160 — Intersection of Two Linked Lists
 *
 * 双指针法:
 *   pA 遍历 A → B, pB 遍历 B → A
 *   若存在交点，pA 和 pB 会同时到达交点
 *   (原理: 两者走过的总路程相同)
 *
 * 时间复杂度 O(n+m), 空间复杂度 O(1)
 */

#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v) : val(v), next(nullptr) {}
};

// ==================== 寻找交点 ====================
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    if (!headA || !headB) return nullptr;

    ListNode* pA = headA;
    ListNode* pB = headB;

    // pA: A → B, pB: B → A
    // 若存在交点，pA==pB 时即为交点
    // 若不存在，最终 pA==pB==nullptr
    while (pA != pB) {
        pA = pA ? pA->next : headB;
        pB = pB ? pB->next : headA;
    }

    return pA;  // 交点或 nullptr
}

// ==================== 方法2: 对齐长度法 ====================
int getLength(ListNode* head) {
    int len = 0;
    while (head) { len++; head = head->next; }
    return len;
}

ListNode* getIntersectionNode_v2(ListNode* headA, ListNode* headB) {
    int lenA = getLength(headA);
    int lenB = getLength(headB);

    // 长链表先走 lenDiff 步，对齐
    while (lenA > lenB) { headA = headA->next; lenA--; }
    while (lenB > lenA) { headB = headB->next; lenB--; }

    // 同时前进找交点
    while (headA != headB) {
        headA = headA->next;
        headB = headB->next;
    }
    return headA;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  寻找两个链表的相交节点                 " << endl;
    cout << "========================================" << endl << endl;

    // 构建相交链表
    // A: 1 → 2 ↘
    //           8 → 10
    // B: 3 → 4 ↗
    ListNode* common = new ListNode(8);
    common->next = new ListNode(10);

    ListNode* a1 = new ListNode(1);
    a1->next = new ListNode(2);
    a1->next->next = common;

    ListNode* b1 = new ListNode(3);
    b1->next = new ListNode(4);
    b1->next->next = common;

    cout << "  A: 1 → 2 ↘" << endl;
    cout << "            8 → 10" << endl;
    cout << "  B: 3 → 4 ↗" << endl << endl;

    // 方法1
    ListNode* result1 = getIntersectionNode(a1, b1);
    cout << "  双指针法: 交点 = " << (result1 ? to_string(result1->val) : "null") << " (期望: 8)" << endl;

    // 方法2
    ListNode* result2 = getIntersectionNode_v2(a1, b1);
    cout << "  对齐长度法: 交点 = " << (result2 ? to_string(result2->val) : "null") << " (期望: 8)" << endl;

    // 不相交的情况
    ListNode* c1 = new ListNode(5);
    ListNode* d1 = new ListNode(6);
    ListNode* noIntersection = getIntersectionNode(c1, d1);
    cout << endl << "  不相交测试: " << (noIntersection ? "有交点" : "无交点 ✓") << endl;

    return 0;
}
