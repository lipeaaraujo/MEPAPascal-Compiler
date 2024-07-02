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
After compilation, a executable will have been created, run the executable passing a simplified **Pascal** file as argument:

```bash
./compiler source.pas
```
If there are no errors, an output **MEPA** file should be created

To clear any compilation files, run the following command:

```bash
make clean
```
