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