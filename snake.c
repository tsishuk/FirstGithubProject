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
		printf("Jaja\n");
		usleep(1000000);
	}
}


int main()
{
	pthread_t* tid;
	pthread_cond_t* cond;
	int ch;
	int xpos = 10;
	int ypos = 20;
	// allocate memory to cond (conditional variable),  
    // thread id's and array of size threads 
    cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 
    tid = (pthread_t*)malloc(sizeof(pthread_t)); 

	printf ("\e[?25l");	//set cursor INVISIBLE
	clrscr();
	print_grid(XMAX,YMAX);
	//printf("\033[15;20HHello");	//Text print example
	printf ("\e[?25h");	//set cursor VISIBLE
	printf("\033[%d;%dH ",xpos,ypos);
	sleep(1);
	pthread_create(tid, NULL, thread_func, NULL);
	
	while(1){
		ch = mygetch();
		if (ch=='p')
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

