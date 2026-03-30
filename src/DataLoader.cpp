#include "../include/DataLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

inline bool isValidString(const std::string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) return true; // Hay al menos una letra/numero
    }
    return false;
}

inline bool isValidDate(const std::string& date) {
    std::string cleanDate = date;
    cleanDate.erase(std::remove(cleanDate.begin(), cleanDate.end(), '\r'), cleanDate.end());
    cleanDate.erase(std::remove(cleanDate.begin(), cleanDate.end(), '\n'), cleanDate.end());

    if (cleanDate.length() != 10) return false;
    if (cleanDate[4] != '-' || cleanDate[7] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(cleanDate[i]))) return false;
    }
    return true;
}


// Funcion para escribir en el archivo de log de errores
void DataLoader::logError(const std::string& message) {
    std::ofstream logFile("errors.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << message << "\n";
        logFile.close();
    }
}

std::vector<Product*> DataLoader::loadFromCSV(const std::string& filename) {
    std::vector<Product*> products;
    std::ifstream file(filename);

    std::ofstream errorLog("reportes/errors.log");
    if (!errorLog.is_open()) {
        std::cout << "Advertencia: No se pudo crear el archivo de log de errores.\n";
    } else {
        errorLog << "=== LOG DE ERRORES DE CARGA CSV ===\n";
        errorLog << "Archivo fuente: " << filename << "\n\n";
    }
    // -------------------------------------------

    if (!file.is_open()) {
        std::cout << "Error: No se pudo abrir el archivo CSV: " << filename << "\n";
        return products;
    }

    std::string line;
    int lineNumber = 1;

    // Saltamos la cabecera
    std::getline(file, line);

    while (std::getline(file, line)) {
        lineNumber++;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string name, barcode, category, expiry_date, brand, priceStr, stockStr;

        // 1. Nombre
        std::getline(ss, name, ',');
        // 2. Codigo de Barras
        std::getline(ss, barcode, ',');
        // 3. Categoria
        std::getline(ss, category, ',');
        // 4. Fecha de Expiracion
        std::getline(ss, expiry_date, ',');
        // 5. Marca
        std::getline(ss, brand, ',');
        // 6. Precio
        std::getline(ss, priceStr, ',');
        // 7. Stock
        std::getline(ss, stockStr);

        // --- VALIDACIONES ESTRICTAS ---
        bool hasError = false;

        if (!isValidString(brand)) {
            if (errorLog.is_open()) errorLog << "Linea " << lineNumber << " [ERROR]: La marca (brand) esta vacia o es invalida.\n";
            hasError = true;
        }
        if (!isValidDate(expiry_date)) {
            if (errorLog.is_open()) errorLog << "Linea " << lineNumber << " [ERROR]: Formato de fecha invalido (" << expiry_date << "). Debe ser AAAA-MM-DD.\n";
            hasError = true;
        }

        if (hasError) continue; // Si hay error, saltamos esta linea

        // Limpiamos la fecha de saltos de linea extraños por si acaso
        expiry_date.erase(std::remove(expiry_date.begin(), expiry_date.end(), '\r'), expiry_date.end());
        expiry_date.erase(std::remove(expiry_date.begin(), expiry_date.end(), '\n'), expiry_date.end());

        try {
            double price = std::stod(priceStr);
            int stock = std::stoi(stockStr);

            if (price < 0.0 || stock < 0) {
                if (errorLog.is_open()) {
                    errorLog << "Linea " << lineNumber << " [ERROR]: El precio (" << price << ") o el stock (" << stock << ") no pueden ser numeros negativos.\n";
                }
                continue; // Si son negativos, abortamos la creacion de este producto
            }

            // Creación del objeto (asegúrate de que el orden coincida con tu constructor)
            Product* p = new Product(name, barcode, category, expiry_date, brand, price, stock);
            products.push_back(p);

        } catch (const std::exception& e) {
            if (errorLog.is_open()) {
                errorLog << "Linea " << lineNumber << " [ERROR]: Precio (" << priceStr << ") o Stock (" << stockStr << ") no son numeros validos.\n";
            }
        }
    }

    file.close();

    if (errorLog.is_open()) {
        errorLog << "\nFin del reporte.\n";
        errorLog.close();
    }

    return products;
}