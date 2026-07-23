/**
 * 链表逆置的三种实现方法
 *
 * 对应教材 3.3 节习题
 *
 * 方法一：迭代头插法 O(n) + O(1)
 *   遍历原链表，依次将每个节点头插到新链表中
 *
 * 方法二：三指针就地逆置 O(n) + O(1)
 *   依次修改每个节点的前后指针方向
 *
 * 方法三：递归逆置 O(n) + O(n) 递归栈
 *   递归到链表末尾，回溯时修改指针
 *
 * 本文件使用 STL list 和自建简单单向链表两种方式演示。
 */

#include <iostream>
#include <list>
#include <forward_list>
using namespace std;

// ==================== 自定义单向链表 ====================
struct SNode {
    int val;
    SNode* next;
    SNode(int v) : val(v), next(nullptr) {}
};

// 头插法构建链表
SNode* buildList(initializer_list<int> vals) {
    SNode* head = nullptr;
    // 反向遍历以保持顺序
    for (auto it = rbegin(vals); it != rend(vals); ++it) {
        SNode* node = new SNode(*it);
        node->next = head;
        head = node;
    }
    return head;
}

void printList(SNode* head) {
    while (head) { cout << head->val << " "; head = head->next; }
}

void deleteList(SNode* head) {
    while (head) { SNode* n = head->next; delete head; head = n; }
}

// 方法一：迭代头插法
// 遍历原链表，逐个节点头插入新链表
SNode* reverse_headInsert(SNode* head) {
    SNode* newHead = nullptr;
    while (head) {
        SNode* next = head->next;   // 保存下一个节点
        head->next = newHead;       // 当前节点接入新链表头部
        newHead = head;             // 更新新链表头
        head = next;                // 移动到原链表下一节点
    }
    return newHead;
}

// 方法二：三指针就地逆置
// 只修改指针方向，不使用额外的头插
SNode* reverse_inPlace(SNode* head) {
    SNode* prev = nullptr;
    SNode* curr = head;
    while (curr) {
        SNode* next = curr->next;   // 保存下一节点
        curr->next = prev;          // 反转当前节点指针
        prev = curr;                // prev 前进
        curr = next;                // curr 前进
    }
    return prev;  // 新的头节点
}

// 方法三：递归逆置
// 递归到底，回溯时修改指针
SNode* reverse_recursive(SNode* head) {
    if (!head || !head->next)       // 递归基：空链表或单节点
        return head;
    SNode* newHead = reverse_recursive(head->next);  // 逆置子链表
    head->next->next = head;        // 将当前节点接在子链表末尾
    head->next = nullptr;           // 当前节点成为新末尾
    return newHead;
}

// ==================== STL list 逆置演示 ====================
void demo_stl_reverse() {
    cout << "[STL list 逆置]" << endl;
    list<int> L = {1, 2, 3, 4, 5, 6};

    cout << "  原链表: ";
    for (int x : L) cout << x << " ";

    // STL 内建 reverse
    L.reverse();

    cout << endl << "  reverse() 后: ";
    for (int x : L) cout << x << " ";
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  链表逆置：三种实现方法对比             " << endl;
    cout << "========================================" << endl << endl;

    // 构建测试链表
    SNode* head1 = buildList({1, 2, 3, 4, 5, 6, 7});
    SNode* head2 = buildList({1, 2, 3, 4, 5, 6, 7});
    SNode* head3 = buildList({1, 2, 3, 4, 5, 6, 7});

    cout << "[自定义单向链表逆置]" << endl;
    cout << "  原链表:       ";
    printList(head1);
    cout << endl;

    // 方法一
    head1 = reverse_headInsert(head1);
    cout << "  头插法逆置:   ";
    printList(head1);
    cout << endl;

    // 方法二
    head2 = reverse_inPlace(head2);
    cout << "  三指针就地:   ";
    printList(head2);
    cout << endl;

    // 方法三
    head3 = reverse_recursive(head3);
    cout << "  递归逆置:     ";
    printList(head3);
    cout << endl << endl;

    // 清理
    deleteList(head1);
    deleteList(head2);
    deleteList(head3);

    // STL 演示
    demo_stl_reverse();

    // 复杂度总结
    cout << endl << "[复杂度对比]" << endl;
    cout << "  方法            时间    空间    备注" << endl;
    cout << "  ─────────────────────────────────" << endl;
    cout << "  头插法           O(n)    O(1)   最简洁直观" << endl;
    cout << "  三指针就地       O(n)    O(1)   不额外分配节点" << endl;
    cout << "  递归法           O(n)    O(n)   代码优雅但有栈溢出风险" << endl;

    return 0;
}
