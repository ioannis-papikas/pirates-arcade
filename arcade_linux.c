/*
 * arcade.c
 * -------
 * In the game there are pirates, wrecks and islands.
 * If the savegame file exists, it reads the first character.
 * If it's "!" then it creates new game, otherwise, it loads the savegame file.
 * When the player wins or looses, a function writes the character "!" to file to indicate the result.
 * Lives,wins and looses can by used for player but you have to uncomment some lines.
 */

/*	Library includes	*/
#include <stdio.h>
#include "genlib.h"
#include "simpio.h"
#include "random.h"

/*	Constants			*/
#define size 10
#define N1 9
#define csea ' '
#define player 'o'
#define wreck 'X'
#define island 'M'
#define pirate '+'

typedef struct {
	int npirates,nwrecks,nislands,lives,wins,looses;
} settings;


/*	Functions			*/
void init_game(char sea[size][size],settings *sett);
void clear_sea(char sea[size][size]);
void print_();
void print_directions(int *level);
void display_board(char sea[size][size]);
void play_round(char sea[size][size],settings *sett,int dir[10][2],bool *playing);
void move_pirates(char sea[size][size],bool *playing,settings *sett);
void move_player(char sea[size][size],int mv,bool *playing,int dir[10][2],settings *sett);
void init_settings(int *level,settings *sett,bool *playing);
void crashed(char sea[size][size],char *txt,bool *playing,settings *sett,bool flag);
void check_pirates(char sea[size][size],bool *playing,settings *sett);
void save_game(char sea[size][size],settings *sett,bool *playing);
void load_game(char sea[size][size],settings *sett);
void canons(char sea[size][size]);
int get_number(int low,int high);

main()
{
	char sea[size][size];
	settings sett;
	int level;
	bool playing = TRUE;
	int direction[10][2] = {{0,0},{1,-1},{1,0},{1,1},{0,-1},{0,0},{0,1},{-1,-1},{-1,0},{-1,1}};
	char *temp;
	FILE *test;
	bool flag = TRUE;

	test = fopen("savegame.txt","r");
	if (test != NULL) {
		temp = fgetc(test);
		fclose(test);
		if (temp != '!') {
			flag = FALSE;
			load_game(sea,&sett);
		}
	} else {
		flag = TRUE;
	}
	if (flag) {
		print_directions(&level);
		init_settings(&level,&sett,&playing);
		init_game(sea,&sett);
	}
	while (playing) {
		system("clear");
		display_board(sea);
		play_round(sea,&sett,direction,&playing);
	};
}

void init_settings(int *level,settings *sett,bool *playing) {
	if (*level == 0) {
		*playing = FALSE;
		printf("Quiting game...\n");
	} else if (*level>9) {
		*level = 9;
	} else if (*level<0) {
		*level = 1;
	}
	sett->npirates = *level/3;
	if (sett->npirates<1) {
		sett->npirates = 1;
	}
	sett->nwrecks = *level;
	if (sett->nwrecks<5) {
		sett->nwrecks = 5;
	}
	sett->nislands = sett->nwrecks- 3;
	sett->lives = 3;
	sett->looses = 0;
	sett->wins = 0;
}

void display_board(char sea[size][size]) {
	int i,j;
	print_();
	for (i=0; i<size; i++) {
		printf("|");
		for (j=0; j<size; j++) {
			printf("%c",sea[i][j]);
			printf("|");
		}
		printf("\n");
		print_();
	}
}

void print_() {
	int i;
	for (i=0; i<2*size+1; i++) {
		printf("-");
	}
	printf("\n");
}

void init_game(char sea[size][size],settings *sett) {
	int i,x1,y1;

	clear_sea(sea);
	for (i=0; i<sett->nwrecks; i++) {
		do {
			x1 = get_number(0,9);
			y1 = get_number(0,9);
		} while ((sea[x1][y1] == wreck) || (sea[x1][y1] == player) || (sea[x1][y1] == pirate) || (sea[x1][y1] == island));
		sea[x1][y1] = wreck;
	}
	for (i=0; i<sett->npirates; i++) {
		do {
			x1 = get_number(0,9);
			y1 = get_number(0,9);
		} while ((sea[x1][y1] == wreck) || (sea[x1][y1] == player) || (sea[x1][y1] == pirate) || (sea[x1][y1] == island));
		sea[x1][y1] = pirate;
	}
	for (i=0; i<sett->nislands; i++) {
		do {
			x1 = get_number(0,9);
			y1 = get_number(0,9);
		} while ((sea[x1][y1] == wreck) || (sea[x1][y1] == player) || (sea[x1][y1] == pirate) || (sea[x1][y1] == island));
		sea[x1][y1] = island;
	}
	do {
		x1 = get_number(0,9);
		y1 = get_number(0,9);
	} while ((sea[x1][y1] == wreck) || (sea[x1][y1] == pirate) || (sea[x1][y1] == island));
	sea[x1][y1] = player;
}
int get_number(int low,int high) {
	int k;
	double d;

	d = (double) rand()/((double)RAND_MAX+1);
	k = (int)(d*(high-low+1));
	return low+k;
}

