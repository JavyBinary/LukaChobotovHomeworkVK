from setuptools import setup, Extension

cjsonModule = Extension(
    'cjson',
    sources=['cjson.cpp'],
    language='c++',
    extra_compile_args=['-std=c++20'],
)

setup(
    name='cjson',
    ext_modules=[cjsonModule],
)
