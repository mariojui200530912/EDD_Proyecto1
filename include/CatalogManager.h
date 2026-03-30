#ifndef PROYECTO1_CATALOGMANAGER_H
#define PROYECTO1_CATALOGMANAGER_H

#include "DataLoader.h"
#include "LinkedList.h"
#include "HashTable.h"
#include "AVLTree.h"
#include "BTree.h"
#include "BPlusTree.h"
#include <string>
#include <random>
#include <cstdlib>
#include <vector>

class CatalogManager {
private:
    // Estructuras de Datos
    LinkedList sequentialList; // Lista no ordenada
    LinkedList orderedList;    // Lista ordenada alfabéticamente
    HashTable hashTable;       // Busqueda por codigo de barra
    AVLTree avlTree;           // Busqueda por nombre
    BTree bTree;               // Busqueda por rango de fechas
    BPlusTree bPlusTree;       // Busqueda por categoria

public:
    CatalogManager();
    ~CatalogManager();

    // Carga inicial masiva
    void loadCatalogFromCSV(const std::string& filename);

    // Operaciones CRUD que orquestan a todas las estructuras
    bool addProduct(Product* p);
    bool removeProduct(const std::string& barcode);

    // Busquedas delegadas a la estructura mas eficiente
    Product* searchByBarcode(const std::string& barcode) const;
    Product* searchByName(const std::string& name) const;
    void searchByDateRange(const std::string& start, const std::string& end);
    void searchByCategory(const std::string& barcode) const;

    // Funciones adicionales
    void printCatalogAlphabetically() const;
    void runPerformanceTests(int N, int M) const;
    void exportTreesToGraphviz() const;

};
#endif //PROYECTO1_CATALOGMANAGER_H