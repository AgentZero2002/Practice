/**
 * 链表环检测 (Detect Cycle) — 快慢指针
 *
 * 经典题: 判断单链表是否存在环，找到环的入口节点
 *
 * Floyd 判圈算法 (Tortoise and Hare):
 *   - 慢指针每次走 1 步，快指针每次走 2 步
 *   - 若有环，快慢指针必定相遇
 *   - 相遇后，将慢指针放回头部，两指针同步前进
 *   - 再次相遇点即为环的入口
 *
 * 时间复杂度 O(n), 空间复杂度 O(1)
 */

#include <iostream>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v) : val(v), next(nullptr) {}
};

// ==================== 检测是否有环 ====================
bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;  // 相遇 → 有环
    }
    return false;  // fast 到达末尾 → 无环
}

// ==================== 找到环的入口 ====================
// 数学证明:
//   设 head 到环入口距离为 a，环入口到相遇点距离为 b，环长度为 L
//   慢指针走过 a+b，快指针走过 2(a+b)
//   快比慢多走 a+b = K×L → 从相遇点再走 a 步恰好到环入口
ListNode* detectCycleEntry(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    // 阶段1: 找相遇点
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }

    if (!fast || !fast->next) return nullptr;  // 无环

    // 阶段2: 找环入口
    slow = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return slow;  // 环入口
}

// ==================== 获取环的长度 ====================
int cycleLength(ListNode* head) {
    ListNode* entry = detectCycleEntry(head);
    if (!entry) return 0;

    int len = 1;
    ListNode* p = entry->next;
    while (p != entry) {
        len++;
        p = p->next;
    }
    return len;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  链表环检测 (快慢指针 / Floyd 判圈)      " << endl;
    cout << "========================================" << endl << endl;

    // 构建带环链表: 1→2→3→4→5→3 (环入口=3)
    ListNode* n1 = new ListNode(1);
    ListNode* n2 = new ListNode(2);
    ListNode* n3 = new ListNode(3);
    ListNode* n4 = new ListNode(4);
    ListNode* n5 = new ListNode(5);
    n1->next = n2; n2->next = n3; n3->next = n4;
    n4->next = n5; n5->next = n3;  // 形成环，入口=3

    cout << "[测试] 链表: 1→2→3→4→5→3(环)" << endl;
    cout << "  有环: " << (hasCycle(n1) ? "✓" : "✗") << endl;

    ListNode* entry = detectCycleEntry(n1);
    cout << "  环入口: " << (entry ? to_string(entry->val) : "无") << " (期望: 3)" << endl;
    cout << "  环长度: " << cycleLength(n1) << " (期望: 3 → 4 → 5 → 3 = 3)" << endl;
    cout << endl;

    cout << "[复杂度]" << endl;
    cout << "  时间: O(n), 空间: O(1)" << endl;
    cout << "  比哈希表方案 (O(n) + O(n) 空间) 更优" << endl;

    // 清理 (需要手动断开环)
    n5->next = nullptr;
    delete n1; delete n2; delete n3; delete n4; delete n5;

    return 0;
}
