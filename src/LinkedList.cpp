#include "../include/LinkedList.h"
#include <iostream>

LinkedList::LinkedList() : head(nullptr) {}

LinkedList::~LinkedList()
{
    Node* current = head;
    while (current != nullptr)
    {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

void LinkedList::insertFront(Product* p)
{
    Node* newNode = new Node(p);
    newNode->next = head;
    head = newNode;
}

Product* LinkedList::searchByName(const std::string& name)
{
    Node* current = head;
    while (current != nullptr)
    {
        if (current->data->name == name)
        {
            return current->data;
        }
        current = current->next;
    }
    return nullptr;
}

void LinkedList::insertSorted(Product* p)
{
    Node* newNode = new Node(p);
    if (head == nullptr || p->name < head->data->name)
    {
        newNode->next = head;
        head = newNode;
        return;
    }
    Node* current = head;
    Node* previous = nullptr;
    while (current != nullptr && current->data->name < p->name)
    {
        previous = current;
        current = current->next;
    }
    newNode->next = current;
    previous->next = newNode;
}

bool LinkedList::removeByBarCode(const std::string& barCode)
{
    Node* current = head;
    Node* previous = nullptr;
    while (current != nullptr && current->data->barcode != barCode)
    {
        previous = current;
        current = current->next;
    }
    if (current == nullptr)
    {
        return false;
    }
    if (previous == nullptr)
    {
        head = head->next;
    }
    else
    {
        previous->next = current->next;
    }
    delete current;
    return true;
}
