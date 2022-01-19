//============================================================================
// Name        : Snake Game
// Author      : Zaid Siddiqui
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D Snake Game...
//============================================================================

#ifndef TETRIS_CPP_
#define TETRIS_CPP_

#include "util.h"
#include <iostream>
#include <fstream>
#include<vector>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<string>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sstream>
#include<cmath>	// for basic math functions such as cos, sin, sqrt

using namespace std;

// Global Declaration

int array[65][65] = {{0}};
int pressing_key_is = 0;
int motion_of_x[65], motion_of_y[65];
int food_x, food_y;
bool is_game_over = false;
bool is_game_started = false;
int power_food_x, power_food_y;
int size_of_snake = 3;
int score = 0;
bool food_dissapear = false;
bool power_food_dissapear = false;
string score_printer;
int hurdle_x[5], hurdle_y[5];
int size_of_hurdle = 5;
int hurdle_counter = 0;
int power_food_counter = 0;
int previous_score;
int new_score;
double startx = 320, starty = 400;

/* Function sets canvas size (drawing area) in pixels...
 *  that is what dimensions (x and y) your game will have
 *  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
 * 
 */

/*
 * Main Canvas drawing function.
 *
 */

void SetCanvasSize(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();
}

/*
 * This function reads the previous high score from the High_Score_storing_file.txt file
 *
 */

void score_reading_function() {
	ifstream previous;
	
	previous.open("High_Score_storing_file.txt");
	
	if(!(previous.is_open())) {
		previous_score = 0;
	} else {
		previous >> previous_score;
		previous.close();
	}
}

/*
 * This function writes the new high score in the High_Score_storing_file.txt file
 *
 */

void score_writing_function() {
	ofstream new_score;
	
	new_score.open("High_Score_storing_file.txt");
	new_score << score;
	new_score.close();
}

/*
 * This function generates Power food for Snake at random locations on the map
 *
 */

void power_food_generation_function() {
	power_food_x = rand () % 63 + 1;
	power_food_y = rand () % 63 + 1;
	
	array[power_food_x][power_food_y] = 25;
}

/*
 * This function removes Power food from the map if its not eaten by the Snake within a time limit
 *
 */

void power_food_dissappear_function() {
	array[power_food_x][power_food_y] = 0;
}

/*
 * This function checks if the Snake has eaten the Power food
 *
 */

void power_food_collision_detector() {
	if (array[motion_of_y[0]][motion_of_x[0]] == 25) {
		score += 20;
		array[power_food_x][power_food_y] = 0;
	}
}

/*
 * This function generates food for Snake at random locations on the map
 *
 */

void food_generation_function(int value) {
	if(food_dissapear == true) {
		array[food_x][food_y] = 0 ;
		food_dissapear = false;
	}

	food_x = rand () % 63 +1;
	food_y = rand () % 63 +1;
	array[food_x][food_y] = 3;
	food_dissapear = true;
     
     glutTimerFunc(20000.0 / FPS, food_generation_function, 1);
}

/*
 * This function checks if the Snake has eaten the food
 *
 */

void food_collision_detector() {
	if (array[motion_of_x[0]][motion_of_y[0]] == array[food_x][food_y]) {
		score += 10;
		food_dissapear = false;
		size_of_snake++;
	}
}

/*
 * This function generates hurdles (walls) at random locations on the map
 * Colliding with the walls will result in the Game Over.
 *
 */

void hurdle_generation_function() {
	hurdle_x[0] = rand () % 63 + 1;
	hurdle_y[0] = rand () % 63 + 1;
	hurdle_x[1] = hurdle_x[0];
	hurdle_y[1] = hurdle_y[0] - 1;
	hurdle_x[2] = hurdle_x[1];
	hurdle_y[2] = hurdle_y[1] - 1;
	hurdle_x[3] = hurdle_x[2];
	hurdle_y[3] = hurdle_y[2] - 1;
	hurdle_x[4] = hurdle_x[3];
	hurdle_y[4] = hurdle_y[3] - 1;
	
	for (int i = 0; i < size_of_hurdle; i++) {
		array[hurdle_x[i]][hurdle_y[i]] = 9;
	}
}

/*
 * This function removes hurdles from the map after a time limit
 *
 */

void hurdle_dissappearance_function() {
	for (int i = 0; i < size_of_hurdle; i++) {
		array[hurdle_x[i]][hurdle_y[i]] = 0;
	}
}

