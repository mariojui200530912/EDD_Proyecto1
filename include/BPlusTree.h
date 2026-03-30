#ifndef PROYECTO1_BPLUSTREE_H
#define PROYECTO1_BPLUSTREE_H
#include "Product.h"
#include "LinkedList.h"
#include <string>

// Estructura para manejar las categorias y sus productos
struct CategoryKey {
    std::string category;
    LinkedList productsList;

    CategoryKey() {}
    CategoryKey(const std::string& cat, Product* p) : category(cat) {
        productsList.insertFront(p);
    }
};

class BPlusTreeNode {
public:
    CategoryKey* keys;
    BPlusTreeNode** children;
    int t;
    int n;
    bool leaf;

    BPlusTreeNode* next;

    BPlusTreeNode(int _t, bool _leaf);
    ~BPlusTreeNode();

    // Funciones
    void insertNonFull(Product* p, const std::string& category);
    void splitChild(int i, BPlusTreeNode* y);
};

class BPlusTree {
private:
    BPlusTreeNode* root;
    int t;

public:
    BPlusTree(int _t = 3);
    ~BPlusTree();

    void insert(Product* p);

    // Busqueda secuencial aprovechando el puntero 'next'
    void searchByCategory(const std::string& category) const;
};

#endif //PROYECTO1_BPLUSTREE_H