#include "../include/BTree.h"

BTree::BTree(int _t) {
    root = nullptr;
    t = _t;
}

void destroyBTree(BTreeNode* node) {
    if (node != nullptr) {
        if (!node->leaf) {
            for (int i = 0; i <= node->n; i++) {
                destroyBTree(node->children[i]);
            }
        }
        delete node;
    }
}

BTree::~BTree() {
    destroyBTree(root);
}

BTreeNode::BTreeNode(int _t, bool _leaf) {
    t = _t;
    leaf = _leaf;
    n = 0; // Al principio está vacío

    // Un nodo puede tener máximo 2t - 1 llaves
    keys = new DateKey[2 * t - 1];

    // Un nodo puede tener máximo 2t hijos
    children = new BTreeNode*[2 * t];
}

BTreeNode::~BTreeNode() {
    delete[] keys;
    delete[] children;
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    // Creamos la nueva caja hermana 'z'
    BTreeNode* z = new BTreeNode(y->t, y->leaf);
    z->n = t - 1;

    // Copiamos la mitad derecha de llaves de 'y' a 'z'
    for (int j = 0; j < t - 1; j++) {
        z->keys[j] = y->keys[j + t];
    }

    // Si no es hoja, también copiamos la mitad derecha de los hijos
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            z->children[j] = y->children[j + t];
        }
    }

    // Reducimos la cuenta de llaves de 'y'
    y->n = t - 1;

    // ATENCION: Movemos los hijos del nodo padre (this) un espacio a la derecha para hacerle el hueco al nuevo hermano 'z'
    for (int j = n; j >= i + 1; j--) {
        children[j + 1] = children[j];
    }
    children[i + 1] = z;

    // Movemos las llaves del nodo padre (this) un espacio a la derecha para hacerle hueco a la llave central que va a subir
    for (int j = n - 1; j >= i; j--) {
        keys[j + 1] = keys[j];
    }
    keys[i] = y->keys[t - 1]; // Subimos la llave central

    // El nodo padre ahora tiene una llave más
    n++;
}

void BTreeNode::insertNonFull(Product* p, const std::string& date) {
    int i = n - 1; // Empezamos a revisar desde la llave mas a la derecha del nodo

    // VALIDACION: ¿La fecha ya existe en este nodo?
    for (int j = 0; j < n; j++) {
        if (keys[j].date == date) {
            // Existe, simplemente agregamos el producto a la lista de este dia
            keys[j].productsList.insertFront(p);
            return;
        }
    }

    // Si llegamos aquí, la fecha no existe en este nodo.
    if (leaf) {
        // ES UNA HOJA: Buscamos su lugar correcto y hacemos el hueco desplazando a la derecha
        while (i >= 0 && keys[i].date > date) {
            keys[i + 1] = keys[i]; // Movemos la fecha un espacio a la derecha
            i--;
        }

        // Insertamos la nueva fecha en el hueco que quedó
        keys[i + 1] = DateKey(date, p);
        n++; // Aumentamos el contador de llaves
    }
    else {
        // NO ES HOJA: Debemos buscar a qué hijo descender
        while (i >= 0 && keys[i].date > date) {
            i--;
        }
        i++; // Este es el indice del hijo por el que debemos bajar

        // Antes de bajar, revisamos si ese hijo esta lleno (llegó a 2t - 1)
        if (children[i]->n == 2 * t - 1) {
            // ¡Cirugía! Dividimos al hijo por la mitad usando la función que hicimos antes
            splitChild(i, children[i]);

            // Al dividirse, la llave del medio subió a este nodo.
            // Revisamos si, por casualidad, la llave que subio es la misma fecha que buscamos
            if (keys[i].date == date) {
                keys[i].productsList.insertFront(p);
                return;
            }

            // Decidimos a cual de las dos mitades resultantes debemos bajar
            if (keys[i].date < date) {
                i++;
            }
        }

        // Bajamos recursivamente al hijo correspondiente
        children[i]->insertNonFull(p, date);
    }
}

void BTree::insert(Product* p) {
    std::string date = p->expiry_date;

    // CASO ARBOL VACIO: Creamos la primera raíz
    if (root == nullptr) {
        root = new BTreeNode(t, true);
        root->keys[0] = DateKey(date, p);
        root->n = 1;
    }
    // CASO ARBOL CON DATOS
    else {
        // Raiz llena
        if (root->n == 2 * t - 1) {
            // Creamos una nueva raíz 's' vacía
            BTreeNode* s = new BTreeNode(t, false);

            // Hacemos que la antigua raíz llena sea el primer hijo de 's'
            s->children[0] = root;

            // Dividimos la antigua raiz por la mitad. La llave central subirá a 's' y se crearán dos hermanos.
            s->splitChild(0, root);

            // Ahora 's' tiene 1 llave y 2 hijos. Tenemos que decidir a cual de los dos hijos bajar para insertar la nueva fecha
            int i = 0;
            if (s->keys[0].date < date) {
                i++;
            }
            // Bajamos al hijo correcto y llamamos a la funcion que hicimos en el paso anterior
            s->children[i]->insertNonFull(p, date);

            // s se vuelve la nueva raiz
            root = s;
        }
        else {
            // Si la raíz no está llena, simplemente bajamos y dejamos que 'insertNonFull' haga su trabajo recursivo
            root->insertNonFull(p, date);
        }
    }
}

