/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* HAL and Application includes */
#include <Application.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>

//these are constant values used throughout the program
#define MaxLengthVal 3  //the max length of a int or char to be merged with a string
#define MoveExplorerGraphicBy 20 //the amount that the explorer is moved by on the display

//these import the images and graphics used
extern const Graphics_Image explore_lvl14BPP_UNCOMP;
extern const Graphics_Image explore_lvl24BPP_UNCOMP;
extern const Graphics_Image explore_lvl34BPP_UNCOMP;
extern const Graphics_Image background8BPP_UNCOMP;
extern const Graphics_Image start_screen8BPP_UNCOMP;
extern const Graphics_Image gameOver8BPP_UNCOMP;
/**
 * The main entry point of your project. The main function should immediately
 * stop the Watchdog timer, call the Application constructor, and then
 * repeatedly call the main super-loop function. The Application constructor
 * should be responsible for initializing all hardware components as well as all
 * other finite state machines you choose to use in this project.
 *
 * THIS FUNCTION IS ALREADY COMPLETE. Unless you want to temporarily experiment
 * with some behavior of a code snippet you may have, we DO NOT RECOMMEND
 * modifying this function in any way.
 */
int main(void)
{
    // Stop Watchdog Timer - THIS SHOULD ALWAYS BE THE FIRST LINE OF YOUR MAIN
    WDT_A_holdTimer();

    // Initialize the system clock and background hardware timer, used to enable
    // software timers to time their measurements properly.
    InitSystemTiming();

    // Initialize the main Application object and HAL object
    HAL hal = HAL_construct();
    Application app = Application_construct();

    // Main super-loop! In a polling architecture, this function should call
    // your main FSM function over and over.
    while (true)
    {
        HAL_refresh(&hal);
        Application_loop(&app, &hal);
    }
}

/**
 * A helper function which increments a value with a maximum. If incrementing
 * the number causes the value to hit its maximum, the number wraps around
 * to 0.
 */
uint32_t CircularIncrement(uint32_t value, uint32_t maximum)
{
    return (value + 1) % maximum;
}

/**
 * The main constructor for your application. This function should initialize
 * each of the FSMs which implement the application logic of your project.
 *
 * @return a completely initialized Application object
 */
Application Application_construct()
{
    Application app;

    // Initialize local application state variables here!
    app.baudChoice = BAUD_9600;
    app.firstCall = true;
    app.firstKeyStroke = false;
    app.lastKeyStroke = false;
    app.explorerFSMstate = SIT;
    app.energy = 1;
    app.exp = 0;
    app.level = 0;
    app.move = 0;
    app.toNext = 1;
    app.explorerXcoord = 53;
    app.explorerYcoord = 65;
    app.explorerPosition = MidMid;
    app.currentGraphic = explore_lvl14BPP_UNCOMP;

    return app;
}

/**
 * The main super-loop function of the application. We place this inside of a
 * single infinite loop in main. In this way, we can model a polling system of
 * FSMs. Every cycle of this loop function, we poll each of the FSMs one time,
 * followed by refreshing all inputs to the system through a convenient
 * [HAL_refresh()] call.
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void Application_loop(Application* app_p, HAL* hal_p)
{
    // The obligatory non-blocking check. At any time in your code, pressing and
    // holding Launchpad S1 should always turn on Launchpad LED1.
    //
    // Not a real TO DO: Ensure that your code never blocks turning on this LED.
    if (Button_isPressed(&hal_p->launchpadS1))
        LED_turnOn(&hal_p->launchpadLED1);
    else
        LED_turnOff(&hal_p->launchpadLED1);

    // Restart/Update communications if either this is the first time the application is
    // run or if Boosterpack S1 is pressed (which means a new baudrate is being set up)
    if (Button_isTapped(&hal_p->boosterpackS1) || app_p->firstCall) {
        Application_updateCommunications(app_p, hal_p);
    }


    //sets up splash screen
    if (!(app_p->firstKeyStroke)){
        SplashScreenSetup(&hal_p->gfx);

    }

    if (UART_hasChar(&hal_p->uart)) {

        // The character received from your serial terminal
        char rxChar = UART_getChar(&hal_p->uart);
        //currently one behind for some reason.

        char txChar = Application_interpretIncomingChar(rxChar);

        // Only send a character if the UART module can send it
        if (UART_canSend(&hal_p->uart))
            UART_putChar(&hal_p->uart, rxChar);

        //updates state of FSM
        if (app_p->firstKeyStroke && !(app_p->lastKeyStroke)){
            ExplorerFSM(app_p, hal_p, &hal_p->gfx, rxChar);
        }


        if(!app_p->firstKeyStroke){
            app_p->firstKeyStroke = true;
            GamePlayScreenSetup(app_p, &hal_p->gfx);
        }

    }






}

/**
 * Updates which LEDs are lit and what baud rate the UART module communicates
 * with, based on what the application's baud choice is at the time this
 * function is called.
 *
 * @param app_p:  A pointer to the main Application object.
 * @param hal_p:  A pointer to the main HAL object
 */
