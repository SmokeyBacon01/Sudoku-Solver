# Sudoku-Solver
Attempt to solve a Sudoku

RULES OF SOLVING:
1. No backtracking - the solver must not backtrack to fix incorrect guesses

2. No brute force - the solver must not brute force a solution using a depth or breadth first search.

3. First try - consequence of rule 1 and 2, the solver must only fill in a square if it is certain that the square is correct.

4. No guessing - consequence of rule 3, the solver is not allowed to guess at all.

    4.1 - The solver may try to resolve a chain, but this method is best avoided if possible.

The idea is to create a program that attempts to solve a Sudoku puzzle in the same way a human does.

The ultimate goal is to produce a program that can defeat a 17-square sudoku.

TODO: 
Naked and hidden double detection
Fishing!

Hopefully both of these can be implemented in a scalable way so I don't have to manually program a leviathan or something.

As techniques get slightly more complicated, they get harder and harder to test. If anyone has a good testing platform pls send :_)