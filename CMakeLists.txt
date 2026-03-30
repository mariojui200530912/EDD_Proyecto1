cmake_minimum_required(VERSION 4.1)
project(Proyecto1)

set(CMAKE_CXX_STANDARD 26)

add_executable(Proyecto1 main.cpp
        src/Product.cpp
        include/Product.h
        include/DataLoader.h
        src/DataLoader.cpp
        include/LinkedList.h
        src/LinkedList.cpp
        include/HashTable.h
        src/HashTable.cpp
        include/AVLTree.h
        src/AVLTree.cpp
        include/CatalogManager.h
        src/CatalogManager.cpp
        include/BTree.h
        src/BTree.cpp
        include/BPlusTree.h
        src/BPlusTree.cpp)
