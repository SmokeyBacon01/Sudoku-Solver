# Sudoku-Solver
Attempt to solve a Sudoku

RULES OF SOLVING:
1. No backtracking - the solver must not backtrack to fix incorrect guesses

2. No brute force - the solver must not brute force a solution using a depth or breadth first search.

3. First try - consequence of rule 1 and 2, the solver must only fill in a square if it is certain that the square is correct.

4. No guessing - consequence of rule 3, the solver is not allowed to guess at all.

    4.1 - The solver may try to resolve a chain, but this method is best avoided if possible.

The idea is to create a program that attempts to solve a Sudoku puzzle in the same way a human does.

The ultimate goal is to produce a program that can defeat a 17-square sudoku in the minimum possible iterations.
    An iteration is counted as a single loop of the main while loop.
    Hopefully, Personally, I want to achieve less than 10 iterations but I don't know if thats possible.

TERMINOLOGY

Unit - A row, a column, or a box where each number 1-9 only appears once.

Pencil - All possible numbers a cell could be.

Naked Singles - A cell that can only contain a number.

Hidden Singles - A cell is the only one in its unit that can contain a number.

Naked Doubles - Two cells in a unit contains only two numbers.

Hidden Doubles - Two cells in a unit that are the only two to contain two numbers.