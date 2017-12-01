Compile using make. I did have problems with declaring i inside for loops which was generating errors for C, but that should be fixed. 

Each function can use 8 registers. All params and local variables are stored on the stack and registers are freed at the end of every expression. 

Every assignment statement ends with the value being placed on the stack, to me this was the simplest way to generate code that shouldnt have register usage issues 
but the code generated tends to be redundant and has a lot of stores and loads of the same value back to back. 

I think the instructions bgt and blt are MARS specific macros, real MIPS only has branch instructions that compare to zero. 

Seg faults if you dont have exactly 1 int instruction in a function call, but I assume this is all we are responsible for. 

Functions as part of boolean expressions dont work becuase of register problems, the return value doesn't go to the right register for some reason. 

Using { } to enclose statement blocks doesn't work, loops and ifs can only have 1 statement. You can use brackets but everything after the first statement gets ignored. 

Expressions ignore order of operations and just go right to left. 

Both the bracketed statement issue and the order of operations issue go back to choices I made in assignment 2. If I could start over from nothing I would have made 
a lot of different design choices, coding assignment 4 made me see all of the bad choices I made when generating the tree.

This is a great project to learn compilers with!
