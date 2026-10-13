#ifndef STACK_H
#define STACK_H

#include <stdexcept>

// ArrayStack
template <typename T>
class ArrayStack {
    T*  data;
    int topIdx;   // index of the top element (-1 when empty)
    int cap;

    void grow() {                       // doubles the capacity, keep the data
        T* bigger = new T[cap * 2];
        for (int i = 0; i <= topIdx; i++) bigger[i] = data[i];
        delete[] data;
        data = bigger;
        cap *= 2;
    }

public:
    explicit ArrayStack(int c = 16) : data(new T[c]), topIdx(-1), cap(c) {}
    ~ArrayStack() { delete[] data; }
    ArrayStack(const ArrayStack&) = delete;
    ArrayStack& operator=(const ArrayStack&) = delete;

    void push(const T& x) {
        if (topIdx + 1 == cap) grow();
        data[++topIdx] = x;
    }
    T pop() {
        if (empty()) throw std::underflow_error("pop from empty stack");
        return data[topIdx--];
    }
    T& top() {
        if (empty()) throw std::underflow_error("top of empty stack");
        return data[topIdx];
    }
    // Look at an element by position: 0 = bottom, size()-1 = top.
    const T& peekAt(int i) const { return data[i]; }

    bool empty() const { return topIdx == -1; }
    int  size()  const { return topIdx + 1; }
};

// LinkedStack
template <typename T>
class LinkedStack {
    struct Node {
        T     val;
        Node* next;
        Node(const T& v, Node* n) : val(v), next(n) {}
    };
    Node* head;
    int   n;

public:
    LinkedStack() : head(nullptr), n(0) {}
    ~LinkedStack() { while (!empty()) pop(); }
    LinkedStack(const LinkedStack&) = delete;
    LinkedStack& operator=(const LinkedStack&) = delete;

    void push(const T& x) { head = new Node(x, head); n++; }
    T pop() {
        if (empty()) throw std::underflow_error("pop from empty stack");
        Node* old = head;
        T v = old->val;
        head = old->next;
        delete old;
        n--;
        return v;
    }
    T& top() {
        if (empty()) throw std::underflow_error("top of empty stack");
        return head->val;
    }
    bool empty() const { return head == nullptr; }
    int  size()  const { return n; }
};

#endif