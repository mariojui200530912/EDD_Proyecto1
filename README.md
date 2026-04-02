# Gestion de Catalogo de Supermercado (C++)
# Estructuras de Datos

Este proyecto implementa estructuras de datos en memoria utilizando C++ puro. Demuestra el uso empírico y teórico de múltiples estructuras de datos avanzadas operando de forma atómica sobre un mismo conjunto de registros (Catálogo de Productos).

El sistema incluye un módulo de *benchmarking* para comprobar la complejidad algorítmica (Big-O) de tiempos de inserción, búsqueda y eliminación masiva.

## 🏗️ Arquitectura de Estructuras de Datos

Un único objeto dinámico `Product` es apuntado simultáneamente por:
* **Tabla Hash (Chaining):** Índice principal por Código de Barras $O(1)$.
* **Árbol AVL:** Índice lexicográfico por Nombre de Producto $O(\log n)$.
* **Árbol B (Grado 3):** Índice por Fecha de Caducidad $O(\log n)$.
* **Árbol B+ (Grado 3):** Índice por Categoría con listas enlazadas en hojas para búsquedas de rango iterativas $O(\log n)$.
* **Listas Enlazadas (Secuencial y Ordenada):** Estructuras de control y contrapunto para pruebas de estrés $O(n)$.

## 📂 Estructura de Carpetas Requerida

Para que el programa funcione correctamente, asegúrate de tener la siguiente estructura en la raíz del proyecto antes de ejecutarlo:

```text
/
├── data/           # Coloca aquí tus archivos CSV (ej. dataset_1000.csv)
├── include/        # Archivos de cabecera (.h)
├── src/            # Código fuente (.cpp)
├── reportes/       # Aquí se generan los logs de error y diagramas (.dot / .png)
├── main.cpp        # Punto de entrada
├── CMakeLists.txt 
└── README.md

```

## ⚙️ Requisitos Previos
* Compilador C++: Soporte para C++17 o superior (MinGW/GCC en Windows, Clang en macOS/Linux).

* CMake o Make: Para la construcción del ejecutable.

* Graphviz: Requerido para la Opción 9 del menú. Transforma los archivos .dot en imágenes .png.

* Windows: Instalar desde graphviz.org y agregar al PATH del sistema.

* Linux: sudo apt install graphviz

* macOS: brew install graphviz

## 🚀 Instrucciones de Compilación 
### Opción A: Usando CMake (Recomendado / CLion) Si usas CLion, el IDE compilará automáticamente.
* cd build
* cmake ..
* cmake --build 
### Opción B: Usando Make (Si tienes un Makefile configurado)Abre tu terminal en la raíz del proyecto
* make clean
* make 
## ▶️ Instrucciones de Ejecución
### Una vez compilado, ejecuta el programa desde la terminal.
* En Windows:
```text
./Proyecto1.exe
```
* Si compilaste con CMake dentro de la carpeta build, ejecuta `./build/Proyecto1.exe o Debug/Proyecto1.exe según tu entorno
## 🛠️ Uso del Programa (Menú Principal)
Al iniciar el programa, interactúa con el menú principal:
1. Cargar Catálogo: Te pedirá la ruta del CSV (ej. ../data/dataset_1000.csv). Importante: El archivo CSV debe tener 7 columnas estrictas separadas por comas: Nombre, Codigo, Categoria, Fecha(AAAA-MM-DD), Marca, Precio, Stock.
2. Buscar Producto: Permite búsquedas veloces usando los árboles (por nombre, código, categoría o fechas).
3. Benchmark de Rendimiento: (Opción 8) Realiza pruebas de estrés iterativas. Recomienda usar N=500 y M=50 para observar diferencias claras entre $O(n)$ y $O(\log n)$.
4. Generar Diagramas: (Opción 9) Exporta el estado actual de la memoria a Graphviz en la carpeta /reportes.
## 📝 Notas de Depuración (QA)
* Log de Errores: Si el archivo CSV contiene datos corruptos (precios negativos, fechas inválidas o celdas vacías), el sistema los omitirá automáticamente para proteger los árboles y generará un reporte detallado en reportes/errors.log.
* Diagramas Pesados: Si cargas más de 50 productos, los árboles generados por Graphviz pueden ser demasiado grandes para renderizarse en PNG. Para visualizar las estructuras en imágenes, se recomienda cargar un CSV de prueba con 20 productos.