/**
* Message module
* @team 128
* @author Ambrose Ledbrook - 79172462
* This module provides simple methods used to display messages to the player
*/
#include "messages.h"
#include "tinygl.h"


/**
* Showing the welcome text on the board
*/
void show_welcome(void) {
    tinygl_text("PRESS BUTTON TO START");
}


/**
* Showing text indicating a win on the board
*/
void show_won(void) {
    tinygl_text("YOU WON!! :)  PRESS BUTTON TO RESTART");
}


/**
* Showing text indicating a loss on the board
*/
void show_lost(void) {
    tinygl_text("YOU LOST :(   PRESS BUTTON TO RESTART");
}
