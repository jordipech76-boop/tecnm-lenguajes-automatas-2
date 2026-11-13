# Bitácora de Requerimientos y Auditoría IA — Sesión 11
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Arquitectura de la Tabla de Símbolos  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Tabla Hash con encadenamiento  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> - **djb2:** empieza en 5381 y por cada letra hace `h * 33 + letra`. Hay que convertir el `char` a `unsigned char`, si no las letras con acento salen negativas.
> - **Buscar duplicados:** al principio quería recorrer toda la tabla, pero un nombre igual siempre cae en la misma cubeta, así que basta revisar esa cadena.
> - **El arreglo `nombre[16]`:** solo caben 15 letras. Si el nombre es más largo regreso -1, para no cortarlo.
> - **Encadenar:** el nodo nuevo apunta al que ya estaba en la cubeta (`nuevo->sig = tabla[i]`) y luego toma su lugar; así nunca se pisa otro símbolo.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Función de dispersión y colisiones O(1) |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: C11, gcc -std=c11 -Wall -Wextra -Werror, Linux. api_sesion11.h ya existe y no
   se puede modificar; ahí está struct Simbolo { char nombre[16]; int tipo;
   int offset; struct Simbolo *sig; }.
R: Ingeniero de compiladores experto en C y tablas de símbolos.
E: Implementa unsigned int hash_djb2(const char *clave) e
   int insertar_simbolo(Simbolo *tabla[], unsigned int cubetas,
                        const char *nombre, int tipo, int offset):
   1. djb2: empieza en 5381 y hace hash * 33 + byte.
   2. insertar devuelve 1 si insertó, 0 si el nombre ya existía y -1 con
      tabla NULL, nombre NULL o cero cubetas.
   3. Las colisiones se encadenan; nunca se sobrescribe un nodo.
O: Solo el .c, empezando con #include "api_sesion11.h", sin main() y sin printf.
```

---

## 3. Código Devuelto por el Modelo (Claude)
```c
#include "api_sesion11.h"
#include <stdlib.h>
#include <string.h>

unsigned int hash_djb2(const char *clave) {
    unsigned int hash = 5381u;
    int c;
    while ((c = (unsigned char)*clave++) != 0) {
        hash = ((hash << 5) + hash) + (unsigned int)c;  /* hash * 33 + c */
    }
    return hash;
}

int insertar_simbolo(Simbolo *tabla[], unsigned int cubetas, const char *nombre, int tipo, int offset) {
    if (tabla == NULL || nombre == NULL || cubetas == 0) {
        return -1;
    }

    unsigned int indice = hash_djb2(nombre) % cubetas;

    /* Recorrer la cadena para detectar duplicados */
    for (Simbolo *actual = tabla[indice]; actual != NULL; actual = actual->sig) {
        if (strcmp(actual->nombre, nombre) == 0) {
            return 0;
        }
    }

    Simbolo *nuevo = (Simbolo *)malloc(sizeof(Simbolo));
    if (nuevo == NULL) {
        return -1;
    }
    strncpy(nuevo->nombre, nombre, sizeof(nuevo->nombre) - 1);
    nuevo->nombre[sizeof(nuevo->nombre) - 1] = '\0';
    nuevo->tipo = tipo;
    nuevo->offset = offset;
    nuevo->sig = tabla[indice];
    tabla[indice] = nuevo;
    return 1;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Manejo de claves duplicadas y colisiones

### Salida del harness
```text
=== AUDITORIA SESION 11 - Tabla hash con encadenamiento ===
Caso                               esperado:              obtenido:              resultado
[ 1] hash_djb2("")                      esperado: 5381         obtenido: 5381         OK
[ 2] hash_djb2("abc")                   esperado: 193485963    obtenido: 193485963    OK
[ 3] insertar x dos veces               esperado: 0            obtenido: 0            OK
[ 4] dos claves, una cubeta             esperado: 2            obtenido: 2            OK
[ 5] offset de beta en cadena           esperado: 4            obtenido: 4            OK
[ 6] insertar en tabla NULL             esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion11.md
```

* [ ] Caso base y límites superados sin *Crash*. (La versión de la IA se cae con `hash_djb2(NULL)`; ver reporte.)
* [ ] No existen accesos fuera de límites ni *Memory Leaks*. (Fugas: el contrato no trae función para liberar la tabla y el harness nunca hace `free`; Valgrind marca 96 bytes perdidos.)
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Artesano e IA sacaron 6/6. Probé casos que el harness no tiene y la IA falla en dos:
>
> 1. **Nombres de más de 15 letras:** la IA los corta con `strncpy` y guarda `"contador_princi"`, pero luego compara con el nombre completo, que nunca es igual. Resultado: insertó **3 veces** el mismo nombre cortado.
>
>    ```text
>    insertar "contador_principal" dos veces -> 1, 1   (debería ser 1, 0)
>    insertar "contador_principalB"         -> 1      (queda igual que el otro)
>    nodos en la tabla: 3, los tres guardados como "contador_princi"
>    ```
>
>    Mi artesano revisa el largo antes y regresa -1, así nunca guarda un nombre cortado.
> 2. **`hash_djb2(NULL)`:** la IA no revisa el puntero y se cae con *Segmentation fault*. El mío regresa 5381.
>
> **Complejidad:** `hash_djb2` es O(L) (largo del nombre); insertar es O(1) en promedio y O(n) en el peor caso, si todos caen en la misma cubeta.

---
**Firma digital:** jordipech76-boop