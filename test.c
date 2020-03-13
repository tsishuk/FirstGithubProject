#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define XMAX 10
#define YMAX 20
#define MAX_COORD_SIZE (XMAX*YMAX)

int direction;
int true_direction;
int snake_length = 1, tail, head, head_old;
int my_mutex = 0;
int CHECK = 1;

struct Point{
	int X;
	int Y;
};


struct Element{
	int X;
	int Y;
	struct Element* next;
    struct Element* previous;
};


struct Point fruit = {2,2};
struct Point snake[MAX_COORD_SIZE];
struct Element elements[(XMAX-1)*(YMAX-1)];


// future fruit generator without generation on the body of snake
void elementsPrint(void)
{
	int i,j;
	static int i_counter;
	int indeks_X;
	int indeks_Y;
	int indeks;
	int begin = 0;
	int random_indeks;
	int counter = 0;
	struct Element* first_element = &elements[0];
	const int max = (XMAX-1)*(YMAX-1) - snake_length;

	i_counter = 0;
    
    // fill elements with init values
    for (i=2;i<=XMAX;i++)
        for (j=2;j<=YMAX;j++){
            elements[counter].X = i;
            elements[counter].Y = j;
            elements[counter].next = &elements[counter+1];
            elements[counter].previous = &elements[counter-1];
            counter++;
        }

    // deleting snake coords from elements
	for (i=tail;i_counter<snake_length;i++){
		i_counter++;
		if (i>=(MAX_COORD_SIZE-1))
			i=0;
		indeks_X = snake[i].X-2;
		indeks_Y = snake[i].Y-2;
		indeks = indeks_X*(XMAX-1) + indeks_Y;
		
        if (indeks == begin){
			begin++;
            first_element = (*first_element).next;
        }
		else {
            (*elements[indeks].previous).next = elements[indeks].next;
            (*elements[indeks].next).previous = elements[indeks].previous;
		}
	}

	// random fruit generation
	//random_indeks = begin + (rand()%(max-1));
	random_indeks = rand()%(max-1);

	//for (i=begin;i<(random_indeks);i++){
	for (i=0;i<(random_indeks);i++){
		first_element = (*first_element).next;
	}

	fruit.X = (*first_element).X;
	fruit.Y = (*first_element).Y;
	//fruit.X = 2+(rand()%(XMAX-1));
	//fruit.Y = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit.X,fruit.Y);		// Paint new fruit
}



void PrintElements(void){
	static int i_counter;
	int i, j;
	int indeks_X;
	int indeks_Y;
	int indeks;
	int begin = 0;
	int random_indeks;
	int counter = 0;
	const int max = (XMAX-1)*(YMAX-1) - snake_length;
	struct Element* first_element = &elements[0];

	counter = 0;
	// fill elements with init values
    for (i=2;i<=XMAX;i++)
        for (j=2;j<=YMAX;j++){
            elements[counter].X = i;
            elements[counter].Y = j;
            elements[counter].next = &elements[counter+1];
            elements[counter].previous = &elements[counter-1];
            counter++;
        }

    counter = 0;
    i_counter = 0;

    for (i=2;i<=XMAX;i++){
        for (j=2;j<=YMAX;j++){ 
        	printf("%2d,%2d   ",elements[counter].X, elements[counter].Y);
        	counter++;
        }
        printf("\n");
    }

    counter = 0;

    // deleting snake coords from elements
	for (i=tail;i_counter<snake_length;i++){
		i_counter++;
		if (i>=(MAX_COORD_SIZE-1))
			i=0;
		indeks_X = snake[i].X-2;
		indeks_Y = snake[i].Y-2;
		indeks = indeks_X*(YMAX-1) + indeks_Y;
		printf("indeks = %d\n",indeks);
		
        if (indeks == begin){
			begin++;
            first_element = (*first_element).next;
        }
		else {
            (*elements[indeks].previous).next = elements[indeks].next;
            (*elements[indeks].next).previous = elements[indeks].previous;
		}
	}


	for (i=0;i<max;i++){
		printf("%2d,%2d   ",(*first_element).X, (*first_element).Y);
		first_element = (*first_element).next;
	}
}



int main()
{
	printf("Hello here\n");
	snake_length = 5;
	snake[0].X = 2;
	snake[0].Y = 2;
	snake[1].X = 3;
	snake[1].Y = 2;
	snake[2].X = 4;
	snake[2].Y = 2;
	snake[3].X = 4;
	snake[3].Y = 3;
	snake[4].X = 4;
	snake[4].Y = 4;

	PrintElements();
	return 0;
}