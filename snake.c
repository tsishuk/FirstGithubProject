#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int mygetch( ) {
	struct termios oldt,
	newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}


/*************************************************************************
*
* Purpose: Clear the screen with VT100 escape codes. This can be done
* with conio.h on PCs - non standard code. Or curses.h, bit of
* a fag...
* Author: M.J. Leslie
* Date: 22-Jun-94
*
******************************************************************/

void clrscr(void);
void print_grid(int X_MAX, int Y_MAX);

int main()
{
	//dummy comment for test
	// Dummy comment second branch
	int ch;
	int xpos = 10;
	int ypos = 10;

	printf ("\e[?25l");	//set cursor INVISIBLE
	clrscr();
	print_grid(15,30);
	printf("First()\n");
	sleep(1);
	printf("\033[15;20HHello");
	printf("\033[%d;%dHHello",xpos,ypos);
	xpos = 25;
	ypos = 15;
	printf("\033[%d;%dHGoodbye",xpos,ypos);
	fflush(NULL);
	printf("The end");
	printf ("\e[?25h");	//set cursor VISIBLE

	
	while(1){
		ch = mygetch();
		if (ch=='z')
			break;
		else if (ch == 'w'){
			printf("\033[%d;%dH ",xpos,ypos);
			xpos--;
			printf("\033[%d;%dH0",xpos,ypos);
		}
		else if (ch == 's'){
			printf("\033[%d;%dH ",xpos,ypos);
			xpos++;
			printf("\033[%d;%dH0",xpos,ypos);
		}
		else if (ch == 'a'){
			printf("\033[%d;%dH ",xpos,ypos);
			ypos--;
			printf("\033[%d;%dH0",xpos,ypos);
		}
		else if (ch == 'd'){
			printf("\033[%d;%dH ",xpos,ypos);
			ypos++;
			printf("\033[%d;%dH0",xpos,ypos);
		}
	}
	

	return 0;
}


void clrscr(void)
{
	printf("\033[2J"); 	/* Clear the entire screen. */
	printf("\033[0;0f");/* Move cursor to the top left hand corner*/
}


void print_grid(int X_MAX, int Y_MAX){
	int x,y;
	for (x=0;x<=X_MAX;x++){
		for (y=0;y<=Y_MAX;y++){
			if (x==0 || x==X_MAX){
					printf("-");
			}
			else {
				if (y==0 || y==Y_MAX)
					printf("|");
				else
					printf(" ");	 
			}
		}
		printf("\n");
	}
}

