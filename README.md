[![LICENSE](https://img.shields.io/badge/license-Anti%20996-blue.svg)](https://github.com/996icu/996.ICU/blob/master/LICENSE)
# VM-to-assembler-translator
### A simple implementation that translates VM code to hack assembly code

compile:
```bash
g++ -g -std=c++11 -Wall -pedantic -o main.out *.cpp
```
run:
```bash
./main.out test.vm 
```
will generate test.asm (with same name)


Debug with extra info:
```bash
./main.out test.vm -v
```

All the white spaces and comments are ignored (content after // in any single line)


