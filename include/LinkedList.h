#ifndef PROYECTO1_LINKEDLIST_H
#define PROYECTO1_LINKEDLIST_H
#include "Product.h"
#include <vector>
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
    bool isEmpty() const;
    void insertFront(Product* p);
    void insertSorted(Product* p);
    Product* searchByName(const std::string& name) const;
    std::vector<std::string> getAllNames() const;
    bool removeByBarCode(const std::string& barCode);
    void print() const;
};

#endif //PROYECTO1_LINKEDLIST_H
