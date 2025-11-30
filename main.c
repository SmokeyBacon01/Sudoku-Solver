#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_ROWS 9
#define MAX_COLS 9
#define MAX_BOXES 9
#define MAX_UNITS 9

#define BOX_ROWS 3
#define BOX_COLS 3

#define NUMBERS 9

#define EMPTY 0

#define NAKED_SET_CAP 5 // arbitrary cap, beyond this naked sets are incredibly rare.

#define MAX_SIZE 1024

enum units {
    ROW,
    COL,
    BOX,
    UNIT_COUNT
};

struct cell {
    int row;
    int col;
};

struct tile {
    int answer;
    bool pencil[NUMBERS];
};

struct answer {
    int answer;
    int row;
    int col;
};

struct box {
    struct tile box[BOX_ROWS][BOX_COLS];
    // starting coordinates for the box
    int row;
    int col;
};

struct hidden_pencil_counter {
    int count;
    int row;
    int col;
};

struct doubles {
    int one;
    int two;
};

struct remove_pointing {
    struct cell protected[BOX_ROWS];
    enum units remove_unit_type;
    int number_to_be_removed;
    bool is_pointing_set_found;
};

struct protected {
    struct cell cell;
    struct protected *next;
};

void initialise_board(struct tile board[MAX_ROWS][MAX_COLS]);
void print_board(struct tile board[MAX_ROWS][MAX_COLS]);
void print_border(void);
void print_divider(void);

bool is_solved(struct tile board[MAX_ROWS][MAX_COLS]);
bool is_failed(bool is_legal, int count);
bool is_legal_board(struct tile board[MAX_ROWS][MAX_COLS]);
bool is_legal_answer(struct tile board[MAX_ROWS][MAX_COLS], struct answer answer);

void initialise_pencil(struct tile board[MAX_ROWS][MAX_COLS]);

void confirm_answer(struct tile board[MAX_ROWS][MAX_COLS], struct answer answer);
void fill_naked_singles(struct tile board[MAX_ROWS][MAX_COLS]);
void fill_hidden_singles(struct tile board[MAX_ROWS][MAX_COLS]);

void eliminate_pointing_sets(struct tile board[MAX_ROWS][MAX_COLS]);
int get_total_candidate_count(struct box box, int candidate_index);
struct remove_pointing find_pointing_set(
    struct tile board[MAX_ROWS][MAX_COLS], 
    struct box box,
    int number, 
    int total_count
);
void remove_pointing_candidates(
    struct tile board[MAX_ROWS][MAX_COLS], 
    struct remove_pointing pointing
);


struct box make_box(
    struct tile board[MAX_ROWS][MAX_COLS],
    int box_index
);
int get_box(int row, int col);

void solve_sudoku(struct tile board[MAX_ROWS][MAX_COLS]);

// debug function to test sudokus. Should replace with a generator or user input.
void debug_input_sudoku(struct tile board[MAX_ROWS][MAX_COLS]) {
    // // Can be solved with only singles
    // int sudoku[MAX_ROWS * MAX_COLS] = {
    // 5, 3, 0, 0, 7, 0, 0, 0, 0,
    // 6, 0, 0, 1, 9, 5, 0, 0, 0,
    // 0, 9, 8, 0, 0, 0, 0, 6, 0,
    // 8, 0, 0, 0, 6, 0, 0, 0, 3,
    // 4, 0, 0, 8, 0, 3, 0, 0, 1,
    // 7, 0, 0, 0, 2, 0, 0, 0, 6,
    // 0, 6, 0, 0, 0, 0, 2, 8, 0,
    // 0, 0, 0, 4, 1, 9, 0, 0, 5,
    // 0, 0, 0, 0, 8, 0, 0, 7, 9
    // };

    // // Needs doubles
    // int sudoku[MAX_ROWS * MAX_COLS] = {
    // 0, 0, 0, 2, 0, 0, 0, 6, 3,
    // 3, 0, 0, 0, 0, 5, 4, 0, 1,
    // 0, 0, 1, 0, 0, 3, 9, 8, 0,
    // 0, 0, 0, 0, 0, 0, 0, 9, 0,
    // 0, 0, 0, 5, 3, 8, 0, 0, 0,
    // 0, 3, 0, 0, 0, 0, 0, 0, 0,
    // 0, 2, 6, 3, 0, 0, 5, 0, 0,
    // 5, 0, 3, 7, 0, 0, 0, 0, 8,
    // 4, 7, 0, 0, 0, 1, 0, 0, 0
    // };

    // Needs pointing pair
    int sudoku[MAX_ROWS * MAX_COLS] = {
        0,0,0,6,9,0,8,0,0,
        7,0,1,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
        3,0,0,5,0,7,0,0,0,
        0,6,0,0,0,0,9,0,0,
        0,0,0,0,0,0,0,0,0,
        4,0,0,2,0,0,0,5,7,
        0,8,0,0,1,0,0,0,0,
        0,0,0,0,0,0,0,3,0
    };
    int i = 0;
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            board[row][col].answer = sudoku[i++];
        }
    }
}