void Application_updateCommunications(Application* app_p, HAL* hal_p)
{
    // When this application first loops, the proper LEDs aren't lit. The
    // firstCall flag is used to ensure that the
    if (app_p->firstCall) {
        app_p->firstCall = false;
    }

    // When Boosterpack S1 is tapped, circularly increment which baud rate is used.
    else
    {
        uint32_t newBaudNumber = CircularIncrement((uint32_t) app_p->baudChoice, NUM_BAUD_CHOICES);
        app_p->baudChoice = (UART_Baudrate) newBaudNumber;
    }

    // Start/update the baud rate according to the one set above.
    UART_SetBaud_Enable(&hal_p->uart, app_p->baudChoice);

    // Based on the new application choice, turn on the correct LED.
    // To make your life easier, we recommend turning off all LEDs before
    // selectively turning back on only the LEDs that need to be relit.
    // -------------------------------------------------------------------------
    LED_turnOff(&hal_p->boosterpackRed);
    LED_turnOff(&hal_p->boosterpackGreen);
    LED_turnOff(&hal_p->boosterpackBlue);

    // DONE: Turn on all appropriate LEDs according to the tasks below.
    switch (app_p->baudChoice)
    {
        // When the baud rate is 9600, turn on Boosterpack LED Red
        case BAUD_9600:
            LED_turnOn(&hal_p->boosterpackRed);
            break;

        // DONE: When the baud rate is 19200, turn on Boosterpack LED Green
        case BAUD_19200:
            LED_turnOn(&hal_p->boosterpackGreen);
            break;

        // DONE: When the baud rate is 38400, turn on Boosterpack LED Blue
        case BAUD_38400:
            LED_turnOn(&hal_p->boosterpackBlue);
            break;

        // DONE: When the baud rate is 57600, turn on all Boosterpack LEDs (illuminates white)
        case BAUD_57600:
            LED_turnOn(&hal_p->boosterpackRed);
            LED_turnOn(&hal_p->boosterpackGreen);
            LED_turnOn(&hal_p->boosterpackBlue);
            break;

        // In the default case, this program will do nothing.
        default:
            break;
    }
}

/**
 * Interprets a character which was incoming and returns an interpretation of
 * that character. If the input character is a letter, it return L for Letter, if a number
 * return N for Number, and if something else, it return O for Other.
 *
 * @param rxChar: Input character
 * @return :  Output character
 */
char Application_interpretIncomingChar(char rxChar)
{
    // The character to return back to sender. By default, we assume the letter
    // to send back is an 'O' (assume the character is an "other" character)
    char txChar = 'O';

    // Numbers - if the character entered was a number, transfer back an 'N'
    if (rxChar >= '0' && rxChar <= '9') {
        txChar = 'N';
    }

    // Letters - if the character entered was a letter, transfer back an 'L'
    if ((rxChar >= 'a' && rxChar <= 'z') || (rxChar >= 'A' && rxChar <= 'Z')) {
        txChar = 'L';
    }

    return (txChar);
}

