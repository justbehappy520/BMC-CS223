#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for access()
#include <ncurses.h>

#define INIT_MAX_LINES 100 // starting capacity for the lines array
#define MAX_LINE_LEN 1024 // maximum length of a single line of text
#define MAX_SEARCH_RESULTS 1000

// global state variables and things of that nature
char** lines = NULL; // array of arrays!! dynamic allocation can save memory
int line_count = 0; // actual number of lines loaded
int line_cap = 0; // current maximum capacity of lines array
int top = 0; // topmost visible line
int cursor_file_row = 0; // row of cursor relative to the entire file
int cursor_file_col = 0; // column of cursor relative to the entire file

// ****** FILE LOADING AND MANAGEMENT ******

// function to increase size of lines array as needed, expandable array if you will
void growLinesArray() {
	// if line_cap = 0 -> INIT else -> *2
	int new_cap = line_cap == 0 ? INIT_MAX_LINES : line_cap * 2;
	char** new_lines = realloc(lines, new_cap * sizeof(char*));

	// check if reallocation was successful (should be?)
	if (new_lines == NULL) {
		printf("Error: Failed to reallocate memory for lines array\n");
		exit(EXIT_FAILURE);
	}

	lines = new_lines;
	line_cap = new_cap;
}

// function to save a file (:w) hehe it looks like a face
void saveFile(int max_rows) {
	//
}

// function to read in a file
void loadFile(char *filename) {
	FILE *fptr = fopen(filename, "r");
	// check if the file has been opened
	if (fptr == NULL) {
		printf("Error: Could not open file %s\n", filename);
		exit(1);
	}

	// make a buffer in the memory to hold the file contents
	char buffer[MAX_LINE_LEN];

	// initial allocation of memory
	growLinesArray();

	// read in the file data
	while(fgets(buffer, sizeof(buffer), fptr)) {
		// reallocate memory when needed
		if (line_count >= line_cap) {
			growLinesArray();
		}

		lines[line_count] = strdup(buffer);
		if (lines[line_count] == NULL) {
			printf("Error: Failed to allocate memory\n");
			exit(EXIT_FAILURE);
		}
		line_count++;
	}
	// close!!
	fclose(fptr);
}

// ****** RENDERING AND BANNER ******

// function to print a new line with row number
void printLine(int x, int y, char* fill) {
        int file_line = top + x + 1; // row number relative to the file
        mvprintw(x, y, "%4d %s", file_line, fill);
}

// function to track row and column
void banner() { // A LITTLE WONK, the numbers aren't enitrely accurate in the column side of things
        int rows, cols;
        getmaxyx(stdscr, rows, cols);

        // set up cursor relative to entire file
	int screen_row = cursor_file_row - top;
	int screen_col = cursor_file_col - 5; // account for padding

        if (screen_col < 5) {
                screen_col = 5;
        }

        // print it out in the bottom right, 1-indexed
        mvprintw(rows-1, cols-20, "Ln %d, Col %d", cursor_file_row+1, cursor_file_col+1);

	// set cursor location, it got stuck before
	if (screen_row >= 0 && screen_row < rows-1) {
		move(screen_row, screen_col);
	} else { // in case the cursor gets stuck somewhere
		move(0, 5);
	}
}

// function to render the screen as the cursor moves
void renderScreen() {
        clear();
        int rows, cols;
        getmaxyx(stdscr, rows, cols);

        if (lines == NULL || line_count == 0) {
		banner();
		refresh();
		return;
	}

	// render the screen by printing the next line
	// rows - 1 leaves the last row for the banner and command line
        for (int i = 0; i < rows-1 && top+i < line_count; i++) {
                printLine(i, 0, lines[top+i]);
        }

        banner();
        refresh();
}

// ****** SCROLLING AND COMMANDS ******

void scrollScreen(int ch, int max_rows) {
	if (lines == NULL || line_count == 0) {
		return;
	}

	// intialization of important variables!!
	int len = strlen(lines[cursor_file_row]);

	// scrolling!! hopefully
        if (ch == KEY_UP) {
		if (cursor_file_row > 0) {
			cursor_file_row--;
		} else if (cursor_file_row < top) {
			top--;
		}
        } else if (ch == KEY_DOWN) {
		// move cursor down until right above last line
		if (cursor_file_row < line_count-1) {
			cursor_file_row++;
		} else if (cursor_file_row >= top+max_rows-1) {
			top++;
		}
        } else if (ch == KEY_LEFT) { 
		if (cursor_file_col > 0) {
			cursor_file_col--;
		}
        } else if (ch == KEY_RIGHT) {
		if (cursor_file_col <= len) {
			cursor_file_col++;
		}
        }

	// ensure cursor remains within bounds of file
	if (cursor_file_col > len) {
		cursor_file_col = len;
	}

        renderScreen(); 
	banner();
	refresh();
}

