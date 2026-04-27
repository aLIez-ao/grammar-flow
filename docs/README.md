# GrammarFlow

Calculadora de conjuntos **PRIMERO** y **SIGUIENTE** para gramáticas libres de contexto, escrita en C++17. Desarrollada como proyecto de la materia *Compiladores*.

---

## Descripción

GrammarFlow recibe una gramática formal "limpia" (sin recursión por la izquierda) y calcula automáticamente los conjuntos PRIMERO y SIGUIENTE de cada no terminal, mostrando los resultados en una tabla organizada. Es el paso previo para construir tablas de análisis sintáctico LL(1).

El programa puede recibir la gramática desde un **archivo de texto** o ingresarla de forma **interactiva** en consola. Al final de cada sesión pregunta si se desea analizar otra gramática.

---

## Requisitos

| Herramienta | Versión mínima |
| --- | --- |
| g++ / clang++ | C++17 |
| CMake | 3.15 |

En Ubuntu/Debian:

```bash
sudo apt-get install -y cmake g++
```

En macOS con Homebrew:

```bash
brew install cmake
```

---

## Instalación y compilación

```bash
# 1. Clonar o descomprimir el proyecto
cd grammar-flow

# 2. Configurar CMake
cmake -S . -B build \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# 3. Compilar
cmake --build build

# 4. Ejecutar
./build/bin/grammar-flow
```

Después de cualquier cambio en el código, basta con:

```bash
cmake --build build && ./build/bin/grammar-flow
```

### VSCode

Abrir la carpeta del proyecto y usar:

- `Ctrl+Shift+B` → *CMake: Build* para compilar.
- `F5` para compilar y depurar con breakpoints.
- Los archivos `.vscode/tasks.json` y `.vscode/launch.json` ya están configurados.

---

## Formato de entrada

Cada regla ocupa una línea con la forma:

```text
NoTerminal -> simbolo1 simbolo2 ... | alternativa1 alternativa2 ...
```

Reglas adicionales para el mismo no terminal pueden declararse en líneas separadas. Las líneas que comienzan con `#` son comentarios e ignoradas por el parser.

| Convención | Significado |
| --- | --- |
| Símbolo en el LHS de alguna regla | No terminal |
| Todo lo demás | Terminal |
| `EPS` o `null` | Cadena vacía (ε) |
| `$` | Fin de cadena (generado automáticamente en SIGUIENTE) |

### Ejemplo — expresiones aritméticas

```text
E  -> T E'
E' -> + T E' | EPS
T  -> F T'
T' -> * F T' | EPS
F  -> ( E ) | id
```

Este archivo está disponible en `input/expresiones.txt`.

---

## Validaciones

Antes de calcular los conjuntos, el programa aplica dos validaciones:

**Recursión por la izquierda directa** — si una producción comienza con el mismo no terminal del LHS, el programa reporta el error y no continúa:

```text
[Error] Recursion izquierda directa en: E -> E ...
```

**No terminales sin producción propia** — si un símbolo aparece en el RHS de alguna regla clasificado como no terminal pero no tiene producción definida, se reporta como indefinido:

```text
[Error] No-terminal 'X' aparece en las producciones pero no tiene regla propia.
```

---

## Salida esperada

Para la gramática de expresiones aritméticas del ejemplo anterior:

```text
+--------------+----------------------------------------+----------------------------------------+
| No Terminal  | PRIMERO                                | SIGUIENTE                              |
+--------------+----------------------------------------+----------------------------------------+
| E            | { (, id }                              | { $, ) }                               |
| E'           | { +, EPS }                             | { $, ) }                               |
| T            | { (, id }                              | { $, ), + }                            |
| T'           | { *, EPS }                             | { $, ), + }                            |
| F            | { (, id }                              | { $, ), *, + }                         |
+--------------+----------------------------------------+----------------------------------------+
```

---

## Estructura del proyecto

```text
grammar-flow/
├── CMakeLists.txt
├── docs/
│   ├── grammar_flow_architecture.svg
│   └── README.md
│
├── .vscode/
│   ├── tasks.json              # Tareas de build y ejecución
│   ├── launch.json             # Configuración del depurador
│   └── c_cpp_properties.json
│
├── src/
│   ├── main.cpp                # Bucle principal y menú de sesión
│   ├── core/
│   │   ├── grammar.h           # Tipos base: Symbol, Production, Grammar
│   │   └── grammar.cpp
│   │
│   ├── io/
│   │   ├── parser.h            # Lectura desde archivo o consola
│   │   ├── parser.cpp
│   │   ├── display.h           # Impresión de tabla de resultados
│   │   └── display.cpp
│   │
│   └── analysis/
│       ├── validator.h         # Validación de la gramática
│       ├── validator.cpp
│       ├── first_set.h         # Cálculo de conjuntos PRIMERO
│       ├── first_set.cpp
│       ├── follow_set.h        # Cálculo de conjuntos SIGUIENTE
│       └── follow_set.cpp
│
├── tests/
│   ├── expeted/            
│   │   ├── epsilon_chain_expected.txt  
│   │   └── expresiones_expected.txt         
│   │ 
│   └── grammars/
│       ├── epsilon_chain.txt           
│       ├── epsilon_productions.txt  
│       ├── expresiones.txt           
│       ├── left_recursion.txt  
│       ├── sentencias.txt          
│       └── undefined_symbol.txt
│
└── input/
    ├── expresiones.txt         # Gramática de expresiones aritméticas
    └── sentencias.txt          # Gramática con recursión izquierda (ejemplo de error)
```

---

## Algoritmo

Ambos conjuntos se calculan mediante **iteración de punto fijo**: se aplican las reglas de cálculo sobre todas las producciones en un bucle, comparando el tamaño de los conjuntos antes y después de cada vuelta. Cuando ningún conjunto crece en una iteración completa, se alcanzó la estabilidad y el resultado es correcto.

**PRIMERO** sigue las reglas estándar: si un no terminal puede derivar ε, se continúa propagando hacia los símbolos siguientes de la secuencia. **SIGUIENTE** reutiliza la función `firstOfSequence` del módulo `first_set` para calcular PRIMERO del sufijo β en cada producción `A → αBβ`.
