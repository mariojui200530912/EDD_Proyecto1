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

bool LinkedList::isEmpty() const
{
    return head == nullptr;
}

void LinkedList::insertFront(Product* p)
{
    Node* newNode = new Node(p);
    newNode->next = head;
    head = newNode;
}

Product* LinkedList::searchByName(const std::string& name) const
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

void LinkedList::insertSorted(Product* p) {
    if (p == nullptr) return;

    Node* newNode = new Node(p);

    // Caso 1: La lista está vacía, o el nuevo producto va al mismísimo principio
    if (head == nullptr || head->data->name >= p->name) {
        newNode->next = head;
        head = newNode;
        return;
    }

    // Caso 2: Buscar su posición en el medio o al final de la lista.
    Node* current = head;

    // Salvavidas 2: Revisamos current->next ANTES de intentar acceder a current->next->data.
    while (current->next != nullptr && current->next->data->name < p->name) {
        current = current->next;
    }

    // Insertamos el nuevo nodo justo después del 'current' donde nos detuvimos.
    newNode->next = current->next;
    current->next = newNode;
}

std::vector<std::string> LinkedList::getAllNames() const {
    std::vector<std::string> names;
    Node* current = head;
    while (current != nullptr) {
        names.push_back(current->data->name);
        current = current->next;
    }
    return names;
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

void LinkedList::print() const {
    Node* current = head;

    if (current == nullptr) {
        std::cout << "  [No hay productos en esta lista]\n";
        return;
    }

    while (current != nullptr) {
        current->data->print();

        current = current->next;
    }
}

LinkedList::LinkedList(const LinkedList& other) {
    head = nullptr;
    Node* current = other.head;
    Node* tail = nullptr;

    while (current != nullptr) {
        Node* newNode = new Node(current->data); // Clonamos el contenedor
        newNode->next = nullptr;
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        current = current->next;
    }
}

// --- OPERADOR DE ASIGNACIÓN (=) ---
LinkedList& LinkedList::operator=(const LinkedList& other) {
    if (this == &other) return *this; // Evitar auto-asignación

    // Primero destruimos la lista actual para evitar fugas
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;

    // Luego clonamos la nueva
    Node* otherCurrent = other.head;
    Node* tail = nullptr;
    while (otherCurrent != nullptr) {
        Node* newNode = new Node(otherCurrent->data);
        newNode->next = nullptr;
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        otherCurrent = otherCurrent->next;
    }
    return *this;
}