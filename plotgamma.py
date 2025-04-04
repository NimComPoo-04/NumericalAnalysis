import functools
import math
import os
import time

import numpy as np
import matplotlib.pyplot as plt

def relevant(name):
    start = time.time()
    os.system(f'./funcs.exe examples/{name} < inputgamma.txt > {name}.outputgamma.txt')
    end = time.time()

    output = []

    with open(f'{name}.outputgamma.txt', 'rb') as f:
        output = [float(t.split(b'=')[1].split()[0]) for t in f.readlines() if t.startswith(b'result')]
        print(output)
        f.close()

    return np.array(output)

test = np.arange(-3.69, 7.37, 0.1)

inputgamma = functools.reduce(lambda a, b: a + '\n' + b, ['call function ' + str(x) for x in test])
with open('inputgamma.txt', 'wb+') as f:
    f.write(bytes(inputgamma, encoding='utf-8'))
    f.close()

actual = list(map(math.gamma, test))

lanc = relevant('gamma.c')
iterativegamma = relevant('iterativegamma.c')
example7 = relevant('example7.c')

plt.figure(figsize = (10, 7))

plt.xlim(-3.69, 7.3)
plt.ylim(-1E-5, 2E-5)

plt.xlabel('Function input')
plt.ylabel('Error = Aproximated - Actual')

plt.plot(test, lanc - actual, label='Lancoz Aproximation', alpha=0.6)
plt.plot(test, iterativegamma - actual, label='Iterative Aproximation', alpha=0.6)
plt.plot(test, example7 - actual, label='Integral Aproxmiation', alpha=0.6)

plt.legend(['Lancoz Aprox.', 'Iterative Aprox.', 'Integral Aprox.'])

plt.savefig('test.png')
