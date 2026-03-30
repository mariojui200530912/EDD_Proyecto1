#include "../include/Product.h"

Product::Product() : name(""), barcode(""), category(""), expiry_date(""), brand(""), price(0.0), stock(0) {}

Product::Product(std::string n, std::string bc, std::string cat,
                 std::string exp, std::string br, double p, int s)
    : name(n), barcode(bc), category(cat), expiry_date(exp),
      brand(br), price(p), stock(s) {}

void Product::print() const {
    std::cout << "Codigo: " << barcode
              << " | Nombre: " << name
              << " | Categoria: " << category
              << " | Caducidad: " << expiry_date
              << " | Marca: " << brand
              << " | Precio: $" << price
              << " | Stock: " << stock << "\n";
}