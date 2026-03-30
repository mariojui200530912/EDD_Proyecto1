#include "../include/DataLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Funcion para escribir en el archivo de log de errores
void DataLoader::logError(const std::string& message) {
    // ios::app abre el archivo en modo "append" (añadir al final)
    std::ofstream logFile("errors.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << "\n";
        logFile.close();
    }
}

std::vector<Product*> DataLoader::loadFromCSV(const std::string& filename) {
    std::vector<Product*> buffer;
    std::ifstream file(filename);

    // Verificar existencia del archivo
    if (!file.is_open()) {
        std::cerr << "Error crítico: No se pudo abrir el archivo " << filename << "\n";
        return buffer;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Aviso: El archivo " << filename << " esta completamente vacio.\n";
        logError("Intento de carga con un archivo vacio: " + filename);
        return buffer;
    }

    std::string line;
    int lineNumber = 0;

    // Saltar la cabecera (primera línea)
    if (std::getline(file, line)) {
        lineNumber++;
    }

    // Bucle de lectura iterativa
    while (std::getline(file, line)) {
        lineNumber++;
        std::stringstream ss(line);
        std::string token;

        // Arreglo temporal para guardar los 7 tokens que esperamos
        std::string data[7];
        int column = 0;

        // Extraer tokens separados por comas
        while (std::getline(ss, token, ',') && column < 7) {
            data[column] = token;
            column++;
        }

        // Si no hay exactamente 7 columnas, la línea esta malformada
        if (column != 7) {
            logError("Linea " + std::to_string(lineNumber) + " malformada: Columnas insuficientes/excesivas.");
            continue; // Saltamos a la siguiente iteración sin detener el programa
        }

        try {
            // Validacion de tipos
            // stod convierte string a double; stoi convierte string a int
            double price = std::stod(data[5]);
            int stock = std::stoi(data[6]);

            // Creacion del objeto dinamico
            Product* newProduct = new Product(data[0], data[1], data[2], data[3], data[4], price, stock);
            buffer.push_back(newProduct);

        } catch (const std::invalid_argument& e) {
            logError("Linea " + std::to_string(lineNumber) + " error de tipo: Precio o Stock no son numericos.");
        } catch (const std::out_of_range& e) {
            logError("Linea " + std::to_string(lineNumber) + " error de rango: Numero demasiado grande.");
        }
    }

    file.close();
    std::cout << "Carga finalizada. Productos validos leidos: " << buffer.size() << "\n";
    return buffer;
}