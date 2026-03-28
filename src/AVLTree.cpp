#include "../include/AVLTree.h"

int AVLTree::getHeight(AVLNode* node) const
{
    if (node == nullptr)
    {
        return 0;
    }
    return node->height;
}

void AVLTree::updateHeight(AVLNode* node)
{
    if (node != nullptr)
    {
        node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    }
}

int AVLTree::getBalanceFactor(AVLNode* node) const
{
    if (node == nullptr)
    {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;      // "x" es el nodo que va a subir
    AVLNode* T2 = x->right;    // "T2" es el subárbol que quedará huérfano

    // Ejecutamos la rotacion (cambiamos los enganches)
    x->right = y;              // El antiguo padre "y" baja a la derecha de "x"
    y->left = T2;              // El huerfano "T2" se acomoda a la izquierda de "y"

    // Actualizamos las alturas
    // Primero actualizamos "y" porque ahora está más abajo, y luego "x" que es la nueva raíz
    updateHeight(y);
    updateHeight(x);

    // Retornamos "x", que es la nueva cabeza de esta parte del árbol
    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;     // "y" es el nodo que va a subir
    AVLNode* T2 = y->left;     // "T2" es el subarbol que quedara huerfano

    // Ejecutamos la rotacion
    y->left = x;               // El padre original "x" baja a la izquierda de "y"
    x->right = T2;             // El huérfano "T2" se acomoda a la derecha de "x"

    // Actualizamos las alturas
    updateHeight(x);
    updateHeight(y);

    // Retornamos "y", que es la nueva cabeza de esta parte del arbol
    return y;
}

AVLNode* AVLTree::balance(AVLNode* node) {
    if (node == nullptr) return node;
    // Actualizamos la altura de este nodo antes de revisar su balance
    updateHeight(node);
    // Revisamos la "balanza"
    int balanceFactor = getBalanceFactor(node);

    // --- CASOS PESA A LA IZQUIERDA ---
    if (balanceFactor > 1) {
        // Revisamos hacia dónde pesa el hijo izquierdo para saber si es línea recta o zig-zag
        if (getBalanceFactor(node->left) < 0) {
            // Caso LR (Zig-zag): Primero rotamos el hijo a la izquierda
            node->left = rotateLeft(node->left);
        }
        // Caso LL (Línea recta) o el paso final del LR: Rotamos el padre a la derecha
        return rotateRight(node);
    }

    // --- CASOS PESA A LA DERECHA ---
    if (balanceFactor < -1) {
        // Revisamos hacia donde pesa el hijo derecho
        if (getBalanceFactor(node->right) > 0) {
            // Caso RL (Zig-zag): Primero rotamos el hijo a la derecha
            node->right = rotateRight(node->right);
        }
        // Caso RR (Línea recta) o el paso final del RL: Rotamos el padre a la izquierda
        return rotateLeft(node);
    }

    // Si el factor está entre -1 y 1, el nodo está perfectamente balanceado
    return node;
}

AVLNode* AVLTree::insertNode(AVLNode* node, Product* p) {
    // CASO BASE: Si llegamos a un nodo nulo, encontramos el lugar correcto. Creamos la hoja.
    if (node == nullptr) {
        return new AVLNode(p);
    }

    // RECORRIDO: Usamos el operador < nativo de C++ para comparar los textos alfabeticamente
    if (p->name < node->data->name) {
        // El producto va antes en el diccionario, viajamos a la izquierda
        node->left = insertNode(node->left, p);
    }
    else if (p->name > node->data->name) {
        // El producto va después en el diccionario, viajamos a la derecha
        node->right = insertNode(node->right, p);
    }
    else { // Si el producto es igual
        node->right = insertNode(node->right, p);
    }

    // LA MAGIA DEL AVL:
    return balance(node);
}

void AVLTree::insert(Product* p) {
    root = insertNode(root, p);
}

