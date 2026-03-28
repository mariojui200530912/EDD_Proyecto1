#ifndef PROYECTO1_PRODUCT_H
#define PROYECTO1_PRODUCT_H

#include <string>
#include <iostream>

struct Product
{
    std::string name;
    std::string barcode;
    std::string category;
    std::string expiry_date;
    std::string brand;
    double price;
    int stock;

    Product();
    Product(std::string name, std::string barcode, std::string category, std::string expiry_date, std::string brand, double price, int stock);

    void print() const;
};


#endif //PROYECTO1_PRODUCT_H