// function to delete character
// currently deletes the char to the left of the cursor
void deleteChar() {
	if (lines == NULL || line_count == 0) {
		return;
	}

	if (cursor_file_col > 0 && cursor_file_row < line_count) {
		char* line = lines[cursor_file_row]; // current working line
		int len = strlen(line);

		// make sure not to delete null termination
		if (cursor_file_row < len) {
			memmove(&line[cursor_file_col-1], &line[cursor_file_col], len-cursor_file_col+1); // shift char left to backspace
			line[len-1] = '\0';
			cursor_file_col--;
			printLine(cursor_file_row-top, 0, line);
		}
	}	

	banner();
	refresh();
}

// function to go to end of file
void goToEnd(int max_rows) {
	if (lines == NULL || line_count == 0) {
		return;
	}

	cursor_file_row = line_count - 1;

	// set file column to end of row
	if (line_count > 0) {
		cursor_file_col = strlen(lines[cursor_file_row]);
	} else {
		cursor_file_col = 0;
	}

	// update top to show bottom line
	if (line_count > max_rows-1) {
		top = line_count - (max_rows - 1);
	} else {
		top = 0;
	}
	
	renderScreen();
	banner();
	refresh();
}

// function to insert a new line, move everything back? TBD
// might not implement in time, search is really kicking my butt

// function to type
// CURRENTLY UNDER MAINTENANCE. WIP.
void type(int max_rows) {
	int r, c;
	getyx(stdscr, r, c);
	int ch;

	// put up the insert sign!!
	mvprintw(max_rows-1, 0, "---INSERT---");
	clrtoeol();
	refresh();

	// keep typing until escape!!
	while ((ch = getch()) != 27) {
		if (lines == NULL || line_count == 0) {
			return;
		}

		int file_line = top + r;
		if (file_line > line_count) { // ensure cursor is within legal file bounds
			goToEnd(max_rows);
			getyx(stdscr, r, c);
			file_line = top + r;
		}
		// working row in lines array
		char* line = lines[file_line];
		int len = strlen(line); // currently working line
		int file_col = c - 6; // padding

		// scrolling
		if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
			scrollScreen(ch, max_rows);
		}

		// hitting enter
		// might not be real

		// backspacing!!
		else if (ch == KEY_BACKSPACE) { // ASCII 127
			if (c > 6) { // backspace inside a row
				deleteChar();
			} else if (c == 6 && r > 0) { // backspace to previous row
				int prev_line = file_line - 1;
				char* line_prev = lines[prev_line]; // previous line
				int len_prev = strlen(line_prev);
				if (len_prev + len < MAX_LINE_LEN) { // to prevent overflow, not sure what to do if overflow
					// concatenate strings and update the cursor
					strcat(line_prev, line); // concat!! :3
					int r_new = r - 1;
					int c_new = len_prev + 6;

					// free up memory and shuffle things down
					free(lines[file_line]);
					for (int i = file_line; i < line_count; i++) {
						lines[i] = lines[i+1];
					}
					line_count--; // update line count!!
					
					// re-draw screen
					clear();
					renderScreen();
					refresh();

					// update cursor location
					move(r_new, c_new);
				}
			}

		// typing!!
		} else if (ch >= 32 && ch <= 126) { // printable characters not including delete
			if (len < MAX_LINE_LEN-1) { // prevent overflow, still not sure what to do if overflow
				memmove(&line[file_col+1], &line[file_col], len-file_col+1); // shift char right
				line[file_col] = ch; // update chars in line array
				printLine(r, 0, line);
				move(r, c+1); // move cursour
			} // can probably keep it like this for now
		}
		getyx(stdscr, r, c); // update for next loop
	}

	// clear insert label!!
	move(max_rows-1, 0);
	clrtoeol();
	refresh();
}

// function to handle various kinds of commands
int editorCommand(int ch, char* command, int *cmd_len) {
	// exit command mode with escape
	if (ch == 27) {
		move(LINES-1, 0);
		clrtoeol();
		return -2; // signal exit command mode
	}

        // pressing enter indicates end of command
        if (ch == '\n') {
                command[*cmd_len] = '\0'; // end of string

                // command inputs
                if (strcmp(command, "q") == 0) { // command to quit
                        return -1; // signal to quit
                } else if (strcmp(command, "w") == 0) { // command to save
			return -3; // signal to save
		}

                int line_num = atoi(command); // gets the line number
                if (line_num > 0 && line_num <= line_count) { // command to jump to line number
                        return line_num; // signal to jump to this line
                }

                // can write in more command inputs

                // reset~
                move(LINES - 1, 0); // move to front of last line!!
                clrtoeol(); // clear to end of line!!
                *cmd_len = 0;

                return 0;
        } else {
                // add ch into command array
			if (*cmd_len < 29) {
                command[(*cmd_len)++] = ch;
                addch(ch);
			}
        }

        return 0;
}

