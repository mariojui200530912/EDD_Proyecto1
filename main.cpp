#include <iostream>
#include <string>
#include "include/CatalogManager.h"

void mostrarMenu()
{
    std::cout << "\n=========================================\n";
    std::cout << "   CATALOGO DE SUPERMERCADO - MENU\n";
    std::cout << "=========================================\n";
    std::cout << "1. Cargar productos desde CSV\n";
    std::cout << "2. Buscar producto por Codigo de Barra (Hash)\n";
    std::cout << "3. Buscar producto por Nombre (AVL)\n";
    std::cout << "4. Buscar productos por rango de fecha de Expiracion\n";
    std::cout << "5. Buscar productos por categoria\n";
    std::cout << "6. Eliminar producto\n";
    std::cout << "7. Listar catalogo ordenado alfabeticamente (AVL In-Order)\n"; // <-- NUEVA
    std::cout << "8. Pruebas comparativas de Busqueda\n"; // <-- Era la 7
    std::cout << "9. Generar visualizacion de los Arboles\n"; // <-- Era la 8
    std::cout << "10. Salir\n"; // <-- Era la 9
    std::cout << "=========================================\n";
    std::cout << "Seleccione una opcion: ";
}

int main()
{
    CatalogManager manager;
    int opcion = 0;
    std::string inputStr;

    while (opcion != 10)
    {
        mostrarMenu();

        // Leemos la opción de forma segura para evitar bucles infinitos si el usuario teclea una letra
        if (!(std::cin >> opcion))
        {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (opcion)
        {
        case 1:
            {
                std::cout << "Ingrese la ruta del archivo CSV (ej. data/dataset.csv): ";
                std::cin >> inputStr;
                manager.loadCatalogFromCSV(inputStr);
                break;
            }
        case 2:
            {
                std::cout << "Ingrese el codigo de barra a buscar: ";
                std::cin >> inputStr;
                Product* p = manager.searchByBarcode(inputStr);
                if (p != nullptr)
                {
                    std::cout << "\n--- Producto Encontrado (Hash O(1)) ---\n";
                    p->print();
                }
                else
                {
                    std::cout << "\nProducto no encontrado.\n";
                }
                break;
            }
        case 3:
            {
                // std::getline nos permite leer nombres con espacios (ej. "Leche Deslactosada")
                std::cout << "Ingrese el nombre del producto: ";
                std::cin.ignore();
                std::getline(std::cin, inputStr);

                Product* p = manager.searchByName(inputStr);
                if (p != nullptr)
                {
                    std::cout << "\n--- Producto Encontrado (AVL O(log n)) ---\n";
                    p->print();
                }
                else
                {
                    std::cout << "\nProducto no encontrado.\n";
                }
                break;
            }
        case 4:
            {
                std::string start, end;
                std::cout << "Ingrese fecha de inicio (AAAA-MM-DD): ";
                std::cin >> start;
                std::cout << "Ingrese fecha de fin (AAAA-MM-DD): ";
                std::cin >> end;

                manager.searchByDateRange(start, end);
                break;
            }
        case 5:
            {
                std::cout << "Ingrese la categoria a buscar (ej. Lacteos, Bebidas): ";
                std::cin >> inputStr;
                manager.searchByCategory(inputStr);
                break;
            }
        case 6:
            {
                std::cout << "Ingrese el codigo de barra del producto a eliminar: ";
                std::cin >> inputStr;
                if (manager.removeProduct(inputStr))
                {
                    std::cout << "\nProducto eliminado de TODAS las estructuras correctamente.\n";
                }
                else
                {
                    std::cout << "\nError: No se encontro el producto para eliminar.\n";
                }
                break;
            }
        case 7:
            {
                std::cout << "\nGenerando listado in-order desde el Arbol AVL...\n";
                manager.printCatalogAlphabetically();
                break;
            }
        case 8:
            {
                int nConsultas, mRepeticiones;
                std::cout << "--- Pruebas de Rendimiento ---\n";
                std::cout << "Ingrese la cantidad de consultas a buscar (N): ";
                std::cin >> nConsultas;
                std::cout << "Ingrese la cantidad de repeticiones para promediar (M): ";
                std::cin >> mRepeticiones;

                manager.runPerformanceTests(nConsultas, mRepeticiones);
                break;
            }
        case 9:
            {
                manager.exportTreesToGraphviz();
                break;
            }
        case 10:
            {
                std::cout << "Saliendo del sistema. Limpiando memoria...\n";
                break;
            }
        default:
            std::cout << "Opcion invalida. Intente de nuevo.\n";
        }
    }

    return 0;
}
