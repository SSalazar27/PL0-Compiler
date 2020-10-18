// Sebastian Salazar
// COP 3402, Spring 2020

// ====================
// P - Machine HW1
// ====================
// Stack Machine

#include <stdio.h>
#include <string.h>

// definitions provided by instructions
#define MAX_STACK_HEIGHT 40
#define MAX_CODE_LENGTH 200
#define MAX_LEXI_LEVELS 3
#define REGISTER_FILE_SIZE 8

// structure for an instruction provided by instructions
typedef struct instruction
{
  int op;
  int r;
  int l;
  int m;
}instruction;

// global arrays required for program such as the stack, the actual code/instructions,
// and the register file
instruction code[MAX_CODE_LENGTH];
int stack[MAX_STACK_HEIGHT];
int registerFile[REGISTER_FILE_SIZE];

// boolean frequency array of whether there are activation records that start on certain registers
int activationRecords[MAX_STACK_HEIGHT];

// declaration and initialization of values as specified by instructions, stack pointer, base pointer,
// program counter, and instruction register respectively
int sp = 0;
int bp = 1;
int pc = 0;
int ir = 0;

// function prototypes
int base(int l, int base);
char* instructionName(int instructionNumber);

// main function for program that takes in command line arguments, intended to take in name of input file
void VMachine(int print)
{
  int i = 0;

  if (print == 1)
  {
    // print statements and formatting to print out the initial values of the register file, stack, pc, bp, and sp
    printf("\t\t\tpc\tbp\tsp\tregisters\n");
    printf("Initial values\t\t%d\t%d\t%d\t",
    pc, bp, sp);
    for (i = 0; i < REGISTER_FILE_SIZE; i++)
    printf("%d ", registerFile[i]);

    printf("\nStack: ");
    for (i = 1; i < MAX_STACK_HEIGHT; i++)
    printf("%d ", stack[i]);
    printf("\n\n");
    printf("\t\t\tpc\tbp\tsp\tregisters\n");
  }

  // while loop which runs the program to completion
  int halt = 0;
  while (!halt)
  {
    // variable to save the current instruction to be utilized
    instruction ir;

    // saving the location of the current instruction to keep track of where activation record bars go
    // beofre the program counter gets incremented in the next step
    int currentInstruction = pc;

    // use the program counter to find the current instruction in the code and then increment the program counter
    // immediately
    ir = code[pc++];

    // switch statement whose case depends on the op code of the current instruction
    switch (ir.op)
    {

      // LIT R, 0, M : Loads a constant value (literal) M into Register R
      case 1:
        registerFile[ir.r] = ir.m;
        break;
      // RTN 0, 0, 0 : Returns from a subroutine and restore the caller environment
      case 2:
        sp = bp - 1;
        bp = stack[sp + 3];
        pc = stack[sp + 4];
        activationRecords[sp + 1] = 0;
        break;
      // LOD R, L, M : Load value into a selected register from the stack location at
      // offset M from L lexicographical levels down
      case 3:
        registerFile[ir.r] = stack[base(ir.l, bp) + ir.m];
        break;
      // STO  R, L, M : Store value from a selected register in the stack location at
      // offset M from L lexicographical levels down
      case 4:
        stack[base(ir.l, bp) + ir.m] = registerFile[ir.r];
        break;
      // CAL 0, L, M : Call procedure at code index M (generates new Activation Record and pc <- M)
      case 5:
        stack[sp + 1] = 0;
        stack[sp + 2] = base(ir.l, bp);
        stack[sp + 3] = bp;
        stack[sp + 4] = pc;
        bp = sp + 1;
        pc = ir.m;
        activationRecords[bp] = 1;
        break;
      // INC  0, 0, M : Allocate M locals (increment sp by M). First four are Functional Value,Static Link (SL),
      //  Dynamic Link (DL),and Return Address (RA)
      case 6:
        sp = sp + ir.m;
        break;
      // JMP 0, 0, M : Jump to instruction M
      case 7:
        pc = ir.m;
        break;
      // JPC  R, 0, M : Jump to instruction M if R = 0
      case 8:
        if (registerFile[ir.r] == 0)
          pc = ir.m;
        break;
      // SIO   R, 0, 1 Write a register to the screen
      case 9:
        printf("%d\n", registerFile[ir.r]);
        break;
      // SIO   R, 0, 2 Read in input from the user and store it in a register
      case 10:
        scanf("%d", &registerFile[ir.r]);
        break;
      // SIO   0, 0, 3 End of program (program stops running)
      case 11:
        halt = 1;
        break;
      // NEG   (R[i] <-- -R[i])
      case 12:
        registerFile[ir.r] *= -1;
        break;
      // ADD   (R[i] <-- R[j] + R[k])
      case 13:
        registerFile[ir.r] = registerFile[ir.l] + registerFile[ir.m];
        break;
      // SUB    (R[i] <-- R[j] - R[k])
      case 14:
        registerFile[ir.r] = registerFile[ir.l] - registerFile[ir.m];
        break;
      // MUL   (R[i] <-- R[j] * R[k])
      case 15:
        registerFile[ir.r] = registerFile[ir.l] * registerFile[ir.m];
        break;
      // DIV     (R[i] <-- R[j] / R[k])
      case 16:
        registerFile[ir.r] = registerFile[ir.l] / registerFile[ir.m];
        break;
      // ODD   (R[i] <-- R[i] mod 2)
      case 17:
        registerFile[ir.r] = registerFile[ir.r] % 2;
        break;
      // MOD  (R[i] <-- R[j] mod  R[k])
      case 18:
        registerFile[ir.r] = registerFile[ir.l] % registerFile[ir.m];
        break;
      //  EQL   (R[i] <-- R[j] = = R[k])
      case 19:
        registerFile[ir.r] = registerFile[ir.l] == registerFile[ir.m];
        break;
      // NEQ   (R[i] <-- R[j] != R[k])
      case 20:
        registerFile[ir.r] = registerFile[ir.l] != registerFile[ir.m];
        break;
      // LSS     (R[i] <-- R[j] < R[k])
      case 21:
        registerFile[ir.r] = registerFile[ir.l] < registerFile[ir.m];
        break;
      // LEQ   (R[i] <-- R[j] <= R[k])
      case 22:
        registerFile[ir.r] = registerFile[ir.l] <= registerFile[ir.m];
        break;
      // GTR   (R[i] <-- R[j] > R[k])
      case 23:
        registerFile[ir.r] = registerFile[ir.l] > registerFile[ir.m];
        break;
      // GEQ   (R[i] <-- R[j] >= R[k])
      case 24:
        registerFile[ir.r] = registerFile[ir.l] >= registerFile[ir.m];
        break;
    }

    if (print == 1)
    {
      // prints out the register file, stack contents, and sp, bp, pc, r, l, and m after every instruction execution
      // in a nice format, with | representing seperation between different activation records
      printf("%d %s  %d %d %d\t\t%d\t%d\t%d\t", currentInstruction, instructionName(ir.op), ir.r, ir.l, ir.m, pc, bp, sp);

      for (i = 0; i < REGISTER_FILE_SIZE; i++)
      printf("%d ", registerFile[i]);

      printf("\nStack: ");
      for (i = 1; i < sp + 1; i++)
      {
        if (activationRecords[i])
        printf("| ");

        printf("%d ", stack[i]);
      }
      printf("\n\n");
    }
  }

  return;
}

int base(int l, int base) // l stand for L in the instruction format
{
  int b1; //find base L levels down
  b1 = base;
  while (l > 0)
  {
    b1 = stack[b1 + 1];
    l--;
  }
  return b1;
}

// function that takes in the opcode/instruction number of an instruction, and outputs the
// name of that specific instruction as a string
char* instructionName(int instructionNumber)
{
  switch (instructionNumber)
  {
    case 1:
      return "lit";
    case 2:
      return "rtn";
    case 3:
      return "lod";
    case 4:
      return "sto";
    case 5:
      return "cal";
    case 6:
      return "inc";
    case 7:
      return "jmp";
    case 8:
      return "jpc";
    case 9:
      return "sio";
    case 10:
      return "sio";
    case 11:
      return "sio";
    case 12:
      return "neg";
    case 13:
      return "add";
    case 14:
      return "sub";
    case 15:
      return "mul";
    case 16:
      return "div";
    case 17:
      return "odd";
    case 18:
      return "mod";
    case 19:
      return "eql";
    case 20:
      return "neq";
    case 21:
      return "lss";
    case 22:
      return "leq";
    case 23:
      return "gtr";
    case 24:
      return "geq";
  }
}
