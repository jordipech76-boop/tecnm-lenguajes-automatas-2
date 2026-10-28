# Bitácora de Requerimientos y Auditoría IA — Sesión 03
**Materia:** Lenguajes y Autómatas II (SCD-1016)  
**Tema:** Google Colab, Mágicos y Flujo de Entrega  
**Docente Titular:** MCC. Iván Márquez Larios (`ijmarquezl`)  
**Estudiante:** Pech Ramírez Jordi Geovany  
**Matrícula:** 24531319  
**Track:** C (el harness evalúa C; la versión en Rust es opcional y no se califica con este harness)  

---

## 1. Fase 1: El Artesano (Dificultad Manual)
*Reto:* Escritura y conteo de líneas en archivos  
*Describe qué problema o restricción de memoria/sintaxis identificaste al codificar a mano:*
> Lo más difícil fue `contar_lineas`. Si nomás cuentas los `\n`, el archivo `"a\nb"` da 1 en vez de 2, y si le sumas uno al final, el archivo vacío da 1 en vez de 0. Lo arreglé guardando el carácter anterior y empezándolo en `'\n'`. También hay que cerrar el archivo antes de cada `return`, y con `-Werror` cualquier warning ya no deja compilar.

---

## 2. Fase 2: El Científico (Ingeniería de Requerimientos C-R-E-O)

### 2.1 Matriz de Especificación Formal
| Elemento C-R-E-O | Definición Técnica |
| :--- | :--- |
| **Contexto (C)** | C99 / GCC o Rust 2021 sin bibliotecas no estándar |
| **Rol (R)** | Ingeniero de software de sistemas y compiladores |
| **Especificación (E)** | Salida estándar y códigos de retorno ABI |
| **Output (O)** | Solo código modular y análisis de complejidad |

### 2.2 Prompt Estructurado
```text
C: Lenguaje C11, gcc 13.3.0, se compila con -Wall -Wextra -Werror.
R: Eres un programador senior de C.
E: Implementa con las firmas exactas de api_sesion03.h:
   int escribir_texto(const char *ruta, const char *texto);
   int contar_lineas(const char *ruta);
   - escribir_texto regresa 0 si escribe bien y -1 si la ruta no se abre.
   - contar_lineas: archivo vacío = 0, la última línea sin '\n' también
     cuenta, archivo inexistente = -1.
   - Cerrar el archivo en todos los caminos.
O: Un solo archivo .c sin main(), solo el código.
```

---

## 3. Código Devuelto por el Modelo
```c
#include "api_sesion03.h"
#include <stdio.h>

int escribir_texto(const char *ruta, const char *texto) {
    if (ruta == NULL || texto == NULL) return -1;
    FILE *archivo = fopen(ruta, "w");
    if (archivo == NULL) return -1;
    if (fputs(texto, archivo) == EOF) {
        fclose(archivo);
        return -1;
    }
    fclose(archivo);
    return 0;
}

int contar_lineas(const char *ruta) {
    if (ruta == NULL) return -1;
    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) return -1;
    int lineas = 0, ch, ultimo_ch = EOF;
    while ((ch = fgetc(archivo)) != EOF) {
        if (ch == '\n') lineas++;
        ultimo_ch = ch;
    }
    if (ultimo_ch != EOF && ultimo_ch != '\n') lineas++;
    fclose(archivo);
    return lineas;
}
```

---

## 4. Fase 3: El Auditor (Resultados del Test Harness)
*Pruebas ejecutadas:* Archivo vacío, sin salto final y ruta inexistente

### Salida del harness
```text
=== AUDITORIA SESION 03 - Archivos: escribir y contar lineas ===
Caso                               esperado:              obtenido:              resultado
[ 1] escribir_texto(...)==0             esperado: 0            obtenido: 0            OK
[ 2] contar_lineas(3 lineas)            esperado: 3            obtenido: 3            OK
[ 3] contar_lineas(sin salto final)     esperado: 2            obtenido: 2            OK
[ 4] contar_lineas(archivo vacio)       esperado: 0            obtenido: 0            OK
[ 5] contar_lineas(inexistente)         esperado: -1           obtenido: -1           OK
[ 6] escribir_texto(ruta invalida)      esperado: -1           obtenido: -1           OK

RESULTADO: 6/6 pruebas superadas
TODO EN VERDE. Documenta tu auditoria en auditorias/auditoria_sesion03.md
```

* [x] Caso base y límites superados sin *Crash*.
* [x] No existen accesos fuera de límites ni *Memory Leaks*.
* [x] La complejidad teórica coincide con la implementada.

### Reporte Crítico
*¿Qué caso borde o problema omitió la IA y cómo lo solucionaste?*
> Las dos versiones (artesano e IA) sacaron 6/6. El código de la IA sí cubre el archivo vacío, el que no tiene salto final y la ruta inválida, pero **no revisa lo que regresa `fclose()`** en `escribir_texto`. `fputs` escribe en un buffer y los datos se mandan al disco hasta el `fclose`; si ahí falla (por ejemplo, disco lleno), la función igual regresa 0 como si todo saliera bien. El harness no lo detecta porque siempre escribe en archivos normales; lo vi leyendo el código y lo comprobé con `escribir_texto("/dev/full", "hola")`: la versión de la IA regresa 0 y la mía -1. Lo solucioné en mi versión con `if (fclose(f) != 0) return -1;`.
>
> Complejidad: las dos funciones son O(n) en tiempo (n = bytes del texto o del archivo) y O(1) en memoria, sin memoria dinámica.

---
**Firma digital:** jordipech76-boop