/**
 * Sets up the graphics needed for the title screen / splash screen
 * by calling text and graphics up through the graphics instance.
 *
 * @param graphicsInterface is the GFX interface being modified
 */
void SplashScreenSetup(GFX* gfx_p){
    //adds a nice background image
    Graphics_drawImage(gfx_p, &start_screen8BPP_UNCOMP, 30, 70);
    //outputs text for the title
    GFX_print(gfx_p, "Explorey Explorer 2", 1, 1);
    GFX_print(gfx_p, "Built by Illa Rochez", 3, 1);
    GFX_print(gfx_p, "-------------------", 5, 1);
    GFX_print(gfx_p, "Press a key to start", 7, 1);
}

/**
 *sets up the visuals for the gameplay screen
 */
void GamePlayScreenSetup(Application* app_p, GFX* gfx_p){
    GFX_clear(gfx_p);
    //gameplay map set up

    Graphics_drawImage(gfx_p, &background8BPP_UNCOMP, 30, 40);
    GFX_drawHollowRectangle(gfx_p, 30, 40, 60, 60);
    Graphics_drawImage(gfx_p, &(app_p->currentGraphic), app_p->explorerXcoord, app_p->explorerYcoord);
    //GFX_drawSolidCircle(gfx_p, app_p->explorerXcoord, app_p->explorerYcoord, 5);

    //gameplay text (ie moves, energy, ect)
    PrintMergedStringWithIntDoubleDigit(gfx_p, "Exp: ", app_p->exp, 0, 0);
    PrintMergedStringWithIntDoubleDigit(gfx_p, "Level: ", app_p->level, 1, 0);
    PrintMergedStringWithInt(gfx_p, "Energy: ", app_p->energy, 0, 10);
    PrintMergedStringWithIntDoubleDigit(gfx_p, "To Next: ", app_p->toNext, 1, 10);
    //char* temp = "Input: ";
    //PrintMergedStringWithChar(gfx_p, "Input: ", ' ', 13, 0);

    //GFX_print(gfx_p, temp, 13, 0); //+ rxChar
    PrintMergedStringWithInt(gfx_p, "Move: ", app_p->move, 13, 12);
}

/**
 *sets up the screen when the game is terminated
 *@param gfx_p is the graphics interface being utilized
 */
void GameOverScreenSetup(GFX* gfx_p, Application* app_p){
    GFX_clear(gfx_p);
    Graphics_drawImage(gfx_p, &gameOver8BPP_UNCOMP, 30, 70);
    GFX_print(gfx_p, "GAME OVER", 4, 6);
    PrintMergedStringWithIntDoubleDigit(gfx_p, "Level Achieved: ", app_p->level, 6, 1);
    app_p->lastKeyStroke = true;

}


/**
 * this method updates the display text by merging a provided number with a provided string
 * this is done by allocating the exact amount of memory space needed using malloc to a temporary value.
 * the provided integer is then converted into a string and merged with the provided string in the temporary
 * character value. this is then provided to the graphics interface as a parameter along with the exact location
 * the string should appear on the monitor.
 *
 * the allocated temporary string is then freed to avoid memory leaks.
 *
 * @param gfx_p is a pointer to the graphics interface being used
 * @param string is the character array containing the string to use in merge
 * @param toMerge is the interger to merge with the provided character array
 * @param y is the row value for the call to GFX_print
 * @param x is the column value for the call to GFX_print
 */
void PrintMergedStringWithInt(GFX* gfx_p, char* string, int toMerge, int y, int x){

    /*char* toMergeChar;
    char* mergedString = malloc(strlen(string) + 2);
    asprintf(&toMergeChar, "%d", toMerge);
    strcat(strcpy(mergedString, string), toMergeChar);

    GFX_print(gfx_p, mergedString, y, x);
    free(mergedString);*/
    int buffer = strlen(string) ;
    char toMergeChar[MaxLengthVal];
    snprintf(toMergeChar, MaxLengthVal, "%d", toMerge);
    GFX_print(gfx_p, string, y, x);
    GFX_print(gfx_p, toMergeChar, y, x + buffer); //for some reason the integer is not updating.

}


