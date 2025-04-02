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

test = np.arange(-5.69, 5.37, 0.1)

inputgamma = functools.reduce(lambda a, b: a + '\n' + b, ['call function ' + str(x) for x in test])
with open('inputgamma.txt', 'wb+') as f:
    f.write(bytes(inputgamma, encoding='utf-8'))
    f.close()

actual = list(map(math.gamma, test))

lanc = relevant('gamma.c')
iterativegamma = relevant('iterativegamma.c')

plt.plot(test, actual - lanc, label='Lancoz Aproximation')
plt.plot(test, actual - iterativegamma, label='Iterative Aproximation')
# plt.plot(test, lanc, label='Lancoz Aproximation')
#plt.plot(test, example7 , label='Iterative Aproximation')

plt.savefig('test.png')
