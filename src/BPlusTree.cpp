#include "../include/BPlusTree.h"
#include <iostream>

// --- Constructor del Nodo ---
BPlusTreeNode::BPlusTreeNode(int _t, bool _leaf) {
    t = _t;
    leaf = _leaf;
    n = 0;
    next = nullptr; // Inicialmente no tiene hermano derecho

    keys = new CategoryKey[2 * t - 1];
    children = new BPlusTreeNode*[2 * t];
}

// --- Destructor del Nodo ---
BPlusTreeNode::~BPlusTreeNode() {
    delete[] keys;
    delete[] children;
}

// --- Constructor del Arbol ---
BPlusTree::BPlusTree(int _t) {
    root = nullptr;
    t = _t;
}

// --- Destructor del Árbol (y función auxiliar) ---
void destroyBPlusTree(BPlusTreeNode* node) {
    if (node != nullptr) {
        if (!node->leaf) {
            for (int i = 0; i <= node->n; i++) {
                destroyBPlusTree(node->children[i]);
            }
        }
        delete node;
    }
}

BPlusTree::~BPlusTree() {
    destroyBPlusTree(root);
}

void BPlusTreeNode::splitChild(int i, BPlusTreeNode* y) {
    // Creamos la nueva caja 'z' -- el hermano derecho
    BPlusTreeNode* z = new BPlusTreeNode(y->t, y->leaf);

    // --- CASO A: ESTAMOS ROMPIENDO UNA HOJA ---
    if (y->leaf) {
        // En una hoja, 'z' se lleva 't' llaves
        z->n = t;
        for (int j = 0; j < t; j++) {
            z->keys[j] = y->keys[j + t - 1]; // Copiamos desde el centro hacia la derecha
        }

        // Rescatamos la llave central para subirla al padre como "letrero"
        CategoryKey keyUp = y->keys[t - 1];

        // 'y' se queda solo con la mitad izquierda
        y->n = t - 1;

        // Enganchamos la lista enlazada de hojas
        z->next = y->next;
        y->next = z;

        // Hacemos el hueco en los hijos del padre para acomodar a 'z'
        for (int j = n; j >= i + 1; j--) {
            children[j + 1] = children[j];
        }
        children[i + 1] = z;

        // Hacemos el hueco en las llaves del padre para subir el "letrero"
        for (int j = n - 1; j >= i; j--) {
            keys[j + 1] = keys[j];
        }
        keys[i] = keyUp;
        n++;
    }
    // --- CASO B: ESTAMOS ROMPIENDO UN NODO INTERNO ---
    else {
        // 'z' se lleva 't - 1' llaves
        z->n = t - 1;
        for (int j = 0; j < t - 1; j++) {
            z->keys[j] = y->keys[j + t];
        }

        // Como no es hoja, 'z' tambien se roba la mitad derecha de los hijos
        for (int j = 0; j < t; j++) {
            z->children[j] = y->children[j + t];
        }

        // Rescatamos la llave central. Esta llave DESAPARECE de abajo y SUBE.
        CategoryKey keyUp = y->keys[t - 1];
        y->n = t - 1;

        // Hacemos los huecos en el padre y subimos la llave
        for (int j = n; j >= i + 1; j--) {
            children[j + 1] = children[j];
        }
        children[i + 1] = z;

        for (int j = n - 1; j >= i; j--) {
            keys[j + 1] = keys[j];
        }
        keys[i] = keyUp;
        n++;
    }
}

void BPlusTreeNode::insertNonFull(Product* p, const std::string& category) {
    int i = n - 1;

    // --- CASO A: LLEGAMOS A UNA HOJA ---
    if (leaf) {
        for (int j = 0; j < n; j++) {
            if (keys[j].category == category) {
                keys[j].productsList.insertFront(p);
                return;
            }
        }

        // Si no existe, buscamos su lugar alfabetico exacto y hacemos el hueco
        while (i >= 0 && keys[i].category > category) {
            keys[i + 1] = keys[i]; // Desplazamos a la derecha
            i--;
        }

        // Insertamos la nueva categoría y su primer producto
        keys[i + 1] = CategoryKey(category, p);
        n++;
    }
    // --- CASO B: ESTAMOS EN UN NODO INTERNO ---
    else {
        // Buscamos por que hijo debemos descender
        while (i >= 0 && keys[i].category > category) {
            i--;
        }
        i++; // Indice del hijo correcto

        // Prevencion: Si el hijo al que vamos a bajar está lleno, lo partimos antes
        if (children[i]->n == 2 * t - 1) {
            splitChild(i, children[i]);

            // Al partirlo, un "letrero" subio a este nodo. Reviisamos si ahora debemos bajar por la mitad izquierda o la derecha.
            if (keys[i].category < category) {
                i++;
            }
        }

        // Bajamos recursivamente con toda seguridad
        children[i]->insertNonFull(p, category);
    }
}

void BPlusTree::insert(Product* p) {
    std::string category = p->category;

    // ARBOL VACIO: Creamos la primera hoja
    if (root == nullptr) {
        root = new BPlusTreeNode(t, true);
        root->keys[0] = CategoryKey(category, p);
        root->n = 1;
    }
    // ARBOL CON DATOS
    else {
        // Si la raiz está completamente llena (2t - 1)
        if (root->n == 2 * t - 1) {
            // El arbol crece hacia arriba. Creamos un nuevo nodo interno 's'
            BPlusTreeNode* s = new BPlusTreeNode(t, false);

            // Enganchamos la vieja raiz llena como hijo de 's'
            s->children[0] = root;

            // Rompemos la vieja raiz por la mitad
            s->splitChild(0, root);

            // Decidimos por cual de las dos nuevas ramas bajar
            int i = 0;
            if (s->keys[0].category < category) {
                i++;
            }
            s->children[i]->insertNonFull(p, category);

            // Coronamos a 's' como la nueva raiz oficial
            root = s;
        }
        else {
            // Si la raiz tiene espacio, simplemente iniciamos el viaje hacia abajo
            root->insertNonFull(p, category);
        }
    }
}

void BPlusTree::searchByCategory(const std::string& category) const {
    // Validamos que el arbol no este vacio
    if (root == nullptr) {
        std::cout << "El arbol de categorias esta vacio.\n";
        return;
    }

    BPlusTreeNode* current = root;

    // FASE DE DESCENSO (Buscando en los letreros) Mientras NO estemos en una hoja, seguimos bajando
    while (!current->leaf) {
        int i = 0;
        // Comparamos alfabeticamente para saber por cual hijo bajar
        while (i < current->n && category > current->keys[i].category) {
            i++;
        }
        // Bajamos al hijo correspondiente
        current = current->children[i];
    }

    // FASE DE LECTURA (Llegamos a la hoja de datos reales) Buscamos nuestra categoria exacta en este nodo
    for (int i = 0; i < current->n; i++) {
        if (current->keys[i].category == category) {
            std::cout << "\n=== Productos en la categoria: " << category << " ===\n";
            // Imprimimos la lista enlazada entera a la velocidad de la luz
            current->keys[i].productsList.print();
            return;
        }
    }

    // Si revisamos la hoja y no estaba, es porque la categoría no existe
    std::cout << "\nNo se encontraron productos en la categoria: " << category << "\n";
}