#ifndef PROYECTO1_BPLUSTREE_H
#define PROYECTO1_BPLUSTREE_H
#include "Product.h"
#include "LinkedList.h"
#include <fstream>
#include <cstdint>
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
    void remove(const std::string& category);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);
    int findKey(const std::string& category);
};

class BPlusTree {
private:
    BPlusTreeNode* root;
    int t;

    BPlusTreeNode* getLeftmostLeaf() const;
    void generateDotRecursively(BPlusTreeNode* node, std::ofstream& out) const;

public:
    BPlusTree(int _t = 3);
    ~BPlusTree();

    void insert(Product* p);
    void searchByCategory(const std::string& category) const;
    void removeProduct(const std::string& category, const std::string& barcode);
    void removeCategory(const std::string& category);
    void exportToDot(const std::string& filename) const;
};

#endif //PROYECTO1_BPLUSTREE_H