// Debug function to view pencil
void debug_print_pencil(struct tile board[MAX_ROWS][MAX_COLS]) {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            printf("DEBUG: %d %d could be:", row, col);
            for (int i = 0; i < NUMBERS; i++) {        
                if (board[row][col].pencil[i]) {
                    printf(" %d", i + 1);
                }
            }
            printf("\n");
        }
    }
}

void debug_print_nakeds(struct tile board[MAX_ROWS][MAX_COLS]) {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            bool naked = true;
            for (int i = 0; i < NUMBERS; i++) {        
                if (board[row][col].pencil[i]) {
                    naked = false;
                } 
            }
            if (naked && board[row][col].answer == 0) {
                printf("DEBUG: %d %d is naked!\n", row, col);
            }
        }
    }
}

int main(void) {
    
    struct tile board[MAX_ROWS][MAX_COLS];
    initialise_board(board);

    // print_board(board);

    debug_input_sudoku(board);

    print_board(board);

    initialise_pencil(board);
    
    // debug_print_pencil(board);

    solve_sudoku(board);
    
    debug_print_pencil(board);
    // debug_print_nakeds(board);

    print_board(board);

    return 0;
}

// Loops until sudoku is solved, makes an illegal move, or infinite loops.
void solve_sudoku(struct tile board[MAX_ROWS][MAX_COLS]) {
    int count = 0;
    bool is_legal = false;

    while (!is_solved(board)) {

        fill_naked_singles(board);
        fill_hidden_singles(board);
        eliminate_pointing_sets(board);

        count++;
        is_legal = is_legal_board(board);
        is_legal = true;

        print_board(board);
        if (is_failed(is_legal, count)) {
            break;
        }
    }

    printf("iteration count = %d\n", count);
}

// Returns true if sudoku solver needs to exit for any reason other than solved puzzle.
bool is_failed(bool is_legal, int count) {
    if (!is_legal) {
        printf("Illegal, quitting\n");
        return true;
    }

    if (count == 10) {\
        printf("%d iterations, quitting\n", count);
        return true;
    }

    return false;
}

/////////////////////////////////////////////////////SOLVING LOGIC//////////////////////////////////////////////////////////

/////////////////////////////////SIMPLE SOLVES////////////////////////////////

// If all candidates of a number in a row/col share a box, they form an Including Set.
// That candidate cannot appear in the rest of the box.

// If all candidates of a number in a box also belong to a row or col, they form a Pointing Set.
// That candidate cannot appear anywhere else in that row/col.
void eliminate_pointing_sets(struct tile board[MAX_ROWS][MAX_COLS]) {
    for (int box_index = 0; box_index < MAX_BOXES; box_index++) {
        struct box box = make_box(board, box_index);
        for (int number = 0; number < NUMBERS; number++) {
            int total_count = get_total_candidate_count(box, number);
            if (total_count == 0) {
                continue;
            }
            struct remove_pointing pointing_set = find_pointing_set(board, box, number, total_count);
            if (pointing_set.is_pointing_set_found) {
                remove_pointing_candidates(board, pointing_set);
            }
        }
    }
}

void remove_pointing_candidates(
    struct tile board[MAX_ROWS][MAX_COLS], 
    struct remove_pointing pointing
) {
    if (pointing.remove_unit_type == ROW) {
        int row = pointing.protected[0].row;
        for (int col = 0; col < MAX_COLS; col++) {
            bool should_skip = false;
            for (int i = 0; i < BOX_ROWS; i++) {
                if (col == pointing.protected[i].col) {
                    should_skip = true;
                }
            }
            if (should_skip) {
                continue;
            } else {
                board[row][col].pencil[pointing.number_to_be_removed] = false;
            }   
        }
    }

    if (pointing.remove_unit_type == COL) {
        int col = pointing.protected[0].col;
        for (int row = 0; row < MAX_ROWS; row++) {
            bool should_skip = false;
            for (int i = 0; i < BOX_ROWS; i++) {
                if (row == pointing.protected[i].row) {
                    should_skip = true;
                }
            }
            if (should_skip) {
                continue;
            } else {
                board[row][col].pencil[pointing.number_to_be_removed] = false;
            }   
        }
    }
}

