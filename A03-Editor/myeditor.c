#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for access()
#include <ncurses.h>

// ****** CONFIG AND STRUCTS ******

// editing modes, like in a game??
#define MODE_NORMAL 0
#define MODE_INSERT 1
#define MODE_COMMAND 2
#define MODE_SEARCH 3

// initial capacities
#define INIT_MAX_LINES 100
#define MAX_LINE_LEN 1024

// global struct for all the pointers
struct MyEditor {
    char **lines; // dynamically allocate array of strings
    int line_count; // current number of lines
    int line_cap; // max number of lines for lines array

    int cursor_row, cursor_col; // cursor location relative to file rather than screen
    int file_top; // topmost file row
    int file_left; // leftmost file column

    char *filename;
    int mode; // current editing mode

    char command[128]; // command line input after ':'
    int cmd_len; // length of command input

    char search[128]; // search phrase after '/'
    int *search_match; // dynamically allocated array of indices of found searches
    int match_len; // length of match array
    int match_idx; // index of current match within the search_match array
};

// global instance of struct
struct MyEditor myeditor; // yayyyyyyyyy~
// NOTE TO SELF!!! row = y; col = x; DO NOT FORGET!!

// ****** UTILITY ******

// function to exit ncurses when things die
void editorDied(const char *s) {
    endwin();
    perror(s);
    exit(1);
}

// function to free all dynamically allocated memory
void editorFree() {
    if (myeditor.lines) {
        for (int i = 0; i < myeditor.line_count; i++) {
            free(myeditor.lines[i]);
        }
        free(myeditor.lines);
        myeditor.lines = NULL; // default safety
    }
    if (myeditor.filename) {
        free(myeditor.filename);
        myeditor.filename = NULL; // default safety
    }
    if (myeditor.search_match) {
        free(myeditor.search_match);
        myeditor.search_match = NULL; // default safety
    }
}

// ****** FILE MANAGEMENT ******

// function to load and open file
void fileLoad(char *filename) {
    myeditor.filename = strdup(filename);
    if (!myeditor.filename) {
        editorDied("strdup filename");
    }

    // pointer!!
    FILE *fptr = fopen(filename, "r");

    // initialization!!
    myeditor.line_cap = INIT_MAX_LINES;
    myeditor.lines = malloc(myeditor.line_cap * sizeof(char *));

    // verify file can be opened
    if (!fptr) {
        printf("Could not open file '%s'\n", filename);
        // if could not open file, still allocate some memory or else things might die, like a new file!!
        myeditor.lines[0] = strdup("");
        myeditor.line_count = 1;
        return;
    }

    // buffer
    char buffer[MAX_LINE_LEN];
    int len;

    // read in all lines
    while (fgets(buffer, sizeof(buffer), fptr)) {
        if (myeditor.line_count >= myeditor.line_cap) {
            myeditor.line_cap *= 2; // like expandable array from data structures
            char** lines_new = realloc(myeditor.lines, myeditor.line_cap * sizeof(char*));
        
            // ensure lines_new is successful
            if (lines_new == NULL) {
                editorDied("Could not reallocate memory\n");
            }
            myeditor.lines = lines_new;
        }

        // apparently fgets adds a newline???
        len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        // duplicate from buffer into lines
        myeditor.lines[myeditor.line_count] = strdup(buffer);
        if (myeditor.lines[myeditor.line_count] == NULL) {
            editorDied("Could not allocate memory\n");
        }

        myeditor.line_count++;
    }

    fclose(fptr);

    // if file is empty, allocate tiny bit of memory
    if (myeditor.line_count == 0) {
        myeditor.lines[0] = strdup("");
        myeditor.line_count = 1;
    }
}

// function to save edited file
void fileSave() {
    if (!myeditor.filename) {
        return;
    }

    // I've been reading in so many files I forget we can write them too, haha
    FILE *fptr = fopen(myeditor.filename, "w");
    if (!fptr) {
        return;
    }

    for (int i = 0; i < myeditor.line_count; i++) {
        fprintf(fptr, "%s\n", myeditor.lines[i]);
    }

    fclose(fptr);
}

