# Entrega · Hito 2

**Equipo:** José Manuel, Oscar Mauricio, María Jesús, Jordi Geovany y Héctor Iván  
**Repositorio:** https://github.com/jordipech76-boop/tecnm-lenguajes-automatas-2  
**Tag:** `v0.2-hito2`  
**Fecha:** 2026-11-24

---

## 1. Qué funciona

```bash
make
make test-hito2
./lenguajito tests/validos/05_shadowing.lj --run    # imprime 2 y 1
./lenguajito tests/propias/run1.lj --run            # factorial de 5: 120
./lenguajito tests/propias/invalido3.lj --check     # E01 por flujo, sale con 1
./lenguajito tests/propias/invalido5.lj --check     # solo el S01, sin E01 de más
```

- `--run` ejecuta el programa interpretando el AST: primero hace la revisión de `--check` y, si no hay errores, recorre las sentencias (`declarar`, `asignar`, `print`, bloques, `if/else` y `while`).
- Las piezas de `--run` están separadas por contratos (`entorno.h`, `evaluador.h`, `interprete.h`): el **entorno** guarda los valores por ámbito con shadowing, el **evaluador** calcula las expresiones (las comparaciones dan 1 o 0 y el divisor se revisa antes de dividir) y el **intérprete** ejecuta las sentencias.
- Errores en ejecución: una división entre una variable que vale 0 se reporta en `stderr` como `línea:columna: error en ejecucion: division entre cero` y el programa sale con 1.
- La semántica es **sensible al flujo**: lo asignado dentro de un `if` sin `else` o de un `while` no cuenta después; en un `if/else` cuenta solo si se asignó en las dos ramas.
- Un error de sintaxis en una declaración ya no produce un `E01` en cascada: la declaración se conserva marcada con `con_error`.
- Todo lo del Hito 1 (`--tokens`, `--ast`, `--check` con `S01`, `E01`, `E02` y `E04`, varios errores por pasada) sigue funcionando.

## 2. Qué no funciona (y por qué)

- [ ] `--tac` y `--asm`: son de los Hitos 3 y 4.
- [ ] `E04` solo detecta la división entre una **constante** cero, como pide la especificación; con una variable, el error aparece hasta `--run`.
- [ ] El análisis de flujo es conservador: lo asignado dentro de un `while` no cuenta después del ciclo, aunque la condición sea siempre verdadera, porque `--check` no evalúa las condiciones.
- [ ] Un desbordamiento (por ejemplo `INT_MAX + 1`) no se reporta: el resultado da la vuelta como un `int` de 32 bits.
- [ ] Los errores en ejecución salen sin el nombre del archivo (`3:9: error en ejecucion: division entre cero`): `interpretar()` solo recibe el AST, y agregar la ruta cambiaría el contrato de `interprete.h`.

## 3. Pruebas

Salida de las pruebas del curso, corridas en Google Colab el 23 de noviembre de 2026 sobre el commit `38cc2c4` (el código de `src/` no ha cambiado después de ese commit):

```text
Hito 1: 4/4 pruebas superadas
Hito 2: 14/14 pruebas superadas
```

Pruebas propias que agregamos en este hito (detalle en [`tests/propias/README.md`](../tests/propias/README.md)):

- `run1.lj` a `run5.lj`: factorial con `while`, `if/else` con comparaciones, shadowing en bloques anidados, precedencia y menos unario, y una tabla con `<=`; cada una con su `.out`, que coincide exacto con la salida de `--run`.
- `invalido3.lj` e `invalido4.lj`: `E01` por inicialización sensible al flujo (`if` sin `else` y `while`).
- `invalido5.lj`: solo el `S01` de `int z = 5 # 2;`, sin `E01` de más al usar `z`.
- `valido4.lj`: `if/else` que asigna en las dos ramas; `--check` sin errores y `--run` imprime `1`.

## 4. Reparto de trabajo

| Integrante | Archivos / etapas | Commits principales |
| :--- | :--- | :--- |
| José Manuel | `entorno.c`: valores de las variables por ámbito | `91f76e4`, `55ea7f8` |
| Oscar Mauricio | contratos `entorno.h`, `evaluador.h` e `interprete.h`; `evaluador.c`; README del proyecto; salidas esperadas de las pruebas de flujo y esta entrega | `585665b`, `f070dcc`, `c054f1d` |
| Jordi Geovany | `interprete.c` y `--run` en `compilador.c` | `37680d4`, `92977a4` |
| Héctor Iván | inicialización sensible al flujo en `semantica.c`; `con_error` en `ast.h` y `parser.c`; pruebas de flujo y recuperación | `2d65686`, `479c02f`, `ab3e986`, `9541680` |
| María Jesús | pruebas propias de `--run` (`run1` a `run5`) y su documentación | `38cc2c4` |

## 5. Bitácora C-R-E-O (IA)

