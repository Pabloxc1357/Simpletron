import re

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

if __name__ == '__main__':
    print("Modulo de Evaluacion de Expresiones - Cargado Correctamente")
