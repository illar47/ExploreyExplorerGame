/*
 * Application.h
 *
 *  Created on: Dec 29, 2019
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhand-Ali
 */

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <HAL/HAL.h>

typedef enum {UP, DOWN, LEFT, RIGHT, FEED, SIT} _explorerFSMstate;
typedef enum{UpLeft, UpMid, UpRight, MidLeft, MidMid, MidRight, DownLeft, DownMid, DownRight} _explorerPosition;

struct _Application
{
    // Put your application members and FSM state variables here!
    // =========================================================================
    UART_Baudrate baudChoice;
    _explorerPosition explorerPosition;
    _explorerFSMstate explorerFSMstate;  //might not be super important now

    bool firstCall;
    bool firstKeyStroke;
    bool lastKeyStroke;
    int exp;
    int energy;
    int level;
    int toNext;
    int move;
    char input;
    int explorerXcoord;
    int explorerYcoord;
    Graphics_Image currentGraphic;



};
typedef struct _Application Application;

// Called only a single time - inside of main(), where the application is constructed
Application Application_construct();

// Called once per super-loop of the main application.
void Application_loop(Application* app, HAL* hal);

// Called whenever the UART module needs to be updated
void Application_updateCommunications(Application* app, HAL* hal);

// Interprets an incoming character and echoes back to terminal what kind of
// character was received (number, letter, or other)
char Application_interpretIncomingChar(char);

// Generic circular increment function
uint32_t CircularIncrement(uint32_t value, uint32_t maximum);

// The general states for the FSM
void ExplorerFSM(Application* app_p, HAL* hal_p, GFX* gfx_p, char rx);
//sets up the different graphic screens (ie splash screen, map, game over screen)
void SplashScreenSetup(GFX* gfx_p);
void GamePlayScreenSetup(Application* app_p, GFX* gfx_p);
void GameOverScreenSetup(GFX* gfx_p, Application* app_p);

//merges strings together with characters and integers
void PrintMergedStringWithInt(GFX* gfx_p, char* string, int toMerge, int y, int x);
void PrintMergedStringWithIntDoubleDigit(GFX* gfx_p, char* string, int toMerge, int y, int x);
void PrintMergedStringWithChar(GFX* gfx_p, char* string, char toMerge, int y, int x);

//moves through different possible positions of the explorer FSM
void moveExplorerFSM(Application* app_p, GFX* gfx_p, _explorerFSMstate directToMove);

//move the explorer graphic on the display
void moveExplorerTo(Application* app_p, GFX* gfx_p, _explorerFSMstate directToMove);

//updates the text values on the screen as well as in the application (ie energy, level, ect)
void UpdateMoveTo(Application* app_p, GFX* gfx_p);
void UpdateEnergy(Application* app_p, GFX* gfx_p, int newValue);
void UpdateExp(Application* app_p, GFX* gfx_p, int newValue);
void UpdateLevel(Application* app_p, GFX* gfx_p);
void UpdateToNext(Application* app_p, GFX* gfx_p);
#endif /* APPLICATION_H_ */