void clear_sea(char sea[size][size]) {
	int i,j;
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			sea[i][j] = csea;
		}
	}
}

void print_directions(int *level) {
	FILE *infile;
	string moves[] = {"to save the game and quit","for SW","for S","for SE","for W","to stay still and shoot","for E","for NW","for N","for NE"};
	int l = 80;
	int i;
	string onoma;
	
	infile = fopen("player_name.txt","r");
	if (infile == NULL) {
		infile = fopen("player_name.txt","w");
		printf("What's your name? ");
		scanf("%s",onoma);
		fprintf(infile,"%s",onoma);
		fclose(infile);
	} else {
		fscanf(infile,"%s",onoma);
		fclose(infile);
	}
	for (i=0; i<l; i++) {
		printf("=");
	}
	if (onoma == NULL) {
		onoma = "Unknown guest";
	}
	printf("\n\t\t\t\tWELCOME %s\n",onoma);
	for (i=0; i<l; i++) {
		printf("=");
	}
	printf("\n");

	/*			Directions			*/
	printf("You are traveling on the coast of Somalia when you suddenly see a number of pirate ships.\n");
	printf("Fortunately for you, the pirates are not all too smart and follow you blindly.\n");
	printf("Try to make them collide with each other or crash into existing wrecks.\n\n");
	printf("YOU : %c\n",player);
	printf("PIRATE : %c\n",pirate);
	printf("WRECK : %c\n",wreck);
	printf("ISLAND : %c\n",island);
	printf("\n\n");
	printf("Use the numeric keypad to move:\n\n");
	for (i=0; i<10; i++) {
		printf("\t%d %s\n",i,moves[i]);
	}
	printf("\n\nEnter the level (1-9) you want to begin with (0 to quit): ");
	scanf("%d",level);
}

void play_round(char sea[size][size],settings *sett,int dir[10][2],bool *playing) {
	int mv;
	bool flag = FALSE;

	printf("\nEnter your move : ");
	scanf("%d",&mv);
	if ((mv>0) && (mv<=9)) {
		move_player(sea,mv,playing,dir,sett);
	} else if (mv == 0) {
		save_game(sea,sett,playing);
	}
}

void move_player(char sea[size][size],int mv,bool *playing,int dir[10][2],settings *sett) {
	int i,j,x1,y1,x2,y2;

	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			if (sea[i][j] == player) {
				x1 = i;
				y1 = j;
				i = size;
				j = size;
			}
		}
	}
	x2 = x1 + dir[mv][0];
	y2 = y1 + dir[mv][1];
	if (x2>size-1)
		x2 = size-1;
	if (x2<0)
		x2 = 0;
	if (y2>size-1)
		y2 = size-1;
	if (y2<0)
		y2 = 0;
	sea[x1][y1] = csea;
	if ((sea[x2][y2] == wreck) || (sea[x2][y2] == island) || (sea[x2][y2] == pirate)) {
		crashed(sea,"You have been crashed!!!",playing,sett,TRUE);
		sett->looses++;
	} else {
		sea[x2][y2] = player;
		if (mv==5) {
			canons(sea);
		}
		move_pirates(sea,playing,sett);
		if (*playing) {
			check_pirates(sea,playing,sett);
		}
	}
}

void canons(char sea[size][size]) {
	int i,j,x1,y1,x2,y2,length;
	char temp;

	length = size;
	x2 = -1;
	y2 = -1;
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			temp = sea[i][j];
			if (sea[i][j] == player) {
				x1 = i;
				y1 = j;
				i = size;
				j = size;
			}
		}
	}
	for (i=x1; i<size; i++) {
		if (sea[i][y1] == pirate) {
			if ((i-x1)<length) {
				length = i-x1;
				x2 = i;
				y2 = y1;
			}
		}
	}
	for (i=x1; i>=0; i--) {
		if (sea[i][y1] == pirate) {
			if ((x1-i)<length) {
				length = x1-i;
				x2 = i;
				y2 = y1;
			}
		}
	}
	for (i=y1; i>=0; i--) {
		if (sea[x1][i] == pirate) {
			if ((y1-i)<length) {
				length = y1-i;
				x2 = x1;
				y2 = i;
			}
		}
	}
	for (i=y1; i<size; i++) {
		if (sea[x1][i] == pirate) {
			if ((i-y1)<length) {
				length = i-y1;
				x2 = x1;
				y2 = i;
			}
		}
	}
	if ((x2>=0) && (length<4)) {
		sea[x2][y2] = wreck;
	}
}

