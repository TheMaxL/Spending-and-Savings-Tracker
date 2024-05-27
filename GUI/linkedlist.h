#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "transaction.h"

struct Node {
    Transaction transaction;
    Node* next;

    Node(const Transaction& t) : transaction(t), next(nullptr) {}
};

class LinkedList {
public:
    LinkedList() : head(nullptr), tail(nullptr) {}

    ~LinkedList() {
        clear();
    }

    void append(const Transaction& transaction) {
        Node* newNode = new Node(transaction);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }

    void clear() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
    }

    Node* getHead() const {
        return head;
    }

private:
    Node* head;
    Node* tail;
};

#endif // LINKEDLIST_H
