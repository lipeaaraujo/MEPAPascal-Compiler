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

## Simplified Pascal Grammar

```c
// syntax:
// <program>                 -> 'program' <IDENTIFIER> ['(' <identifier-list> ')'] ';' <block> '.'

// <block>                   -> [<label-declaration>] [<var-declaration>]
//                              [<subroutines>] <statement-list>

// <label-declaration>       -> 'label' <NUMBER> (',' <NUMBER>)* ';'

// <var-declaration>         -> 'var' <identifier-list> ':' <type>
//                              (';' <identifier-list> ':' <type>)* ';'

// <identifier-list>         -> <IDENTIFIER> (',' <IDENTIFIER>)*

// <type>                    -> 'integer' | 'real' | 'boolean'

// <subroutines>             -> (<procedure> | <function>)*

// <procedure>               -> 'procedure' <IDENTIFIER> [<params>] ';' <block>

// <function>                -> 'function' <IDENTIFIER> [<params>] ':' <IDENTIFIER> ';' <block>

// <params>                  -> '(' [var] <identifier-list> : <IDENTIFIER> 
//                              (';' [var] <identifier-list> : <IDENTIFIER>)* ')'

// <statement-list>          -> 'begin' <statement> (';' <statement>) 'end'

// <statement>               -> [<NUMBER> ':'] <assignment> | <procedure-call> | <deviation> |
//                              <statement-list> | <if-statement> | <while-statement> |
//                              <write-statement> | <read-statement>

// <assignment>              -> <IDENTIFIER> ':=' <expression>

// <subroutine-call>         -> <IDENTIFIER> ['(' <expression-list> ')']

// <deviation>               -> 'goto' <NUMBER>

// <if-statement>            -> 'if' <expression> 'then' <statement> ('else' <statement>)?

// <while-statement>         -> 'while' <expression> 'do' <statement>

// <write-statement>         -> 'write' '(' <expression-list> ')'

// <read-statement>          -> 'read' '(' <identifier-list> ')'

// <expression-list>         -> <expression> (',' <expression>)*

// <expression>              -> <simple-expression> [<relation> <simple-expression>]

// <relation>                -> '=' | '<>' | '<' | '<=' | '>=' | '>'

// <simple-expression>       -> ['+' | '-'] <term> (('+' | '-' | 'or') <term>)*

// <term>                    -> <factor> (('*' | '/' | 'div' | 'and') <factor>)

// <factor>                  -> <IDENTIFIER> | <NUMBER> | <subroutine-call> | '(' <expression> ')' |
//                              'not' <factor>
```

