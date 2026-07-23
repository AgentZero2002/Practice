/**
 * 合并 K 个有序链表
 *
 * 经典题: LeetCode 23 — Merge K Sorted Lists
 *
 * 三种方法:
 *   1. 逐一合并 O(K²n)
 *   2. 分治合并 O(Kn log K) — 归并思想
 *   3. 优先队列 O(Kn log K) — 堆思想
 *
 * 本实现展示方法 2 (分治) 和 方法 3 (堆)。
 */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v) : val(v), next(nullptr) {}
};

// ==================== 方法1: 合并两个有序链表 ====================
ListNode* mergeTwoLists(ListNode* a, ListNode* b) {
    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (a && b) {
        if (a->val <= b->val) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;
}

// ==================== 方法2: 分治法 ====================
ListNode* mergeKLists_divide(const vector<ListNode*>& lists, int lo, int hi) {
    if (lo == hi) return lists[lo];
    if (lo > hi) return nullptr;  // 空区间
    int mi = (lo + hi) / 2;
    ListNode* left = mergeKLists_divide(lists, lo, mi);
    ListNode* right = mergeKLists_divide(lists, mi + 1, hi);
    return mergeTwoLists(left, right);
}

ListNode* mergeKLists(const vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;
    return mergeKLists_divide(lists, 0, lists.size() - 1);
}

// ==================== 方法3: 优先队列法 ====================
ListNode* mergeKLists_heap(const vector<ListNode*>& lists) {
    // 最小堆: {值, 节点指针}
    auto cmp = [](ListNode* a, ListNode* b) { return a->val > b->val; };
    priority_queue<ListNode*, vector<ListNode*>, decltype(cmp)> pq(cmp);

    for (ListNode* head : lists)
        if (head) pq.push(head);

    ListNode dummy(0);
    ListNode* tail = &dummy;

    while (!pq.empty()) {
        ListNode* node = pq.top(); pq.pop();
        tail->next = node;
        tail = node;
        if (node->next) pq.push(node->next);
    }

    return dummy.next;
}

void printList(ListNode* head) {
    while (head) { cout << head->val << " "; head = head->next; }
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  合并 K 个有序链表                      " << endl;
    cout << "========================================" << endl << endl;

    // 构建 3 个有序链表
    ListNode* l1 = new ListNode(1); l1->next = new ListNode(4); l1->next->next = new ListNode(7);
    ListNode* l2 = new ListNode(2); l2->next = new ListNode(5); l2->next->next = new ListNode(8);
    ListNode* l3 = new ListNode(3); l3->next = new ListNode(6); l3->next->next = new ListNode(9);

    vector<ListNode*> lists = {l1, l2, l3};

    cout << "  链表 1: "; printList(l1); cout << endl;
    cout << "  链表 2: "; printList(l2); cout << endl;
    cout << "  链表 3: "; printList(l3); cout << endl;

    // 分治法
    ListNode* result1 = mergeKLists(lists);
    cout << endl << "  合并 (分治): "; printList(result1); cout << endl;

    // 堆法 (需要重新构建链表)
    ListNode* a = new ListNode(1); a->next = new ListNode(4); a->next->next = new ListNode(7);
    ListNode* b = new ListNode(2); b->next = new ListNode(5); b->next->next = new ListNode(8);
    ListNode* c = new ListNode(3); c->next = new ListNode(6); c->next->next = new ListNode(9);
    vector<ListNode*> lists2 = {a, b, c};
    ListNode* result2 = mergeKLists_heap(lists2);
    cout << "  合并 (堆):   "; printList(result2); cout << endl;

    return 0;
}
