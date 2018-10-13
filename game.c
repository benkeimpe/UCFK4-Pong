/**
* Game module source code
* @team 128
* @author Ambrose Ledbrook - 79172462
* @author Josh Jarvis -
* @date 09-oct-2018
*
* @brief This module provides the main functionality of the game, allowing
*        the two players of the game to actually play the game.
*/


// Inlcuding modules used
#include "system.h"
#include "tinygl.h"
#include "pacer.h"
#include "paddle.h"
#include "messages.h"
#include "font3x5_1.h"
#include "font.h"
#include "button.h"
#include "comms.h"
#include "game.h"
#include "led.h"
#include "ballTest.h"


/*
* Intialises the tinygl module used by the game
*/
void tiny_init(void) {
    // Setting up initial values of the tinygl module
    tinygl_init (PACER_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
}


/*
* Intialises the game enviroment and other modules used by the game, while also
* setting the initial game state
*/
void game_init(void) {
    system_init();
    ball_init();
    navswitch_init();
    paddle_init();
    pacer_init(PACER_RATE);
    button_init();
    tiny_init();
    comms_init();
    led_init();
    // Turning the led off
    led_set(0, 0);
    // Setting the initial state of the game
    game_state = NOT_STARTED;
}


/*
* Handles the movement of the game paddle through the use of the paddle module.
*/
void paddle_task(void) {
    paddle_move();
}


/*
* Clears the display of the ledmat
*/
void clear_display(void) {
    tinygl_clear();
}


/**
* Changes the state of the game to that of the passed state. Also performs
* initializing tasks for the new state.
*/
void change_states(Game_states new_state) {
    // Updating the game state
    game_state = new_state;
    // Checking the new current state and performing actions if required
    // dependent on the current state
    switch (game_state) {
        case WON :
            // Showing a winning message and turing the led on
            show_won();
            led_set(0, 1);
            break;
        case LOST :
            // Showing a losing message, turing the led ff and notifying the
            // other board that they won the game
            show_lost();
            send_won();
            led_set(0, 0);
            break;
        case PLAYING :
            // Turning the led on
            led_set(0, 1);
            break;
        case WAITING :
            // Turning the led on
            led_set(0, 0);
        default :
            break;
    }
}


/**
* Checks if the button is pressed if so the game state is changed to PLAYING
*/
void button_task(void) {
    // Chekcing for a button event
    if (button_push_event_p(0)) {
        // Clearing the display
        clear_display();
        // Updating the game state to WAITING
        change_states(WAITING);
        // Sendng a signal to the other board to start the game
        send_start();
    }
}


/**
* Checks if the ball has reached the top of the ledmat if so the ball is sent
* to the other board. Otherwise the ball postion is updated.
*/
void ball_task(void) {
    //TODO Check ball collisons with function in ball module
    if (check_send()) {
        // sending the ball
        send_ball_pos();
        // Changing states to wait for the ball to return
        change_states(WAITING);
    } else {
        // updating the postion of the ball
        ball_update();
    }
}


/**
* Receives data through the ir receiver. If the data recieved matches the
* defined START_CODE then the game state is changed to PLAYING.
*/
void check_start(void) {
    // Receiving data
    Data data = receiveData();
    // Checking if data recieved is a START_CODE
    if (data.type == START_CODE) {
        // Clearing the display and changing the game state to PLAYING
        clear_display();
        change_states(PLAYING);
    }
}


/**
* This method receives data through the ir receiver. It then checks if the
* received data is any of the expected values, a WIN_CODE or a BALL_CODE.
* Corresponding to either a notification that this board has won the game or
* passing the ball back to this board.
*/
void check_ir(void) {
    // Receiving data
    Data received = receiveData();
    // Checking the type of the recieved data
    switch(received.type) {
        case WIN_CODE :
            // Clearing the display and showing the winning message
            clear_display();
            change_states(WON);
            break;
        case BALL_CODE :
            // Receiving the ball from the other board
            receiveBall(received.ball_pos, received.ball_dir);
            // Changing the state t PLAYING
            change_states(PLAYING);
            break;
        default :
            break;
    }
}


//TODO Docstring this
/*
* Runs the game
*/
int main(void) {
    // Initializing the game enviroment and used modules
    game_init();
    // Showing the welcome message
    show_welcome();
    // Intialising tick varaibles used in the pacer loop
    uint8_t game_tick = 0;
    uint8_t ball_tick = 0;
    //TODO Need to handle ball hitting paddle
    //TODO Need to test and better configure the ir communications
    // Pace looping to run the game
    while(1) {
        // Waiting
        pacer_wait();
        // Incrementing ticks
        game_tick++;
        ball_tick++;
        // Updating button and navswitch at 20 hertz
        //TODO Check this and change BUTTON_RATE
        if (game_tick == (PACER_RATE / BUTTON_RATE)) {
            navswitch_update();
            button_update();
            game_tick = 0;
        }
        // Updating tinygl
        tinygl_update();
        // Checking the curent game state
        switch(game_state) {
            case WON : // Fall through to NOT_STARTED state
            case LOST : // Fall through to NOT_STARTED state
            case NOT_STARTED :
                // chekcing for a button press or START_CODE to start the game
                button_task();
                check_start();
                break;
            case PLAYING :
                // If updating ball every 50 game ticks
                if (ball_tick >= BALL_RATE) {
                    ball_task();
                    ball_tick = 0;
                }
                // Checking for paddle movement
                paddle_task();
                break;
            case WAITING :
                // Waiting for a BALL_CODE or WIN_CODE
                check_ir();
                // Checking for paddle movement
                paddle_task();
                break;
            break;
        }
    }
}
