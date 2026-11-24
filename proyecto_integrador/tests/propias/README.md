# Pruebas propias

Programas del equipo, además de las pruebas doradas del curso.

| Archivo | Qué cubre | Salida esperada |
| :--- | :--- | :--- |
| `valido1.lj` | `while`, asignaciones y `print` | `valido1.ast` |
| `valido2.lj` | precedencia, paréntesis y menos unario `(neg ...)` | `valido2.ast` |
| `valido3.lj` | `if/else`, `while` y variable declarada dentro de un bloque | `valido3.ast` |
| `invalido1.lj` | caracteres inválidos `@` y `#` (`S01`) | `invalido1.check` |
| `invalido2.lj` | `E01` (no declarada y sin valor), `E02` y `E04` en una pasada | `invalido2.check` |

Para comprobarlas (desde `proyecto_integrador/`):

```bash
./lenguajito tests/propias/valido2.lj --ast        # debe ser igual a valido2.ast
./lenguajito tests/propias/invalido2.lj --check    # debe ser igual a invalido2.check y salir con 1
```

## Pruebas de --run (Hito 2)

| Archivo | Qué cubre | Salida esperada |
| :--- | :--- | :--- |
| `run1.lj` | `while` que calcula el factorial de 5 | `run1.out` |
| `run2.lj` | `if/else` con `>` y `==` | `run2.out` |
| `run3.lj` | shadowing en bloques anidados | `run3.out` |
| `run4.lj` | precedencia, paréntesis, menos unario y asociatividad | `run4.out` |
| `run5.lj` | `while` con `<=` (tabla del 3) | `run5.out` |

Para comprobar una (desde `proyecto_integrador/`):

```bash
./lenguajito tests/propias/run1.lj --run | diff - tests/propias/run1.out && echo OK
```