void move_pirates(char sea[size][size],bool *playing,settings *sett) {
	int i,j,x2,y2,x3,y3;
	int n1 = 0;
	int coords[3][2];

	for (i=0; i<3; i++) {
		for (j=0; j<2; j++) {
			coords[i][j] = -1;
		}
	}
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			if (sea[i][j] == player) {
				x2 = i;
				y2 = j;
				i = size;
				j = size;
			}
		}
	}
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			if (sea[i][j] == pirate) {
				x3 = i;
				y3 = j;
				if (i<x2) {
					x3 = i+1;
				} else if (i>x2) {
					x3 = i-1;
				}
				if (j<y2) {
					y3 = j+1;
				} else if (j>y2) {
					y3 = j-1;
				}
				sea[i][j] = csea;
				if ((sea[x3][y3] == island) || (sea[x3][y3] == wreck)) {
					sett->npirates--;
					sea[x3][y3] = wreck;
					sett->nwrecks++;
				} else {
					if (sea[x3][y3] == player) {
						sea[x3][y3] = wreck;
						i = size;
						j = size;
						crashed(sea,"You have been crashed!!!",playing,sett,TRUE);
						sett->looses++;
					} else {
						coords[n1][0] = x3;
						coords[n1][1] = y3;
						n1++;
					}
				}
			}
		}
	}
	for (i=0; i<3; i++) {
		if (coords[i][0] >0) {
			if (sea[coords[i][0]][coords[i][1]] == pirate) {
				sea[coords[i][0]][coords[i][1]] = wreck;
			} else {
				sea[coords[i][0]][coords[i][1]] = pirate;
			}
		}
	}
}

void crashed(char sea[size][size],char *txt,bool *playing,settings *sett,bool flag) {
	FILE *outfile;
	char *temp;
	/*Remove The comments in case you want to have lives in the game */
	//int x1,y1;

	/*if ((flag) && (sett.lives)) {
		sett->lives -=1;
		display_board(sea);
		printf("\n%s\n\n",txt);
		printf("That cost a life. Lifes remain: %d",sett->lives);
		do {
			x1 = get_number(0,9);
			y1 = get_number(0,9);
		} while ((sea[x1][y1] == wreck) || (sea[x1][y1] == pirate) || (sea[x1][y1] == island));
		sea[x1][y1] = player;
	} else {*/
	if (flag) {
		*playing = FALSE;
	}
		display_board(sea);
		printf("\n%s\n\n",txt);
		outfile = fopen("savegame.txt","w");
		fprintf(outfile,"%c",'!');
		if (flag) {
			scanf("%c",&temp);
		}
	//}
}

void check_pirates(char sea[size][size],bool *playing,settings *sett) {
	int i,j;
	int sum = 0;

	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			if (sea[i][j] == pirate) {
				sum++;
			}
		}
	}
	if (sum == 0) {
		crashed(sea,"You rule in the seas of Somalia...!!!",playing,sett,TRUE);
		sett->wins++;
	}
}

void save_game(char sea[size][size],settings *sett,bool *playing) {
	FILE *outfile;
	int i,j;

	outfile = fopen("savegame.txt","w");
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			fprintf(outfile,"%c",sea[i][j]);
		}
		fprintf(outfile,"\n");
	}
	//fprintf(outfile,"%d",sett->lives);
	//fprintf(outfile,"%d",sett->wins);
	//fprintf(outfile,"%d",sett->looses);
	fclose(outfile);
	printf("Game saved successfully...");
	*playing = FALSE;
	scanf("%d",&j);
}

void load_game(char sea[size][size],settings *sett) {
	FILE *infile;
	int i,j;
	int n2 = 0;
	char *buffer;
	
	infile = fopen("savegame.txt","r");
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			buffer = fgetc(infile);
			sea[i][j] = buffer;
		}
		buffer = fgetc(infile);
	}
	//buffer = fgetc(infile);
	//sett->lives = (int)buffer;
	//buffer = fgetc(infile);
	//buffer = fgetc(infile);
	//sett->wins = (int)buffer;
	//buffer = fgetc(infile);
	//buffer = fgetc(infile);
	//sett->looses = (int)buffer;
	sett->nislands = 0;
	sett->npirates = 0;
	sett->nwrecks = 0;
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {
			switch (sea[i][j]) {
			case pirate:
				sett->npirates++;
				break;
			case wreck:
				sett->nwrecks++;
				break;
			case island:
				sett->nislands++;
				break;
			}
		}
	}
	fclose(infile);
}
