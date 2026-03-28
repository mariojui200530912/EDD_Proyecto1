#ifndef PROYECTO1_HASHTABLE_H
#define PROYECTO1_HASHTABLE_H

#include "Product.h"
#include <string>

class HashTable
{
private:
    struct HashNode
    {
        Product* data;
        HashNode* next;
        HashNode(Product* p): data(p), next(nullptr){}
    };
    HashNode** table;
    int capacity;
    int size;
    int hashFunction(const std::string& barcode) const;

public:

    HashTable(int cap = 2000);
    ~HashTable();

    bool insert(Product* p);
    Product* search(const std::string& barcode) const;
    bool remove(const std::string& barcode);
};

#endif //PROYECTO1_HASHTABLE_H