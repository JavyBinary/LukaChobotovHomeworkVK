#include <Python.h>
#include <string>
#include <sstream>
#include <stdexcept>

class JsonException : public std::runtime_error {
public:
    explicit JsonException(const std::string& message) : std::runtime_error("Exception: " + message) {}
};

namespace CJsonImpl {
static std::string Trim(const std::string& str) {
    const auto firstPos = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == firstPos) {
        return str;
    }
    const auto lastPos = str.find_last_not_of(" \t\n\r");
    return str.substr(firstPos, (lastPos - firstPos + 1));
}

static PyObject* Parse(const std::string& jsonStr) {
    PyObject* dict = PyDict_New();
    if (!dict) {
        throw JsonException("Dict failed");
    }

    const auto startPos = jsonStr.find('{');
    const auto endPos = jsonStr.rfind('}');
    if (startPos == std::string::npos || endPos == std::string::npos) {
        Py_DECREF(dict);
        throw JsonException("Invalid format");
    }

    std::string content = jsonStr.substr(startPos + 1, endPos - startPos - 1);
    if (Trim(content).empty()) {
        return dict;
    }

    std::stringstream ss(content);
    std::string pair;

    while (std::getline(ss, pair, ',')) {
        if (Trim(pair).empty()) continue;

        const auto colonPos = pair.find(':');
        if (colonPos == std::string::npos) {
            Py_DECREF(dict);
            throw JsonException("Invalid pair");
        }

        std::string keyRaw = Trim(pair.substr(0, colonPos));
        if (keyRaw.length() < 2 || keyRaw.front() != '"' || keyRaw.back() != '"') {
            Py_DECREF(dict);
            throw JsonException("Invalid key");
        }
        std::string keyStr = keyRaw.substr(1, keyRaw.length() - 2);

        std::string valueRaw = Trim(pair.substr(colonPos + 1));
        PyObject* pyValue = nullptr;

        if (valueRaw.front() == '"' && valueRaw.back() == '"') {
            std::string valueStr = valueRaw.substr(1, valueRaw.length() - 2);
            pyValue = Py_BuildValue("s", valueStr.c_str());
        } else {
            try {
                long long valueLL = std::stoll(valueRaw);
                pyValue = Py_BuildValue("L", valueLL);
            } catch (...) {
                Py_DECREF(dict);
                throw JsonException("Invalid value");
            }
        }

        if (!pyValue) {
            Py_DECREF(dict);
            throw JsonException("Build value failed");
        }

        PyObject* pyKey = Py_BuildValue("s", keyStr.c_str());
        if (!pyKey) {
            Py_DECREF(pyValue);
            Py_DECREF(dict);
            throw JsonException("Build key failed");
        }

        if (PyDict_SetItem(dict, pyKey, pyValue) < 0) {
            Py_DECREF(pyKey);
            Py_DECREF(pyValue);
            Py_DECREF(dict);
            throw JsonException("Set item failed");
        }

        Py_DECREF(pyKey);
        Py_DECREF(pyValue);
    }

    return dict;
}

static std::string Dump(PyObject* dict) {
    std::stringstream ss;
    ss << "{";

    PyObject *key, *value;
    Py_ssize_t pos = 0;
    bool first = true;

    while (PyDict_Next(dict, &pos, &key, &value)) {
        if (!first) {
            ss << ", ";
        }
        first = false;

        const char* keyStr = PyUnicode_AsUTF8(key);
        if (!keyStr) {
            throw JsonException("Key not string");
        }
        ss << "\"" << keyStr << "\": ";

        if (PyUnicode_Check(value)) {
            const char* valueStr = PyUnicode_AsUTF8(value);
            ss << "\"" << valueStr << "\"";
        } else if (PyLong_Check(value)) {
            long long valueLL = PyLong_AsLongLong(value);
            if (PyErr_Occurred()) {
                PyErr_Clear();
                throw JsonException("Value too large");
            }
            ss << valueLL;
        } else {
            throw JsonException("Unsupported type");
        }
    }

    ss << "}";
    return ss.str();
}
}

static PyObject* CJsonLoads(PyObject* self, PyObject* args) {
    const char* jsonStrC;
    if (!PyArg_ParseTuple(args, "s", &jsonStrC)) {
        return NULL;
    }

    try {
        return CJsonImpl::Parse(jsonStrC);
    } catch (const JsonException& e) {
        PyErr_SetString(PyExc_ValueError, e.what());
        return NULL;
    }
}

static PyObject* CJsonDumps(PyObject* self, PyObject* args) {
    PyObject* dict;
    if (!PyArg_ParseTuple(args, "O!", &PyDict_Type, &dict)) {
        return NULL;
    }

    try {
        std::string result = CJsonImpl::Dump(dict);
        return Py_BuildValue("s", result.c_str());
    } catch (const JsonException& e) {
        PyErr_SetString(PyExc_TypeError, e.what());
        return NULL;
    }
}

static PyMethodDef cjsonMethods[] = {
    {"loads", CJsonLoads, METH_VARARGS, NULL},
    {"dumps", CJsonDumps, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef cjsonModule = {
    PyModuleDef_HEAD_INIT,
    "cjson",
    NULL,
    -1,
    cjsonMethods
};

PyMODINIT_FUNC PyInit_cjson(void) {
    return PyModule_Create(&cjsonModule);
}
