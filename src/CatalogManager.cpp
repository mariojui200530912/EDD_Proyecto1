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
    bTree.insert(p);               // O(log n)
    bPlusTree.insert(p);

    return true; // Insercion atomizada exitosa
}

Product* CatalogManager::searchByBarcode(const std::string& barcode) const {
    auto start = std::chrono::high_resolution_clock::now();
    Product* p = hashTable.search(barcode);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (p != nullptr) {
        std::cout << "\n[Exito: Producto encontrado en " << duration << " us | Tabla Hash O(1)]\n";
        // Nota: Asumo que en tu main.cpp haces algo como: if(p) p->print();
    } else {
        std::cout << "\n[Fallo: Producto no encontrado | Tiempo: " << duration << " us]\n";
    }
    return p;
}

Product* CatalogManager::searchByName(const std::string& name) const {
    auto start = std::chrono::high_resolution_clock::now();
    Product* p = avlTree.searchByName(name);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (p != nullptr) {
        std::cout << "\n[Exito: Producto encontrado en " << duration << " us | Arbol AVL O(log n)]\n";
    } else {
        std::cout << "\n[Fallo: Producto no encontrado | Tiempo: " << duration << " us]\n";
    }
    return p;
}

void CatalogManager::searchByDateRange(const std::string& start, const std::string& end) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // La función interna del Árbol B ya imprime los productos
    bTree.searchByDateRange(start, end);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

    std::cout << "\n[Busqueda por rango completada en " << duration << " us | Arbol B]\n";
}

void CatalogManager::searchByCategory(const std::string& category) const {
    auto start = std::chrono::high_resolution_clock::now();

    // La función interna del Árbol B+ ya imprime la lista secuencial
    bPlusTree.searchByCategory(category);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "\n[Busqueda por categoria completada en " << duration << " us | Arbol B+]\n";
}

bool CatalogManager::removeProduct(const std::string& barcode) {
    Product* p = hashTable.search(barcode);
    if (p == nullptr) {
        std::cout << "Error: No se encontro el producto con codigo " << barcode << ".\n";
        return false;
    }

    // Rescatamos los atributos antes de borrar el objeto
    std::string nameToRemove = p->name;
    std::string dateToRemove = p->expiry_date;
    std::string categoryToRemove = p->category;

    // Cronometramos y eliminamos en las estructuras base
    auto startHash = std::chrono::high_resolution_clock::now();
    hashTable.remove(barcode);
    auto endHash = std::chrono::high_resolution_clock::now();
    auto durationHash = std::chrono::duration_cast<std::chrono::microseconds>(endHash - startHash).count();

    auto startList = std::chrono::high_resolution_clock::now();
    sequentialList.removeByBarCode(barcode);
    auto endList = std::chrono::high_resolution_clock::now();
    auto durationList = std::chrono::duration_cast<std::chrono::microseconds>(endList - startList).count();

    auto startListOrdered = std::chrono::high_resolution_clock::now();
    orderedList.removeByBarCode(barcode);
    auto endListOrdered = std::chrono::high_resolution_clock::now();
    auto durationListOrdered = std::chrono::duration_cast<std::chrono::microseconds>(endListOrdered - startListOrdered).count();

    auto startAVL = std::chrono::high_resolution_clock::now();
    avlTree.remove(nameToRemove);
    auto endAVL = std::chrono::high_resolution_clock::now();
    auto durationAVL = std::chrono::duration_cast<std::chrono::microseconds>(endAVL - startAVL).count();

    auto startBTree = std::chrono::high_resolution_clock::now();
    bTree.removeProduct(dateToRemove, barcode);
    auto endBTree = std::chrono::high_resolution_clock::now();
    auto durationBTree = std::chrono::duration_cast<std::chrono::microseconds>(endBTree - startBTree).count();

    auto startBPlusTree = std::chrono::high_resolution_clock::now();
    bPlusTree.removeProduct(categoryToRemove, barcode);
    auto endBPlusTree = std::chrono::high_resolution_clock::now();
    auto durationBPlusTree = std::chrono::duration_cast<std::chrono::microseconds>(endBPlusTree - startBPlusTree).count();

    // 4. Liberamos la memoria del objeto
    delete p;

    // Reporte de tiempos
    std::cout << "\n--- Producto Eliminado Exitosamente ---\n";
    std::cout << "Tiempos de re-estructuracion:\n";
    std::cout << " - Hash (O(1))    : " << durationHash << " us\n";
    std::cout << " - Lista (O(n))   : " << durationList << " us\n";
    std::cout << " - Lista Ordenada : " << durationListOrdered << " us\n";
    std::cout << " - AVL (O(log n)) : " << durationAVL << " us\n";
    std::cout << " - BTree (O(n))   : " << durationBTree << " us\n";
    std::cout << " - BPlusTree (O(log n)) : " << durationBPlusTree << " us\n";

    return true;
}

void CatalogManager::printCatalogAlphabetically() const
{
    avlTree.printInOrder();
}

