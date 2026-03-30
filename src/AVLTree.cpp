#include "../include/AVLTree.h"

AVLTree::AVLTree() {
    root = nullptr;
}

void destroyTree(AVLNode* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node; // Destruimos el nodo, NO el Product
    }
}

AVLTree::~AVLTree() {
    destroyTree(root);
}

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

Product* AVLTree::searchByName(const std::string& name) const {
    AVLNode* current = root;

    while (current != nullptr) {
        if (name < current->data->name) {
            current = current->left; // Buscamos a la izquierda
        }
        else if (name > current->data->name) {
            current = current->right; // Buscamos a la derecha
        }
        else {
            return current->data; // Coincidencia exacta
        }
    }

    return nullptr; // Llegamos al final de la hoja y no estaba
}

AVLNode* AVLTree::findMin(AVLNode* node) const {
    AVLNode* current = node;
    // Seguimos la izquierda mientras exista
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

AVLNode* AVLTree::removeNode(AVLNode* node, const std::string& name) {
    // CASO BASE: Llegamos al final y no lo encontramos
    if (node == nullptr) {
        return node;
    }

    // FASE DE BUSQUEDA
    if (name < node->data->name) {
        node->left = removeNode(node->left, name);
    }
    else if (name > node->data->name) {
        node->right = removeNode(node->right, name);
    }
    // SE ENCUENTRA DATO -- PROCEDE CON ELIMINACION
    else {
        // Escenario 1 y 2: El nodo tiene 0 o 1 hijo
        if (node->left == nullptr || node->right == nullptr) {
            // Guardamos el hijo que sí existe (si no hay ninguno, temp será nullptr)
            AVLNode* temp = node->left ? node->left : node->right;

            // Escenario 1: No tiene hijos
            if (temp == nullptr) {
                temp = node;
                node = nullptr; // Marcamos el nodo actual como nulo
            }
            // Escenario 2: Tiene 1 hijo
            else {
                *node = *temp; // Copiamos el contenido del hijo al nodo actual
            }
            delete temp;
        }
        // Escenario 3: El nodo tiene 2 hijos
        else {
            // Buscamos al sucesor (el más pequeño del lado derecho)
            AVLNode* temp = findMin(node->right);

            // Copiamos SOLO EL PUNTERO del producto del sucesor a este nodo
            node->data = temp->data;

            // Mandamos a eliminar recursivamente al sucesor en la rama derecha
            node->right = removeNode(node->right, temp->data->name);
        }
    }

    // Si el arbol solo tenía 1 nodo y lo borramos, regresamos nulo
    if (node == nullptr) {
        return node;
    }

    // El nodo se audita a si mismo, recalcula su altura y aplica rotaciones si es necesario
    return balance(node);
}

bool AVLTree::remove(const std::string& name) {
    if (searchByName(name) == nullptr) {
        return false;
    }
    root = removeNode(root, name);
    return true;
}


std::string AVLTree::getFirstAlphabetical() const {
    if (root == nullptr) return "";
    AVLNode* current = root;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current->data->name;
}

std::string AVLTree::getLastAlphabetical() const {
    if (root == nullptr) return "";
    AVLNode* current = root;
    while (current->right != nullptr) {
        current = current->right;
    }
    return current->data->name;
}

void AVLTree::printInOrder() const {
    if (root == nullptr) {
        std::cout << "El catalogo esta vacio.\n";
        return;
    }
    std::cout << "\n--- Catalogo Ordenado por Nombre (A-Z) ---\n";
    printInOrderRecursively(root);
    std::cout << "------------------------------------------\n";
}

void AVLTree::printInOrderRecursively(AVLNode* node) const {
    if (node != nullptr) {
        // Visitamos el subarbol izquierdo
        printInOrderRecursively(node->left);

        // Imprimimos el nodo actual usando el metodo de struct Product
        node->data->print();

        // Visitamos el subarbol derecho
        printInOrderRecursively(node->right);
    }
}

void AVLTree::exportToDot(const std::string& filename) const {
    // Abrimos (o creamos) el archivo de texto
    std::ofstream out(filename);

    if (!out.is_open()) {
        std::cout << "Error: No se pudo crear el archivo " << filename << "\n";
        return;
    }

    // Escribimos la cabecera estandar de Graphviz para grafos dirigidos (digraph)
    out << "digraph AVLTree {\n";
    // Le damos un poco de estilo visual a los nodos
    out << "    node [fontname=\"Arial\", shape=circle, style=filled, fillcolor=lightblue];\n";

    // Si el arbol esta vacio, dibujamos un nodo que lo indique
    if (root == nullptr) {
        out << "    empty [label=\"Arbol Vacio\"];\n";
    } else {
        // Iniciamos el viaje recursivo
        generateDotRecursively(root, out);
    }

    // Cerramos la llave del grafo y el archivo
    out << "}\n";
    out.close();

    std::cout << "Archivo " << filename << " generado con exito.\n";
}

void AVLTree::generateDotRecursively(AVLNode* node, std::ofstream& out) const {
    if (node == nullptr) return;

    // Declaramos el nodo actual.
    // Usamos su dirección de memoria como ID interno unico para Graphviz
    out << "    " << reinterpret_cast<uintptr_t>(node)
        << " [label=\"" << node->data->name << "\\nBF: " << getBalanceFactor(node) << "\"];\n";

    // Si tiene hijo izquierdo, declaramos la conexion (flecha) y bajamos recursivamente
    if (node->left != nullptr) {
        out << "    " << reinterpret_cast<uintptr_t>(node) << " -> "
            << reinterpret_cast<uintptr_t>(node->left) << ";\n";
        generateDotRecursively(node->left, out);
    }

    // Si tiene hijo derecho, declaramos la conexion y bajamos recursivamente
    if (node->right != nullptr) {
        out << "    " << reinterpret_cast<uintptr_t>(node) << " -> "
            << reinterpret_cast<uintptr_t>(node->right) << ";\n";
        generateDotRecursively(node->right, out);
    }
}