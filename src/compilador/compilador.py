import sys, re

OPCODES = {'READ': 10, 'WRITE': 11, 'LOAD': 20, 'STORE': 21, 'ADD': 30, 'SUBTRACT': 31, 'DIVIDE': 32, 'MULTIPLY': 33, 'REMAINDER': 34, 'EXPONENTIATION': 35, 'BRANCH': 40, 'BRANCHNEG': 41, 'BRANCHZERO': 42, 'HALT': 43}

def validar_parentesis(expresion):
    pila = []
    pares = {')': '(', ']': '[', '}': '{'}
    for char in expresion:
        if char in "( [{": pila.append(char)
        elif char in ") ]}":
            if not pila or pila[-1] != pares[char]: return False
            pila.pop()
    return len(pila) == 0

def infijo_a_posfijo(expresion):
    precedencia = {'+': 1, '-': 1, '*': 2, '/': 2, '%': 2, '^': 3}
    salida, pila = [], []
    tokens = re.findall(r'[a-zA-Z]+|\d+|[+\-*/%^()]', expresion)
    for token in tokens:
        if token.isalnum(): salida.append(token)
        elif token == '(': pila.append(token)
        elif token == ')':
            while pila and pila[-1] != '(': salida.append(pila.pop())
            if pila: pila.pop()
        elif token in precedencia:
            while pila and pila[-1] in precedencia and precedencia[pila[-1]] >= precedencia[token]: salida.append(pila.pop())
            pila.append(token)
    while pila: salida.append(pila.pop())
    return salida

class CompiladorSimple:
    def __init__(self):
        self.tabla_simbolos = {}
        self.instrucciones_sml = [0] * 100
        self.inst_counter = 0
        self.data_counter = 99
        self.referencias_pendientes = []

    def obtener_o_crear_variable(self, var):
        if var not in self.tabla_simbolos:
            self.tabla_simbolos[var] = ('V', self.data_counter)
            self.data_counter -= 1
        return self.tabla_simbolos[var][1]

    def obtener_o_crear_constante(self, val):
        clave = f"CONST_{val}"
        if clave not in self.tabla_simbolos:
            self.tabla_simbolos[clave] = ('C', self.data_counter)
            self.instrucciones_sml[self.data_counter] = int(val)
            self.data_counter -= 1
        return self.tabla_simbolos[clave][1]

    def compilar_expresion(self, posfijo):
        pila_temp = []
        for token in posfijo:
            if token.isalpha(): pila_temp.append(self.obtener_o_crear_variable(token))
            elif token.isdigit(): pila_temp.append(self.obtener_o_crear_constante(token))
            elif token in ('+', '-', '*', '/', '%', '^'):
                op2, op1 = pila_temp.pop(), pila_temp.pop()
                self.instrucciones_sml[self.inst_counter] = OPCODES['LOAD'] * 100 + op1
                self.inst_counter += 1
                op_name = {'+':'ADD', '-':'SUBTRACT', '*':'MULTIPLY', '/':'DIVIDE', '%':'REMAINDER', '^':'EXPONENTIATION'}[token]
                self.instrucciones_sml[self.inst_counter] = OPCODES[op_name] * 100 + op2
                self.inst_counter += 1
                temp_dir = self.data_counter
                self.data_counter -= 1
                self.instrucciones_sml[self.inst_counter] = OPCODES['STORE'] * 100 + temp_dir
                self.inst_counter += 1
                pila_temp.append(temp_dir)
        return pila_temp.pop()

    def compilar(self, ruta_simple):
        with open(ruta_simple, 'r') as f:
            lineas = [l.strip() for l in f.readlines() if l.strip()]

        for linea in lineas:
            partes = linea.split(maxsplit=2)
            num_linea, comando = int(partes[0]), partes[1].lower()
            self.tabla_simbolos[num_linea] = ('L', self.inst_counter)

            if comando == 'rem': continue
            elif comando == 'input':
                var_dir = self.obtener_o_crear_variable(partes[2])
                self.instrucciones_sml[self.inst_counter] = OPCODES['READ'] * 100 + var_dir
                self.inst_counter += 1
            elif comando == 'print':
                var_dir = self.obtener_o_crear_variable(partes[2])
                self.instrucciones_sml[self.inst_counter] = OPCODES['WRITE'] * 100 + var_dir
                self.inst_counter += 1
            elif comando == 'goto':
                self.referencias_pendientes.append((self.inst_counter, int(partes[2])))
                self.instrucciones_sml[self.inst_counter] = OPCODES['BRANCH'] * 100
                self.inst_counter += 1
            elif comando == 'let':
                var_dest, exp = partes[2].split('=', 1)
                posfijo = infijo_a_posfijo(exp.strip())
                res_dir = self.compilar_expresion(posfijo)
                dir_dest = self.obtener_o_crear_variable(var_dest.strip())
                self.instrucciones_sml[self.inst_counter] = OPCODES['LOAD'] * 100 + res_dir
                self.inst_counter += 1
                self.instrucciones_sml[self.inst_counter] = OPCODES['STORE'] * 100 + dir_dest
                self.inst_counter += 1
            elif comando == 'end':
                self.instrucciones_sml[self.inst_counter] = OPCODES['HALT'] * 100
                self.inst_counter += 1

        for dir_inst, linea_dest in self.referencias_pendientes:
            if linea_dest in self.tabla_simbolos:
                self.instrucciones_sml[dir_inst] += self.tabla_simbolos[linea_dest][1]

        return self.instrucciones_sml

if __name__ == '__main__':
    comp = CompiladorSimple()
    codigo_sml = comp.compilar(sys.argv[1])
    with open(sys.argv[2], 'w') as f:
        for inst in codigo_sml: f.write(f"{inst:+05d}\n")
    print("Compilacion exitosa.")
