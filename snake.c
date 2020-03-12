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
#define MAX_COORD_SIZE (XMAX*YMAX)

int direction;
int snake_length = 1, tail, head, head_old;
int my_mutex = 0;
int counter = 0;
int CHECK = 1;


struct Point{
	int X;
	int Y;
};

//struct Point snake = {20,10};
struct Point fruit = {2,2};
struct Point snake[MAX_COORD_SIZE];



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
	printf("X:%d",snake[head].X);		// Print X:
	printf("\033[%d;%dH       ",22,42);	// Clear previous Y:
	printf("\033[%d;%dH",22,42);		
	printf("Y:%d",snake[head].Y);
	printf("\033[%d;%dH           ",23,42);	// Clear previous length:
	printf("\033[%d;%dH",23,42);		
	printf("Length:%d",snake_length);
}


// void printFruitXY(void){
// 	printf("\033[%d;%dH       ",15,42);	// Clear previous X:
// 	printf("\033[%d;%dH",15,42);		// Set cursor on the current position
// 	printf("FrX:%d",fruit.X);				// Print X:
// 	printf("\033[%d;%dH       ",16,42);	// Clear previous Y:
// 	printf("\033[%d;%dH",16,42);		
// 	printf("FrY:%d",fruit.Y);
// }

void generateFruit(void){
	fruit.X = 2+(rand()%(XMAX-1));
	fruit.Y = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit.X,fruit.Y);		// Paint new fruit
}


void* inputThreadFunc(void* arg){
	int ch;

	while(CHECK){
		ch = mygetch();

		if (ch=='p'){
			CHECK=0;
			break;
		}
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
		usleep(10000);	
	}
}


void* thread_func(void* arg){
	int x,y;
	int i,j;

	while(CHECK){
		my_mutex = 0;
		head_old = head;
		x = snake[head].X;
		y = snake[head].Y;
		head++;	
		if (head >= (MAX_COORD_SIZE-1))
			head = 0;
		snake[head] = snake[head_old];
		switch(direction){
			case 3:	(y==YMAX) ? (snake[head].Y = 2) : (snake[head].Y++);
					break;
			case 6:	(x==XMAX) ? (snake[head].X = 2) : (snake[head].X++);
					break;
			case 9:	(y==2) ? (snake[head].Y = YMAX) : (snake[head].Y--);
					break;
			case 0:	(x==2) ? (snake[head].X = XMAX) : (snake[head].X--);
					break;
		}

		// // Check for self eating
		for (i=tail;i!=head;i++)
			for (j=i+1;j!=head;j++){
					if (i==j)
						continue;
					if (i>MAX_COORD_SIZE)
						i=0;
					if (j>MAX_COORD_SIZE) 
						j=0;
					if ((snake[i].X==snake[j].X)&&(snake[i].Y==snake[j].Y)){
						if (CHECK){
							CHECK = 0;
							printf("\bX");
							printf("\033[%d;%dH ",22,10);
							printf("GAME OVER\n");
							fflush(stdout);										// force clear console buffer
							break;
						}
					}
			}

		if (CHECK == 1){
			// If HEAD == FRUIT (eat fruit)
			if ((snake[head].X==fruit.X)&&(snake[head].Y==fruit.Y)){
				snake_length++;
				generateFruit();
			}
			// Erase previous snake tail
			else {
				printf("\033[%d;%dH ",snake[tail].X,snake[tail].Y);	// Clear previous tail if no fruit eat
				tail++;
				if (tail >= (MAX_COORD_SIZE-1))
					tail = 0;
			}

			// Print new HEAD of snake
			printf("\033[%d;%dH0",snake[head].X,snake[head].Y);	// Print new "HEAD" of snake

			fflush(stdout);										// force clear console buffer
			my_mutex = 1;
			usleep(100000);
		}
	}
}


int main()
{
	struct termios oldt,newt;
	pthread_t* tid;
	pthread_t* tid2;
	pthread_cond_t* cond;
	direction = 3;
	srand(time(0));

	snake[0].X = 5;
	snake[0].Y = 5;

	// allocate memory to cond (conditional variable),  
    // thread id's and array of size threads 
    cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 
    tid  = (pthread_t*)malloc(sizeof(pthread_t)); 
    tid2 = (pthread_t*)malloc(sizeof(pthread_t)); 

	printf ("\e[?25l");	//set cursor INVISIBLE
	clrscr();
	print_grid(XMAX,YMAX);
	printf ("\e[?25h");	//set cursor VISIBLE
	fruit.X = 2+(rand()%(XMAX-1));
	fruit.Y = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit.X,fruit.Y);		// Paint new fruit
	printf("\033[%d;%dH ",snake[0].X,snake[0].Y);
	//sleep(1);

	pthread_create(tid, NULL, thread_func, NULL);

	pthread_create(tid2, NULL, inputThreadFunc, NULL);
	
	while(1){

		if (CHECK == 0){
			pthread_cancel(*tid);
			pthread_cancel(*tid2);
			printf("\033[%d;%dH ",23,10);
			printf("END OF GAME\n");
			break;
		}
	}

	// Force make console symbols visible after thread kill with getchar()
	tcgetattr( STDIN_FILENO, &oldt );
	oldt.c_lflag |= ( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

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

