#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MEMORY_SIZE 100
#define SENTINEL 9999
#define MIN_WORD -9999
#define MAX_WORD 9998

/* C�digos de operaci�n SML est�ndar */
#define READ 10
#define WRITE 11

#define LOAD 20
#define STORE 21

#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33

#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43

/* Prototipos de funciones */
void initializeMemory(int memory[]);
void loadProgram(int memory[]);
void executeProgram(int memory[]);
void dump(int memory[], int accumulator, int instructionCounter, int instructionRegister, int operationCode, int operand);
bool isValidWord(int word);

int main(void) {
    int memory[MEMORY_SIZE];

    printf("*** Bienvenido a Simpletron ***\n");
    printf("*** Por favor, introduzca su programa instruccion ***\n");
    printf("*** (o palabra de datos) a la vez. Yo escribire ***\n");
    printf("*** el numero de ubicacion y un signo de interrogacion (?) ***\n");
    printf("*** Usted escribira la palabra para esa ubicacion. ***\n");
    printf("*** Teclee el centinela 9999 para finalizar la introduccion. ***\n\n");

    initializeMemory(memory);
    loadProgram(memory);
    executeProgram(memory);

    return 0;
}

void initializeMemory(int memory[]) {
    int i;
    for (i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

bool isValidWord(int word) {
    return (word >= MIN_WORD && word <= MAX_WORD);
}

void loadProgram(int memory[]) {
    int instructionCount = 0;
    int word = 0;

    while (instructionCount < MEMORY_SIZE) {
        printf("%02d ? ", instructionCount);
        
        if (scanf("%d", &word) != 1) {
            printf("Entrada invalida. Introduzca un numero entero.\n");
            while (getchar() != '\n'); /* Limpiar buffer */
            continue;
        }

        if (word == SENTINEL) {
            printf("*** Carga del programa completada ***\n");
            printf("*** Empieza la ejecucion del programa ***\n\n");
            break;
        }

        if (isValidWord(word)) {
            memory[instructionCount] = word;
            instructionCount++;
        } else {
            printf("Palabra invalida. Debe estar entre %d y %d.\n", MIN_WORD, MAX_WORD);
        }
    }
}

void executeProgram(int memory[]) {
    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;
    bool fatalError = false;

    while (instructionCounter < MEMORY_SIZE && !fatalError) {
        instructionRegister = memory[instructionCounter];
        int absInstruction = abs(instructionRegister);
        
        operationCode = absInstruction / 100;
        operand = absInstruction % 100;

        instructionCounter++;

        switch (operationCode) {
            case READ: {
                int inputWord = 0;
                bool valid = false;
                while (!valid) {
                    printf("Introduzca un entero: ");
                    if (scanf("%d", &inputWord) == 1 && isValidWord(inputWord)) {
                        memory[operand] = inputWord;
                        valid = true;
                    } else {
                        printf("Entrada invalida. Debe estar entre %d y %d.\n", MIN_WORD, MAX_WORD);
                        while (getchar() != '\n');
                    }
                }
                break;
            }

            case WRITE:
                printf("Resultado Simpletron: %+05d\n", memory[operand]);
                break;

            case LOAD:
                accumulator = memory[operand];
                break;

            case STORE:
                memory[operand] = accumulator;
                break;

            case ADD:
                accumulator += memory[operand];
                if (accumulator < MIN_WORD || accumulator > 9999) {
                    printf("\n*** ERROR FATAL: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case SUBTRACT:
                accumulator -= memory[operand];
                if (accumulator < MIN_WORD || accumulator > 9999) {
                    printf("\n*** ERROR FATAL: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case DIVIDE:
                if (memory[operand] == 0) {
                    printf("\n*** ERROR FATAL: Division entre cero ***\n");
                    fatalError = true;
                } else {
                    accumulator /= memory[operand];
                }
                break;

            case MULTIPLY:
                accumulator *= memory[operand];
                if (accumulator < MIN_WORD || accumulator > 9999) {
                    printf("\n*** ERROR FATAL: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case BRANCH:
                instructionCounter = operand;
                break;

            case BRANCHNEG:
                if (accumulator < 0) {
                    instructionCounter = operand;
                }
                break;

            case BRANCHZERO:
                if (accumulator == 0) {
                    instructionCounter = operand;
                }
                break;

            case HALT:
                printf("\n*** Simpletron finalizo su ejecucion ***\n");
                dump(memory, accumulator, instructionCounter, instructionRegister, operationCode, operand);
                return;

            default:
                printf("\n*** ERROR FATAL: Codigo de operacion invalido (%02d) ***\n", operationCode);
                fatalError = true;
                break;
        }

        if (fatalError) {
            printf("*** La ejecucion de Simpletron se origino de forma anormal ***\n");
            dump(memory, accumulator, instructionCounter, instructionRegister, operationCode, operand);
            return;
        }
    }
}

void dump(int memory[], int accumulator, int instructionCounter, int instructionRegister, int operationCode, int operand) {
    int i, j;
    printf("\nREGISTROS:\n");
    printf("accumulator          %+05d\n", accumulator);
    printf("instructionCounter      %02d\n", instructionCounter);
    printf("instructionRegister  %+05d\n", instructionRegister);
    printf("operationCode           %02d\n", operationCode);
    printf("operand                 %02d\n\n", operand);

    printf("MEMORIA:\n");
    printf("       0     1     2     3     4     5     6     7     8     9\n");

    for (i = 0; i < 10; i++) {
        printf("%02d ", i * 10);
        for (j = 0; j < 10; j++) {
            printf("%+05d ", memory[i * 10 + j]);
        }
        printf("\n");
    }
    printf("\n");
}
