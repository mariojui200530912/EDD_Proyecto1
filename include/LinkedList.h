#ifndef PROYECTO1_LINKEDLIST_H
#define PROYECTO1_LINKEDLIST_H
#include "Product.h"
#include <string>

struct Node
{
    Product* data;
    Node* next;

    Node(Product* p) : data(p), next(nullptr){}
};

class LinkedList
{
private:
    Node* head;
public:
    LinkedList();
    ~LinkedList();
    void insertFront(Product* p);
    void insertSorted(Product* p);
    Product* searchByName(const std::string& name);
    bool removeByBarCode(const std::string& barCode);
    void printAll() const;
};

#endif //PROYECTO1_LINKEDLIST_H
