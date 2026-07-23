/**
 * 哈希表实现（分离链法 + 开放定址法）
 *
 * 对应教材 第9章：词典
 *
 * 哈希表通过散列函数将 key 映射到数组索引，实现 O(1) 期望查找。
 *
 * 实现:
 *   1. 分离链法 (Separate Chaining) — 每个桶是一个链表
 *   2. 开放定址法 (Open Addressing) — 冲突时探测下一个空位
 *      · 线性探测 (Linear Probing)
 *      · 平方探测 (Quadratic Probing)
 *
 * 散列函数: MAD (Multiply-Add-Divide) 方法
 */

#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>
using namespace std;

// ==================== 散列函数 ====================
// MAD 方法: h(k) = (a*k + b) % M
// 其中 M 为素数，a,b 为随机选取的正整数
template <typename K>
struct MADHash {
    size_t operator()(K key, size_t M) const {
        size_t h = hash<K>{}(key);
        return h % M;
    }
};

// ==================== 分离链法 ====================
template <typename K, typename V>
class ChainingHashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry(K k, V v) : key(k), value(v) {}
    };

    vector<list<Entry>> _buckets;
    int _size;

public:
    ChainingHashMap(int capacity = 101) : _buckets(capacity), _size(0) {}

    int size() const { return _size; }

    // 插入/更新
    void put(K key, V value) {
        size_t idx = MADHash<K>{}(key, _buckets.size());
        for (auto& entry : _buckets[idx]) {
            if (entry.key == key) {
                entry.value = value;  // 更新
                return;
            }
        }
        _buckets[idx].push_back(Entry(key, value));
        _size++;
    }

    // 查找
    V* get(K key) {
        size_t idx = MADHash<K>{}(key, _buckets.size());
        for (auto& entry : _buckets[idx])
            if (entry.key == key) return &entry.value;
        return nullptr;
    }

    // 删除
    bool remove(K key) {
        size_t idx = MADHash<K>{}(key, _buckets.size());
        for (auto it = _buckets[idx].begin(); it != _buckets[idx].end(); ++it) {
            if (it->key == key) {
                _buckets[idx].erase(it);
                _size--;
                return true;
            }
        }
        return false;
    }

    // 负载因子
    double loadFactor() const {
        return (double)_size / _buckets.size();
    }
};

// ==================== 开放定址法（线性探测） ====================
template <typename K, typename V>
class LinearProbingHashMap {
private:
    enum SlotState { EMPTY, OCCUPIED, DELETED };

    struct Slot {
        K key;
        V value;
        SlotState state;
        Slot() : state(EMPTY) {}
    };

    vector<Slot> _table;
    int _size;
    int _capacity;

    int hash(K key) const {
        return MADHash<K>{}(key, _capacity);
    }

public:
    LinearProbingHashMap(int cap = 101) : _table(cap), _size(0), _capacity(cap) {}

    int size() const { return _size; }

    void put(K key, V value) {
        int idx = hash(key);

        // 线性探测
        while (_table[idx].state == OCCUPIED) {
            if (_table[idx].key == key) {
                _table[idx].value = value;  // 更新
                return;
            }
            idx = (idx + 1) % _capacity;
        }

        _table[idx].key = key;
        _table[idx].value = value;
        _table[idx].state = OCCUPIED;
        _size++;
    }

    V* get(K key) {
        int idx = hash(key);

        while (_table[idx].state != EMPTY) {
            if (_table[idx].state == OCCUPIED && _table[idx].key == key)
                return &_table[idx].value;
            idx = (idx + 1) % _capacity;
        }

        return nullptr;
    }

    bool remove(K key) {
        int idx = hash(key);

        while (_table[idx].state != EMPTY) {
            if (_table[idx].state == OCCUPIED && _table[idx].key == key) {
                _table[idx].state = DELETED;  // 懒惰删除
                _size--;
                return true;
            }
            idx = (idx + 1) % _capacity;
        }

        return false;
    }
};

#endif // HASH_TABLE_HPP
