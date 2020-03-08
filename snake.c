#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define XMAX 5
#define YMAX 5

int direction;
int snake_length;
int my_mutex = 0;
int counter = 0;
int fruit_x_pos=2, fruit_y_pos=2;

struct Point{
	int X;
	int Y;
};

struct Element{
	int X;
	int Y;
	struct Element* next;
};

struct Point snake[XMAX*YMAX];
struct Element elements[XMAX*YMAX];


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


void elementsInit(void)
{
	int counter=0;
	int i,j;
	for (i=2;i<(XMAX+2);i++)
		for (j=2;j<(YMAX+2);j++){
			elements[counter].X = i;
			elements[counter].Y = j;
			elements[counter].next = &elements[counter+1];
			counter++;
		}
}


void elementsPrint(void)
{
	int i;
	int indeks_X;
	int indeks_Y;
	int indeks;
	int begin = 0;
	struct Element* next_element = &elements[0];
	const int max = XMAX*YMAX - snake_length;

	for (i=0;i<snake_length;i++){
		indeks_X = snake[i].X-2;
		indeks_Y = snake[i].Y-2;
		indeks = indeks_X*XMAX + indeks_Y;
		printf("deleted indeks = %d, X:%d, Y:%d\n",indeks, elements[indeks].X, elements[indeks].Y);
		
		if (indeks == begin)
			begin++;
		else {
			elements[indeks-1].next = &elements[indeks+1];
		}
		
	}

	for (i=begin;i<(max+begin);i++){
		printf("X:%d, Y:%d indeks = %d\n", (*next_element).X, (*next_element).Y, i);
		next_element = (*next_element).next;
	}
}


void clrscr(void);
void print_grid(int X_MAX, int Y_MAX);


void printSnakeInfo(void){
	printf("\033[%d;%dH       ",21,42);	// Clear previous X:
	printf("\033[%d;%dH",21,42);		// Set cursor on the current position
	printf("X:%d",snake[0].X);				// Print X:
	printf("\033[%d;%dH       ",22,42);	// Clear previous Y:
	printf("\033[%d;%dH",22,42);		
	printf("Y:%d",snake[0].Y);
	printf("\033[%d;%dH           ",23,42);	// Clear previous length:
	printf("\033[%d;%dH",23,42);		
	printf("Length:%d",snake_length);
}



void generateFruit(void){
	fruit_x_pos = 2+(rand()%(XMAX-1));
	fruit_y_pos = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit_x_pos,fruit_y_pos);		// Paint new fruit
}



int main()
{
	printf("Hello world\n");
	snake[0].X = 3;
	snake[0].Y = 2;

	snake[1].X = 4;
	snake[1].Y = 2;

	snake[2].X = 4;
	snake[2].Y = 3;
	snake_length = 3;
	elementsInit();
	elementsPrint();

	return 0;
}
