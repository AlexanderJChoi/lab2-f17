#include "types.h"
#include "user.h"

int test(int n) { 
    int x = n + 1;
    return x;
}

int lab3(int count)
{
  

  if(count <  1000) {
    
    lab3(count+1);
  }
   register int sp asm ("sp");
   printf(1, "stack pointer: %d\n", sp);
 return sp;
}

int main(int argc, char *argv[]) {
    printf(1, "####################################################\n");
    printf(1, "# This program tests the correctness of your lab #3\n");
    printf(1, "####################################################\n");

    printf(1, "Test 1: Simple test\n");
    int pid = 0;
    pid = fork();
    if(pid == 0){
        test(1);
        exit();
    }
    wait();

    printf(1, "Test 2: Stack growth test.\n");
    //int originalSp = sp;
  //Write your own test
    printf(1, "stack pointer %d", lab3(0));
    printf(1, "If the inital print of stack pointer and the final print of stack pointer are more than 4096 apart, the test passes");
    exit();
}