// ****** SEARCH ****** 

// CURRENTLY BROKEN AND CAUSES THINGS TO DIE

// function to implement a search
// seg faults LMAO
void searchDoc(char* searchPhrase, int* searchBase, int *count) {
	*count = 0; // reset the count
	for (int i = 0; i < line_count; i++) {
		char* line = lines[i]; // separate out each line
		char* found = line; // the string to compare the substring against
		// if lines the array of content contains the phrase from search
		while ((found = strstr(found, searchPhrase)) != NULL) {
			// add start index into searchBase
			searchBase[*count] = found - line; // should theoretically store the column
			searchBase[(*count)+1] = i; // should theoretically store the row
			*count += 2; // increment by 2
			found++; // move forward in the line to keep looking
		}
	}
}

// function to implement search bar
void searchBar(int ch, char* search, int *search_len) {
	// highlight nearest instance of the phrase in the search bar, updating as the phrase changes

	// array to store the start index of each phrase
	static int *searchBase = NULL; // 2D array, array of arrays of ints
	static int *count = 0; // total count of search results
	
	// pressing enter indicates end of search phrase
        if (ch == '\n') {
                // since enter is pressed, phrase is entered; mark end of string
                search[*search_len] = '\0';
		if (searchBase) {
			free(searchBase); // free up to reset
		}

		searchBase = malloc(sizeof(int) * 2 * MAX_SEARCH_RESULTS);
		searchDoc(search, searchBase, count);
                
                move(LINES - 1, 0); // move to last line!!
                clrtoeol(); // clear to end of line
		printw("Found %d results", *count); // print out the results in the last line?
        } else {
                // add ch into search array
                search[(*search_len)++] = ch;
                addch(ch);
        }

	free(searchBase);
}

// ****** MAIN METHOD ******

// main method
int main(int argc, char** argv)
{
	// intake command line and load file
	char* filename;
	if (argc < 2) { // verify the filename is present
		printf("usage: %s <filename>\n", argv[0]);
		return 0;
	} 
	filename = argv[1]; // save the filename!!
	if (access(filename, F_OK) != 0) { // check it's a valid file!!
		printf("%s not found\n", filename);
	}
	loadFile(filename); // load file!!

	// start cursed mode hehe
	initscr();
	keypad(stdscr, TRUE); // enable keyboard arrows
	raw(); // disable line buffering
	noecho(); // don't show typed input automatically

	// get stdscr max
	int max_rows, max_cols;
	getmaxyx(stdscr, max_rows, max_cols);
	
	// display file contents on screen
	renderScreen();
	move(0, 5); // move cursor to account for line number

	// declaration and initialization
	int ch;
	int command_mode = 0; // triggered by ':'
	char command[30]; // array inputted into command line
	int cmd_len = 0; // length of current command
	
	int search_mode = 0; // triggered by '/'
	char search[30]; // array inputted into search
	int search_len = 0; // length of current search

	int timeToQuit = 0; // quit button

	// input!
	while (!timeToQuit) {
		ch = getch(); // wait for user input
		
		if (command_mode) {
			int cmd_input = editorCommand(ch, command, &cmd_len);
			
			if (cmd_input == -1) {
				timeToQuit = 1; // quit!! with :q
			} else if (cmd_input == -2) {
				command_mode = 0; // no more comms!!
			} else if (cmd_input == -2) {
			        saveFile(max_rows); // save!! with :w
				command_mode = 0;	
			} else if (cmd_input > 0 && cmd_input < line_count) {
				top = cmd_input - 1; // line number is 1-based
				renderScreen();
				move(0, 6);
				command_mode = 0;
			}
		} else if (search_mode) {
			searchBar(ch, search, &search_len);
		} else {
			if (ch == 'i') {
                                // type(max_rows);
				// renderScreen();	
                        } else if (ch == 'x') {
                                deleteChar(); // it be wonk
                        } else if (ch == 'G') {
                                goToEnd(max_rows);
                        } else if (ch == ':') {
                                command_mode = 1;
                                cmd_len = 0;
                                move(max_rows-1, 0);
                                clrtoeol();
                                addch(':');
                        } else if (ch == '/') { // currently out of commission
                                // search_mode = 1;
                                search_len = 0;
                                move(max_rows-1, 0);
                                clrtoeol();
                                addch('/');
                        }
			// scrolling!!
			else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT) {
				scrollScreen(ch, max_rows);
			}
		}
	}
	// end cursed mode
	endwin();

	// freeeeeeeeee
	if (lines != NULL) {
		for (int i = 0; i < line_count; i++) {
			free(lines[i]);
		}
		free(lines);
	}

	// finish!!
	return 0;
}

