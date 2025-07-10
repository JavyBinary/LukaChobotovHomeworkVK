.PHONY: all install clean test perfTest
all: install
install: python3 setup.py install --user
test: python3 test_cjson.py
perfTest: python3 perf_test.py
clean: rm -rf build dist *.egg-info cjson.cpython-*.so