// ****** SCROLLING AND RENDERING ******

// function to render basically anything that gets rendered
void screenRender() {
    clear();
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // render file content
    // rows-2 leaves room for the command line
    for (int i = 0; i < rows-2; i++) {
        int file_row = i + myeditor.file_top; // row offset from topline
        
        // giving up on the line numbers, the padding required afterward really hurt my brain
        if (file_row < myeditor.line_count) {
            char *line = myeditor.lines[file_row]; // current working row in lines array
            mvprintw(i, 0, "%s", line);
        }
    }

    // render the banner, leaving room for the command line
    int banner_row = rows - 2;
    mvprintw(banner_row, cols-20, " Ln: %d Col: %d ", myeditor.cursor_row+1, myeditor.cursor_col+1); // +1 allows for 1-indexing

    // render command line and search bar
    if (myeditor.mode == MODE_COMMAND ||
        myeditor.mode == MODE_SEARCH) {
        mvprintw(rows-1, 0, "%s%s", (myeditor.mode == MODE_COMMAND) ? ":" : "/", myeditor.command);
        move(rows-1, myeditor.cmd_len+1); // move cursor to end of command / search
    } else {
        move(myeditor.cursor_row - myeditor.file_top,
            myeditor.cursor_col - myeditor.file_left
        );
    }

    refresh();
}

// function to scroll with arrow keys
void screenScroll(int ch) {
    int len = (myeditor.cursor_row < myeditor.line_count) ? strlen(myeditor.lines[myeditor.cursor_row]) : 0;

    // handles key input and cursor movement
    if (ch == KEY_UP) {
        if (myeditor.cursor_row > 0) {
            myeditor.cursor_row--;
        }
    } else if (ch == KEY_DOWN) {
        if (myeditor.cursor_row < myeditor.line_count-1) {
            myeditor.cursor_row++;
        }
    } else if (ch == KEY_LEFT) {
        if (myeditor.cursor_col > 0) {
            myeditor.cursor_col--;
        }
    } else if (ch == KEY_RIGHT) {
        if (myeditor.cursor_col < len) {
            myeditor.cursor_col++;
        }
    }

    // update cursor_col if the current line is shorter
    len = (myeditor.cursor_row < myeditor.line_count) ? strlen(myeditor.lines[myeditor.cursor_row]) : 0;
    if (myeditor.cursor_row > len) {
        myeditor.cursor_col = len;
    }

    // this is half a tomato, DO NOT TOUCH
    // get the screen size
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // vertical scrolling
    if (myeditor.cursor_row < myeditor.file_top) {
        myeditor.file_top = myeditor.cursor_row;
    }
    if (myeditor.cursor_row >= myeditor.file_top+rows-2) {
        myeditor.file_top = myeditor.cursor_row-rows+3;
    }

    // horizontal scrolling
    if (myeditor.cursor_col < myeditor.file_left) {
        myeditor.file_left = myeditor.cursor_col;
    }
    if (myeditor.cursor_col >= myeditor.file_left+cols) {
        myeditor.file_left = myeditor.cursor_row - cols + 1;
    }

    screenRender();
    refresh();
}

// ****** SEARCH ******

// function to search and store row index of searches
void searchDoc() {
    if (myeditor.search_match) {
        free(myeditor.search_match);
        myeditor.search_match = NULL; // default it
    }
    myeditor.match_len = 0;
    myeditor.match_idx = -1;

    // search phrase is empty
    if (myeditor.search[0] == '\0') {
        return;
    }

    // search through file and store row index in search_match
    for (int i = 0; i < myeditor.line_count; i++) {
        if (strstr(myeditor.lines[i], myeditor.search) != NULL) { // substring
            myeditor.search_match = realloc(myeditor.search_match, (myeditor.match_len+1) * sizeof(int));
            if (!myeditor.search_match) {
                editorDied("realloc search_match");
            }
            myeditor.search_match[myeditor.match_len] = i;
            myeditor.match_len++;
        }
    }
}

