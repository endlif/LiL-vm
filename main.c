#include <stdio.h>
#include "vm.h"

byte sampleProgram[] = {
  0x01, 0x00, 0x00,       // i8load !!0 ?00
  0x01, 0x01, 0x03,       // i8load !!1 ?03
  0x01, 0x02, 0xFF,       // i8load !!2 ?FF
  0x01, 0x03, 0x09,       // i8load !!2 ?FF
  0x03, 0x00, 0x00, 0x01, // i8add !!0 !!0 !!1
  0x07, 0x09, 0x00, 0x03, // jne ?9 !!0 !!2
  0x00                    // halt
};

int main(int argc, char **argv) {
  int flag=1;
  int a,b,c;
  while(flag){
    printf("请输入2个数字：a b \n  ");
    scanf("%d %d",&a,&b);
    c=a+b;
    if(40==getNum(a,b)){
      flag=0;
      printf("恭喜你输入成功拿到flag\n");
    } else{
      printf("错误请重新输入!\n");
    };
  }
}


int  getNum(int a,int b){
  // Create a machine with 8 registers and 1kb of memory
  vm *vm = createVM(8, 1024);

  // Run a simple program
  int z=exec(vm, sampleProgram,a,b);

  // Free the machine's memory and registers
  destroyVM(vm);

  return z;

}
