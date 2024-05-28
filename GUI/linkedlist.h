#ifndef LINKEDLIST_H
#define LINKEDLIST_H

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

    // Additional methods like remove, find, etc., can be added here
};

#endif // LINKEDLIST_H