/*
 * This function checks if the Snake has collided with the hurdles
 *
 */

bool hurdle_collision_detector() {
	if (array[motion_of_y[0]][motion_of_x[0]] == 9) {
			return true;
	}
	return false;
}

/*
 * This function builds the required shapes for the Snake Game
 * Green Circle:  used for food
 * Blue Circle:   used for Power food
 * Red Circle:    used for Snake's head
 * White Square:  used for boundary walls
 * Red Square:    used for hurdles
 * Yellow Square: used for Snake's body
 *
 */

void displayBlock(bool print = true) {
	if (print) {
		for (int i = 0; i < 65; i++) {
			for (int j = 0; j < 65; j++) {
				if (array[j][i] == 1) {
					DrawCircle ( i*10+5 , j*10+5 , 5 , colors[5]);
				} else if (array[j][i] == 2) {
					DrawSquare(i*10 , j*10 , 10 , colors[16]);
				} else if (array[j][i] == 3) {
					DrawCircle(i*10+3, j*10+3, 3, colors[GREEN]);
				} else if (array[i][j] == 50) {
					DrawSquare(i*10, j*10, 8, colors[WHITE]);
				} else if (array[i][j] == 25) {
					DrawCircle (i*10+8, j*10+8, 8, colors[CYAN]);
				} else if (array[i][j] == 9) {
					DrawSquare(i*10, j*10, 8, colors[FIREBRICK]);
				}
			}
		}
	}
}

/*
 * This function checks if Snake collides with the Map's boundary
 *
 */

bool isSnakeHeadCollidingWithWalls() {
	if (motion_of_x[0] <= 0) {
		return true;
	} else if ( motion_of_x[0] >= 64) {
		return true;
	} else if (motion_of_y[0] <= 0) {
		return true;
	} else if (motion_of_y[0] >= 64) {
		return true;
	}

	return false;
}

/*
 * This function checks if Snake collides with itself
 *
 */

bool isSnakeCollidingWithItself() {
	if (array[motion_of_x[0]][motion_of_y[0]] == 2) {
		 return true;
	}

	return false;
}

/*
 * This function sets up the required checks for the game to run smoothly
 *
 */