// funcion recursiva en el nodo
void BTreeNode::searchByDateRange(const std::string& start, const std::string& end) {
    int i = 0;

    // Buscar la primera fecha que sea mayor o igual a 'start'
    while (i < n && keys[i].date < start) {
        i++;
    }

    // Empezamos a recorrer desde ese punto válido
    for (; i < n; i++) {
        // Si no somos hoja, antes de revisar la llave actual, bajamos por su hijo izquierdo porque ahí hay fechas menores
        if (!leaf) {
            children[i]->searchByDateRange(start, end);
        }

        // Si la fecha actual esta dentro del rango
        if (keys[i].date >= start && keys[i].date <= end) {
            std::cout << "\n--- Productos que caducan el " << keys[i].date << " ---\n";
            // Aqui llamamos a la funcion de imprimir de tu propia LinkedList
            keys[i].productsList.print();
        }

        // Si nos pasamos de la fecha final frenamos
        if (keys[i].date > end) {
            return;
        }
    }

    // Si revisamos todas las llaves y no superamos la fecha limite, debemos revisar el ultimo hijo a la derecha
    if (!leaf) {
        children[i]->searchByDateRange(start, end);
    }
}

void BTree::searchByDateRange(const std::string& start, const std::string& end) {
    if (root == nullptr) {
        std::cout << "El arbol esta vacio. No hay fechas registradas.\n";
        return;
    }

    // Validamos que el usuario no haya metido las fechas al revés
    if (start > end) {
        std::cout << "Error: La fecha de inicio debe ser menor o igual a la fecha final.\n";
        return;
    }

    std::cout << "\nBuscando productos entre " << start << " y " << end << "...\n";
    // Iniciamos la recursividad desde la raíz
    root->searchByDateRange(start, end);
    std::cout << "Fin de la busqueda.\n";
}

void BTree::removeProduct(const std::string& date, const std::string& barcode) {
    if (root == nullptr) return;

    BTreeNode* current = root;
    while (current != nullptr) {
        int i = 0;
        while (i < current->n && date > current->keys[i].date) i++;

        if (i < current->n && current->keys[i].date == date) {
            // Encontramos la fecha. Borramos el producto de la lista enlazada.
            current->keys[i].productsList.removeByBarCode(barcode);

            // Si la lista se quedo vacia, borramos la llave (fecha) del Árbol B
            if (current->keys[i].productsList.isEmpty()) {
                removeDate(date);
            }
            return;
        }

        if (current->leaf) return; // No se encontró
        current = current->children[i];
    }
}

void BTree::removeDate(const std::string& date) {
    if (root == nullptr) return;

    // Llamamos a la funcion recursiva del nodo
    root->remove(date);

    // Caso especial: Si despues de borrar, la raíz se quedo sin llaves (n == 0)
    if (root->n == 0) {
        BTreeNode* tmp = root;
        // Si es hoja, el arbol queda vacío. Si no, su único hijo sube a ser la nueva raíz.
        if (root->leaf) {
            root = nullptr;
        } else {
            root = root->children[0];
        }
        delete tmp; // Liberamos la memoria de la antigua raíz
    }
}

int BTreeNode::findKey(const std::string& date) {
    int idx = 0;
    while (idx < n && keys[idx].date < date) ++idx;
    return idx;
}

void BTreeNode::remove(const std::string& date) {
    int idx = findKey(date);

    // CASO 1: La llave esta en ESTE nodo
    if (idx < n && keys[idx].date == date) {
        if (leaf) {
            removeFromLeaf(idx);
        } else {
            removeFromNonLeaf(idx);
        }
    }
    // CASO 2: hay que bajar
    else {
        if (leaf) {
            std::cout << "La fecha " << date << " no existe en el arbol.\n";
            return;
        }

        bool isLastChild = (idx == n);

        // Si el hijo tiene t-1 llaves, lo "rellenamos" antes de bajar.
        if (children[idx]->n < t) {
            fill(idx);
        }

        // Si rellenamos el ultimo hijo y se fusiono con el anterior, idx disminuye
        if (isLastChild && idx > n) {
            children[idx - 1]->remove(date);
        } else {
            children[idx]->remove(date);
        }
    }
}

void BTreeNode::removeFromLeaf(int idx) {
    // Es una hoja, simplemente desplazamos todas las llaves hacia la izquierda para tapar el hueco
    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }
    n--; // Reducimos el contador
}

