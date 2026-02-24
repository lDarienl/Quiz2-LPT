# Quiz2-LPT

**Quiz #2 de Lenguajes de Programación y Transducción** — Implementación de un **AFN (autómata finito no determinista)** en **Python** y **C**.

---

## 1. Enunciado del problema

Implementar un **autómata finito no determinista** que reconozca las siguientes expresiones regulares, en **Python** y en **C**:

| Expresión regular | Token |
|-------------------|--------|
| `+`               | **SUMA** |
| `++`              | **INCR** |
| `[A-Z][a-z0-9]*`  | **ID** (identificador: una mayúscula seguida de cero o más minúsculas o dígitos) |

- **Entrada:** archivo de texto (`.txt`).
- **Salida:** por pantalla, imprimiendo cada token reconocido y su lexema. Si una palabra no es aceptada por el autómata, se imprime **NO ACEPTA** y el programa termina.

**Ejemplo de entrada** (en un archivo `entrada.txt`):

```
+ ++ Abc x1
```

**Ejemplo de salida:**

```
SUMA	+
INCR	++
ID	Abc
NO ACEPTA
```

---

## 2. Enfoque de la solución

Se implementa un **AFN** con **máximo avance** (longest match): para cada lexema se avanza carácter a carácter, se mantienen los conjuntos de estados y en cada paso se registra el último token aceptado. Al final del lexema (o al llegar a un estado sin transiciones), se emite ese último token y el lexema correspondiente.

### Estados del autómata

- **0:** estado inicial.
- **1:** se leyó un `+` (acepta **SUMA**).
- **2:** se leyeron `++` (acepta **INCR**).
- **3:** se leyó una mayúscula (acepta **ID** de un solo carácter).
- **4:** se leyó una mayúscula seguida de una o más minúsculas/dígitos (acepta **ID**).

### Transiciones

- Desde **0:** con `+` → {1}, con `[A-Z]` → {3}.
- Desde **1:** con `+` → {2}.
- Desde **3:** con `[a-z0-9]` → {4}.
- Desde **4:** con `[a-z0-9]` → {4}.

Prioridad al elegir token: **INCR** > **SUMA** > **ID**.

---

## 3. Implementación en C (`afoc.c`)

### 3.1 Estructura del código

| Sección | Líneas (aprox.) | Contenido |
|--------|------------------|------------|
| Includes y constante | 1–6 | `stdio`, `stdlib`, `ctype`, `string.h`, `START = 0` |
| Conjunto de estados (bitset) | 8–22 | `set_add`, `set_has`, `set_upper`, `set_lower` |
| Función de transición | 24–45 | `move(state, c)` → nuevo conjunto de estados |
| Token preferido | 47–53 | `best_token(state)` → "INCR", "SUMA", "ID" o NULL |
| Lectura de archivo | 55–74 | `read_file(path, &n)` → buffer y longitud |
| `main` — análisis por lexemas | 76–127 | Lectura, bucle por lexemas, impresión, "NO ACEPTA" |

### 3.2 Cómo funciona

- El **conjunto de estados** se representa con un **entero sin signo** (bitset): el bit `i` indica si el estado `i` está activo.
- `move(state, c)` recorre los estados activos y, según el carácter `c`, activa los nuevos estados en `next`.
- Para cada **lexema** (palabra separada por espacios):
  - Se parte del estado inicial {0}.
  - Se avanza `j` carácter a carácter, actualizando `state = move(state, text[j])`.
  - En cada paso, si `best_token(state)` no es NULL, se guarda como `best` y `best_end = j+1`.
  - Si en algún momento el conjunto de estados queda vacío, se sale del bucle.
- Al terminar el lexema:
  - Si no hubo ningún token aceptado (`best == NULL`), se imprime **NO ACEPTA**, se libera memoria y se sale.
  - Si no, se imprime el token `best` y el lexema `text[i..best_end]`, y se avanza `i = best_end`.

### 3.3 Requisitos para ejecutar

- Compilador **C99** (o posterior): **GCC**, **Clang** o **MSVC** (Visual Studio).
- Solo se usan la biblioteca estándar (sin dependencias externas).

### 3.4 Cómo compilar y ejecutar

**Windows (PowerShell o CMD):**