// function to toggle between found phrases
void searchNextPrev(int match) {
    if (myeditor.search_match == 0) {
        return;
    }

    // indicate in main that match can only be 1 or -1
    myeditor.match_idx += match;

    // implement wrap-around
    if (myeditor.match_idx < 0) {
        myeditor.match_idx = myeditor.match_len - 1;
    } else if (myeditor.match_idx >= myeditor.match_len) {
        myeditor.match_idx = 0;
    }

    // move cursor to stored index
    myeditor.cursor_row = myeditor.search_match[myeditor.match_idx];
    myeditor.cursor_col = 0; // borken

    screenScroll(0);
}

// ****** TYPING ******

// function to delete a character
void charDelete(int ch) {
    if (myeditor.cursor_row >= myeditor.line_count) {
        return;
    }

    char *line = myeditor.lines[myeditor.cursor_row]; // current working row
    int len = strlen(line);

    // shift chars right of cursor to the left by one
    memmove(&line[myeditor.cursor_col], &line[myeditor.cursor_col+1], len-myeditor.cursor_col+1);

    // reallocate space after deleting a char
    myeditor.lines[myeditor.cursor_row] = realloc(line, len);
    if (myeditor.lines[myeditor.cursor_row] == NULL && len > 0) {
        editorDied("realloc delete char");
    }

    // it was not a tomato, probably
}

// function to insert a character
void charInsert(int ch) {
    if (myeditor.cursor_row >= myeditor.line_count) {
        return;
    }

    char *line = myeditor.lines[myeditor.cursor_row]; // current working row
    int len = strlen(line);

    // reallocate space for the new char
    myeditor.lines[myeditor.cursor_row] = realloc(line, len+2);
    if (!myeditor.lines[myeditor.cursor_row]) {
        editorDied("realloc insert char");
    }
    line = myeditor.lines[myeditor.cursor_row];

    // shift chars from under the cursor to the right by one
    memmove(&line[myeditor.cursor_col+1], &line[myeditor.cursor_col], len-myeditor.cursor_col+1);

    // insert new char and update cursor location
    line[myeditor.cursor_col] = ch;
    myeditor.cursor_col++;
}

// ****** COMMAND AND INPUT ******

// function to handle regular input into the command line
int modeNormal(int ch) {
    int timeToQuit = 0;

    if (ch == ':') { // open command mode
        myeditor.mode = MODE_COMMAND;
        myeditor.cmd_len = 0;
        myeditor.command[0] = '\0';
    } else if (ch == '/') { // open search mode
        myeditor.mode = MODE_SEARCH;
        myeditor.match_len = 0;
        myeditor.search[0] = '\0';
    } else if (ch == 'i') { // open editing mode
        addch(ch);
        myeditor.mode = MODE_INSERT;
    } else if (ch == 'n') {
        searchNextPrev(1);
    } else if (ch == 'p') {
        searchNextPrev(-1);
    } else if (ch == 'x') {
        charDelete(ch);
    } else if (ch == 'G') {
        myeditor.cursor_row = myeditor.line_count - 1;
        myeditor.cursor_col = 0;
        screenScroll(0);
    } else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
        screenScroll(ch);
    }

    return timeToQuit;
}

// function to handle text editing, when you press 'i'
void modeInsert(int ch) {
    if (ch == 27) {
        myeditor.mode = MODE_NORMAL;
        if (myeditor.cursor_col > 0) {
            myeditor.cursor_col--;
        }
    } else if (ch == KEY_BACKSPACE || ch == 127) {
        if (myeditor.cursor_col > 0) {
            charDelete(ch);
            myeditor.cursor_col--;
        }
    } else if (ch >= 32 && ch <= 126) { // ascii values of alphanumberical characters
        charInsert(ch);
    }
}

