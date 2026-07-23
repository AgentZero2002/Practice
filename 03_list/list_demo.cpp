/**
 * 链表操作演示
 *
 * 对应教材 第3章：列表
 *
 * 演示 List 模板类的各项操作:
 *   构造、插入、删除、查找、排序、去重、翻转
 */

#include <iostream>
#include "list_impl.hpp"
using namespace std;

// 打印函数对象
template <typename T>
struct Print {
    void operator()(T& e) const { cout << e << " "; }
};

template <typename T>
void printList(const List<T>& L, const string& label) {
    cout << "  " << label << " (" << L.size() << " 个): ";
    Print<T> pr;
    L.traverse(pr);
    cout << endl;
}

int main() {
    cout << "========================================" << endl;
    cout << "  列表 (List) 基本操作演示               " << endl;
    cout << "========================================" << endl << endl;

    // 1. 构造
    cout << "[构造]" << endl;
    List<int> L;
    for (int i = 1; i <= 5; i++)
        L.insertAsLast(i * 10);
    L.insertAsFirst(5);
    printList(L, "初始列表");
    cout << endl;

    // 2. 插入
    cout << "[插入操作]" << endl;
    L.insertAfter(L.first(), 15);    // 在首节点后插入
    L.insertBefore(L.last(), 45);    // 在末节点前插入
    printList(L, "两次插入后");
    cout << endl;

    // 3. 删除
    cout << "[删除操作]" << endl;
    L.remove(L.first());             // 删除首节点
    L.remove(L.last());              // 删除末节点
    printList(L, "删除首尾后");
    cout << endl;

    // 4. 查找
    cout << "[查找操作]" << endl;
    ListNode<int>* p = L.find(30);
    if (p) cout << "  找到 30" << endl;
    else cout << "  未找到 30" << endl;
    p = L.find(100);
    if (p) cout << "  找到 100" << endl;
    else cout << "  未找到 100" << endl;
    cout << endl;

    // 5. 翻转
    cout << "[翻转]" << endl;
    printList(L, "翻转前");
    L.reverse();
    printList(L, "翻转后");
    L.reverse();  // 翻转回原顺序
    cout << endl;

    // 6. 插入排序
    cout << "[插入排序]" << endl;
    List<int> L2;
    int arr[] = {38, 27, 43, 3, 9, 82, 10};
    for (int i = 0; i < 7; i++) L2.insertAsLast(arr[i]);
    printList(L2, "排序前");
    L2.insertionSort();
    printList(L2, "排序后");
    cout << endl;

    // 7. 选择排序
    cout << "[选择排序]" << endl;
    List<int> L3;
    int arr2[] = {64, 25, 12, 22, 11};
    for (int i = 0; i < 5; i++) L3.insertAsLast(arr2[i]);
    printList(L3, "排序前");
    L3.selectionSort();
    printList(L3, "排序后");
    cout << endl;

    // 8. 去重
    cout << "[有序列表唯一化]" << endl;
    List<int> L4;
    int arr3[] = {1, 1, 2, 3, 3, 3, 5, 8, 8, 13};
    for (int x : arr3) L4.insertAsLast(x);
    printList(L4, "去重前");
    L4.uniquify();
    printList(L4, "去重后");

    return 0;
}
