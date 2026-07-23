/**
 * 二叉搜索树 (BST) 操作演示
 *
 * 对应教材 第7章：搜索树
 *
 * 演示 BST 的增删查操作，以及范围查询（2021年826真题）
 */

#include <iostream>
#include <vector>
#include "bst_impl.hpp"
using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "  二叉搜索树 (BST) 操作演示              " << endl;
    cout << "========================================" << endl << endl;

    BST<int> tree;

    // 1. 插入
    cout << "[插入操作]" << endl;
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 35, 45, 55};
    cout << "  插入序列: ";
    for (int k : keys) { cout << k << " "; tree.insert(k); }
    cout << endl;

    // 2. 中序遍历（应有序）
    vector<int> sorted = tree.toSortedVector();
    cout << "  中序遍历: ";
    for (int x : sorted) cout << x << " ";
    cout << endl;
    cout << "  节点数: " << tree.size() << endl << endl;

    // 3. 查找
    cout << "[查找操作]" << endl;
    for (int target : {40, 55, 100, 50}) {
        bool found = tree.search(target) != nullptr;
        cout << "  查找 " << target << ": " << (found ? "✓ 找到" : "✗ 未找到") << endl;
    }
    cout << endl;

    // 4. 最值
    cout << "[最值查询]" << endl;
    cout << "  最小值: " << tree.minimum() << " (应为20)" << endl;
    cout << "  最大值: " << tree.maximum() << " (应为80)" << endl << endl;

    // 5. 删除
    cout << "[删除操作]" << endl;
    cout << "  删除 20 (叶节点)" << endl;
    tree.remove(20);
    cout << "  删除后中序: ";
    for (int x : tree.toSortedVector()) cout << x << " ";
    cout << endl;

    cout << "  删除 70 (有两个孩子)" << endl;
    tree.remove(70);
    cout << "  删除后中序: ";
    for (int x : tree.toSortedVector()) cout << x << " ";
    cout << endl << endl;

    // 6. 范围查询 ★ (2021年826真题)
    cout << "[范围查询] (2021 826真题)" << endl;
    BST<int> tree2;
    for (int k : {50, 30, 70, 20, 40, 60, 80, 35, 45, 55})
        tree2.insert(k);

    cout << "  查询 [35, 60]: ";
    vector<int> range = tree2.rangeSearch(35, 60);
    for (int x : range) cout << x << " ";
    cout << " (时间复杂度 O(k+h))" << endl;

    // 7. BST 验证
    cout << endl << "[BST 合法性验证]" << endl;
    cout << "  是否为合法 BST: " << (tree2.isValidBST() ? "✓" : "✗") << endl;

    return 0;
}
