import functools
import math
import os
import time

testcases = list(map(lambda x: x / 10, list(range(1, 5)))) + [2, 3, 4, 5]
gammresults = list(map(lambda x: math.gamma(x), testcases))

inputgamma = functools.reduce(lambda a, b: a + '\n' + b, ['call function ' + str(x) for x in testcases])
with open('inputgamma.txt', 'wb+') as f:
    f.write(bytes(inputgamma, encoding='utf-8'))
    f.close()

def relevant(name):
    start = time.time()
    os.system(f'./funcs.exe examples/{name} < inputgamma.txt > {name}.outputgamma.txt')
    end = time.time()

    output = []

    with open(f'{name}.outputgamma.txt', 'rb') as f:
        output = [float(t.split(b'=')[1].split()[0]) for t in f.readlines() if t.startswith(b'result')]
        print(output)
        f.close()

    print('-----')
    err = map(lambda a: a[0] - a[1], zip(output, gammresults))
    zipper = zip(testcases, output, gammresults, err)

    print('Name   = ', name)

    print('Input & Output & Actual & Error\\\\')
    for zi in zipper:
        print(*zi, sep=' & ', end='\\\\\n')

    print('Time   = ', end - start)
    print('Avarage Time = ', (end - start)/len(testcases))
    print('-----')

relevant('iterativegamma.c')
relevant('example7.c')
relevant('gamma.c')
