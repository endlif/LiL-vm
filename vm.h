#ifndef VM_H
#define VM_H

#include <stdint.h>

typedef uint8_t byte;

typedef struct
{
    size_t numRegisters;
    size_t memorySize;
    byte *registers;
    byte *memory;
} vm;

vm *createVM(size_t numRegisters, size_t memorySize);

void destroyVM(vm *vm);

int exec(vm *vm, byte *code, int a, int b);

int getNum(int a, int b);

void printInfo(vm *vm);

#endif
