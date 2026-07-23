/**
 * 向量基本操作演示
 *
 * 对应教材 第2章：向量
 *
 * 演示 Vector 模板类的各项操作，包括:
 *   插入、删除、查找、排序、去重、扩容
 */

#include <iostream>
#include "vector_impl.hpp"
using namespace std;

// 打印函数对象
template <typename T>
struct Print {
    void operator()(T& e) { cout << e << " "; }
};

int main() {
    cout << "========================================" << endl;
    cout << "  向量 (Vector) 基本操作演示             " << endl;
    cout << "========================================" << endl << endl;

    // 1. 构造与插入
    cout << "[构造与插入]" << endl;
    Vector<int> v;
    for (int i = 0; i < 10; i++) {
        v.insert(i, i * 2);  // 在末尾插入
    }
    cout << "  插入 10 个元素后: size=" << v.size() << " cap=" << v.capacity() << endl;
    cout << "  内容: ";
    for (int i = 0; i < v.size(); i++) cout << v[i] << " ";
    cout << endl << endl;

    // 2. 删除
    cout << "[删除操作]" << endl;
    int removed = v.remove(5);  // 删除秩为5的元素
    cout << "  删除 v[5]=" << removed << " 后: ";
    for (int i = 0; i < v.size(); i++) cout << v[i] << " ";
    cout << endl << endl;

    // 3. 查找
    cout << "[查找操作]" << endl;
    int pos = v.find(12);
    cout << "  查找 12: 秩 = " << (pos >= 0 ? to_string(pos) : "未找到") << endl;
    pos = v.find(100);
    cout << "  查找 100: " << (pos >= 0 ? to_string(pos) : "未找到") << endl << endl;

    // 4. 排序与有序查找
    cout << "[排序与二分查找]" << endl;
    v.mergeSort();
    cout << "  归并排序后: ";
    for (int i = 0; i < v.size(); i++) cout << v[i] << " ";
    cout << endl;
    pos = v.search(10);
    cout << "  二分查找 ≤10 的最后一个元素: v[" << pos << "] = " << v[pos] << endl;
    pos = v.search(100);
    cout << "  二分查找 ≤100: v[" << pos << "] = " << v[pos] << "（末尾）" << endl << endl;

    // 5. 去重
    cout << "[去重（唯一化）]" << endl;
    Vector<int> v2;
    int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    for (int x : arr) v2.push_back(x);
    cout << "  去重前 (" << v2.size() << " 个): ";
    for (int i = 0; i < v2.size(); i++) cout << v2[i] << " ";
    cout << endl;
    int removed_cnt = v2.deduplicate();
    cout << "  无序去重: 移除 " << removed_cnt << " 个重复元素" << endl;
    cout << "  去重后 (" << v2.size() << " 个): ";
    for (int i = 0; i < v2.size(); i++) cout << v2[i] << " ";
    cout << endl << endl;

    // 6. 有序唯一化
    cout << "[有序唯一化]" << endl;
    Vector<int> v3;
    int sorted[] = {1, 1, 2, 3, 3, 3, 5, 8, 8, 13};
    for (int x : sorted) v3.push_back(x);
    cout << "  去重前 (" << v3.size() << " 个): ";
    for (int i = 0; i < v3.size(); i++) cout << v3[i] << " ";
    cout << endl;
    removed_cnt = v3.uniquify();
    cout << "  有序唯一化: 移除 " << removed_cnt << " 个重复元素" << endl;
    cout << "  去重后 (" << v3.size() << " 个): ";
    for (int i = 0; i < v3.size(); i++) cout << v3[i] << " ";
    cout << endl << endl;

    // 7. 扩容策略演示
    cout << "[加倍式扩容演示]" << endl;
    Vector<int> v4;
    for (int i = 1; i <= 20; i++) {
        v4.push_back(i);
        if (v4.size() == v4.capacity())
            cout << "  插入 " << i << " 后触发扩容: cap = " << v4.capacity() << endl;
    }
    cout << "  最终: size=" << v4.size() << " cap=" << v4.capacity() << endl;

    return 0;
}
