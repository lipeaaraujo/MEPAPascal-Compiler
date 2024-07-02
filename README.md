# Pascal-MEPA Compiler

Compiler of a simplified version of the Pascal language to MEPA, developed for the Compiladores course, lectured by Bruno César Ribas at the University of Brasília.

## How to use

Clone this repository:
```bash
git clone https://github.com/lipeaaraujo/PascalMEPA-Compiler.git
```

Compile the main files using the Makefile:

```bash
make
```
After compilation, the executable "compiler" will be created, to run the compiler, run the executable passing a simplified Pascal file as argument:

```bash
./compiler source.pas
```
To clean the executable, simply run the following make command:

```bash
make clean
```
