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
#include <time.h>

#define XMAX 20
#define YMAX 40

int direction;
//int snake_x_pos = 10;
//int snake_y_pos = 20;
int snake_length = 1;
int my_mutex = 0;
int counter = 0;
int fruit_x_pos=2, fruit_y_pos=2;

struct Point{
	int X;
	int Y;
};

struct Point snake = {20,10};

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


void printSnakeInfo(void){
	printf("\033[%d;%dH       ",21,42);	// Clear previous X:
	printf("\033[%d;%dH",21,42);		// Set cursor on the current position
	printf("X:%d",snake.X);				// Print X:
	printf("\033[%d;%dH       ",22,42);	// Clear previous Y:
	printf("\033[%d;%dH",22,42);		
	printf("Y:%d",snake.Y);
	printf("\033[%d;%dH           ",23,42);	// Clear previous length:
	printf("\033[%d;%dH",23,42);		
	printf("Length:%d",snake_length);
}


// void printFruitXY(void){
// 	printf("\033[%d;%dH       ",15,42);	// Clear previous X:
// 	printf("\033[%d;%dH",15,42);		// Set cursor on the current position
// 	printf("FrX:%d",fruit_x_pos);				// Print X:
// 	printf("\033[%d;%dH       ",16,42);	// Clear previous Y:
// 	printf("\033[%d;%dH",16,42);		
// 	printf("FrY:%d",fruit_y_pos);
// }

void generateFruit(void){
	fruit_x_pos = 2+(rand()%(XMAX-1));
	fruit_y_pos = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit_x_pos,fruit_y_pos);		// Paint new fruit
}


void* thread_func(void* arg){
	while(1){
		my_mutex = 0;
		printf("\033[%d;%dH ",snake.X,snake.Y);	// Clear current position symbol
		switch(direction){
			case 3:	snake.Y++;
					break;
			case 6:	snake.X++;
					break;
			case 9:	snake.Y--;
					break;
			case 0:	snake.X--;
					break;
		}
		if ((snake.X==fruit_x_pos)&&(snake.Y==fruit_y_pos)){
			snake_length++;
			generateFruit();
		}
		printSnakeInfo();

		printf("\033[%d;%dH0",snake.X,snake.Y);	// Print new "HEAD" of snake


		// counter++;
		// if (counter == 5){
		// 	counter = 0;
		// 	printf("\033[%d;%dH ",fruit_x_pos,fruit_y_pos);		// Erase old fruit
		// 	fruit_x_pos = 2+(rand()%(XMAX-1));
		// 	fruit_y_pos = 2+(rand()%(YMAX-1));
		// 	printf("\033[%d;%dHF",fruit_x_pos,fruit_y_pos);		// Paint new fruit
		// 	//printFruitXY();
		// }

		fflush(stdout);						// force clear console buffer
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
	srand(time(0));
	// allocate memory to cond (conditional variable),  
    // thread id's and array of size threads 
    cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 
    tid = (pthread_t*)malloc(sizeof(pthread_t)); 

	printf ("\e[?25l");	//set cursor INVISIBLE
	clrscr();
	print_grid(XMAX,YMAX);
	printf ("\e[?25h");	//set cursor VISIBLE
	fruit_x_pos = 2+(rand()%(XMAX-1));
	fruit_y_pos = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit_x_pos,fruit_y_pos);		// Paint new fruit
	printf("\033[%d;%dH ",snake.X,snake.Y);
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