void Display() {
	// set the background color using function glClearColotgr.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim rid and 1 means pure red and so on.

	glClearColor(0/*Red Component*/, 0.0/*Green Component*/,
				0.0/*Blue Component*/, 0 /*Alpha component*/);// Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT);   //Update the colors

	if (!is_game_started) {
		DrawString (200, 300, "START YOUR GAME!", colors[WHITE]);
	
		if (pressing_key_is != 0) {
			is_game_started = true;
		}
	} else {
		//DrawSquare( 300 , 300 ,100,colors[10]);  //This will draw a square shape of size 100x100  at x=300 and y=300
		//DrawString( 50, 600, "Here are some are basic shapes", colors[MISTY_ROSE]); // this will print given string at x=50 , y=600
		//DrawString( 50, 570, "You will use these to make your game", colors[MISTY_ROSE]); // this will print given string at x=50 , y=570
		//DrawCircle( 200 , 200 , 50 , colors[5]); // This will draw a circle at x=200,y=200 of radius 50

		if (!is_game_over) {
			displayBlock( false );
			displayBlock();

			if (isSnakeHeadCollidingWithWalls() || isSnakeCollidingWithItself() || hurdle_collision_detector()) {
				is_game_over = true;
			} else {
				food_collision_detector ();
				power_food_collision_detector ();
				
				hurdle_counter++;
				power_food_counter++;
				
				if (power_food_counter == 300) {
					power_food_generation_function ();
					power_food_counter = 0;
				} else if (power_food_counter == 75) {
					power_food_dissappear_function ();
				}
				
				if (hurdle_counter == 150) {
					hurdle_generation_function ();
				} else if (hurdle_counter == 300) {
					hurdle_dissappearance_function ();
					hurdle_counter = 140;
				}
				
				if (score > previous_score) {
					score_writing_function();
				}
				
				DrawString( 50, 570, "Score:", colors[GOLDEN_ROD]);
				score_printer = static_cast<ostringstream*>( &(ostringstream() << score) )->str();
				DrawString( 120, 570, score_printer, colors[GOLDEN_ROD]);
				
				if (pressing_key_is != 0) {
					array[motion_of_x[size_of_snake-1]][motion_of_y[size_of_snake-1]] = 0;
		
					for(int i = size_of_snake - 1; i > 0; i--) {
						motion_of_x[i]=motion_of_x[i-1];
						motion_of_y[i]=motion_of_y[i-1];
					}
					
					if (pressing_key_is == 4) {
						motion_of_x[0]++;
					} else if (pressing_key_is == 5) {
						motion_of_x[0]--;
					} else if (pressing_key_is == 6) {
						motion_of_y[0]++;
					} else if (pressing_key_is == 7) {
						motion_of_y[0]--;
					}
				}
			}
		} else if (is_game_over == true) {
			DrawString (200, 380, "MAZARAT! GAME OVER!", colors[MISTY_ROSE]);
			DrawString (200, 360, "FINAL SCORE IS:", colors[MISTY_ROSE]);
			DrawString( 400, 360, score_printer, colors[MISTY_ROSE]);
		}

		array[motion_of_x[0]][motion_of_y[0]] = 1;

		for(int i = 1; i < size_of_snake; i++) {
			array[motion_of_x[i]][motion_of_y[i]] = 2;
		}
	}
	
	//                    v1( x,y )   v2( x,y )  , v3( x,y ) 
	//DrawTriangle( 300, 50 , 500, 50 , 400 , 250, colors[MISTY_ROSE] );  // Trianlge Vertices v1(300,50) , v2(500,50) , v3(400,250)

	//DrawLine(int x1, int y1, int x2, int y2, int lwidth, float *color)
	//DrawLine( 550 , 50 ,  550 , 600 , 20 , colors[MISTY_ROSE] );
 
	glutSwapBuffers(); // do not modify this line..
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
 * is pressed from the keyboard
 *
 * You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
 *
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 *
 * */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT   /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		pressing_key_is = 7;					// what to do when left key is pressed...
	} else if (key == GLUT_KEY_RIGHT  /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {
		pressing_key_is = 6;
	} else if (key == GLUT_KEY_UP) /*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/ {
		pressing_key_is = 4;
	} else if (key == GLUT_KEY_DOWN)   /*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/ {
		pressing_key_is = 5;
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	 * this function*/
	
	glutPostRedisplay();
}

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
 * is pressed from the keyboard
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 * */

void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(0); // exit the program when escape key is pressed.
	}
	
	if (key == 'R' || key=='r'/* Escape key ASCII*/) {
		//exit(1); // exit the program when escape key is pressed.
		//aswangle+=90;
	} else if (int(key) == 13) {

	}

	glutPostRedisplay();
}

/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */

void Timer(int m) {
	// implement your functionality here
	glutPostRedisplay();

	// once again we tell the library to call our Timer function after next 1000/FPS
	glutTimerFunc(350.0 / FPS, Timer, 0);
}

/*
 * our gateway main function
 * */
int main(int argc, char*argv[]) {
	int width = 650, height = 650; // i have set my window size to be 800 x 600

	score_reading_function();

	motion_of_x[0] = 35;
	motion_of_y[0] = 35;
	motion_of_x[1] = 34;
	motion_of_y[1] = 35;
	motion_of_x[2] = 33;
	motion_of_y[2] = 35;
	motion_of_x[3] = 32;
	motion_of_y[3] = 35;

	InitRandomizer(); // seed the random number generator...
	glutInit(&argc, argv); // initialize the graphics library...

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("PF's Snake Game"); // set the title of our game window
	SetCanvasSize(width, height); // set the number of pixels...

	for (int i = 0; i < 65; i++) {
		array[0][i] = 50 ;
	}

	for (int i = 0; i < 65; i++) {
		array[i][0] = 50 ;
	}

	for (int i = 0; i < 65; i++) {
		array[64][i] = 50 ;
	}

	for (int i = 0; i < 65; i++) {
		array[i][64] = 50 ;
	}

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	//glutDisplayFunc(display); // tell library which function to call for drawing Canvas.
	glutDisplayFunc(Display); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters

	// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(500.0 / FPS, Timer, 0);

	glutTimerFunc(20000.0 / FPS, food_generation_function, 1);

	// now handle the control to library and it will call our registered functions when
	// it deems necessary...
	glutMainLoop();
	
	return 1;
}

#endif /* Snake Game */