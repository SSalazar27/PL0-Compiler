// Sebastian Salazar
// COP 3402, Spring 2020

// =========================================================================
// Parser/Intermediate Code Generator (with procedure, call, and else) - HW4
// =========================================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define the maximum number of symbols in the symbol table and set it to some arbitrary value that can be
// changed
#define MAX_SYMBOL_TABLE_LENGTH 500

// structure for symbols as specified by instructions
typedef struct
{
  int kind; 		  // const = 1, var = 2, proc = 3
	char name[12];	// name up to 11 chars
	int val; 		    // number (ASCII value)
	int level; 		  // L level
	int addr; 		  // M address
  int mark;       // to indicate that code has been generated already for a block
} symbol;


// Defijne all virtual machine instructions as their op codes for increased readability
#define LIT 1
#define RTN 2
#define LOD 3
#define STO 4
#define CAL 5
#define INC 6
#define JMP 7
#define JPC 8
#define PRT 9
#define SCN 10
#define END 11
#define NEG 12
#define ADD 13
#define SUB 14
#define MUL 15
#define DIV 16
#define ODD 17
#define MOD 18
#define EQL 19
#define NEQ 20
#define LSS 21
#define LEQ 22
#define GTR 23
#define GEQ 24

// Array of symbols that will act as the symbol table for the program
symbol symbolTable[MAX_SYMBOL_TABLE_LENGTH];

// Declare global currToken varibale which will store the type of the current token being
// analyzed by the parser/code generator
int currToken;

// Declare and initialize currentToken index to be able to iterate through the array of tokens
// produced by the scanner and store them in currToken
int currTokenIndex = 0;

// cx is the index for the array of instructions "code", tx is the index for the symbol table
// currentReg keeps track of what register is open (registers function like a stack, so if registers
// 2 is open for example, it means 2-7 are open and 0 and 1 are in use)
int cx = 0;
int tx = 1;
int currentReg = 0;

// lastIdent keeps track of the most recent identifier token analyzed, for use in the symbol table
char lastIdent[12];

// lastNum keeps track of the most recent number token analyzed, for codegen purposes and table purposes
int lastNum;

// Current lexicographical level
int currLevel = 0;

void error(int n);
void getToken();

// Functions that mirror the structure of the PL/0 grammar that enable parsing/code generation
void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();

void emit(int op, int r, int l, int m);
int findInSymbolTable(char* ident);
void enterTable(int k, int pos);

// Parsing program that calls the scanner and them parses its output as well as generates code with it
// depending on whether printCode and printScan are 1 or not, prints output for itself and or scanner
void Parser(char* filename, int printCode, int printScan)
{
  // Run scanner with filename and prints output depending on value of printScan, then run program
  Scanner(filename, printScan);
  program();
  printf("No errors, program is syntactically correct\n\n");

  // If this line is reached, no errors were detected and it prints out a success message

  // If printCode is 1, that means the user used the -g directive and wants the generated code to be
  // printed to the screen
  if (printCode == 1)
  {
    int i;
    printf("Generated Code\n");
    for (i = 0; i < cx; i++)
    {
      printf("%d %d %d %d\n", code[i].op, code[i].r, code[i].l, code[i].m);
    }
  }

  return;
}

// Error function that takes in number and prints out error corresponding to that number, then end the program
void error(int n)
{
  switch(n)
  {
    case 1:
      printf("Use = instead of :=");
      break;
    case 2:
      printf("= must be followed by a number");
      break;
    case 3:
      printf("Identifier must be followed by =");
      break;
    case 4:
      printf("const, var, procedure must be followed by identifier");
      break;
    case 5:
      printf("Semicolon or comma missing");
      break;
    case 9:
      printf("Period expected");
      break;
    case 11:
      printf("Undeclared identifier");
      break;
    case 12:
      printf("Assignment to constant or procedure is not allowed");
      break;
    case 13:
      printf("Assignment operator expected");
      break;
    case 14:
      printf("call must be followed by an identifier");
      break;
    case 15:
      printf("Call of a constant or variable is meaningless");
      break;
    case 16:
      printf("then expected");
      break;
    case 18:
      printf("do expected");
      break;
    case 19:
      printf("end symbol expected");
      break;
    case 20:
      printf("Relational operator expected");
      break;
    case 22:
      printf("Right parenthesis missing");
      break;
    case 23:
      printf("The preceding factor cannot begin with this symbol");
      break;
    case 26:
      printf("Variable expected after read");
      break;
    case 27:
      printf("Variable expected after write");
      break;
    case 31:
      printf("Code has exceeded max length");
      break;
  }
  printf("\n");
  // Exit program
  exit(EXIT_SUCCESS);
}

