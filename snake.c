#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
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


void generateFruit(void){
	fruit.X = 2+(rand()%(XMAX-1));
	fruit.Y = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit.X,fruit.Y);		// Paint new fruit
}


void printInfo(void)
{
	printf("\033[%d;%dH      ",4,YMAX+8);
	printf("\033[%d;%dH %d",4,YMAX+8, tail);
	printf("\033[%d;%dH      ",5,YMAX+8);
	printf("\033[%d;%dH %d",5,YMAX+8, head);
	printf("\033[%d;%dH      ",6,YMAX+8);
	printf("\033[%d;%dH %d",6,YMAX+8, snake_length);
	printf("\033[%d;%dH      ",9,YMAX+10);
	printf("\033[%d;%dH %d",9,YMAX+10, snake[head].X);
	printf("\033[%d;%dH      ",10,YMAX+10);
	printf("\033[%d;%dH %d",10,YMAX+10, snake[head].Y);
}


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
				case 'w': (true_direction==6) ? (direction = 6) : (direction = 0);
						  break;
				case 'd': (true_direction==9) ? (direction = 9) : (direction = 3);
						  break;
				case 's': (true_direction==0) ? (direction = 0): (direction = 6);
						  break;
				case 'a': (true_direction==3) ? (direction = 3) : (direction = 9);
						  break;					  					  
			}
		}
		usleep(10000);	
	}
}


void* thread_func(void* arg){
	int x,y;
	int i,j;
	int i_counter, j_counter;

	while(CHECK){
		true_direction = direction;
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

		if (CHECK == 1){
			// If HEAD == FRUIT (eat fruit)
			if ((snake[head].X==fruit.X)&&(snake[head].Y==fruit.Y)){
				snake_length++;
				//generateFruit();
				elementsPrint();
				printf("\033[%d;%dH    ",XMAX+2,15);
				printf("\033[%d;%dH %d",XMAX+2,14,snake_length*10);
			}
			// Erase previous snake tail
			else {
				printf("\033[%d;%dH ",snake[tail].X,snake[tail].Y);	// Clear previous tail if no fruit eat
				tail++;
				if (tail >= (MAX_COORD_SIZE-1))
					tail = 0;
			}

			printInfo();

			if (snake_length>1)
				printf("\033[%d;%dHo",snake[head_old].X,snake[head_old].Y);	// Print new "HEAD" of snake
			// Print new HEAD of snake
			printf("\033[%d;%dHO",snake[head].X,snake[head].Y);	// Print new "HEAD" of snake

			if (snake_length>4){	// because <4 cant eat itself
				i_counter = 0;
				for (i=tail; i_counter<snake_length-1; i++){
					i_counter++;
					if (i>=MAX_COORD_SIZE)
						i=0;
					if ((snake[i].X==snake[head].X)&&(snake[i].Y==snake[head].Y)){
						if (CHECK){
							CHECK = 0;
							printf("\bX");
							printf("\033[%d;%dH %d",8,YMAX+5, i);
							printf("\033[%d;%dH ",XMAX+2,25);
							printf("GAME OVER\n");
							fflush(stdout);			// force clear console buffer
							break;
						}
					}
				}
			}

			fflush(stdout);	// force clear console buffer
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
	fruit.X = 2+(rand()%(XMAX-1));
	fruit.Y = 2+(rand()%(YMAX-1));
	printf("\033[%d;%dHF",fruit.X,fruit.Y);		// Paint new fruit
	printf("\033[%d;%dH ",snake[0].X,snake[0].Y);
	printf("\033[%d;%dH Total Score: 10", XMAX+2, 0);
	printf("\033[%d;%dH Press (p) key for exit", 2, YMAX+2);
	printf("\033[%d;%dH w,a,s,d - control", 3, YMAX+2);
	printf("\033[%d;%dH Tail:", 4, YMAX+2);
	printf("\033[%d;%dH Head:", 5, YMAX+2);
	printf("\033[%d;%dH Leng:", 6, YMAX+2);
	printf("\033[%d;%dH i:", 8, YMAX+2);
	printf("\033[%d;%dH HEAD.X:", 9, YMAX+2);
	printf("\033[%d;%dH HEAD.Y:", 10, YMAX+2);

	pthread_create(tid, NULL, thread_func, NULL);
	pthread_create(tid2, NULL, inputThreadFunc, NULL);
	
	while(1){

		if (CHECK == 0){
			pthread_cancel(*tid);
			pthread_cancel(*tid2);
			printf("\033[%d;%dH ",XMAX+3,25);
			printf("END OF GAME\n");
			break;
		}
	}

	// Force make console symbols visible after thread kill with getchar()
	tcgetattr( STDIN_FILENO, &oldt );
	oldt.c_lflag |= ( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );

	printf ("\e[?25h");		//set cursor VISIBLE

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

