#include "../include/BPlusTree.h"
#include <iostream>

inline std::string sanitizeHTML(std::string text) {
    std::string safeText = "";
    for (char c : text) {
        if (c == '&') safeText += "&amp;";
        else if (c == '<') safeText += "&lt;";
        else if (c == '>') safeText += "&gt;";
        else safeText += c;
    }
    return safeText;
}

// --- Constructor del Nodo ---
BPlusTreeNode::BPlusTreeNode(int _t, bool _leaf) {
    t = _t;
    leaf = _leaf;
    n = 0;
    next = nullptr; // Inicialmente no tiene hermano derecho

    keys = new CategoryKey[2 * t - 1];
    children = new BPlusTreeNode*[2 * t];

    for (int i = 0; i < 2 * t; i++) {
        children[i] = nullptr;
    }
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

// Funcion auxiliar para encontrar primera hoja
BPlusTreeNode* BPlusTree::getLeftmostLeaf() const {
    if (root == nullptr) return nullptr;

    BPlusTreeNode* current = root;
    while (!current->leaf) {
        current = current->children[0];
    }
    return current;
}

void BPlusTree::removeProduct(const std::string& category, const std::string& barcode) {
    if (root == nullptr) return;

    BPlusTreeNode* current = root;
    // Bajamos directamente a la hoja (fase iterativa)
    while (!current->leaf) {
        int i = 0;
        while (i < current->n && category > current->keys[i].category) i++;
        current = current->children[i];
    }

    // Buscamos en la hoja
    for (int i = 0; i < current->n; i++) {
        if (current->keys[i].category == category) {
            current->keys[i].productsList.removeByBarCode(barcode);

            // Si ya no hay productos en esta categoria, la borramos estructuralmente
            if (current->keys[i].productsList.isEmpty()) {
                removeCategory(category);
            }
            return;
        }
    }
}

void BPlusTree::removeCategory(const std::string& category) {
    if (root == nullptr) return;

    root->remove(category);

    // Si la raíz se queda vacía después de borrar
    if (root->n == 0) {
        BPlusTreeNode* tmp = root;
        if (root->leaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete tmp;
    }
}

int BPlusTreeNode::findKey(const std::string& category) {
    int idx = 0;
    while (idx < n && keys[idx].category < category) ++idx;
    return idx;
}

void BPlusTreeNode::remove(const std::string& category) {
    int idx = findKey(category);

    // Si estamos en una hoja, hacemos la eliminación real
    if (leaf) {
        // Verificamos si la categoría realmente está aquí
        if (idx < n && keys[idx].category == category) {
            // Desplazamos las llaves a la izquierda para tapar el hueco
            for (int i = idx + 1; i < n; ++i) {
                keys[i - 1] = keys[i];
            }
            n--;
        } else {
            std::cout << "La categoria " << category << " no existe.\n";
        }
        return;
    }

    // Si NO somos hoja, solo estamos bajando guiados por los letreros
    bool isLastChild = (idx == n);

    // Asegurarnos de que el hijo al que bajamos tenga al menos 't' llaves
    if (children[idx]->n < t) {
        fill(idx);
    }

    // Si la función fill() causó que el último hijo se fusionara con el anterior, el índice baja
    if (isLastChild && idx > n) {
        children[idx - 1]->remove(category);
    } else {
        children[idx]->remove(category);
    }
}

void BPlusTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->n >= t) {
        borrowFromPrev(idx);
    } else if (idx != n && children[idx + 1]->n >= t) {
        borrowFromNext(idx);
    } else {
        if (idx != n) {
            merge(idx);
        } else {
            merge(idx - 1);
        }
    }
}

void BPlusTreeNode::borrowFromPrev(int idx) {
    BPlusTreeNode* child = children[idx];
    BPlusTreeNode* sibling = children[idx - 1];

    // Desplazamos todo en 'child' a la derecha
    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    // Diferencia B+: Si es hoja, copiamos el dato real. Si es interno, bajamos el letrero.
    if (child->leaf) {
        child->keys[0] = sibling->keys[sibling->n - 1];
        keys[idx - 1] = child->keys[0]; // Actualizamos el letrero en el padre
    } else {
        child->keys[0] = keys[idx - 1];
        keys[idx - 1] = sibling->keys[sibling->n - 1];
        child->children[0] = sibling->children[sibling->n];
    }

    child->n += 1;
    sibling->n -= 1;
}

void BPlusTreeNode::borrowFromNext(int idx) {
    BPlusTreeNode* child = children[idx];
    BPlusTreeNode* sibling = children[idx + 1];

    if (child->leaf) {
        child->keys[child->n] = sibling->keys[0];
        keys[idx] = sibling->keys[1]; // El letrero del padre ahora es el nuevo primero del hermano
    } else {
        child->keys[child->n] = keys[idx];
        keys[idx] = sibling->keys[0];
        child->children[child->n + 1] = sibling->children[0];
    }

    // Movemos todo en el hermano a la izquierda
    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    child->n += 1;
    sibling->n -= 1;
}

