// Sebastian Salazar
// COP 3402, Spring 2020

// ======================
// Lexical Analyzer - HW2
// ======================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Constants provided by instructions and own constant
#define MAX_ID_LEN  11
#define MAX_NUM_LEN 5
#define MAX_TOKENS 10000

// Token types as enumeration from 1-33
typedef enum
{
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym,  slashsym, oddsym, eqlsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

// Token structure, lexeme name can only be 11 characters because that is
// the maximum identifier length provided by the instructions
typedef struct
{
  int type;
  char lexeme[MAX_ID_LEN + 1];

} token;

// Array of every reserved word in language
char *reservedWords[14] =
{
  "begin", "end", "if", "then", "while", "do",  "call", "const", "var", "procedure", "write",
  "read", "else", "odd"
};

// Array of every special symbol in language
char specialSymbols[13] =
{
  '+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':'
};

// Global token pointer for array of tokens that have dynamic memory assigned
// to it in main, as well as the index to that array to keep track of how
// many tokens have been created thus far
token tokens[MAX_TOKENS];
int tokenIndex = 0;

// Function prototypes
void createToken(int type, char *lexeme);
int validSymbol(char c);
int validNumber(int n);
int validLetter(char c);
int isResWord(char *string);
int whichSymbol(char c);
int isWhiteSpace(char c);
char* whatName(int n);
void clearCurrString();

char temp;
int i;
char currString[MAX_ID_LEN + 1];
void Scanner(char* filename, int print)
{
  // Take the filename from command line parameters and open it, creating an
  // input file pointer ifp
  FILE* ifp = fopen(filename, "r");

  // Dynamically allocate memory to tokens array depending on constant of how
  // many tokens are allowed

  // Create temporary character variable where intermediately scanned in characters
  // will be stored, and an array index variable i

  // Continue to scan in characters until EOF is reached
  while ((temp = getc(ifp)) != EOF)
  {
    // At the beginning of every loop iteration, i is set to zero to start over
    // in array "currString" which just stores the intermediate characters that
    // the program is figuring out how to tokenize
    i = 0;
    currString[MAX_ID_LEN + 1];

    // First character scanned in is a valid number, verified by validNumber
    // function, which means we are either dealing with a potential number or
    // an identifier that starts with a number, which is an error
    if (validNumber(temp))
    {
      // Add temp character that was just scanned in into string of characters, and
      // increment i. Then continue scanning in characters as long as you don't reach
      // EOF
      currString[i++] = temp;
      while ((temp = getc(ifp)) != EOF)
      {
        // If whitespace character is scanned in, ignore it and tokenize what is stored
        // in currString with createToken function, setting the ith character in array
        // to null character to effectively truncate string at that point, enabling comparisons
        if (isWhiteSpace(temp))
        {
          currString[i] = '\0';
          createToken(numbersym, currString);
          break;
        }

        // If valid symbol is scanned in, verified by validSymbol function, backtrack and
        // return the symbol, and tokenize what you had before you scanned it in
        if (validSymbol(temp))
        {
          ungetc(temp, ifp);
          currString[i] = '\0';
          createToken(numbersym, currString);
          break;
        }

        // If a valid number is scanned in, store it in string and continue to scan things in
        if (validNumber(temp))
          currString[i++] = temp;

        // If a valid letter is scanned in, you have tried creating an Identifier
        // that starts with a number which is an error
        else if (validLetter(temp))
        {
          printf("ERROR: Variable does not start with letter\n");
          return;
        }

        // If i is at any point greater than MAX_NUM_LEN, your number is larger than the allowed
        // limit which is an error
        if (i > MAX_NUM_LEN)
        {
          printf("ERROR: Number too long\n");
          return;
        }
      }
    }

    // Scanned in valid letter so dealing with a potential identifier or reserved word
    else if (validLetter(temp))
    {
      // Add character to string
      currString[i++] = temp;

      // Continue scanning in characters so long as EOF not reached
      while ((temp = getc(ifp)) != EOF)
      {
        // If white space character read in, check if what is in currString
        // so far is a reserved word with function, and if so, use the return
        // value of the function to figure out which reserved word it was
        if (isWhiteSpace(temp))
        {
          int res = isResWord(currString);
          if (res <= 33)
            createToken(res, reservedWords[res - 21]);
          else if (res == 34)
            createToken(8, "odd");

          // If its not a reserved word, it must be an identifier, so
          // tokenize it as such
          else
          {
            currString[i] = '\0';
            createToken(identsym, currString);
          }

          break;
        }

        // If a valid letter or number is read in, continue reading characters in
        // because it is a valid identifier so far
        if (validLetter(temp) || validNumber(temp))
        {
          currString[i++] = temp;
        }

        // If a valid symbol is read in, once again check whether a reserved
        // word exists in currString and if so tokenize, if not tokenize everything
        // before the symbol as an identifier, and unget the valid symbol
        else if (validSymbol(temp))
        {
          int res = isResWord(currString);
          if (res <= 33)
            createToken(res, reservedWords[res - 21]);
          else if (res == 34)
            createToken(oddsym, "odd");

          else
          {
            currString[i] = '\0';
            createToken(identsym, currString);
          }

          ungetc(temp, ifp);
          break;
        }

        else if (!(isWhiteSpace(temp)))
        {
          printf("ERROR: Invalid symbol '%c' in identifier\n", temp);
          return;
        }

        // If at any point i exceeds maximum allowed identifier length, error generated
        if (i > MAX_ID_LEN)
        {
          printf("ERROR: Name too long\n");
          return;
        }
      }
    }

    // Scanned in valid symbol, so dealing with valid symbol or rel-op
    else if (validSymbol(temp))
    {
      currString[i++] = temp;
      temp = getc(ifp);
      currString[i++] = temp;

      if (!strncmp(currString, ":=", 2))
        createToken(becomessym, ":=");

      else if (!strncmp(currString, "<>", 2))
        createToken(neqsym, "<>");

      else if (!strncmp(currString, "<=", 2))
        createToken(leqsym, "<=");

      else if (!strncmp(currString, ">=", 2))
        createToken(geqsym, ">=");

      // If comments are found, ignore everything after it until the ending characters
      // are found and if they never are, return an error
      else if (!strncmp(currString, "/*", 2))
      {
        int stop = 0;
        while (((temp = getc(ifp)) != EOF) && (stop != 1))
        {
          if (temp == '*')
          {
            temp = getc(ifp);

            if (temp == '/')
              stop = 1;

            else
              ungetc(temp, ifp);
          }
        }

        if (stop != 1)
        {
          printf("ERROR: '*/' not found\n");
          return;
        }

      }

      else
      {
        ungetc(temp, ifp);
        i--;
        currString[1] = '\0';
        createToken(whichSymbol(currString[0]), currString);
      }
    }

    // If you get to this point and the symbol scanned in is not a white space
    // character, it is a symbol that is not valid and therefore cannot be tokenized
    else if (!(isWhiteSpace(temp)))
    {
      printf("ERROR: Invalid symbol: '%c'\n", temp);
      return;
    }
  }

  fclose(ifp);

  if (print == 1)
  {
    /*
    printf("Lexeme Table:\n");
    printf("lexeme\t\ttoken type\n");
    for(i = 0; i < tokenIndex; i++)
    {
      printf("%-9s\t%d\n", tokens[i].lexeme, tokens[i].type);
    }
    */

    // Prints out the Lexeme List as indicated by the instructions with nice
    // formatting by iterating through token array

    printf("\nLexeme List:\n");
    printf("\n(Internal)\n");
    printf("|");
    for (i = 0; i < tokenIndex; i++)
    {
      if (tokens[i].type == 3 || tokens[i].type == 2)
      printf("%d %s|", tokens[i].type, tokens[i].lexeme);
      else
      printf("%d|", tokens[i].type);
    }
    printf("\n\n");
    printf("(Symbolic)\n");
    printf("|");
    for (i = 0; i < tokenIndex; i++)
    {
      if (tokens[i].type == 3 || tokens[i].type == 2)
      printf("%s %s|", whatName(tokens[i].type), tokens[i].lexeme);
      else
      printf("%s|", whatName(tokens[i].type));
    }
    printf("\n");
  }
  printf("\n");
}

// Function that checks whethedr a charcter is a valid Special Symbol or not
// returning 1 if it is and 0 if it is not
int validSymbol(char c)
{
  return ((c >= '(' && c <= '/') || (c >= ':' && c <= '>'));
}

// Function that checks whether a character is a valid number, returning 1 if it
// is and 0 if it is not
int validNumber(int n)
{
  return (n >= '0' && n <= '9');
}

// Function that checks whether a character is a valid letter or not, returning 1
// if it is and 0 if it is not
int validLetter(char c)
{
  return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

// Function that checks whether a string is a reserved word or not by comparing it
// to every reserved word in the global reserved word array, returning the token
// type of the reserved word if it is one, with the exception of odd, which is handled
// in main
int isResWord(char *string)
{
  int i;
  for (i = 0; i < 14; i++)
  {
    if (!strncmp(string, reservedWords[i], strlen(reservedWords[i])))
      break;
  }

  i += 21;

  return i;
}

// Function that creates a token, adding it to the array of tokens. It takes in the
// type of token and assigns it to the type field in the token structure, and takes
// in the lexeme name and assigns it to the name field in the token structure
void createToken(int type, char *lexeme)
{
  strcpy(tokens[tokenIndex].lexeme, lexeme);
  tokens[tokenIndex].type = type;

  tokenIndex++;

  clearCurrString();

  return;
}

// Function that figures out the token type (integer) of a given valid symbol and
// returns it
int whichSymbol(char c)
{
  switch(c)
  {
    case '+':
      return plussym;
    case '-':
      return minussym;
    case '*':
      return multsym;
    case '/':
      return slashsym;
    case '(':
      return lparentsym;
    case ')':
      return rparentsym;
    case '=':
      return eqlsym;
    case ',':
      return commasym;
    case '.':
      return periodsym;
    case '<':
      return lessym;
    case '>':
      return gtrsym;
    case ';':
      return semicolonsym;
  }
}

// Function that determines whether a character is a whitespace character, returning
// 1 if it is and 0 if it is not
int isWhiteSpace(char c)
{
  return (c == '\n' || c == ' ' || c == '\t');
}

// Function that takes in an integer n which corresponds to the number of a token and prints out the
// name of the token as given by the instructions
char* whatName(int n)
{
  switch (n)
  {
    case 1:
      return "nulsym";
    case 2:
      return "identsym";
    case 3:
      return "numbersym";
    case 4:
      return "plussym";
    case 5:
      return "minussym";
    case 6:
      return "multsym";
    case 7:
      return "slashsym";
    case 8:
      return "oddsym";
    case 9:
      return "eqlsym";
    case 10:
      return "neqsym";
    case 11:
      return "lessym";
    case 12:
      return "leqsym";
    case 13:
      return "gtrsym";
    case 14:
      return "geqsym";
    case 15:
      return "lparentsym";
    case 16:
      return "rparentsym";
    case 17:
      return "commasym";
    case 18:
      return "semicolonsym";
    case 19:
      return "periodsym";
    case 20:
      return "becomessym";
    case 21:
      return "beginsym";
    case 22:
      return "endsym";
    case 23:
      return "ifsym";
    case 24:
      return "thensym";
    case 25:
      return "whilesym";
    case 26:
      return "dosym";
    case 27:
      return "callsym";
    case 28:
      return "constsym";
    case 29:
      return "varsym";
    case 30:
      return "procsym";
    case 31:
      return "writesym";
    case 32:
      return "readsym";
    case 33:
      return "elsesym";
  }
}

// Function that clears the contents of currString for use after token is created
void clearCurrString()
{
  int j;
  for (j = 0; j < MAX_ID_LEN; j++)
  {
    currString[j] = ' ';
  }

  currString[MAX_ID_LEN] = '\0';
}
