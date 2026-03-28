#include "../include/HashTable.h"

int HashTable::hashFunction(const std::string& barcode) const
{
    unsigned long hashValue = 0;

    for (char c : barcode) {
        hashValue = (hashValue * 31) + c;
    }

    return hashValue % capacity;
}

bool HashTable::insert(Product* p)
{
    int index = hashFunction(p->barcode);

    HashNode* current = table[index];

    while (current != nullptr) {
        // Si encontramos un producto con el mismo codigo de barra, rechazamos la inserción
        if (current->data->barcode == p->barcode) {
            return false; // El DataLoader vera este 'false' y podra loggear el error
        }
        current = current->next; // Avanzamos al siguiente nodo de la colisión
    }

    HashNode* newNode = new HashNode(p);

    newNode->next = table[index];

    table[index] = newNode;

    size++;
    return true;
}

Product* HashTable::search(const std::string& barcode) const {
    int index = hashFunction(barcode);

    HashNode* current = table[index];

    while (current != nullptr) {
        if (current->data->barcode == barcode) {
            return current->data; // Devolvemos el puntero al producto
        }
        current = current->next; // Avanzamos al siguiente nodo de la colisión
    }

    return nullptr;
}

bool HashTable::remove(const std::string& barcode) {
    int index = hashFunction(barcode);

    HashNode* current = table[index];
    HashNode* previous = nullptr;

    while (current != nullptr && current->data->barcode != barcode) {
        previous = current;
        current = current->next;
    }

    if (current == nullptr) {
        return false;
    }

    if (previous == nullptr) {
        table[index] = current->next;
    }
    else {
        previous->next = current->next;
    }

    delete current;

    // 8. ¡IMPORTANTE! Restar al tamaño total para que tu análisis del Factor de Carga sea correcto
    size--;
    return true;
}