| Contexto | Rol | Especificación | Output | ¿Qué aceptaron, corrigieron o rechazaron? |
| :--- | :--- | :--- | :--- | :--- |
| Hito 2: había que dividir `--run` en piezas para que tres personas programaran al mismo tiempo | Ingeniero de compiladores en C11 | Firmas de `entorno.h`, `evaluador.h` e `interprete.h` según el plan del equipo, compatibles con el `ASTNode` existente, que compilen con `-Wall -Wextra -Werror -Wpedantic` | Los tres `.h` (Claude) | **Aceptado**, con dos decisiones del equipo: `entorno_declarar` guarda una **copia** del nombre (en la sesión 13 vimos que guardar el puntero falla si el texto cambia) y `evaluar_expr` pone `*error` en 0 al empezar. Se agregó `EVAL_VAR_DESCONOCIDA` solo como protección. |
| Hito 2: faltaba `evaluador.c` para que `--run` calculara las expresiones | Ingeniero de compiladores en C11 | Contrato de `evaluador.h`: `AST_NUM`, `AST_VAR`, `AST_NEG` y `AST_BINARIA`; comparaciones que dan 1 o 0; revisar el divisor antes de dividir; propagar el primer error; sin comportamiento indefinido | `evaluador.c` (Claude) | **Aceptado:** evaluación recursiva con una función auxiliar. La propuesta hace las cuentas en `long long`, así un desbordamiento da la vuelta como un `int` de 32 bits y `INT_MIN / -1` no truena. Lo probamos solo con 19 casos y con `make test-hito2` (14/14) en Colab. |
| Hito 2: faltaba el entorno de valores para `--run` | ⚠️ *pendiente* | Contrato de `entorno.h`: pila de ámbitos con valores, shadowing, copia del nombre, no cerrar el global | `entorno.c` (⚠️ *pendiente: qué IA*) | ⚠️ *pendiente (José Manuel)* |
| Hito 2: faltaba ejecutar el programa con `--run` (`interpretar()` y conectarlo en `compilador.c`) | Programador de C y compiladores | Contrato de `interprete.h`: recorrer el AST, `print` con `"%d\n"`, cada bloque abre y cierra un ámbito, `while` vuelve a evaluar la condición, 0 si todo bien y 1 si hay error en ejecución (mensaje a `stderr`); usar solo `entorno.h` y `evaluador.h`; antes de ejecutar, correr la revisión de `--check` | `interprete.c` y el caso `--run` de `compilador.c` (Claude) | **Aceptado:** un `switch` por tipo de sentencia; el bloque cierra su ámbito aunque falle algo adentro; las declaraciones sin valor quedan en 0. Se comprobó con `make test-hito2` 14/14 y `valgrind` sin fugas. |
| Como `evaluador.c` de Oscar todavía no estaba, la IA hizo un evaluador provisional para probar | — | El evaluador es de Oscar y se programa por separado | `evaluador` de prueba (Claude) | **Rechazado para el repo:** solo se usó para probar en local y no se subió. Con el `evaluador.c` real de Oscar se volvió a probar: 14/14. |
| Los errores en ejecución salen sin el nombre del archivo (`3:9: error en ejecucion: division entre cero`) | — | `interpretar()` solo recibe el AST, no la ruta | — | **Aceptado como limitación:** para poner el archivo habría que cambiar el contrato de `interprete.h`, y eso afecta a los demás. |
| Hito 2: la lectura antes de asignar no seguía el flujo y salía un `E01` de más tras un error de sintaxis | ⚠️ *pendiente* | `if` sin `else` y `while` no cuentan después; `if/else` solo si se asignó en las dos ramas; conservar la declaración con error | `semantica.c`, `ast.h` y `parser.c` (⚠️ *pendiente: qué IA*) | ⚠️ *pendiente (Héctor)* |
| Hito 2: faltaban pruebas propias de `--run` | ⚠️ *pendiente* | Programas con su salida exacta: `while`, `if/else`, shadowing, precedencia | `run1` a `run5` (⚠️ *pendiente: qué IA, si se usó*) | ⚠️ *pendiente (María)* |

## 6. Demo

- [ ] Preparada para 3 minutos.
- [ ] Cualquier integrante puede explicar cualquier parte.

Orden propuesto (unos 30 segundos por punto):

1. `make test-hito2`: 14/14 en verde.
2. `--run` con `tests/validos/05_shadowing.lj`: la `x` del bloque tapa a la de afuera y al cerrar el bloque vuelve la de afuera (entorno).
3. `--run` con `tests/propias/run1.lj`: un `while` que calcula el factorial (intérprete y evaluador).
4. `--check` con `tests/propias/invalido3.lj`: el `E01` sale porque el `if` no tiene `else` (análisis de flujo).
5. `--check` con `tests/propias/invalido5.lj`: solo sale el `S01`, sin `E01` en cascada.
6. Cerrar con cómo se dividió el trabajo usando contratos `.h`, igual que en los laboratorios.
