/*************************************************************************
*
* Purpose: Clear the screen with VT100 escape codes. This can be done
* with conio.h on PCs - non standard code. Or curses.h, bit of
* a fag...
* Author: M.J. Leslie
* Date: 22-Jun-94
*
******************************************************************/
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define XMAX 20
#define YMAX 40

int direction;
int xpos = 10;
int ypos = 20;
int my_mutex = 0;

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

void clrscr(void);
void print_grid(int X_MAX, int Y_MAX);


void* thread_func(void* arg){
	while(1){
		my_mutex = 0;
		printf("\033[%d;%dH ",xpos,ypos);	// Clear current position symbol
		switch(direction){
			case 3:	ypos++;
					break;
			case 6:	xpos++;
					break;
			case 9:	ypos--;
					break;
			case 0:	xpos--;
					break;
		}
		printf("\033[%d;%dH       ",21,42);	// Clear previous X:
		printf("\033[%d;%dH",21,42);		// Set cursor on the current position
		printf("X:%d",xpos);
		printf("\033[%d;%dH       ",22,42);	// Clear previous Y:
		printf("\033[%d;%dH",22,42);		
		printf("Y:%d",ypos);
		printf("\033[%d;%dH0",xpos,ypos);	// Print new "HEAD" of snake
		fflush(stdout);		// force clear console buffer
		my_mutex = 1;
		usleep(500000);
	}
}


int main()
{
	pthread_t* tid;
	pthread_cond_t* cond;
	direction = 3;
	int ch;

	// allocate memory to cond (conditional variable),  
    // thread id's and array of size threads 
    cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 
    tid = (pthread_t*)malloc(sizeof(pthread_t)); 

	printf ("\e[?25l");	//set cursor INVISIBLE
	clrscr();
	print_grid(XMAX,YMAX);
	printf ("\e[?25h");	//set cursor VISIBLE
	printf("\033[%d;%dH ",xpos,ypos);
	sleep(1);
	pthread_create(tid, NULL, thread_func, NULL);
	
	while(1){
		ch = mygetch();
		if (ch=='p')
			break;
		if (my_mutex){
		switch (ch){
			case 'w': direction = 0;
					  break;
			case 'd': direction = 3;
					  break;
			case 's': direction = 6;
					  break;
			case 'a': direction = 9;
					  break;					  					  
		}
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

