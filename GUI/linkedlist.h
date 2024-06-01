#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "transaction.h"

template <typename T>
class Node {
public:
    T data;
    Node* next;

    Node(T data) : data(data), next(nullptr) {}
};

template <typename T>
class LinkedList {
public:
    Node<T>* head;

    LinkedList() : head(nullptr) {}

    ~LinkedList() {
        Node<T>* current = head;
        while (current) {
            Node<T>* nextNode = current->next;
            delete current;
            current = nextNode;
        }
    }

    void append(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        if (!head) {
            head = newNode;
        } else {
            Node<T>* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    void clear() {
        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
    }

    bool deleteNode(Transaction transaction)
    {
        Node<T>* current = head;
        Node<T>* previous = nullptr;

        while (current != nullptr) {
            if (current->data == transaction) {
                if (previous == nullptr) {
                    // Deleting the head node
                    head = current->next;
                } else {
                    previous->next = current->next;
                }
                delete current;
                return true;
            }
            previous = current;
            current = current->next;
        }
        return false;
    }

    T* at(int index) const {
        Node<T>* current = head;
        int count = 0;

        while (current != nullptr) {
            if (count == index) {
                return &(current->data);
            }
            count++;
            current = current->next;
        }
        return nullptr;  // Index out of bounds
    }

};


#endif // LINKEDLIST_H
