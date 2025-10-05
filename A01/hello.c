/******************************************************************
 * hello.c
 * Author: gloz 
 * Print Hello World
 * Dispense simple comment:
 * 	"The skies smile when you step outdoors."
 * 	"Stars shine when you laugh."
 * 	"Flowers bloom when you smile." 	
 * 	"Birds sing when you walk by."
 * 	"Programs run first try under your hands."
 * 	"Trees sway when you dance."
 */

#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hellow World!\n");
    
    // user input
    int yourNum;
    printf("Please type a number between 0 and 5 and press enter: ");
    scanf("%d", &yourNum);
    printf("Your number is: %d\n", yourNum);
    
    // ensure number validity
    if(yourNum < 0) {
        printf("ERROR: Invalid Number - number is less than 0\n");
        return 0;
    }

    if(yourNum > 5) {
	printf("ERROR: Invalid Number - number is greater than 5\n");
        return 0;
    }

    // string array of 10 strings, max len of each string is 100
    char arr[6][50] = {"The skies smile when you step outdoors.",
	"Stars shine when you laugh.",
   	"Flowers bloom when you smile.",
	"Birds sing when you walk by.",
	"Programs run first try under your hands.",
   	"Trees sway when you dance."};

    printf("%s\n", arr[yourNum]);
    return 0;
}

