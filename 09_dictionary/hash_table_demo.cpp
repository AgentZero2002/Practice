/**
 * 哈希表操作演示
 *
 * 对应教材 第9章：词典
 *
 * 演示分离链法哈希表的增删查操作和性能特征
 */

#include <iostream>
#include <string>
#include "hash_table.hpp"
using namespace std;

int main() {
    cout << "========================================" << endl;
    cout << "  哈希表 (Hash Table) 操作演示           " << endl;
    cout << "========================================" << endl << endl;

    // ==================== 分离链法 ====================
    cout << "[分离链法 ChainingHashMap]" << endl;
    ChainingHashMap<string, int> map;

    // 插入
    map.put("Alice", 85);
    map.put("Bob", 92);
    map.put("Charlie", 78);
    map.put("Diana", 95);
    map.put("Eve", 88);

    cout << "  插入 5 个条目: size=" << map.size() << endl;
    cout << "  负载因子: " << map.loadFactor() << endl;

    // 查询
    cout << "  查询: Alice=" << *map.get("Alice") << ", Bob=" << *map.get("Bob") << endl;
    cout << "  不存在的 key: " << (map.get("Zack") ? "存在" : "不存在") << endl;

    // 更新
    map.put("Alice", 90);
    cout << "  更新 Alice: " << *map.get("Alice") << endl;

    // 删除
    map.remove("Charlie");
    cout << "  删除 Charlie: size=" << map.size() << ", 查询="
         << (map.get("Charlie") ? "仍存在" : "已删除") << endl;
    cout << endl;

    // ==================== 线性探测法 ====================
    cout << "[线性探测法 LinearProbingHashMap]" << endl;
    LinearProbingHashMap<string, int> lp;

    lp.put("red", 1);
    lp.put("orange", 2);
    lp.put("yellow", 3);
    lp.put("green", 4);
    lp.put("blue", 5);

    cout << "  插入 5 个条目: size=" << lp.size() << endl;
    cout << "  查询 red: " << *lp.get("red") << ", blue: " << *lp.get("blue") << endl;
    cout << endl;

    // ==================== 散列冲突演示 ====================
    cout << "[散列冲突]" << endl;
    cout << "  当两个 key 散列到同一位置时:" << endl;
    cout << "  · 分离链法: 追加到链表中，最坏 O(n)" << endl;
    cout << "  · 开放定址: 线性探测下一位置，可能形成聚集" << endl;
    cout << "  · 负载因子 α = n/M 越大，冲突概率越高" << endl;
    cout << "  · 通常建议 α < 0.75" << endl << endl;

    // ==================== ASL 计算 ====================
    cout << "[例题] 哈希表查找 ASL (平均查找长度)" << endl;
    cout << "  H(key)=key%13, 线性探测, 插入: 26,41,25,18,39,50" << endl;
    int keys[] = {26, 41, 25, 18, 39, 50};
    int table[13] = {0};
    cout << "  散列过程:" << endl;
    for (int k : keys) {
        int h = k % 13;
        int probes = 0;
        while (table[h] != 0 && probes < 13) {
            h = (h + 1) % 13;
            probes++;
        }
        table[h] = k;
        cout << "    " << k << " → h=" << k%13 << ", 最终位置 " << h
             << " (" << (probes+1) << " 次探测)" << endl;
    }

    return 0;
}