void BTreeNode::removeFromNonLeaf(int idx) {
    std::string date = keys[idx].date;

    // Caso A: Si el hijo izquierdo tiene suficientes llaves (>= t), le robamos su llave más grande (predecesor)
    if (children[idx]->n >= t) {
        DateKey pred = getPredecessor(idx);
        keys[idx] = pred;
        children[idx]->remove(pred.date);
    }
    // Caso B: Si el hijo derecho tiene suficientes llaves, le robamos su llave más pequeña (sucesor)
    else if (children[idx + 1]->n >= t) {
        DateKey succ = getSuccessor(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ.date);
    }
    // Caso C: Ambos hijos están al mínimo (t-1). Los fusionamos.
    else {
        merge(idx);
        children[idx]->remove(date);
    }
}

// Helpers para Predecesor y Sucesor
DateKey BTreeNode::getPredecessor(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf) {
        cur = cur->children[cur->n]; // A la derecha
    }
    return cur->keys[cur->n - 1];
}

DateKey BTreeNode::getSuccessor(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->leaf) {
        cur = cur->children[0]; // A la izquierda
    }
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    // Si el hermano anterior tiene más de t-1 llaves, le pedimos prestada una
    if (idx != 0 && children[idx - 1]->n >= t) {
        borrowFromPrev(idx);
    }
    // Si el hermano siguiente tiene más de t-1, le pedimos a el
    else if (idx != n && children[idx + 1]->n >= t) {
        borrowFromNext(idx);
    }
    // Si ambos hermanos son pobres, nos fusionamos con uno de ellos
    else {
        if (idx != n) {
            merge(idx); // Fusionar con el siguiente
        } else {
            merge(idx - 1); // Fusionar con el anterior
        }
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];

    // Hacemos espacio en 'child' desplazando a la derecha
    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    // La llave del padre baja al hijo
    child->keys[0] = keys[idx - 1];
    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }

    // La llave más grande del hermano sube al padre
    keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n += 1;
    sibling->n -= 1;
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    // La llave del padre baja al hijo (al final)
    child->keys[child->n] = keys[idx];
    if (!child->leaf) {
        child->children[child->n + 1] = sibling->children[0];
    }

    // La primera llave del hermano sube al padre
    keys[idx] = sibling->keys[0];

    // Movemos en el hermano un espacio a la izquierda para tapar el hueco
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

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    // Bajamos la llave central del padre al hijo (se pone en la posición 't-1')
    child->keys[t - 1] = keys[idx];

    // Copiamos todas las llaves del hermano al hijo
    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + t] = sibling->keys[i];
    }

    // Si no son hojas, tambien copiamos los punteros de los hijos del hermano
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + t] = sibling->children[i];
        }
    }

    // Movemos las llaves y punteros del PADRE un espacio a la izquierda para tapar el hueco de la llave que bajo
    for (int i = idx + 1; i < n; ++i) {
        keys[i - 1] = keys[i];
    }
    for (int i = idx + 2; i <= n; ++i) {
        children[i - 1] = children[i];
    }

    // Ajustamos los contadores
    child->n += sibling->n + 1;
    n--;

    // Se destruye al hermano queda vacio
    delete sibling;
}

void BTree::exportToDot(const std::string& filename) const {
    std::ofstream out(filename);

    if (!out.is_open()) {
        std::cout << "Error: No se pudo crear el archivo " << filename << "\n";
        return;
    }

    out << "digraph BTree {\n";
    // shape=record hace que los nodos sean cajas divididas
    out << "    node [fontname=\"Arial\", shape=record, style=filled, fillcolor=lightyellow];\n";

    if (root == nullptr) {
        out << "    empty [label=\"Arbol B Vacio\"];\n";
    } else {
        generateDotRecursively(root, out);
    }

    out << "}\n";
    out.close();

    std::cout << "Archivo " << filename << " generado con exito.\n";
}

void BTree::generateDotRecursively(BTreeNode* node, std::ofstream& out) const {
    if (node == nullptr) return;

    // Identificador unico del nodo basado en su memoria
    uintptr_t nodeId = reinterpret_cast<uintptr_t>(node);

    // Construimos la "caja" del nodo.
    // El formato sera algo como: <c0> | 2026-04-15 | <c1> | 2026-05-20 | <c2>
    out << "    node" << nodeId << " [label=\"";
    for (int i = 0; i < node->n; i++) {
        // <c...> es el "puerto" de donde saldra la flecha para el hijo izquierdo
        out << "<c" << i << "> | " << node->keys[i].date << " | ";
    }
    // El ultimo puerto para el hijo derecho extremo
    out << "<c" << node->n << ">\"];\n";

    // Si no es hoja, dibujamos las flechas hacia sus hijos
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++) {
            if (node->children[i] != nullptr) {
                uintptr_t childId = reinterpret_cast<uintptr_t>(node->children[i]);

                // Conectamos el puerto especifico (<c0>, <c1>...) con el nodo hijo
                out << "    node" << nodeId << ":c" << i << " -> node" << childId << ";\n";

                // Llamada recursiva
                generateDotRecursively(node->children[i], out);
            }
        }
    }
}