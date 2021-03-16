#!/bin/python3

import math
import os
import subprocess

NUM_PRUEBAS = 5
ARCHIVO_LECTURA = "res.txt"
ARCHIVO_SALIDA = "res.csv"

Cs = [4, 8, 20, 40]
Ls = [512, 768, 2048, 6144, 65536, 98304, 262144, 524288]


def guardar_linea(F, C, T):
    f = open(ARCHIVO_SALIDA, "a")
    f.write(str(F) + "," + str(C) + "," + ",".join(T) + "\n")
    f.close()

def ejecutar(ej, L, F, C):
    print("L=%-10s F=%-10s C=%-10s" % (str(L), str(F), str(C)))
    T = [0] * NUM_PRUEBAS
    for i in range(0, NUM_PRUEBAS):
        subprocess.run(["./" + ej, str(F), str(C)], stdout=subprocess.DEVNULL)
        f = open(ARCHIVO_LECTURA, "r")
        T[i] = f.read()
        f.close()
    guardar_linea(F, C, T)

def ap1():
    for C in Cs:
        for L in Ls:
            F = 0
            if C == 4:
                F = L * 2
            else:
                F = L

            ejecutar("ap1", L, F, C)

def ap2():
    for C in Cs:
        for L in Ls:
            F = 0
            if C == 4:
                F = L * 2
            elif C == 8:
                F = L
            elif C == 20:
                F = math.floor(L / 5) * 2 + (1 if L % 5 <= 3 else 2)
            elif C == 40:
                F = math.ceil(L / 5)

            ejecutar("ap1", L, F, C)


if __name__ == "__main__":
    f = open(ARCHIVO_SALIDA, 'w')
    f.write("F,C,")
    for i in range(1, NUM_PRUEBAS + 1):
        if i > 1:
            f.write(",")
        f.write("T" + str(i))
    f.write("\n")
    f.close()

    ap2()
