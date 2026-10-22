# Bitácora de Requerimientos y Auditoría IA — Sesión 01
- **Materia:** Lenguajes y Autómatas II (SCD-1016)
- **Tema:** Encuadre, Diagnóstico de Punteros y Metodología IA
- **Docente Titular:** MCC. Iván Márquez Larios (ijmarquezl)
- **Estudiante:** Pech Ramírez Jordi Geovany
- **Matrícula:** 24531319
- **Track:** C

---

## 1. Fase 1: El Artesano (Dificultad Manual)
**Reto:** Inversión de buffer in-place (char*)

**Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:**

Lo hice con dos índices, uno al inicio y otro al final, intercambiando con un char temporal hasta que se cruzan. Lo que más me costó fue el caso de la cadena vacía: como `strlen` regresa un `size_t` (sin signo), si la cadena mide 0 entonces `len - 1` no da -1, sino un número gigante, y el índice final se sale del buffer. Lo arreglé checando `len < 2` antes de empezar. También tuve que agregar la validación de `NULL`, porque si no, `strlen(NULL)` truena el programa.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
|---|---|
| Contexto (C) | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| Rol (R) | Ingeniero de software de sistemas y compiladores |
| Especificación (E) | Punteros, O(n) tiempo, O(1) memoria |
| Output (O) | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
> Actúa como ingeniero de software de sistemas y compiladores. En C99 con GCC y sin bibliotecas no estándar, escribe la función `void invertir(char *s)` que invierta un buffer char* in-place usando aritmética de punteros, en O(n) tiempo y O(1) memoria. No incluyas main(). Devuelve solo el código modular y un análisis breve de complejidad.

---

## 3. Código Devuelto por el Modelo
```c
/*
 * Invierte un buffer char* in-place usando dos punteros.
 * Tiempo:  O(n) -> una pasada para hallar el final + n/2 intercambios
 * Memoria: O(1) -> solo dos punteros y un char temporal
 */
void invertir(char *s) {
    if (!s || !*s) return;            // NULL o cadena vacia: nada que hacer

    char *izq = s;
    char *der = s;
    while (*(der + 1) != '\0') der++; // der queda en la ultima letra

      while (izq < der) {
        char tmp = *izq;
        *izq++ = *der;
        *der-- = tmp;
    }
}
````

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
**Pruebas ejecutadas:** Valores NULL, buffers vacíos y longitudes impares

- [x] Caso base y límites superados sin Crash.
- [x] No existen accesos fuera de límites ni Memory Leaks.
- [x] La complejidad teórica coincide con la implementada.

**Salida del harness:**
**Versión Artesano:**
```
=== AUDITORIA SESION 01 - Inversion de buffer in-place ===
[ 1] invertir("HOLA")     esperado: ALOH        obtenido: ALOH        OK
[ 2] invertir("abc")      esperado: cba         obtenido: cba         OK
[ 3] invertir("")         esperado:             obtenido:             OK
[ 4] invertir("AB")       esperado: BA          obtenido: BA          OK
[ 5] invertir("a")        esperado: a           obtenido: a           OK
[ 6] invertir(NULL)       esperado: sin crash   obtenido: sin crash   OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE.
```

**Versión IA:**
```
=== AUDITORIA SESION 01 - Inversion de buffer in-place ===
[ 1] invertir("HOLA")     esperado: ALOH        obtenido: ALOH        OK
[ 2] invertir("abc")      esperado: cba         obtenido: cba         OK
[ 3] invertir("")         esperado:             obtenido:             OK
[ 4] invertir("AB")       esperado: BA          obtenido: BA          OK
[ 5] invertir("a")        esperado: a           obtenido: a           OK
[ 6] invertir(NULL)       esperado: sin crash   obtenido: sin crash   OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE.
````

### Reporte Crítico
**¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?**

En este caso la IA sí validó `NULL` y la cadena vacía desde el inicio, y pasó los 6 casos del harness (6/6). 
Pero al revisarlo vi que esa validación no es opcional: el ciclo `while (*(der + 1) != '\0')` lee la posición siguiente, entonces si la cadena viniera vacía y se quitara el `if`, leería fuera del buffer.
O sea, todo el código depende de esa primera línea. Lo detecté comparándolo con mi versión manual, donde me pasó algo parecido con el `len - 1` en la cadena vacía. Las dos versiones son O(n) en tiempo y O(1) en memoria; la mía usa índices y `strlen`, y la de la IA usa solo punteros.

---
**Firma digital:** jordipech76-boop