/**
 * this method updates the display text by merging a provided number with a provided string
 * this is done by allocating the exact amount of memory space needed using malloc to a temporary value.
 * the provided integer is then converted into a string and merged with the provided string in the temporary
 * character value. this is then provided to the graphics interface as a parameter along with the exact location
 * the string should appear on the monitor.This version provides two digits always (ie 02 instead of 2)
 *
 * the allocated temporary string is then freed to avoid memory leaks.
 *
 * @param gfx_p is a pointer to the graphics interface being used
 * @param string is the character array containing the string to use in merge
 * @param toMerge is the interger to merge with the provided character array
 * @param y is the row value for the call to GFX_print
 * @param x is the column value for the call to GFX_print
 */
void PrintMergedStringWithIntDoubleDigit(GFX* gfx_p, char* string, int toMerge, int y, int x){

    /*char* toMergeChar;
    char* mergedString = malloc(strlen(string) + 2);
    asprintf(&toMergeChar, "%02d", toMerge);
    strcat(strcpy(mergedString, string), toMergeChar);

    GFX_print(gfx_p, mergedString, y, x);
    free(mergedString);*/

    int buffer = strlen(string);
    //asprintf(&toMergeChar, "%02d", toMerge);
    char toMergeChar[MaxLengthVal];
    snprintf(toMergeChar, MaxLengthVal, "%02d", toMerge);
    GFX_print(gfx_p, string, y, x);
    GFX_print(gfx_p, toMergeChar, y, x + buffer);
}

/**
 * this method updates the display text by merging a provided character with a provided string
 * this is done by allocating the exact amount of memory space needed using malloc to a temporary value.
 * the provided character is then merged with the provided string in the temporary character value.
 * this is then provided to the graphics interface as a parameter along with the exact location
 * the string should appear on the monitor.
 *
 * the allocated temporary string is then freed to avoid memory leaks.
 *
 * @param gfx_p is a pointer to the graphics interface being used
 * @param string is the character array containing the string to use in merge
 * @param toMerge is the character to merge with the provided character array
 * @param y is the row value for the call to GFX_print
 * @param x is the column value for the call to GFX_print
 */
void PrintMergedStringWithChar(GFX* gfx_p, char* string, char toMerge, int y, int x){

    /*char* mergedString = malloc(strlen(string) + 1);
    strcat(strcpy(mergedString, string), &toMerge);

    GFX_print(gfx_p, mergedString, y, x);
    free(mergedString);*/

    int buffer = strlen(string);
    char toMergeChar[MaxLengthVal];
    //char toMergeChar = (char) toMerge;
    //asprintf(&toMergeChar, "%c", toMerge);
    snprintf(toMergeChar, MaxLengthVal, "%c", toMerge);
    GFX_print(gfx_p, string, y, x);
    GFX_print(gfx_p, toMergeChar, y, x + buffer);


}
/**
 * the finite state machine that determines the exact state of the explorer
 * it is updated by the user entering in keystrokes.
 * movement oriented keystrokes result in the movement of the explorer graphic,
 * and feeding oriented keystrokes result in the increase of energy
 *
 * additional features and states are also implemented via additional functions that are called within this
 * main FMS
 *
 * @param app_p is the application being utilized
 * @param hal_p is the UART interface being utilized
 * @param gfx_p is the graphics interface being utilized
 * @param rx is the current keystroke that has been entered by the user
 */
void ExplorerFSM(Application* app_p, HAL* hal_p, GFX* gfx_p, char rx){

    switch(rx){

        case 'w':
            moveExplorerFSM(app_p, gfx_p, UP);

            break;
        case 's':
            moveExplorerFSM(app_p, gfx_p, DOWN);

            break;
        case 'd':
            moveExplorerFSM(app_p, gfx_p, RIGHT);

            break;
        case 'a':
            moveExplorerFSM(app_p, gfx_p, LEFT);

            break;
        case 'f':
            if (app_p->energy < 5){
                UpdateEnergy(app_p, gfx_p, app_p->energy + 1);
            }

            break;
        default:

            break;
    }

}