// getToken uses the currTokenIndex to iterate through the array of tokens and get the next token and set its type
// to currToken
void getToken()
{
  //printf("%s\n", tokens[currTokenIndex].lexeme);
  currToken = tokens[currTokenIndex++].type;

  // if currToken is a variable, save it as the last identifier
  if (currToken == 2)
    strcpy(lastIdent, tokens[currTokenIndex - 1].lexeme);

  // if currToken is a number, save it as the last number
  if (currToken == 3)
    lastNum = atoi(tokens[currTokenIndex - 1].lexeme);
}

void program()
{
  getToken();
  block();
  if (currToken != periodsym)
    // "Period expected"
    error(9);

  // Emit end of program instruction at the end of the program then return
  emit(END, 0, 0, 3);
  return;
}

void block()
{

  // space keeps track owf the position on the stack in the virtual machine, by default it is 4
  // because of the 4 default values added at the start of every activation record, this goes up depending on
  // how many variables get declared in the instruction
  int space = 4;
  //currLevel++;
  int jmpCodeAddr = cx;
  emit(JMP, 0, 0, 0);

  if (currToken == constsym)
  {
    do
    {
      getToken();
      if (currToken != identsym)
        // "const, var, procedure must be followed by identifier"
        error(4);
      getToken();
      if (currToken == becomessym)
        // "Use = instead of :="
        error(1);
      if (currToken != eqlsym)
        // "Identifier must be followed by ="
        error(3);
      getToken();
      if (currToken != numbersym)
        // "= must be followed by a number"
        error(2);
      // entering a constant (1) with a lexicographical level of 0
      enterTable(1, space);
      space++;
      getToken();
    } while(currToken == commasym);
    if (currToken != semicolonsym)
      // "semicolon or comma missing"
      error(5);
    getToken();
  }

  if (currToken == varsym)
  {
    do
    {
      getToken();
      if (currToken != identsym)
        // "const, var, procedure must be followed by identifier"
        error(4);
        // entering a variable (2) with a lexicographical level of 2
      enterTable(2, space);
      space++;
      getToken();
    } while(currToken == commasym);
    if (currToken != semicolonsym)
      // "semicolon or comma missing"
      error(5);
    getToken();
  }

  while (currToken == procsym)
  {
    int procTx = tx;
    int iter;
    getToken();
    if (currToken != identsym)
      // "const, var, procedure must be followed by identifier"
      error(4);

    enterTable(3, space);
    symbolTable[tx - 1].addr = jmpCodeAddr + 1;

    getToken();
    if (currToken != semicolonsym)
      error(5);
    getToken();
    currLevel++;
    block();

    for (iter = procTx + 1; iter < tx; iter++)
    {
      // if (symbolTable[iter].kind != 3
        symbolTable[iter].mark = 1;
    }

    emit(RTN, 0, 0, 0);
    if (currToken != semicolonsym)
      error(5);
    getToken();
  }

  // INC op depending on space which depends on how many variables were declared
  code[jmpCodeAddr].m = cx;
  emit(INC, 0, 0, space);
  statement();
  currLevel--;
}

