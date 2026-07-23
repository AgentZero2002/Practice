/**
 * 队列 ADT 实现（循环队列 + 链式队列）
 *
 * 对应教材 4.2 节：队列
 *
 * FIFO (First-In-First-Out) 结构
 * 实现:
 *   - CircularQueue: 基于数组的循环队列
 *   - LinkQueue: 基于链表的链式队列
 */

#ifndef QUEUE_IMPL_HPP
#define QUEUE_IMPL_HPP

#include <iostream>
#include <stdexcept>
using namespace std;

// ==================== 循环队列 ====================
// 使用模运算实现循环利用数组空间
// 保留一个空位以区分空/满状态
template <typename T>
class CircularQueue {
private:
    T* _elem;
    int _capacity;
    int _front;  // 队首位置
    int _rear;   // 队尾位置（下一个入队位置）

public:
    CircularQueue(int cap = 16) : _capacity(cap + 1) {  // +1 区分空满
        _elem = new T[_capacity];
        _front = _rear = 0;
    }

    ~CircularQueue() { delete[] _elem; }

    bool empty() const { return _front == _rear; }
    int size() const { return (_rear - _front + _capacity) % _capacity; }
    int capacity() const { return _capacity - 1; }

    bool full() const { return (_rear + 1) % _capacity == _front; }

    void enqueue(T const& e) {
        if (full()) throw runtime_error("Queue is full");
        _elem[_rear] = e;
        _rear = (_rear + 1) % _capacity;
    }

    T dequeue() {
        if (empty()) throw runtime_error("Queue is empty");
        T e = _elem[_front];
        _front = (_front + 1) % _capacity;
        return e;
    }

    T& front() {
        if (empty()) throw runtime_error("Queue is empty");
        return _elem[_front];
    }
};

// ==================== 链式队列 ====================
template <typename T>
class LinkQueue {
private:
    struct Node {
        T data;
        Node* next;
        Node(T const& e, Node* n = nullptr) : data(e), next(n) {}
    };
    Node* _front;
    Node* _rear;
    int _size;

public:
    LinkQueue() : _front(nullptr), _rear(nullptr), _size(0) {}

    ~LinkQueue() { while (!empty()) dequeue(); }

    bool empty() const { return _front == nullptr; }
    int size() const { return _size; }

    void enqueue(T const& e) {
        Node* node = new Node(e);
        if (empty())
            _front = _rear = node;
        else {
            _rear->next = node;
            _rear = node;
        }
        _size++;
    }

    T dequeue() {
        if (empty()) throw runtime_error("Queue is empty");
        Node* old = _front;
        T e = old->data;
        _front = old->next;
        if (!_front) _rear = nullptr;
        delete old;
        _size--;
        return e;
    }

    T& front() {
        if (empty()) throw runtime_error("Queue is empty");
        return _front->data;
    }
};

#endif // QUEUE_IMPL_HPP
