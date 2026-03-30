#ifndef PROYECTO1_BTREE_H
#define PROYECTO1_BTREE_H
#include "Product.h"
#include "LinkedList.h"
#include <vector>
#include <string>

struct DateKey {
    std::string date;
    LinkedList productsList; // Lista de productos que caducan este día

    DateKey() {} // Constructor por defecto necesario para los arreglos
    DateKey(const std::string& d, Product* p) : date(d) {
        productsList.insertFront(p);
    }
};

class BTreeNode {
public:
    DateKey* keys;        // Arreglo dinamico de llaves
    BTreeNode** children; // Arreglo dinamico de punteros a hijos
    int t;                // Grado mínimo
    int n;                // Cantidad actual de llaves en el nodo
    bool leaf;            // ¿Es hoja?

    BTreeNode(int _t, bool _leaf);

    // Funciones
    void insertNonFull(Product* p, const std::string& date);
    void splitChild(int i, BTreeNode* y);
    void searchByDateRange(const std::string& start, const std::string& end);
    ~BTreeNode();
};

class BTree {
private:
    BTreeNode* root;
    int t; //Grado minimo del arbol

public:
    BTree(int _t = 3); // Por defecto se usara grado 3
    ~BTree();

    void insert(Product* p);
    void searchByDateRange(const std::string& start, const std::string& end);
    bool remove(Product* p);
};

#endif //PROYECTO1_BTREE_H