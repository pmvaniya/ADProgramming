# AD Programming Language

* Self Hosted Compiler
* Stack Based Programming
* Simple Syntax like Python

<br>

### A brief overview

1. The `source` file contains the source code. A sample source file has been provided in the repo.
2. Compiler first generates `stage1`, which acts as an intermediate code for the compiler.
3. Then, compiler converts this `stage1` code to `assembly.asm` assembly code.
4. Finally, this assembly code is executed to get the desired result.

Before, executing the code, make sure you have the following tools installed on your system: `gcc` `nasm` `ld`.

<br>

### Compiling the source filel

To compile the source code, execute the command in the parent directory:
```
make
```

To run the generated machine code, execute:
```
./machine
```

Finally, to check the output, execute:
```
echo $?
```