void BPlusTreeNode::merge(int idx) {
    BPlusTreeNode* child = children[idx];
    BPlusTreeNode* sibling = children[idx + 1];

    if (child->leaf) {
        // Fusión de HOJAS (El B+ en acción)
        for (int i = 0; i < sibling->n; ++i) {
            child->keys[child->n + i] = sibling->keys[i];
        }
        child->n += sibling->n;

        // ¡SUPER IMPORTANTE! Mantenemos viva la lista enlazada inferior
        child->next = sibling->next;
    } else {
        // Fusión de NODOS INTERNOS (Igual que el B-Tree normal)
        child->keys[t - 1] = keys[idx];
        for (int i = 0; i < sibling->n; ++i) {
            child->keys[i + t] = sibling->keys[i];
        }
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + t] = sibling->children[i];
        }
        child->n += sibling->n + 1;
    }

    // Desplazamos en el padre para tapar el hueco del letrero que bajó o se eliminó
    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }
    for (int i = idx + 2; i <= n; ++i) {
        children[i - 1] = children[i];
    }
    n--;

    delete sibling;
}

// --- Funcion Principal de Exportacion ---
void BPlusTree::exportToDot(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo " << filename << "\n";
        return;
    }

    out << "digraph BPlusTree {\n";

    // --- NUEVO ESTILO GLOBAL VERDE ---
    out << "    fontname=\"Helvetica,Arial,sans-serif\";\n";
    out << "    label=\"Indice por Categoria (Arbol B+)\";\n";
    out << "    labelloc=\"t\"; fontsize=20;\n";
    out << "    nodesep=0.8; ranksep=1.0;\n\n";

    out << "    node [fontname=\"Helvetica,Arial,sans-serif\", shape=plaintext];\n";
    out << "    edge [fontname=\"Helvetica,Arial,sans-serif\", color=\"#006633\", penwidth=1.5];\n\n";

    if (root == nullptr) {
        out << "    empty [label=\"Arbol B+ Vacio\", shape=plaintext, fontcolor=red];\n";
    } else {
        generateDotRecursively(root, out);

        // --- DISEÑO DE LA LISTA ENLAZADA INFERIOR (Flecha Esmeralda Gruesa) ---
        BPlusTreeNode* leaf = getLeftmostLeaf();
        while (leaf != nullptr && leaf->next != nullptr) {
            uintptr_t currentId = reinterpret_cast<uintptr_t>(leaf);
            uintptr_t nextId = reinterpret_cast<uintptr_t>(leaf->next);

            // constraint=false evita que Graphviz deforme el arbol verticalmente
            out << "    node" << currentId << " -> node" << nextId
                << " [color=\"#00C896\", penwidth=3, style=dashed, constraint=false];\n";

            leaf = leaf->next;
        }
    }

    out << "}\n";
    out.close();
    std::cout << "Archivo " << filename << " generado con exito.\n";
}

void BPlusTree::generateDotRecursively(BPlusTreeNode* node, std::ofstream& out) const {
    if (node == nullptr) return;

    uintptr_t nodeId = reinterpret_cast<uintptr_t>(node);

    // --- DISEÑO DE TABLA HTML PARA ARBOL B+ (Tonos Verdes) ---
    out << "    node" << nodeId << " [label=<\n";

    // Si es hoja, usamos un verde mas fuerte para diferenciar
    std::string bgColor = node->leaf ? "#E6FFFA" : "#CCF2E6";

    out << "      <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" BGCOLOR=\"" << bgColor << "\">\n";
    out << "      <TR>\n";

    for (int i = 0; i < node->n; i++) {
        // Envolvemos la categoria con sanitizeHTML
        out << "        <TD PORT=\"c" << i << "\"><font color=\"#004d33\"><b>" << sanitizeHTML(node->keys[i].category) << "</b></font>";

        if (node->leaf) {
            out << "<BR/>";
            std::vector<std::string> productNames = node->keys[i].productsList.getAllNames();
            for (const std::string& name : productNames) {
                // Envolvemos el nombre con sanitizeHTML
                out << " <font point-size=\"10\">* " << sanitizeHTML(name) << "</font><BR/>";
            }
        }
        out << "</TD>\n";
    }
    // Puerto final para el último hijo derecho
    out << "        <TD PORT=\"c" << node->n << "\"></TD>\n";
    out << "      </TR>\n";
    out << "      </TABLE>\n";
    out << "    >];\n\n";

    // Conectamos padres con hijos
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++) {
            if (node->children[i] != nullptr) {
                uintptr_t childId = reinterpret_cast<uintptr_t>(node->children[i]);
                out << "    node" << nodeId << ":c" << i << " -> node" << childId << ";\n";
                generateDotRecursively(node->children[i], out);
            }
        }
    }
}