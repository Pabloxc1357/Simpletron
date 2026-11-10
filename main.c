#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MEMORY_SIZE 100
#define SENTINEL 9999
#define MIN_WORD -9999
#define MAX_WORD 9999

#define READ 10
#define WRITE 11

#define LOAD 20
#define STORE 21

#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33

#define REMAINDER 34
#define EXPONENTIATION 35

#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43

void initializeMemory(int memory[]);
bool loadProgramFromFile(int memory[], const char *filename);
void loadProgramInteractive(int memory[]);
void executeProgram(int memory[], FILE *logFile);
void dump(FILE *logFile, int memory[], int accumulator, int instructionCounter, int instructionRegister, int operationCode, int operand);
bool isValidWord(int word);

int main(int argc, char *argv[]) {
    int memory[MEMORY_SIZE];
    bool loaded = false;
    FILE *logFile = NULL;

    initializeMemory(memory);

    logFile = fopen("ejecucion_simpletron.log", "w");
    if (!logFile) {
        printf("Advertencia: No se pudo crear el archivo de log.\n");
    }

    printf("*** Bienvenido a Simpletron ***\n");

    if (argc > 1) {
        loaded = loadProgramFromFile(memory, argv[1]);
    }

    if (!loaded) {
        if (argc > 1) {
            printf("Fallo la lectura del archivo. Pasando a modo interactivo.\n\n");
        }
        loadProgramInteractive(memory);
    }

    executeProgram(memory, logFile);

    if (logFile) {
        fclose(logFile);
        printf("\n[LOG] La ejecucion y el dump se guardaron en 'ejecucion_simpletron.log'.\n");
    }

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

bool loadProgramFromFile(int memory[], const char *filename) {
    FILE *file = fopen(filename, "r");
    int instructionCount = 0;
    int word = 0;

    if (!file) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return false;
    }

    while (fscanf(file, "%d", &word) == 1 && instructionCount < MEMORY_SIZE) {
        if (word == SENTINEL) break;

        if (isValidWord(word)) {
            memory[instructionCount++] = word;
        } else {
            printf("Advertencia: Palabra invalida (%d) ignorada en linea %d.\n", word, instructionCount);
        }
    }

    fclose(file);
    printf("*** Programa cargado exitosamente desde '%s' (%d palabras) ***\n\n", filename, instructionCount);
    return true;
}

void loadProgramInteractive(int memory[]) {
    int instructionCount = 0;
    int word = 0;

    printf("*** Introduzca su programa instruccion a la vez. ***\n");
    printf("*** Teclee 9999 para finalizar la introduccion. ***\n\n");

    while (instructionCount < MEMORY_SIZE) {
        printf("%02d ? ", instructionCount);
        if (scanf("%d", &word) != 1) {
            printf("Entrada invalida. Introduzca un entero.\n");
            while (getchar() != '\n');
            continue;
        }

        if (word == SENTINEL) break;

        if (isValidWord(word)) {
            memory[instructionCount++] = word;
        } else {
            printf("Palabra invalida. Debe estar entre %d y %d.\n", MIN_WORD, MAX_WORD);
        }
    }
    printf("*** Carga interactiva completada ***\n\n");
}

void executeProgram(int memory[], FILE *logFile) {
    int accumulator = 0;
    int instructionCounter = 0;
    int instructionRegister = 0;
    int operationCode = 0;
    int operand = 0;
    bool fatalError = false;

    while (instructionCounter < MEMORY_SIZE && !fatalError) {
        int absInstruction;
        instructionRegister = memory[instructionCounter];
        absInstruction = abs(instructionRegister);
        
        operationCode = absInstruction / 100;
        operand = absInstruction % 100;

        instructionCounter++;

        if (operand < 0 || operand >= MEMORY_SIZE) {
            printf("\n*** ERROR FATAL: Acceso a direccion de memoria invalida (%02d) ***\n", operand);
            fatalError = true;
            break;
        }

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
                        printf("Entrada invalida. Rango: %d a %d.\n", MIN_WORD, MAX_WORD);
                        while (getchar() != '\n');
                    }
                }
                break;
            }

            case WRITE:
                printf("Resultado Simpletron: %+05d\n", memory[operand]);
                if (logFile) fprintf(logFile, "Salida SML: %+05d\n", memory[operand]);
                break;

            case LOAD:
                accumulator = memory[operand];
                break;

            case STORE:
                memory[operand] = accumulator;
                break;

            case ADD:
                accumulator += memory[operand];
                if (!isValidWord(accumulator)) {
                    printf("\n*** ERROR FATAL: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case SUBTRACT:
                accumulator -= memory[operand];
                if (!isValidWord(accumulator)) {
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
                if (!isValidWord(accumulator)) {
                    printf("\n*** ERROR FATAL: Desbordamiento del acumulador ***\n");
                    fatalError = true;
                }
                break;

            case REMAINDER:
                if (memory[operand] == 0) {
                    printf("\n*** ERROR FATAL: Modulo entre cero ***\n");
                    fatalError = true;
                } else {
                    accumulator %= memory[operand];
                }
                break;

            case EXPONENTIATION: {
                double res = pow((double)accumulator, (double)memory[operand]);
                if (res < MIN_WORD || res > MAX_WORD) {
                    printf("\n*** ERROR FATAL: Desbordamiento en exponenciacion ***\n");
                    fatalError = true;
                } else {
                    accumulator = (int)res;
                }
                break;
            }

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
                printf("\n*** Simpletron finalizo su ejecucion correctamente ***\n");
                dump(logFile, memory, accumulator, instructionCounter, instructionRegister, operationCode, operand);
                return;

            default:
                printf("\n*** ERROR FATAL: Codigo de operacion invalido (%02d) ***\n", operationCode);
                fatalError = true;
                break;
        }

        if (fatalError) {
            printf("*** La ejecucion termino de forma anormal ***\n");
            dump(logFile, memory, accumulator, instructionCounter, instructionRegister, operationCode, operand);
            return;
        }
    }
}

void dump(FILE *logFile, int memory[], int accumulator, int instructionCounter, int instructionRegister, int operationCode, int operand) {
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

    if (logFile) {
        fprintf(logFile, "\nREGISTROS:\n");
        fprintf(logFile, "accumulator          %+05d\n", accumulator);
        fprintf(logFile, "instructionCounter      %02d\n", instructionCounter);
        fprintf(logFile, "instructionRegister  %+05d\n", instructionRegister);
        fprintf(logFile, "operationCode           %02d\n", operationCode);
        fprintf(logFile, "operand                 %02d\n\n", operand);

        fprintf(logFile, "MEMORIA:\n");
        fprintf(logFile, "       0     1     2     3     4     5     6     7     8     9\n");
        for (i = 0; i < 10; i++) {
            fprintf(logFile, "%02d ", i * 10);
            for (j = 0; j < 10; j++) {
                fprintf(logFile, "%+05d ", memory[i * 10 + j]);
            }
            fprintf(logFile, "\n");
        }
    }
}
