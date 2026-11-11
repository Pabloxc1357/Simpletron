#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MEMORY_SIZE 100
#define MIN_VAL -9999
#define MAX_VAL 9999


#define READ 10         // Lee un entero desde teclado a la memoria
#define WRITE 11        // Imprime un entero desde la memoria
#define LOAD 20         // Carga de la memoria al acumulador
#define STORE 21        // Guarda del acumulador a la memoria
#define ADD 30          // Suma memoria al acumulador
#define SUBTRACT 31     // Resta memoria al acumulador
#define DIVIDE 32       // Divide acumulador entre memoria
#define MULTIPLY 33     // Multiplica acumulador por memoria
#define BRANCH 40       // Salto incondicional a una dirección
#define BRANCHNEG 41    // Salta si acumulador < 0
#define BRANCHZERO 42   // Salta si acumulador == 0
#define HALT 43         // Detiene la ejecución del programa


#define NEWLINE 12      // Imprime un salto de línea
#define READ_STRING 13  // Lee una cadena de caracteres
#define WRITE_STRING 14 // Imprime una cadena de caracteres
#define REMAINDER 34    // Módulo / Residuo de la división
#define EXPONENTIATION 35 // Exponenciación (potencia)

void dump_memory(int accumulator, int instructionCounter, int instructionRegister, 
                 int operationCode, int operand, const int memory[]);
int check_overflow(int value);

int main(int argc, char *argv[]) {
    int memory[MEMORY_SIZE] = {0};
    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;

    printf("*** Bienvenido a Simpletron ***\n\n");

    // CARGA DEL PROGRAMA: Desde archivo .sml o por consola
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            printf("*** ERROR FATAL: No se pudo abrir el archivo %s ***\n", argv[1]);
            return 1;
        }
        int addr = 0, word = 0;
        while (fscanf(file, "%d", &word) == 1 && addr < MEMORY_SIZE) {
            if (word < MIN_VAL || word > MAX_VAL) {
                printf("*** ERROR FATAL: Valor invalido %d en dirección %d ***\n", word, addr);
                fclose(file);
                return 1;
            }
            memory[addr++] = word;
        }
        fclose(file);
        printf("*** Programa '%s' cargado exitosamente ***\n\n", argv[1]);
    } else {
        printf("*** Modo interactivo: Ingrese instrucciones (-99999 para terminar) ***\n");
        int addr = 0, word = 0;
        while (addr < MEMORY_SIZE) {
            printf("%02d ? ", addr);
            if (scanf("%d", &word) != 1 || word == -99999) break;
            memory[addr++] = word;
        }
    }

    printf("*** Inicio de ejecucion del programa ***\n\n");

    // CICLO DE INSTRUCCIÓN
    while (1) {
        // 1. FETCH (Traer la instrucción)
        if (instructionCounter < 0 || instructionCounter >= MEMORY_SIZE) {
            printf("\n*** ERROR FATAL: Contador fuera de rango ***\n");
            dump_memory(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
            return 1;
        }

        instructionRegister = memory[instructionCounter];

        // 2. DECODE (Decodificar opcode y operando)
        operationCode = instructionRegister / 100;
        operand = instructionRegister % 100;

        instructionCounter++; // Avanzar al siguiente paso

        // 3. EXECUTE (Ejecutar operación)
        switch (operationCode) {
            case READ:
                printf("Entrada > ");
                scanf("%d", &memory[operand]);
                break;

            case WRITE:
                printf("Salida > %d\n", memory[operand]);
                break;

            case NEWLINE:
                printf("\n");
                break;

            case READ_STRING: {
                char str[MEMORY_SIZE];
                printf("Entrada Cadena > ");
                scanf("%s", str);
                int i = 0;
                while (str[i] != '\0' && (operand + i) < MEMORY_SIZE) {
                    memory[operand + i] = (int)str[i];
                    i++;
                }
                if ((operand + i) < MEMORY_SIZE) memory[operand + i] = 0;
                break;
            }

            case WRITE_STRING: {
                int i = operand;
                printf("Salida Cadena > ");
                while (i < MEMORY_SIZE && memory[i] != 0) {
                    printf("%c", (char)memory[i]);
                    i++;
                }
                printf("\n");
                break;
            }

            case LOAD:
                accumulator = memory[operand];
                break;

            case STORE:
                memory[operand] = accumulator;
                break;

            case ADD:
                accumulator += memory[operand];
                if (check_overflow(accumulator)) return 1;
                break;

            case SUBTRACT:
                accumulator -= memory[operand];
                if (check_overflow(accumulator)) return 1;
                break;

            case DIVIDE:
                if (memory[operand] == 0) {
                    printf("\n*** ERROR FATAL: Division entre cero ***\n");
                    dump_memory(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
                    return 1;
                }
                accumulator /= memory[operand];
                break;

            case MULTIPLY:
                accumulator *= memory[operand];
                if (check_overflow(accumulator)) return 1;
                break;

            case REMAINDER:
                if (memory[operand] == 0) {
                    printf("\n*** ERROR FATAL: Modulo entre cero ***\n");
                    dump_memory(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
                    return 1;
                }
                accumulator %= memory[operand];
                break;

            case EXPONENTIATION:
                accumulator = (int)pow(accumulator, memory[operand]);
                if (check_overflow(accumulator)) return 1;
                break;

            case BRANCH:
                instructionCounter = operand;
                break;

            case BRANCHNEG:
                if (accumulator < 0) instructionCounter = operand;
                break;

            case BRANCHZERO:
                if (accumulator == 0) instructionCounter = operand;
                break;

            case HALT:
                printf("*** Ejecucion finalizada correctamente ***\n");
                dump_memory(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
                return 0;

            default:
                printf("\n*** ERROR FATAL: Opcode invalido (%02d) ***\n", operationCode);
                dump_memory(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory);
                return 1;
        }
    }
}

int check_overflow(int value) {
    if (value < MIN_VAL || value > MAX_VAL) {
        printf("\n*** ERROR FATAL: Desbordamiento de memoria/acumulador (%d) ***\n", value);
        return 1;
    }
    return 0;
}

void dump_memory(int accumulator, int instructionCounter, int instructionRegister, 
                 int operationCode, int operand, const int memory[]) {
    printf("\nREGISTROS:\n");
    printf("accumulator          %+05d\n", accumulator);
    printf("instructionCounter      %02d\n", instructionCounter);
    printf("instructionRegister  %+05d\n", instructionRegister);
    printf("operationCode           %02d\n", operationCode);
    printf("operand                 %02d\n", operand);

    printf("\nMEMORIA:\n");
    printf("       0     1     2     3     4     5     6     7     8     9\n");
    for (int i = 0; i < MEMORY_SIZE; i += 10) {
        printf("%02d ", i);
        for (int j = 0; j < 10; j++) {
            printf("%+05d ", memory[i + j]);
        }
        printf("\n");
    }
    printf("\n");
}
