# Bitácora de Requerimientos y Auditoría IA — Sesión 16
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Firmas de Función y Parámetros  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Registro de prototipos y aridad  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **Tres respuestas distintas:** 1 si la llamada es válida, 0 si la función existe pero con otro número de argumentos y -1 si no existe. Al principio regresaba 0 en los dos casos de error.
> - **Recorrer todo el catálogo:** no me puedo quedar con el primer nombre que coincide, porque puede haber dos firmas con el mismo nombre (por ejemplo `max/2` y `max/3`). Uso una bandera `encontrada` y solo regreso 0 o -1 al final.
> - **Comparar textos:** el nombre va con `strcmp`, no con `==`.
> - **Entradas vacías:** si una entrada del catálogo tiene nombre `NULL`, la salto para no pasarle `NULL` a `strcmp`.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Coincidencia estricta de parámetros |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion16.h ya existe y no
   se puede modificar: struct Funcion { const char *nombre; int aridad; }.
R: Ingeniero de compiladores experto en C y verificación de llamadas.
E: Implementa int verificar_llamada(const Funcion *catalogo, int total,
                                     const char *nombre, int nargs):
   1. Devuelve 1 si la llamada es válida, 0 si la aridad no coincide y -1 si el
      nombre no existe.
   2. La búsqueda es exacta y recorre el catálogo completo.
   3. Una función con cero parámetros solo acepta cero argumentos.
   4. Catálogo NULL, nombre NULL o aridad negativa devuelven -1.
O: Solo el .c, empezando con #include "api_sesion16.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion16.h"
#include <stddef.h>
#include <string.h>

int verificar_llamada(const Funcion *catalogo, int total, const char *nombre, int nargs) {
    if (catalogo == NULL || nombre == NULL || nargs < 0) {
        return -1;
    }
    for (int i = 0; i < total; i++) {
        if (strcmp(catalogo[i].nombre, nombre) == 0) {
            return (catalogo[i].aridad == nargs) ? 1 : 0;
        }
    }
    return -1;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Llamadas con número incorrecto de args

### Salida del harness
```text
=== AUDITORIA SESION 16 - Registro de prototipos y aridad ===
Caso                               esperado:              obtenido:              resultado
[ 1] suma con 3 argumentos              esperado: 1            obtenido: 1            OK
[ 2] suma con 2 argumentos              esperado: 0            obtenido: 0            OK
[ 3] suma con 4 argumentos              esperado: 0            obtenido: 0            OK
[ 4] funcion no registrada              esperado: -1           obtenido: -1           OK
[ 5] main sin argumentos                esperado: 1            obtenido: 1            OK
[ 6] nombre NULL                        esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion16.md
```

* [ ] Caso base y límites superados sin *Crash*. (La versión de la IA se cae con una entrada del catálogo sin nombre; ver reporte.)
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6, pero la IA **regresa en el primer nombre que coincide**, aunque el contrato dice que se recorre el catálogo completo. El harness no lo detecta porque sus catálogos tienen una sola función:
>
> | Caso | IA | Mi artesano |
> | :--- | :--- | :--- |
> | catálogo `{max/2, max/3}`, llamar `max` con 3 | 0 (aridad incorrecta) | 1 (válida) |
> | catálogo `{NULL/0, suma/3}`, llamar `suma` con 3 | se cae (*Segmentation fault*) | 1 |
>
> 1. **Sobrecarga:** la IA ve `max/2`, no coincide la aridad y regresa 0 sin revisar `max/3`. Lo solucioné con una bandera `encontrada`: si alguna firma coincide regreso 1, y solo al terminar decido entre 0 y -1.
> 2. **Nombre `NULL` en el catálogo:** la IA llama `strcmp(NULL, ...)` y truena. En mi versión esa entrada se salta.
>
> **Complejidad:** O(n·L), con n las funciones del catálogo y L el largo del nombre (`strcmp`); memoria O(1).

---
**Firma digital:** jordipech76-boop