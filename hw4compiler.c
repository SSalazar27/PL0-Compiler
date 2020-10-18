// Sebastian Salazar
// COP 3402, Spring 2020

// ============================================================
// Compiler Driver - HW4
// Combines Scanner, Parser/Code Generator, and Virtual Machine
// ============================================================

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Use the Scanner, Parser/Code Generator, and Virtual Machine as libraries
#include "VMachine.h"
#include "Scanner.h"
#include "Parser.h"

// Define true and false as 1 and 0 respectively for the command line arguments/readability
#define TRUE 1
#define FALSE 0

int main(int argc, char** argv)
{
  // Take the filename from command line parameters and open it, creating an
  // input file pointer ifp
  char* filename;
  filename = argv[1];
  FILE* ifp = fopen(filename, "r");

  // Close file when finished using it
  fclose(ifp);

  // Create flags to detect compiler directives and set them to 0 (false) initially
  int aFlag = FALSE;
  int vFlag = FALSE;
  int lFlag = FALSE;

  // Code for detecting compiler directives and setting appropriate flags to true (1)

  // If there are 5 command line arguments, it means all of the directives were used
  // so set all flags equal to 1 (true)
  if (argc == 5)
  {
    aFlag = TRUE;
    vFlag = TRUE;
    lFlag = TRUE;
  }

  // Otherwise, unless there were only 2 command line arguments, in which case there
  // would be no compiler directives, try to figure out which compiler directives were used
  else if (argc != 2)
  {
    // If the second character of the first directive is v, set vFlag to true
    if (argv[2][1] == 'v')
      vFlag = TRUE;
    // Otherwise, check if the second character is a, and set aFlag to true if so
    else if (argv[2][1] == 'a')
      aFlag = TRUE;
    // Otherwise, it MUST be the lFlag so set it to true
    else
      lFlag = TRUE;
    // Only continute if argc is 4, if its 3 then there are no more compiler directives
    if (argc == 4)
    {
      // Check the second character of the second directive and using the same logic as before
      // figure out what it is and set the correct flag to true accordingly
      if (argv[2][2] == 'v')
        vFlag = TRUE;
      else if (argv[2][2] == 'a')
        aFlag = TRUE;
      else
        lFlag = TRUE;
    }
  }

  // Run the parser, feeding it the filename and the aFlag and lFlag for it to know whether or
  // not to print output
  Parser(filename, aFlag, lFlag);
  // If vFlag is set to TRUE (1), runs and prints out the virtual machine trace, if not,
  // run but dont print anything
  VMachine(vFlag);

}