// function to handle command line type stuff, when you press ':'
int modeCommand(int ch) {
    int timeToQuit = 0;

    if (ch == '\n') { // return to normal when command is finished inputting and enter is hit
        myeditor.mode = MODE_NORMAL;
        if (strcmp(myeditor.command, "q") == 0) {
            timeToQuit = 1; // quitquitquitquitquit
        } else if (strcmp(myeditor.command, "w") == 0) {
            fileSave(); // save!!
        } else if (myeditor.command[0] >= '0' && myeditor.command[0] <= '9') { // check if the first char is a number, borken
            int line_num = atoi(myeditor.command); // turn the string into a number
            if (line_num > 0 ) {
                myeditor.cursor_row = line_num - 1; // because line numbers are 1-indexed
                if (myeditor.cursor_row >= myeditor.line_count) {
                    myeditor.cursor_row = myeditor.line_count - 1;
                    myeditor.cursor_col = 0;
                    screenScroll(0);
                }
            }
        }
        // reset!!
        myeditor.cmd_len = 0;
        myeditor.command[0] = '\0';
    } else if (ch >= 32 && ch <= 126 && myeditor.cmd_len < sizeof(myeditor.command)-1) { // ascii values of alphanumerical characters
        myeditor.command[myeditor.cmd_len++] = ch;
        myeditor.command[myeditor.cmd_len] = '\0';
    } else if (ch == 27) { // hit escape to leave command mode and return to normal mode
        myeditor.mode = MODE_NORMAL;
        myeditor.cmd_len = 0;
        myeditor.command[0] = '\0';
    }
    return timeToQuit;
}

// function to handle search, when you press '/'
void modeSearch(int ch) {
    if (ch == '\n') { 
        myeditor.mode = MODE_NORMAL; // return to normal when full search phrase entered
        strcpy(myeditor.search, myeditor.command); // copy command line into search
        searchDoc();
        searchNextPrev(1);
        // reset!!
        myeditor.cmd_len = 0;
        myeditor.command[0] = '\0';
    } else if (ch == 27) { 
        myeditor.mode = MODE_NORMAL; // exit with escape
        myeditor.cmd_len = 0; // reset!!
        myeditor.command[0] = '\0';
        myeditor.search[0] = '\0';
    } else if (ch >= 32 && ch <= 126 && myeditor.cmd_len < sizeof(myeditor.command)-1) { // ascii values of alphanumerical characters
        myeditor.command[myeditor.cmd_len++] = ch;
        myeditor.command[myeditor.cmd_len] = '\0';
    }
}

// ****** MAIN METHOD ******

int main (int argc, char** argv) {
    memset(&myeditor, 0, sizeof(struct MyEditor));
    myeditor.mode = MODE_NORMAL;

    // intake command line and load file
	char* filename;
    if (argc < 2) { // verify the filename is present
		printf("usage: %s <filename>\n", argv[0]);
		return 1;
	}
    filename = argv[1]; // save the filename!!
	if (access(filename, F_OK) != 0) { // check it's a valid file!!
		printf("%s not found\n", filename);
	}
	fileLoad(filename); // load file!!

    // start cursed mode hehehehehehehehehehe
    initscr();
    keypad(stdscr, TRUE); // enable keyboard arrows
	raw(); // disable line buffering
	noecho(); // don't show typed input automatically

    // initialization and declaration
    int ch; // THIS WAS THE PROBLEM AHAAHHHHH
    int timeToQuit = 0;

    // main loop!!!!!!
    while (!timeToQuit) {
        screenRender();
        ch = getch();

        // cycling through the modes until it's time to quit!!
        if (myeditor.mode == MODE_NORMAL) {
            timeToQuit = modeNormal(ch);
        } else if (myeditor.mode == MODE_INSERT) {
            modeInsert(ch);
        } else if (myeditor.mode == MODE_COMMAND) {
            timeToQuit = modeCommand(ch);
        } else if (myeditor.mode == MODE_SEARCH) {
            modeSearch(ch);
        }
    }
    // free and finish!!
    editorFree();
    endwin();
    return 0;
}
