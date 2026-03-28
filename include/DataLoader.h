#ifndef PROYECTO1_DATALOADER_H
#define PROYECTO1_DATALOADER_H

#include <string>
#include <vector>
#include "Product.h"

class DataLoader {
public:
    static std::vector<Product*> loadFromCSV(const std::string& filename);

private:
    static void logError(const std::string& message);
};


#endif //PROYECTO1_DATALOADER_H