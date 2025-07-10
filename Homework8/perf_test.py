import timeit
import random
import string
import json

try:
    import ujson
    ujsonAvailable = True
except ImportError:
    ujsonAvailable = False

import cjson

def GenerateDict(items):
    d = {}
    for _ in range(items):
        key = ''.join(random.choices(string.ascii_lowercase, k=8))
        if random.random() > 0.5:
            value = random.randint(0, 1000000)
        else:
            value = ''.join(random.choices(string.ascii_lowercase, k=15))
        d[key] = value
    return d

def RunBenchmark():
    testDict = GenerateDict(5000)
    testStr = json.dumps(testDict)
    number = 100

    tCjson = timeit.timeit(lambda: cjson.dumps(testDict), number=number)
    print(f"cjson: {tCjson:.4f}s")
    tJson = timeit.timeit(lambda: json.dumps(testDict), number=number)
    print(f"json:  {tJson:.4f}s")
    if ujsonAvailable:
        tUjson = timeit.timeit(lambda: ujson.dumps(testDict), number=number)
        print(f"ujson: {tUjson:.4f}s")

    tCjsonLoads = timeit.timeit(lambda: cjson.loads(testStr), number=number)
    print(f"cjson: {tCjsonLoads:.4f}s")
    tJsonLoads = timeit.timeit(lambda: json.loads(testStr), number=number)
    print(f"json:  {tJsonLoads:.4f}s")
    if ujsonAvailable:
        tUjsonLoads = timeit.timeit(lambda: ujson.loads(testStr), number=number)
        print(f"ujson: {tUjsonLoads:.4f}s")

if __name__ == "__main__":
    RunBenchmark()
