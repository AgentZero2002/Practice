/**
 * 双端队列 (Deque) 实现
 *
 * 对应教材 4.3.3 节：双端队列
 *
 * 支持在两端进行插入和删除操作的队列
 * 实现：基于数组的环形双端队列
 *
 * 操作列表:
 *   push_front / push_back  — 前端/后端入队
 *   pop_front / pop_back    — 前端/后端出队
 *   front / back            — 访问两端元素
 */

#ifndef DEQUE_IMPL_HPP
#define DEQUE_IMPL_HPP

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class Deque {
private:
    T* _elem;
    int _capacity;
    int _front;  // 队首位置
    int _rear;   // 队尾的下一个位置
    int _size;

public:
    Deque(int cap = 16) : _capacity(cap), _front(0), _rear(0), _size(0) {
        _elem = new T[_capacity];
    }

    ~Deque() { delete[] _elem; }

    bool empty() const { return _size == 0; }
    int size() const { return _size; }
    int capacity() const { return _capacity; }

    // 前端入队
    void push_front(T const& e) {
        if (_size == _capacity)
            throw runtime_error("Deque is full");
        _front = (_front - 1 + _capacity) % _capacity;
        _elem[_front] = e;
        _size++;
    }

    // 后端入队
    void push_back(T const& e) {
        if (_size == _capacity)
            throw runtime_error("Deque is full");
        _elem[_rear] = e;
        _rear = (_rear + 1) % _capacity;
        _size++;
    }

    // 前端出队
    T pop_front() {
        if (empty()) throw runtime_error("Deque is empty");
        T e = _elem[_front];
        _front = (_front + 1) % _capacity;
        _size--;
        return e;
    }

    // 后端出队
    T pop_back() {
        if (empty()) throw runtime_error("Deque is empty");
        _rear = (_rear - 1 + _capacity) % _capacity;
        _size--;
        return _elem[_rear];
    }

    // 访问元素
    T& front() {
        if (empty()) throw runtime_error("Deque is empty");
        return _elem[_front];
    }

    T& back() {
        if (empty()) throw runtime_error("Deque is empty");
        return _elem[(_rear - 1 + _capacity) % _capacity];
    }
};

#endif // DEQUE_IMPL_HPP
