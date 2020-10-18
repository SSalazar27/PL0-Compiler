README

Instructions to utilize the PM1 Stack machine included

1. Open command prompt terminal (Eustis preferably)
2. Transfer all files included into eustis memory, as well as any code you wish to run on it
3. Compile Compile Driver code by typing:
    gcc hw4compiler.c
4. After compilation, you must run the output file with the text file instructions you wish to run
  as command line parameters like so:

  ./a.out "NameOfFile.txt"

	Where "NameOfFile.txt" should be replaced by the name of the desired file.

5. Include compiler directives -a for printout of generated assembly code, -l for printout of lexemes/tokens, and -v for
    printout of virtual machine execution trace, after the name of the file, for example, if you want to use all of the
    compiler directives you would type:

    ./a.out "NameOfFile.txt" -l -a -v

6. Watch the output of your program! Enjoy!

*** Included is also a sample input file text file as well as the corresponding output file to use as an example.
Note that in the output file, backspace characters may appear as squares in front of instruction names due to formatting issues
