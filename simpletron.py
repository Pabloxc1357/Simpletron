def simpletron():
    memory = [0] * 100
    accumulator = 0
    instruction_counter = 0
    instruction_register = 0
    operation_code = 0
    operand = 0

    # 1. Mensaje inicial de bienvenida
    print("*** ¡Bienvenido a Simpletron! ***")
    print("*** Introduzca su programa una instrucción ***")
    print("*** (o palabra de datos) a la vez en la línea ***")
    print("*** de texto de entrada. Yo indicará el número ***")
    print("*** de posición y una interrogación(?). Usted ***")
    print("*** tecleará entonces una palabra para esa ***")
    print("*** posición. Capture el número 9999 para ***")
    print("*** terminar de introducir su programa. ***\n")

    # 2. Carga del programa a la memoria
    location = 0
    while location < 100:
        try:
            entry = int(input(f"{location:02d} ? "))
            if entry == 9999:
                break
            if -9999 <= entry <= 9999:
                memory[location] = entry
                location += 1
            else:
                print("*** Número fuera de rango (-9999 a +9999) ***")
        except ValueError:
            print("*** Entrada no válida ***")

    print("*** Se terminó de cargar el programa. ***")
    print("*** Comienza la ejecución del programa. ***\n")

    # 3. Ciclo de Ejecución (Fetch / Decode / Execute)
    running = True
    while running and instruction_counter < 100:
        instruction_register = memory[instruction_counter]
        operation_code = abs(instruction_register) // 100
        operand = abs(instruction_register) % 100

        current_pc = instruction_counter
        instruction_counter += 1

        if operation_code == 10:    # READ
            valid = False
            while not valid:
                try:
                    val = int(input("Teclee un Entero: "))
                    if -9999 <= val <= 9999:
                        memory[operand] = val
                        valid = True
                    else:
                        print("*** Número fuera de rango ***")
                except ValueError:
                    print("*** Valor no válido ***")

        elif operation_code == 11:  # WRITE
            print(f"{operand:02d} => {memory[operand]}")

        elif operation_code == 20:  # LOAD
            accumulator = memory[operand]

        elif operation_code == 21:  # STORE
            memory[operand] = accumulator

        elif operation_code == 30:  # ADD
            accumulator += memory[operand]

        elif operation_code == 31:  # SUBTRACT
            accumulator -= memory[operand]

        elif operation_code == 32:  # DIVIDE
            if memory[operand] == 0:
                print("*** ERROR: Intento de división entre cero ***")
                running = False
            else:
                accumulator //= memory[operand]

        elif operation_code == 33:  # MULTIPLY
            accumulator *= memory[operand]

        elif operation_code == 40:  # BRANCH
            instruction_counter = operand

        elif operation_code == 41:  # BRANCHNEG
            if accumulator < 0:
                instruction_counter = operand

        elif operation_code == 42:  # BRANCHZERO
            if accumulator == 0:
                instruction_counter = operand

        elif operation_code == 43:  # HALT
            instruction_counter = current_pc
            print("*** La ejecución de Simpletron terminó normalmente ***\n")
            running = False

        else:
            print(f"*** Código de operación no válido: {operation_code} ***")
            running = False

        # Control de desbordamiento en acumulador
        if not (-9999 <= accumulator <= 9999):
            print("*** ERROR: Desbordamiento del acumulador ***")
            running = False

    # 4. Volcado final de Registros y Memoria (Dump)
    print("Registros:")
    print(f"acumulador:                       {accumulator:+05d}")
    print(f"instructionCounter:                  {instruction_counter:2d}")
    print(f"instructionRegister:              {instruction_register:+05d}")
    print(f"operationcode:                       {operation_code:2d}")
    print(f"operand:                              {operand:1d}\n")

    print("MEMORIA")
    print("       0     1     2     3     4     5     6     7     8     9")
    for row in range(10):
        row_str = f"{row:2d}"
        for col in range(10):
            idx = row * 10 + col
            row_str += f" {memory[idx]:+05d}"
        print(row_str)

if __name__ == "__main__":
    simpletron()