```powershell
# Con GCC (MinGW, etc.):
gcc -o afoc afoc.c
.\afoc entrada.txt

# Con Clang:
clang -o afoc afoc.c
.\afoc entrada.txt

# Con MSVC (Developer Command Prompt):
cl afoc.c /Fe:afoc.exe
afoc entrada.txt
```

**Linux (Ubuntu/Debian):**

```bash
gcc -o afoc afoc.c
./afoc entrada.txt
```

**macOS:**

```bash
gcc -o afoc afoc.c
./afoc entrada.txt
# o con Clang (por defecto en Xcode):
clang -o afoc afoc.c
./afoc entrada.txt
```

En todos los sistemas el primer argumento es la ruta al archivo `.txt` de entrada.

---

## 4. Implementación en Python (`afopy.py`)

### 4.1 Estructura del código

| Sección | Líneas (aprox.) | Contenido |
|--------|------------------|------------|
| Imports y constantes | 1–11 | `sys`, `re` (no usado), `START`, diccionario `ACCEPT` |
| Clasificación de caracteres | 13–18 | `is_upper(c)`, `is_lower(c)` para `[A-Z]` y `[a-z0-9]` |
| Función de transición | 19–36 | `move(state, c)` con `set` de estados |
| Token preferido | 38–46 | `best_token(current_state)` → "INCR", "SUMA", "ID" o None |
| Análisis léxico | 48–81 | `token(text)`: recorre lexemas, imprime token y lexema o "NO ACEPTA" |
| Punto de entrada | 83–95 | `main()`: argumentos, lectura del archivo, llamada a `token()` |

### 4.2 Cómo funciona

- El **conjunto de estados** es un `set` de enteros.
- `move(state, c)` crea un nuevo conjunto `next` y, para cada estado en `state`, aplica las transiciones según `c` (igual que en el diseño del AFN).
- La función `token(text)`:
  - Recorre el texto con índice `i`.
  - Salta espacios.
  - Para cada lexema (desde `i` hasta el siguiente espacio), avanza `j` actualizando `state = move(state, text[j])` y guardando el último token aceptado en `best` y `best_end`.
  - Si al terminar el lexema no hay token aceptado, imprime **NO ACEPTA** y hace `return`.
  - Si hay token, imprime `best` y el lexema, y avanza `i = best_end`.

Misma prioridad que en C: INCR > SUMA > ID.

### 4.3 Requisitos para ejecutar

- **Python 3.7+** (para anotaciones tipo y `str | None` en versiones recientes; en 3.9 o 3.10 puede ser necesario ajustar sintaxis según el entorno).
- Solo biblioteca estándar; el módulo `re` está importado pero no se usa (el reconocimiento es por AFN).

### 4.4 Cómo ejecutar

**Windows (PowerShell o CMD):**

```powershell
python afopy.py entrada.txt
# o, si tiene varias versiones de Python:
python3 afopy.py entrada.txt
py -3 afopy.py entrada.txt
```

**Linux (Ubuntu/Debian):**

```bash
python3 afopy.py entrada.txt
```

**macOS:**

```bash
python3 afopy.py entrada.txt
```

En todos los casos, el único argumento es la ruta al archivo `.txt` de entrada.

---

## 5. Resumen: por qué está construido así

- **AFN en lugar de expresiones regulares directas:** para cumplir el enunciado (autómata finito) y mostrar explícitamente estados y transiciones.
- **Conjunto de estados:** en C con bitset (eficiente en memoria y en tiempo); en Python con `set` (claro y portable).
- **Máximo avance:** se elige siempre el último token aceptado al avanzar, de modo que `++` se reconoce como INCR y no como dos SUMA.
- **Prioridad INCR > SUMA > ID:** evita ambigüedad cuando varios tokens podrían aceptar el mismo prefijo.
- **Entrada por archivo .txt y salida por pantalla:** según el enunciado; la salida es la misma en ambas implementaciones para facilitar comparación y pruebas.

---

## 6. Ejemplo rápido en cualquier sistema

1. Crea un archivo `entrada.txt` con una línea, por ejemplo:
   ```
   + ++ Abc x1
   ```
2. **C:** compila y ejecuta con el comando correspondiente a tu SO (secciones 3.4 y 4.4).
3. **Python:** ejecuta `python` o `python3` con `afopy.py` y la ruta a `entrada.txt`.

La salida esperada es:

```
SUMA	+
INCR	++
ID	Abc
NO ACEPTA
```

(El programa termina al encontrar el primer lexema no aceptado, en este caso `x1`.)
