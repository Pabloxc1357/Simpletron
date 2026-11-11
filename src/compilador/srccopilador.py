import sys
import re

OPCODES = {
    'READ': 10,
    'WRITE': 11,
    'LOAD': 20,
    'STORE': 21,
    'ADD': 30,
    'SUBTRACT': 31,
    'DIVIDE': 32,
    'MULTIPLY': 33,
    'REMAINDER': 34,
    'EXPONENTIATION': 35,
    'BRANCH': 40,
    'BRANCHNEG': 41,
    'BRANCHZERO': 42,
    'HALT': 43
}

def validar_parentesis(expresion):
    pila = []
    pares = {')': '(', ']': '[', '}': '{'}
    for char in expresion:
        if char in "( [{":
            pila.append(char)
        elif char in ") ]}":
            if not pila or pila[-1] != pares[char]:
                return False
            pila.pop()
    return len(pila) == 0

def infijo_a_posfijo(expresion):
    precedencia = {'+': 1, '-': 1, '*': 2, '/': 2, '%': 2, '^': 3}
    salida = []
    pila = []
    tokens = re.findall(r'[a-zA-Z]+|\d+|[+\-*/%^()]', expresion)

    for token in tokens:
        if token.isalnum():
            salida.append(token)
        elif token == '(':
            pila.append(token)
        elif token == ')':
            while pila and pila[-1] != '(':
                salida.append(pila.pop())
            if pila:
                pila.pop()
        elif token in precedencia:
            while (pila and pila[-1] in precedencia and 
                   precedencia[pila[-1]] >= precedencia[token]):
                salida.append(pila.pop())
            pila.append(token)

    while pila:
        salida.append(pila.pop())

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
        pila_temporales = []
        
        for token in posfijo:
            if token.isalpha():
                pila_temporales.append(self.obtener_o_crear_variable(token))
            elif token.isdigit():
                pila_temporales.append(self.obtener_o_crear_constante(token))
            elif token in ('+', '-', '*', '/', '%', '^'):
                op2 = pila_temporales.pop()
                op1 = pila_temporales.pop()

                self.instrucciones_sml[self.inst_counter] = OPCODES['LOAD'] * 100 + op1
                self.inst_counter += 1

                opcode_name = {'+': 'ADD', '-': 'SUBTRACT', '*': 'MULTIPLY', 
                               '/': 'DIVIDE', '%': 'REMAINDER', '^': 'EXPONENTIATION'}[token]
                self.instrucciones_sml[self.inst_counter] = OPCODES[opcode_name] * 100 + op2
                self.inst_counter += 1

                temp_dir = self.data_counter
                self.data_counter -= 1
                self.instrucciones_sml[self.inst_counter] = OPCODES['STORE'] * 100 + temp_dir
                self.inst_counter += 1
                pila_temporales.append(temp_dir)

        return pila_temporales.pop()

    def compilar(self, ruta_archivo_simple):
        with open(ruta_archivo_simple, 'r') as f:
            lineas = [l.strip() for l in f.readlines() if l.strip()]

        for linea in lineas:
            partes = linea.split(maxsplit=2)
            num_linea = int(partes[0])
            comando = partes[1].lower()

            self.tabla_simbolos[num_linea] = ('L', self.inst_counter)

            if comando == 'rem':
                continue

            elif comando == 'input':
                var = partes[2]
                var_dir = self.obtener_o_crear_variable(var)
                self.instrucciones_sml[self.inst_counter] = OPCODES['READ'] * 100 + var_dir
                self.inst_counter += 1

            elif comando == 'print':
                var = partes[2]
                var_dir = self.obtener_o_crear_variable(var)
                self.instrucciones_sml[self.inst_counter] = OPCODES['WRITE'] * 100 + var_dir
                self.inst_counter += 1

            elif comando == 'goto':
                linea_dest = int(partes[2])
                self.referencias_pendientes.append((self.inst_counter, linea_dest))
                self.instrucciones_sml[self.inst_counter] = OPCODES['BRANCH'] * 100
                self.inst_counter += 1

            elif comando == 'if':
                match = re.match(r'(\w+)\s*(>=|<=|==|!=|>|<)\s*(\w+)\s+goto\s+(\d+)', partes[2])
                if match:
                    var1, op, var2, dest_linea = match.groups()
                    dir1 = self.obtener_o_crear_variable(var1) if var1.isalpha() else self.obtener_o_crear_constante(var1)
                    dir2 = self.obtener_o_crear_variable(var2) if var2.isalpha() else self.obtener_o_crear_constante(var2)

                    self.instrucciones_sml[self.inst_counter] = OPCODES['LOAD'] * 100 + dir1
                    self.inst_counter += 1
                    self.instrucciones_sml[self.inst_counter] = OPCODES['SUBTRACT'] * 100 + dir2
                    self.inst_counter += 1

                    self.referencias_pendientes.append((self.inst_counter, int(dest_linea)))
                    self.instrucciones_sml[self.inst_counter] = OPCODES['BRANCHZERO'] * 100 if op == '==' else OPCODES['BRANCHNEG'] * 100
                    self.inst_counter += 1

            elif comando == 'let':
                var_dest, exp = partes[2].split('=', 1)
                var_dest = var_dest.strip()
                exp = exp.strip()

                if not validar_parentesis(exp):
                    raise ValueError(f"Expresion mal balanceada en linea {num_linea}")

                posfijo = infijo_a_posfijo(exp)
                res_dir = self.compilar_expresion(posfijo)

                dir_dest = self.obtener_o_crear_variable(var_dest)
                self.instrucciones_sml[self.inst_counter] = OPCODES['LOAD'] * 100 + res_dir
                self.inst_counter += 1
                self.instrucciones_sml[self.inst_counter] = OPCODES['STORE'] * 100 + dir_dest
                self.inst_counter += 1

            elif comando == 'end':
                self.instrucciones_sml[self.inst_counter] = OPCODES['HALT'] * 100
                self.inst_counter += 1

        for dir_inst, linea_dest in self.referencias_pendientes:
            if linea_dest in self.tabla_simbolos:
                dir_sml_dest = self.tabla_simbolos[linea_dest][1]
                opcode_base = self.instrucciones_sml[dir_inst]
                self.instrucciones_sml[dir_inst] = opcode_base + dir_sml_dest

        return self.instrucciones_sml

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Uso: python compilador.py <archivo.simple> <salida.sml>")
        sys.exit(1)

    comp = CompiladorSimple()
    codigo_sml = comp.compilar(sys.argv[1])
    
    with open(sys.argv[2], 'w') as f:
        for inst in codigo_sml:
            f.write(f"{inst:+05d}\n")
            
    print(f"Compilacion exitosa. Archivo SML generado en: {sys.argv[2]}")