#ifndef PROYECTO1_BTREE_H
#define PROYECTO1_BTREE_H
#include "Product.h"
#include "LinkedList.h"
#include <fstream>
#include <cstdint>
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

    // Funcion principal recursiva de eliminacion
    void remove(const std::string& date);

    // Funciones auxiliares para los distintos casos
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);

    // Obtener predecesor y sucesor
    DateKey getPredecessor(int idx);
    DateKey getSuccessor(int idx);

    // Funciones de re-balanceo
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);

    // Función de ayuda para buscar el índice de la primera llave >= date
    int findKey(const std::string& date);
    void searchByDateRange(const std::string& start, const std::string& end);
    ~BTreeNode();
};

class BTree {
private:
    BTreeNode* root;
    int t; //Grado minimo del arbol
    void generateDotRecursively(BTreeNode* node, std::ofstream& out) const;

public:
    BTree(int _t = 3); // Por defecto se usara grado 3
    ~BTree();

    void insert(Product* p);
    void searchByDateRange(const std::string& start, const std::string& end);
    void removeProduct(const std::string& date, const std::string& barcode);
    void removeDate(const std::string& date);
    void exportToDot(const std::string& filename) const;
};

#endif //PROYECTO1_BTREE_H