void statement()
{
  // Declare varibale i to store the index of the identifier in the symbol table or 0 if the identifier is not found
  // also declare variable ctemp to be able to use for changing jump location later
  int i, ctemp, ctemp2;
  if (currToken == identsym)
  {
    i = findInSymbolTable(lastIdent);
    if (i == 0)
      // "Undeclared identifier"
      error(11);
    // if symbol is not a variable (kind != 2)
    if (symbolTable[i].kind != 2)
      // "Assignment to constant or procedure is not allowed"
      error(12);
    getToken();
    if (currToken != becomessym)
      // "Assignment operator expected"
      error(13);
    getToken();
    expression();
    // Program set up in a way to guarantee that whenever a value is to be put back into storage, it will reside in
    // register 0
    currentReg = 0;
    emit(STO, currentReg, currLevel - symbolTable[i].level, symbolTable[i].addr);
  }

  else if (currToken == callsym)
  {
    getToken();
    if (currToken != identsym)
      // "call must be followed by an identifier"
      error(14);
    i = findInSymbolTable(lastIdent);
    if (i == 0)
      // "Undeclared identifier"
      error(11);
    // if symbol is not a procedure (kind != 3)
    if (symbolTable[i].kind != 3)
      // "call must be followed by a procedure identifier"
      error(15);
    else
    {
      emit(CAL, 0, currLevel - symbolTable[i].level, symbolTable[i].addr);
    }
    getToken();
  }

  else if (currToken == beginsym)
  {
    getToken();
    statement();
    while (currToken == semicolonsym)
    {
      getToken();
      statement();
    }
    if (currToken != endsym)
      // "end symbol expected"
      error(19);
    getToken();
  }

  else if (currToken == ifsym)
  {
    getToken();
    condition();
    if (currToken != thensym)
      // "then expected"
      error(16);
    getToken();

    // Set cx to a temporary code index, then run  function with a dummy location to jump to
    ctemp = cx;
    // Conditional jump to a temporary value until the location of the exit of the loop is determined
    emit(JPC, currentReg - 1, 0, 0);
    currentReg = 0;
    statement();

    // else
    if (currToken == elsesym)
    {
      printf("HI\n");
      int ctemp2 = cx;

      emit (JMP, 0, 0, 0);

      code[ctemp].m = cx;
      getToken();
      statement();
      code[ctemp2].m = cx;
    }

    // After running statement, use ctemp to change the jump destination to the new cx, now that you
    // know where you need to actually jump to
    else
    {
      currTokenIndex--;
      getToken();
      code[ctemp].m = cx;
    }
  }

  else if (currToken == whilesym)
  {
    // Save the current index of the code to be able to tell jump where to go at end of while loop
    ctemp = cx;
    getToken();
    condition();

    // After condition is ran save the current index of the code in the other temporary index to
    // know where in code the instruction to be changed is
    ctemp2 = cx;
    currentReg = 0;
    emit(JPC, currentReg, 0, 0);

    if (currToken != dosym)
      // "do expected"
      error(18);
    getToken();
    statement();

    // Jump to ctemp
    emit(JMP, 0, 0, ctemp);

    // Change the destination of the conditional jump instruction created earlier
    code[ctemp2].m = cx;

  }

  else if (currToken == writesym)
  {
    getToken();

    if (currToken != identsym)
      error(27);

    i = findInSymbolTable(lastIdent);
    if (i == 0)
      // "Undeclared identifier"
      error(11);
    currentReg = 0;
    // Program set up in such a way where register 0 will be empty and it can be the destination of LOD and
    // subsequently PRINT (SIO) when this needs to be called
    emit(LOD, currentReg, currLevel - symbolTable[i].level, symbolTable[i].addr);
    emit(PRT, currentReg, 0, 1);

    getToken();
  }

  else if (currToken == readsym)
  {
    getToken();
    if (currToken != identsym)
      // "Variable expected after read"
      error(26);

    i = findInSymbolTable(lastIdent);
    if (i == 0)
      // "Undeclared identifier"
      error(11);
    else if(symbolTable[i].kind != 2)
      // "Assignment to constant or procedure is not allowed"
      error(12);

    // Scan and store input from the user into currentReg
    emit(SCN, currentReg, 0, 2);
    emit(STO, currentReg, currLevel - symbolTable[i].level, symbolTable[i].addr);
    getToken();
  }
}

void condition()
{
  int relOp;
  if (currToken == oddsym)
  {
    getToken();
    expression();
    // Machine set up in a way where value to be analyzed whether odd or not will always
    // be at register 0 already if we get 0 at this point, then store result back in 0
    emit (ODD, 0, 0, 0);
  }
  else
  {
    expression();

    // range of relational operators
    if (currToken < eqlsym || currToken > geqsym)
      // "Relational operator expected"
      error(20);
    relOp = currToken;
    getToken();
    expression();

    switch (relOp)
    {
      case 9:
        // equal sign
        emit(EQL, currentReg - 2, currentReg - 2, currentReg - 1);
        break;
      case 10:
        // not equal sign
        emit(NEQ, currentReg - 2, currentReg - 2, currentReg - 1);
        break;
      case 11:
        // less than sign
        emit(LSS, currentReg - 2, currentReg - 2, currentReg - 1);
        break;
      case 12:
        // less than or equal to sign
        emit(LEQ, currentReg - 2, currentReg - 2, currentReg - 1);
        break;
      case 13:
        // greater than sign
        emit(GTR, currentReg - 2, currentReg - 2, currentReg - 1);
        break;
      case 14:
        // greater than or equal to sign
        emit(GEQ, currentReg - 2, currentReg - 2, currentReg - 1);
        break;
    }
    currentReg--;
  }
}

