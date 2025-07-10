import cjson

def TestExceptions():
    try:
        cjson.loads('{key: 10}')
        assert False, "Failed"
    except ValueError:
        pass

    try:
        cjson.dumps({123: "value"})
        assert False, "Failed"
    except TypeError:
        pass

def Main():
    jsonStr = '{"hello": 10, "world": "value"}'
    expectedDict = {"hello": 10, "world": "value"}

    loadedDict = cjson.loads(jsonStr)
    assert loadedDict == expectedDict

    dumpedStr = cjson.dumps(loadedDict)
    assert cjson.loads(dumpedStr) == expectedDict

    assert cjson.loads("{}") == {}
    assert cjson.dumps({}) == "{}"

    TestExceptions()

    print("Passed")

if __name__ == "__main__":
    Main()
