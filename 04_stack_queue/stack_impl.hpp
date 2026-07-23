/**
 * 栈 ADT 实现（顺序栈 + 链式栈）
 *
 * 对应教材 4.1 节：栈
 *
 * LIFO (Last-In-First-Out) 结构
 * 实现:
 *   - SeqStack: 基于向量的顺序栈
 *   - LinkStack: 基于链表的链式栈
 */

#ifndef STACK_IMPL_HPP
#define STACK_IMPL_HPP

#include <iostream>
#include <stdexcept>
using namespace std;

// ==================== 顺序栈（基于数组） ====================
template <typename T>
class SeqStack {
private:
    T* _elem;
    int _capacity;
    int _top;  // 栈顶位置（指向栈顶元素的下一个位置）

    void expand() {
        T* old = _elem;
        _elem = new T[_capacity <<= 1];
        for (int i = 0; i < _top; i++)
            _elem[i] = old[i];
        delete[] old;
    }

public:
    SeqStack(int cap = 16) : _capacity(cap), _top(0) {
        _elem = new T[_capacity];
    }

    ~SeqStack() { delete[] _elem; }

    bool empty() const { return _top == 0; }
    int size() const { return _top; }

    void push(T const& e) {
        if (_top == _capacity) expand();
        _elem[_top++] = e;
    }

    T pop() {
        if (empty()) throw runtime_error("Stack underflow");
        return _elem[--_top];
    }

    T& top() {
        if (empty()) throw runtime_error("Stack is empty");
        return _elem[_top - 1];
    }
};

// ==================== 链式栈（基于链表） ====================
template <typename T>
class LinkStack {
private:
    struct Node {
        T data;
        Node* next;
        Node(T const& e, Node* n = nullptr) : data(e), next(n) {}
    };
    Node* _top;
    int _size;

public:
    LinkStack() : _top(nullptr), _size(0) {}
    ~LinkStack() { while (!empty()) pop(); }

    bool empty() const { return _top == nullptr; }
    int size() const { return _size; }

    void push(T const& e) {
        _top = new Node(e, _top);
        _size++;
    }

    T pop() {
        if (empty()) throw runtime_error("Stack underflow");
        Node* old = _top;
        T e = old->data;
        _top = old->next;
        delete old;
        _size--;
        return e;
    }

    T& top() {
        if (empty()) throw runtime_error("Stack is empty");
        return _top->data;
    }
};

#endif // STACK_IMPL_HPP