void expression()
{
  int addOP;
  if (currToken == plussym || currToken == minussym)
  {
    addOP = currToken;
    getToken( );
    term();
    if(addOP == minussym)
      emit(NEG, currentReg, 0, 0);
  }

  else
    term();

  while (currToken == plussym || currToken == minussym)
  {
    addOP = currToken;
    getToken();
    term();
    if (addOP == plussym)
      emit(ADD, currentReg - 2, currentReg - 2, currentReg - 1);
    else
      emit(SUB, currentReg - 2, currentReg - 2, currentReg - 1);
    currentReg--;
  }
}

void term()
{
  int termOP;
  factor();
  while (currToken == multsym || currToken == slashsym)
  {
    termOP = currToken;
    getToken();
    factor();
    if (termOP == multsym)
      emit(MUL, currentReg - 2, currentReg - 2, currentReg - 1);
    else
      emit(DIV, currentReg - 2, currentReg - 2, currentReg - 1);
    currentReg--;
  }
}

void factor()
{
  int i, kind, level, addr, val;
  if (currToken == identsym)
  {
    i = findInSymbolTable(lastIdent);
    if (i == 0)
      // "Undeclared identifier"
      error(11);
    kind = symbolTable[i].kind;
    level = symbolTable[i].level;
    addr = symbolTable[i].addr;
    val = symbolTable[i].val;

    if (kind == 1)
    {
      emit(LIT, currentReg, 0, val);
      currentReg++;
    }
    else
    {
      emit(LOD, currentReg, currLevel - level, addr);
      currentReg++;
    }

    getToken();
  }

  else if (currToken == numbersym)
  {
    emit(LIT, currentReg, 0, lastNum);
    currentReg++;
    getToken();
  }
  else if (currToken == lparentsym)
  {
    getToken();
    expression();
    if (currToken != rparentsym)
      // "Right parenthesis missing"
      error(22);
    getToken();
  }
  else
    // "The preceding factor cannot begin with this symbol"
    error(23);
}

// Takes in an op code, an r, an l, and an m, and generates an instruction with those and adds it
// to the code array
void emit(int op, int r, int l, int m)
{
  if(cx > MAX_CODE_LENGTH)
    error(31);
  else
  {
    code[cx].op = op; //opcode
    code[cx].r = r; //register
    code[cx].l = l;// lexicographical level
    code[cx].m = m;// modifier
    cx++;
  }
}

// Takes in an identifier and tries to find it in the symbol table, returning its index in the symbol
// table if it is found and 0 otherwise
int findInSymbolTable(char * ident)
{
  int i = tx;
  while (i > 0)
  {
    //printf("Name: %s\nMark: %d\n", symbolTable[i].name, symbolTable[i].mark);

    if ((!strcmp(symbolTable[i].name, ident)) && (symbolTable[i].mark == 0))
    {
      return i;
    }
    i--;
  }
  return i;
}

// Enters a variable/constant into the symbol table depending on the k value at a lexicographical level "lev"
void enterTable(int k, int pos)
{
  // copy the last identifier into the name field of the new symbol in table
  strcpy(symbolTable[tx].name, lastIdent);

  // copy k value parameter into kind field of the new symbol in table
  symbolTable[tx].kind = k;

  switch (k)
  {
    // if the symbol is a constant
    case 1:
      symbolTable[tx].val = lastNum;
      symbolTable[tx].mark = 0;
      break;
    // if the symbol is a variable
    case 2:
      symbolTable[tx].level = currLevel;
      symbolTable[tx].addr = pos;
      symbolTable[tx].mark = 0;
      break;
    // if the symbol is a procedure
    case 3:
      symbolTable[tx].level = currLevel;
      symbolTable[tx].mark = 0;
      break;
  }

  tx++;
}
