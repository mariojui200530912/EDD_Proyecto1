#ifndef PROYECTO1_AVLTREE_H
#define PROYECTO1_AVLTREE_H

#include "Product.h"
#include <string>
#include <algorithm>

struct AVLNode {
    Product* data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Product* p) : data(p), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    int getHeight(AVLNode* node) const;
    int getBalanceFactor(AVLNode* node) const;
    void updateHeight(AVLNode* node);

    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* balance(AVLNode* node);

    AVLNode* insertNode(AVLNode* node, Product* p);
    AVLNode* removeNode(AVLNode* node, const std::string& name);
    AVLNode* findMin(AVLNode* node) const;

    void inOrderTraversal(AVLNode* node) const;

public:
    AVLTree();
    ~AVLTree();

    void insert(Product* p);
    bool remove(const std::string& name);
    Product* searchByName(const std::string& name) const;
    void printInOrder() const;
};
#endif //PROYECTO1_AVLTREE_H