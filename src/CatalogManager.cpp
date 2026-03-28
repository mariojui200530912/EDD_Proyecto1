#include "../include/CatalogManager.h"
#include <iostream>
#include <chrono>

CatalogManager::CatalogManager() {

}

CatalogManager::~CatalogManager() {

}

void CatalogManager::loadCatalogFromCSV(const std::string& filename) {
    std::cout << "Iniciando carga del catalogo...\n";

    // 1. Usamos el DataLoader para obtener el buffer temporal
    std::vector<Product*> buffer = DataLoader::loadFromCSV(filename);

    int successCount = 0;
    int duplicateCount = 0;

    // Intentamos insertar cada producto del buffer en nuestro sistema
    for (Product* p : buffer) {
        if (addProduct(p)) {
            successCount++;
        } else {
            // Si addProduct devuelve false, era un duplicado. Destruimos el objeto de la memoria RAM porque ninguna estructura lo guardó.
            delete p;
            duplicateCount++;
        }
    }

    std::cout << "Carga finalizada en las estructuras.\n";
    std::cout << "Insertados con exito: " << successCount << "\n";
    std::cout << "Omitidos por codigo duplicado: " << duplicateCount << "\n";
}

bool CatalogManager::addProduct(Product* p) {
    // Validar unicidad usando la Tabla Hash (O(1))
    if (!hashTable.insert(p)) {
        return false; // El codigo ya existe, abortamos la insercion en las demas
    }

    // Si pasó la prueba, propagamos la insercion a las demas estructuras
    sequentialList.insertFront(p); // O(1)
    orderedList.insertSorted(p);   // O(n)
    avlTree.insert(p);             // O(log n)

    // bTree.insert(p); ... más adelante
    // bPlusTree.insert(p); ... más adelante

    return true; // Inserción atomizada exitosa
}

// Busquedas ultra rapidas delegando a la estructura correcta
Product* CatalogManager::searchByBarcode(const std::string& barcode) const {
    return hashTable.search(barcode);
}

Product* CatalogManager::searchByName(const std::string& name) const {
    return avlTree.searchByName(name);
}

bool CatalogManager::removeProduct(const std::string& barcode) {
    Product* p = hashTable.search(barcode);
    if (p == nullptr) return false;
    std::string nameToRemove = p->name;

    // --- 1. Cronometrar la Tabla Hash ---
    auto startHash = std::chrono::high_resolution_clock::now();
    hashTable.remove(barcode);
    auto endHash = std::chrono::high_resolution_clock::now();
    auto durationHash = std::chrono::duration_cast<std::chrono::microseconds>(endHash - startHash).count();

    // --- 2. Cronometrar la Lista Enlazada ---
    auto startList = std::chrono::high_resolution_clock::now();
    sequentialList.removeByBarCode(barcode);
    auto endList = std::chrono::high_resolution_clock::now();
    auto durationList = std::chrono::duration_cast<std::chrono::microseconds>(endList - startList).count();

    // (Omitimos la lista ordenada aquí para no hacer largo el ejemplo)
    orderedList.removeByBarCode(barcode);

    // --- 3. Cronometrar el Árbol AVL ---
    auto startAVL = std::chrono::high_resolution_clock::now();
    avlTree.remove(nameToRemove);
    auto endAVL = std::chrono::high_resolution_clock::now();
    auto durationAVL = std::chrono::duration_cast<std::chrono::microseconds>(endAVL - startAVL).count();

    delete p; // Limpieza final

    // --- Imprimir o guardar los resultados individuales ---
    std::cout << "Tiempos de Eliminacion aislados:\n";
    std::cout << "Hash (O(1)): " << durationHash << " us\n";
    std::cout << "Lista (O(n)): " << durationList << " us\n";
    std::cout << "AVL (O(log n)): " << durationAVL << " us\n";

    return true;
}