struct remove_pointing find_pointing_set(
    struct tile board[MAX_ROWS][MAX_COLS], 
    struct box box,
    int number, 
    int total_count
) {
    struct remove_pointing pointing;
    pointing.is_pointing_set_found = false;

    for (int row = box.row; row < BOX_ROWS + box.row; row++) {
        int count = 0;
        int index = 0;
        for (int col = box.col; col < BOX_COLS + box.col; col++) {
            pointing.protected[index].row = row;
            pointing.protected[index].col = col;
            index++;
            if (board[row][col].answer != 0) {
                continue;
            } else if (board[row][col].pencil[number]) {
                count++;
            }

        }
        if (count == total_count) {
            pointing.remove_unit_type = ROW;
            pointing.is_pointing_set_found = true;
            pointing.number_to_be_removed = number;
            printf("DEBUG: POINTING PAIR ON unit %d, member %d %d, target %d\n", pointing.remove_unit_type, pointing.protected[0].row, pointing.protected[0].col, pointing.number_to_be_removed);
            return pointing;
        }
    }

    for (int col = box.col; col < BOX_COLS + box.col; col++) {
        int count = 0;
        int index = 0;
        for (int row = box.row; row < BOX_ROWS + box.row; row++) {
            pointing.protected[index].row = row;
            pointing.protected[index].col = col;
            index++;
            if (board[row][col].answer != 0) {
                continue;
            } else if (board[row][col].pencil[number]) {
                count++;
            }
        }
        if (count == total_count) {
            pointing.remove_unit_type = COL;
            pointing.is_pointing_set_found = true;
            pointing.number_to_be_removed = number;
            printf("DEBUG: POINTING PAIR ON unit %d, member %d %d, target %d\n", pointing.remove_unit_type, pointing.protected[0].row, pointing.protected[0].col, pointing.number_to_be_removed);
            return pointing;
        }
    }

    return pointing;
}

int get_total_candidate_count(struct box box, int candidate_index) {
    int count = 0;
    for (int row = 0; row < BOX_ROWS; row++) {
        for (int col = 0; col < BOX_COLS; col++ ) {
            if(box.box[row][col].pencil[candidate_index]) {
                count++;
            }
        }
    }
    return count;
}





// If two cells contain two of the same number and are the only cells to contain them, it is a Hidden Pair.
// Those cells cannot contain any other two numbers.

// If two cells in a unit contain only two numbers, it is a Naked Pair. Those two numbers cannot appear anywhere else in the unit.

// Naked set -> identify sets of naked numbers. Iterate from n = 2 -> n = 5.
// 1. Take a number,  n = [2,5]
// 2. Create a set of numbers with n unique numbers. 
// 3. Take a unit
// 4. Iterate across unit and check for pencils that exclusively contain numbers from the set.
// 5. If count = n, then a naked set has been found
// 6. Those cells become protected and the rest of the unit has those numbers removed from their pencils.

// Create set of all numbers. First, create a simple one that contains combinations of a single number.




