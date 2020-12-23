#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#define int long long // work with 64bit target

// instructions
enum { LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH, OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD, OPEN, READ,CLOS, PRTF, MALC, MSET, MCMP, EXIT };

int token;            // current token
char *src, *old_src;  // pointer to source code string;
int poolsize;         // default size of text/data/stack
int line;             // line number
int *text,            // text segment
    *old_text,        // for dump text segment
    *stack;           // stack
char *data;           // data segment

// virtual machine registers
int *pc,              // next instruction to be run           
    *bp,              // base pointer used in function calls
    *sp,              // the top of the stack
    ax,               // result of an instruction
    cycle;

void next() {
    token = *src++;
    return;
}

void expression(int level) {
    // do nothing
}

void program() {
    next();                  // get next token
    while (token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}

int eval() { 
    int op, *tmp;
    while (1) {
        op = *pc++;             // get next operation code
        if (op == IMM) {        // load immediate value to ax  
            ax = *pc++;
        }                                  
        else if (op == LC) {    // load character to ax, address in ax
            ax = *(char *)ax;
        }                              
        else if (op == LI) {    // load integer to ax, address in ax
            ax = *(int *)ax;
        }                                
        else if (op == SC) {     // save character to address, value in ax,
            ax = *(char *)*sp++ = ax; // address on stack
        }                  
        else if (op == SI) {    // save integer to address, value in ax, 
            *(int *)*sp++ = ax; // address on stack
        }                       
    }
    return 0;
}

int main(int argc, char **argv)
{
    int i, fd;

    argc--;
    argv++;

    poolsize = 256 * 1024; // arbitrary size
    line = 1;

    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }

    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }

    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }

    src[i] = 0; // add EOF character
    close(fd);

    // allocate memory for virtual machine
    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    bp = sp = (int *)((int)stack + poolsize);
    ax = 0;

    program();
    return eval();
}