void CatalogManager::exportTreesToGraphviz() const {
    std::string folderName = "reportes";
    std::string path = folderName + "/";

    std::string mkdirCmd = "if not exist " + folderName + " mkdir " + folderName;
    system(mkdirCmd.c_str());

    std::cout << "1. Generando archivos .dot en la carpeta '" << path << "'...\n";

    // Le pegamos la ruta al nombre del archivo
    avlTree.exportToDot(path + "arbol_avl.dot");
    bTree.exportToDot(path + "arbol_b_fechas.dot");
    bPlusTree.exportToDot(path + "arbol_bplus_categorias.dot");

    std::cout << "2. Intentando convertir a imagenes PNG usando Graphviz...\n";

    std::string cmdAvl = "dot -Tpng " + path + "arbol_avl.dot -o " + path + "arbol_avl.png";
    std::string cmdB = "dot -Tpng " + path + "arbol_b_fechas.dot -o " + path + "arbol_b_fechas.png";
    std::string cmdBPlus = "dot -Tpng " + path + "arbol_bplus_categorias.dot -o " + path + "arbol_bplus_categorias.png";

    int resAvl = system(cmdAvl.c_str());
    int resB = system(cmdB.c_str());
    int resBPlus = system(cmdBPlus.c_str());

    if (resAvl == 0 && resB == 0 && resBPlus == 0) {
        std::cout << "\n¡Exito rotundo! Las imagenes PNG se han guardado en la carpeta '" << folderName << "'.\n";
    } else {
        std::cout << "\n[Aviso]: Se crearon los .dot en '" << folderName << "', pero no se pudieron generar los PNG automaticamente.\n";
        std::cout << "Verifica que Graphviz este instalado en Windows.\n";
    }
}

void CatalogManager::runPerformanceTests(int N, int M) const {
    // OBTENER TODOS LOS DATOS
    std::vector<std::string> allNames = sequentialList.getAllNames();
    if (allNames.empty()) {
        std::cout << "El catalogo esta vacio.\n";
        return;
    }

    // PREPARAR LOS 4 ESCENARIOS
    // A. Busquedas Aleatorias Exitosas
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<std::string> randomNames = allNames;
    std::shuffle(randomNames.begin(), randomNames.end(), g); // Revolvemos
    if (randomNames.size() > N) {
        randomNames.resize(N); // Cortamos solo los N que pidió el usuario
    }

    // B. Busqueda Fallida
    std::vector<std::string> fakeNames(N, "ZZZ_PRODUCTO_INEXISTENTE_999");

    // C. y D. Extremos del AVL
    std::vector<std::string> firstExtreme = { avlTree.getFirstAlphabetical() };
    std::vector<std::string> lastExtreme = { avlTree.getLastAlphabetical() };

    std::cout << "\n=================================================================\n";
    std::cout << " INICIANDO BENCHMARK: N = " << N << " consultas | M = " << M << " repeticiones\n";
    std::cout << "=================================================================\n";

    // Medir Tiempos
    auto measureTime = [&](const std::vector<std::string>& targets, int structType) -> double {
        double totalAvg = 0;
        for (int m = 0; m < M; m++) {
            auto start = std::chrono::high_resolution_clock::now();

            for (const auto& name : targets) {
                if (structType == 1) sequentialList.searchByName(name);
                else if (structType == 2) orderedList.searchByName(name);
                else if (structType == 3) avlTree.searchByName(name);
            }

            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            totalAvg += (duration / targets.size());
        }
        return totalAvg / M;
    };

    // EJECUCION Y REPORTE
    std::cout << "--- 1. CASO: BUSQUEDA EXITOSA ALEATORIA ---\n";
    std::cout << "Lista No Ordenada: " << measureTime(randomNames, 1) << " us\n";
    std::cout << "Lista Ordenada   : " << measureTime(randomNames, 2) << " us\n";
    std::cout << "Arbol AVL        : " << measureTime(randomNames, 3) << " us\n\n";

    std::cout << "--- 2. CASO: BUSQUEDA FALLIDA ---\n";
    std::cout << "Lista No Ordenada: " << measureTime(fakeNames, 1) << " us (Recorre TODO buscando)\n";
    std::cout << "Lista Ordenada   : " << measureTime(fakeNames, 2) << " us (Frena por orden alfabetico)\n";
    std::cout << "Arbol AVL        : " << measureTime(fakeNames, 3) << " us (Baja rapido a una hoja vacia)\n\n";

    std::cout << "--- 3. CASO: EXTREMO INFERIOR (Primero: " << firstExtreme[0] << ") ---\n";
    std::cout << "Lista No Ordenada: " << measureTime(firstExtreme, 1) << " us\n";
    std::cout << "Lista Ordenada   : " << measureTime(firstExtreme, 2) << " us\n";
    std::cout << "Arbol AVL        : " << measureTime(firstExtreme, 3) << " us\n\n";

    std::cout << "--- 4. CASO: EXTREMO SUPERIOR (Ultimo: " << lastExtreme[0] << ") ---\n";
    std::cout << "Lista No Ordenada: " << measureTime(lastExtreme, 1) << " us\n";
    std::cout << "Lista Ordenada   : " << measureTime(lastExtreme, 2) << " us\n";
    std::cout << "Arbol AVL        : " << measureTime(lastExtreme, 3) << " us\n";
    std::cout << "=================================================================\n";
}