/**
 * this is a more specific FSM that focuses on the exact position of the explorer on the 3x3 grid.
 * It moves the explorer graphic if it can in its specific grid location, and updates its graphic
 * in addition to the numbers associated with the user's score (ie level, exp, ect)
 *
 * @param app_p the application being use
 * @param gfx_p the graphic interface being used
 * @param directToMove the direction that the explorer has been instructed to move in
 */
void moveExplorerFSM(Application* app_p, GFX* gfx_p, _explorerFSMstate directToMove){
    _explorerPosition currentPos = app_p->explorerPosition;

    switch(currentPos){
        case UpLeft:
            if (directToMove != UP && directToMove !=LEFT){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case UpMid:
            if (directToMove !=UP){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case UpRight:
            if (directToMove != UP && directToMove !=RIGHT){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case MidLeft:
            if (directToMove != LEFT){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case MidMid:
            moveExplorerTo(app_p, gfx_p, directToMove);
            UpdateMoveTo(app_p, gfx_p);
            break;
        case MidRight:
            if(directToMove != RIGHT){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case DownLeft:
            if(directToMove != DOWN && directToMove != LEFT ){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case DownMid:
            if(directToMove != DOWN){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;
        case DownRight:
            if(directToMove != DOWN && directToMove !=RIGHT){
                moveExplorerTo(app_p, gfx_p, directToMove);
                UpdateMoveTo(app_p, gfx_p);
            }
            break;

    }

}


/**
 * this method updates the applications stored explorer coordinates based on
 * the provided direction to move in.
 *
 * the explorer enum is set up so that by modifying the value by 1 you will move one
 * left or right in the array, and so that by modifying the value by 3 you will move
 * one up or down in the array.
 *
 * the graphic is also updated here along with the numeric values associated with the explorer
 *
 * @param app_p the application interface being utilized
 * @param gfx_p the graphic interface being utilized
 * @param directToMove the direction that the explorer is to move in.
 */
void moveExplorerTo(Application* app_p, GFX* gfx_p,_explorerFSMstate directToMove){

    //removes the previous icon
    Graphics_drawImage(gfx_p, &background8BPP_UNCOMP, 30, 40);
    GFX_drawHollowRectangle(gfx_p, 30, 40, 60, 60);

    //makes life easier
    int xcoord = app_p->explorerXcoord;
    int ycoord = app_p->explorerYcoord;


    //changes the coordinates based on the move being processed
    switch(directToMove){
        case UP:
            app_p->explorerYcoord = ycoord - MoveExplorerGraphicBy;
            app_p->explorerPosition -=3;
            break;

        case DOWN:
            app_p->explorerYcoord = ycoord + MoveExplorerGraphicBy;
            app_p->explorerPosition +=3;
            break;

        case LEFT:
            app_p->explorerXcoord = xcoord - MoveExplorerGraphicBy;
            app_p->explorerPosition--;
            break;

        case RIGHT:
            app_p->explorerXcoord = xcoord + MoveExplorerGraphicBy;
            app_p->explorerPosition++;
            break;
    }

    //draws the icon in its new location.
    Graphics_drawImage(gfx_p, &(app_p->currentGraphic), app_p->explorerXcoord, app_p->explorerYcoord);

    UpdateExp(app_p, gfx_p, app_p->exp + 1);
    UpdateLevel(app_p, gfx_p);
}

/**
 * this method updates the move value, decrementing it unless an energy is "consumed"
 * and will also update the display accordingly.
 *
 * If the character moves are currently at 0 the energy will also be updated
 *
 * @param app_p is the application being used
 * @param gfx_p is a pointer to the graphic interface being used
 */
void UpdateMoveTo(Application* app_p, GFX* gfx_p){
    if (app_p->move == 1){
        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        PrintMergedStringWithInt(gfx_p, "Move: ", app_p->move, 13, 12);
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);
        app_p->move = 0;
        PrintMergedStringWithInt(gfx_p, "Move: ", app_p->move, 13, 12);
        UpdateEnergy(app_p, gfx_p, app_p->energy - 1);

    }
    else{
        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        PrintMergedStringWithInt(gfx_p, "Move: ", app_p->move, 13, 12);
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);
        app_p->move++;
        PrintMergedStringWithInt(gfx_p, "Move: ", app_p->move, 13, 12);
    }

}


/**
 * this method updates the Energy value based on the provided newValue
 * and also will update the display accordingly
 *
 * If the energy is to be updated to 0, then the game over screen is displayed
 *
 * @param app_p is the application being used
 * @param gfx_p is a pointer to the graphic interface being used
 * @param newValue is the new value of energy being used
 */
void UpdateEnergy(Application* app_p, GFX* gfx_p, int newValue){
    if (newValue == 0){
        GameOverScreenSetup(gfx_p, app_p);
    }
    else{
        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        PrintMergedStringWithInt(gfx_p, "Energy: ", app_p->energy, 0, 10);
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);
        app_p->energy = newValue;
        PrintMergedStringWithInt(gfx_p, "Energy: ", app_p->energy, 0, 10);
    }
}

/**
 * updates the experience of the player on the display by changing
 * foreground color and rewriting the phrase with the modified experience value
 *
 * new experience value is stored in the application value exp
 *
 * @param app_p is the application being used
 * @param gfx_p is the graphics interface being used
 * @param newValue is the new value that experience will be updated to
 */
void UpdateExp(Application* app_p, GFX* gfx_p, int newValue){
    GFX_setForeground(gfx_p, gfx_p->defaultBackground);
    PrintMergedStringWithIntDoubleDigit(gfx_p, "Exp: ", app_p->exp, 0, 0);
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);
    app_p->exp = newValue;
    PrintMergedStringWithIntDoubleDigit(gfx_p, "Exp: ", app_p->exp, 0, 0); //new exp doesn't appear. correct value was stored (1)
}

/**
 * This method updates the ToNext value. the current to next on the display is updated using setForeground and
 * the PrintMergedString methods. Then toNext is updated by the predetermined formula: (2*currentLevel) + 1
 *
 * the new values are then printed to the graphics display
 *
 * @param app_p is the application interface being used
 * @param gfx_p is a pointer to the graphics interface being used
 */
void UpdateToNext(Application* app_p, GFX* gfx_p){
    GFX_setForeground(gfx_p, gfx_p->defaultBackground);
    PrintMergedStringWithIntDoubleDigit(gfx_p, "To Next: ", app_p->toNext, 1, 10);
    GFX_setForeground(gfx_p, gfx_p->defaultForeground);
    app_p->toNext = (2*app_p->level) + 1;
    PrintMergedStringWithIntDoubleDigit(gfx_p, "To Next: ", app_p->toNext, 1, 10);
}

/**
 * This method updates the current level. First it ensures that the experience and the
 * toNext level values are equal, then it updates the display and increments the level both visually
 * and within the application
 *
 * this method also checks to see if the level has updated. On an updated level, the explorer's appearance changes
 * @param app_p is the application interface being used
 * @param gfx_p is the graphic interface being used.
 */
void UpdateLevel(Application* app_p, GFX* gfx_p){
    if (app_p->exp == app_p->toNext){

        GFX_setForeground(gfx_p, gfx_p->defaultBackground);
        PrintMergedStringWithIntDoubleDigit(gfx_p, "Level: ", app_p->level, 1, 0);
        app_p->level++;
        UpdateToNext(app_p, gfx_p);
        UpdateExp(app_p, gfx_p, 0);
        GFX_setForeground(gfx_p, gfx_p->defaultForeground);
        PrintMergedStringWithIntDoubleDigit(gfx_p, "Level: ", app_p->level, 1, 0);
    }
    if (app_p->level == 5){
        app_p->currentGraphic = explore_lvl24BPP_UNCOMP;

    }

    if(app_p->level == 7){
        app_p->currentGraphic = explore_lvl34BPP_UNCOMP;
    }

}

