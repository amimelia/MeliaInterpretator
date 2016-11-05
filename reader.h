#ifndef __reader__
#define __reader__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environment.h"

/* File: reader.h
 * ------------------
 * Defines the main file which defines functions for reading command from user
 * and processing them if they are system files, parsing it in list, and so on.
 */


//const int MAX_COMMAND_LENGTH = 1024;
#define MAX_COMMAND_LENGTH 1024

/* reads single line from user (checks if its some kind of syntax for how to read)
 * next line or something like it (read help for more options) performs reading actions
 * while command is not confirmed and returns it */
void readCommand(char *command);

/* checks if given command is shechema command
 * or some kind of request for reading from file  */
int isSchemeCommand(char * comm);

/* parses given scheme command in list and returns it as pointer to
 * very start of that list */
listNode * parseCommand(char *command, vector *memory);

/* returns firt node of parced list from char ** command 
 * command is pointer on pointer of char which is start of list "(" */
listNode *getListFromCommand(char **command, vector *memory);

/* perfors check on syntax if "(" and ")" are correclty positioned 
 * in given command, if there is some kind of error writes it and 
 * returns 1 else returns 0 */
size_t isSyntaxWrong(char * command);

/* checks if given string is list operation it means that it can be
 * parced in list. (should be starting with "(") if it doesnot, that means 
 * user may have submitted single number or symbol like it. unless its it's 
 * sequence of numbers we write scheme error message else we just output first
 * element i dont know why this case is not error message.  */
size_t notListCommand(char * command);

/* moves (*command) <- this pointer, makes him to 
 * skip every delimeter like " " "\n" or so on so we dont 
 * reed them */
void skipDelimethers(char **command);

/* returns copy of first token from command
 * this copy should be placed in heap memory */
void * getDataValueFrom(char **command);

/* gets first token from command and writes it in placeToWrite */
void getFirstToken(char * command, char *placeToWrite);

/* writes resultList on console  */
void writeResult(listNode *commandResult);

/* recursivly writes list elements after "(" is written */
void writeListResult(listNode *commandResult);
#endif