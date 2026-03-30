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
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cout << "Error: No se pudo crear el archivo " << filename << "\n";
        return;
    }

    out << "digraph AVLTree {\n";

    // --- NUEVO ESTILO GLOBAL AZUL ---
    out << "    // Configuracion General\n";
    out << "    fontname=\"Helvetica,Arial,sans-serif\";\n";
    out << "    label=\"Arbol AVL\";\n";
    out << "    labelloc=\"t\"; // Titulo arriba\n";
    out << "    fontsize=20;\n";
    out << "    nodesep=0.5; // Espacio entre nodos\n";
    out << "    ranksep=0.7; // Espacio entre niveles\n";
    out << "    rankdir=TB;  // Arriba hacia abajo\n\n";

    out << "    // Estilo predefinido de Nodos\n";
    // Nodos redondos, borde azul fuerte, fondo azul palido
    out << "    node [fontname=\"Helvetica,Arial,sans-serif\", shape=ellipse, style=filled, \n";
    out << "          fillcolor=\"#E6F3FF\", color=\"#0066CC\", fontcolor=\"#003366\", penwidth=2];\n\n";

    out << "    // Estilo predefinido de Flechas\n";
    out << "    edge [fontname=\"Helvetica,Arial,sans-serif\", color=\"#004080\", penwidth=1.5];\n\n";

    if (root == nullptr) {
        out << "    empty [label=\"Arbol AVL Vacio\", shape=plaintext, fontcolor=red];\n";
    } else {
        generateDotRecursively(root, out);
    }

    out << "}\n";
    out.close();
    std::cout << "Archivo " << filename << " generado con exito.\n";
}

void AVLTree::generateDotRecursively(AVLNode* node, std::ofstream& out) const {
    if (node == nullptr) return;

    uintptr_t nodeId = reinterpret_cast<uintptr_t>(node);

    // Dibujamos el nodo actual (usando el estilo predefinido arriba)
    out << "    node" << nodeId << " [label=\"" << node->data->name << "\"];\n";

    // Conectamos con hijo izquierdo
    if (node->left != nullptr) {
        uintptr_t leftId = reinterpret_cast<uintptr_t>(node->left);
        out << "    node" << nodeId << " -> node" << leftId << " [label=\"L\"];\n";
        generateDotRecursively(node->left, out);
    } else {
        // Opcional: Dibujar hojas nulas para ver el balanceo completo
        // out << "    nullL" << nodeId << " [shape=point, color=gray];\n";
        // out << "    node" << nodeId << " -> nullL" << nodeId << " [color=gray, weight=0];\n";
    }

    // Conectamos con hijo derecho
    if (node->right != nullptr) {
        uintptr_t rightId = reinterpret_cast<uintptr_t>(node->right);
        out << "    node" << nodeId << " -> node" << rightId << " [label=\"R\"];\n";
        generateDotRecursively(node->right, out);
    } else {
        // out << "    nullR" << nodeId << " [shape=point, color=gray];\n";
        // out << "    node" << nodeId << " -> nullR" << nodeId << " [color=gray, weight=0];\n";
    }
}