// If a unit only has one spot for a number, even if the cell has other possibilities, it is a Hidden Single.
// Keep a counter for each number. Iterate across a unit resetting the counter each complete pass.
void fill_hidden_singles(struct tile board[MAX_ROWS][MAX_COLS]) {
    struct answer answer;
    struct hidden_pencil_counter pencil_count[NUMBERS];
    for (int i = 0; i < NUMBERS; i++) {
        pencil_count[i].count = 0;
    }

    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            for (int i = 0; i < NUMBERS; i++) {
                if (board[row][col].answer != EMPTY) {
                    continue;
                }
                if (board[row][col].pencil[i]) {
                    pencil_count[i].count++;
                    pencil_count[i].row = row;
                    pencil_count[i].col = col;
                }
            }
        }
        for (int i = 0; i < NUMBERS; i++) {
            if (pencil_count[i].count == 1) {
                answer.row = pencil_count[i].row;
                answer.col = pencil_count[i].col;
                answer.answer = i + 1;
                printf("DEBUG: Hidden single in row %d\n", row);
                confirm_answer(board, answer);
            }
            pencil_count[i].count = 0;
        }
    }


    for (int col = 0; col < MAX_COLS; col++) {
        for (int row = 0; row < MAX_ROWS; row++) {
            for (int i = 0; i < NUMBERS; i++) {
                if (board[row][col].answer != EMPTY) {
                    continue;
                }
                if (board[row][col].pencil[i]) {
                    pencil_count[i].count++;
                    pencil_count[i].row = row;
                    pencil_count[i].col = col;
                }
            }
        }
        for (int i = 0; i < NUMBERS; i++) {
            if (pencil_count[i].count == 1) {
                answer.row = pencil_count[i].row;
                answer.col = pencil_count[i].col;
                answer.answer = i + 1;
                printf("DEBUG: Hidden single in col %d\n", col);
                confirm_answer(board, answer);
            }
            pencil_count[i].count = 0;
        }
    }

    for (int box_index = 0; box_index < MAX_BOXES; box_index++) {
        struct box box = make_box(board, box_index);
        for (int row = box.row; row < BOX_ROWS + box.row; row++) {
            for (int col = box.col; col < BOX_COLS + box.col; col++) {
                if (board[row][col].answer != EMPTY) {
                    continue;
                }
                for (int i = 0; i < NUMBERS; i++) {
                    if (board[row][col].pencil[i]) {
                        pencil_count[i].count++;
                        pencil_count[i].row = row;
                        pencil_count[i].col = col;
                    }
                }
            }
        }
        for (int i = 0; i < NUMBERS; i++) {
            if (pencil_count[i].count == 1) {
                answer.row = pencil_count[i].row;
                answer.col = pencil_count[i].col;
                answer.answer = i + 1;
                printf("DEBUG: Hidden single in box %d\n", box_index);
                confirm_answer(board, answer);
            }
            pencil_count[i].count = 0;
        }
    }
}


// If a cell can only contain one number, it is a Naked Single. 
// Take a single cell and check how many pencil marks it has.
// If that number is 1, then that is the answer.
void fill_naked_singles(struct tile board[MAX_ROWS][MAX_COLS]) {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            if (board[row][col].answer != EMPTY) {
                continue;
            }
            
            int pencil_count = 0;
            for (int i = 0; i < NUMBERS; i++) {
                if (board[row][col].pencil[i]) {
                    pencil_count++;
                }
            }
            if (pencil_count == 1) {
                int i = 0;
                while (!board[row][col].pencil[i]) {
                    i++;
                }
                struct answer answer;
                answer.answer = i + 1;
                answer.row = row;
                answer.col = col;
                printf("DEBUG: naked single\n");
                confirm_answer(board, answer);
            }

        }
    }
}

////////////////////////////////SOLVING UTILITIES/////////////////////////////////

// Returns true if every grid has been filled. Does not check if it has been solved correctly.
bool is_solved(struct tile board[MAX_ROWS][MAX_COLS]) {
    for(int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            if (board[row][col].answer == EMPTY) {
                return false;
            }
        }
    }

    return true;
}

// Takes an answer and inserts it into the board. Will then remove pencils of the input cell,
// as well as eliminate respective pencils for the units that contained the cell.
void confirm_answer(struct tile board[MAX_ROWS][MAX_COLS], struct answer answer) {
    printf("DEBUG: %d is at %d %d\n", answer.answer, answer.row, answer.col);

    board[answer.row][answer.col].answer = answer.answer;
    if (!is_legal_answer(board, answer)) {
        printf("DEBUG: ILLEGAL answer of %d on %d %d\n", answer.answer, answer.row, answer.col);
    }

    for (int i = 0; i < NUMBERS; i++) {
        board[answer.row][answer.col].pencil[i] = false;
    }

    for (int row = 0; row < MAX_ROWS; row++) {
        board[row][answer.col].pencil[answer.answer - 1] = false;
    }

    for (int col = 0; col < MAX_COLS; col++) {
        board[answer.row][col].pencil[answer.answer - 1] = false;
    }

    struct box box = make_box(board, get_box(answer.row, answer.col));
    for (int row = box.row; row < BOX_ROWS + box.row; row++) {
        for (int col = box.col; col < BOX_COLS + box.col; col++) {
            board[row][col].pencil[answer.answer - 1] = false;
        }
    }
}

