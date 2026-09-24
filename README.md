# Simulador de Computadora Simpletron (SML)

Simulador funcional desarrollado en **Lenguaje C** que simula la arquitectura de la computadora Simpletron y ejecuta programas escritos en lenguaje máquina Simpletron (SML).

## Características principales
- Memoria principal de 100 posiciones (`00` a `99`).
- Registros principales: `accumulator`, `instructionCounter`, `instructionRegister`, `operationCode` y `operand`.
- Soporte para las 12 operaciones estándar SML.
- Detección y manejo de errores fatales (división entre cero, código de operación inválido y desbordamiento del acumulador).
- Vaciado formateado de memoria y registros (*dump*).
- Validación estricta de palabras en el rango de `-9999` a `+9998` con valor centinela `9999`.

## Operaciones Implementadas

| Código | Operación | Descripción |
| :---: | :---: | :--- |
| **10** | `READ` | Lee una palabra desde el teclado y la almacena en memoria. |
| **11** | `WRITE` | Muestra en pantalla la palabra almacenada en memoria. |
| **20** | `LOAD` | Carga en el acumulador el valor de memoria. |
| **21** | `STORE` | Guarda el valor del acumulador en memoria. |
| **30** | `ADD` | Suma el valor de memoria al acumulador. |
| **31** | `SUBTRACT` | Resta el valor de memoria al acumulador. |
| **32** | `DIVIDE` | Divide el acumulador entre el valor de memoria. |
| **33** | `MULTIPLY` | Multiplica el acumulador por el valor de memoria. |
| **40** | `BRANCH` | Bifurcación incondicional a una posición de memoria. |
| **41** | `BRANCHNEG` | Bifurcación si el acumulador es negativo. |
| **42** | `BRANCHZERO` | Bifurcación si el acumulador es cero. |
| **43** | `HALT` | Finaliza la ejecución del programa. |

## Instrucciones de Compilación y Ejecución

### Desde Dev-C++ / Code::Blocks
1. Abrir el archivo `main.c`.
2. Presionar **F11** (*Compilar y Ejecutar*).

### Desde la Terminal (GCC)
```bash
gcc main.c -o simpletron
./simpletron