// Initialises pencil value to true if it could theoretically be an answer and false if it cannot.
void initialise_pencil(struct tile board[MAX_ROWS][MAX_COLS]) {
    struct answer answer;
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            answer.row = row;
            answer.col = col;
            for (int i = 0; i < NUMBERS; i++) {                        
                if (board[row][col].answer != EMPTY) {
                    board[row][col].pencil[i] = false;
                    continue;
                }
                
                answer.answer = i + 1;
                board[row][col].pencil[i] = is_legal_answer(board, answer);
            }
        }
    }
}

// Checks if the board has any rule-breaking solutions. Returns true if a unit has 2 of the same number.
bool is_legal_board(struct tile board[MAX_ROWS][MAX_COLS]) {
    struct answer answer;
    bool is_legal = true;
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            answer.answer = board[row][col].answer;
            answer.row = row;
            answer.col = col;
            is_legal = is_legal_answer(board, answer);
            if (!is_legal) {
                return false;
            }
        }
    }

    return true;
}

// Checks if an answer can belong in a cell.
bool is_legal_answer(struct tile board[MAX_ROWS][MAX_COLS], struct answer answer) {
    
    // printf("DEBUG: checking %d %d\n", answer.row, answer.col);
    if (answer.answer == EMPTY) {
        // printf("DEBUG: legal\n");
        return true;
    }

    for (int row = 0; row < MAX_ROWS; row++) {
        if (answer.answer != board[row][answer.col].answer) {
        } else if (row == answer.row) {
            continue;
        } else {
            return false;
        }
    }

    for (int col = 0; col < MAX_COLS; col++) {
        if (answer.answer != board[answer.row][col].answer) {
            continue;
        } else if (col == answer.col) {
            continue;
        } else {
            return false;
        }
    }

    struct box box = make_box(board, get_box(answer.row, answer.col));
    for (int row = box.row; row < BOX_ROWS + box.row; row++) {
        for (int col = box.col; col < BOX_COLS + box.col; col++) {
            if (answer.answer != box.box[row - box.row][col - box.col].answer) {
                continue;
            } else if (row == answer.row && col == answer.col) {
                continue;
            } else {
                return false;
            }
        }
    }

    return true;
}

int get_box(int row, int col) {
    return row - row % BOX_ROWS  + col / BOX_COLS;
}

//////////////////////////////////////////////////////SETUP PHASE///////////////////////////////////////////////////////////

// makes a box of a certain index

/*
0 1 2
3 4 5
6 7 8
*/
struct box make_box(
    struct tile board[MAX_ROWS][MAX_COLS],
    int box_index
) {
    struct tile box[BOX_ROWS][BOX_COLS];
    int board_row = (box_index / BOX_ROWS) * BOX_ROWS;
    int board_col = (box_index % BOX_COLS) * BOX_COLS;

    for (int i = 0; i < BOX_ROWS; i++) {
        board_col = (box_index % BOX_COLS) * BOX_COLS;
        for (int j = 0; j < BOX_COLS; j++) {
            box[i][j] = board[board_row][board_col];
            board_col++;
        }
        board_row++;
    }

    struct box final_box;
    memcpy(final_box.box, box, sizeof(box));
    final_box.row = (box_index / BOX_ROWS) * BOX_ROWS;
    final_box.col = (box_index % BOX_COLS) * BOX_COLS;

    return final_box;
}

// Initialises every cell to EMPTY (0).
void initialise_board(struct tile board[MAX_ROWS][MAX_COLS]) {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            board[row][col].answer = EMPTY;
        }
    }
}

// Prints the board.
void print_board(struct tile board[MAX_ROWS][MAX_COLS]) {
    print_border();
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            if ((col) % BOX_COLS == 0) {
                printf("|");
            } else {
                printf(".");
            }

            if (board[row][col].answer == EMPTY) {
                printf("   ");
            } else {
                printf(" %d ", board[row][col].answer);
            }
        }
        printf("|\n");
        if ((row + 1) % BOX_ROWS == 0 && row != MAX_ROWS - 1) {
            print_divider();
        }
    }
    print_border();
}

// Helper function to print the border.
void print_border(void) {
    printf("-------------------------------------\n");
}

// Helper function to print dividers.
void print_divider(void) {
    printf("|-----------+-----------+-----------|\n");
}