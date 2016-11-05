#ifndef __evaluator__
#define __evaluator__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environment.h"

/* File: evaluator.h
 * ------------------
 * Defines the main functions for evaluation scheme commands
 * command is given as list and result is returned as list too
 */


/* evaluates given command @command. in an given environment @working_env  
 * *memory is place where all new listNodes are created */
listNode *evalCommand(listNode *command, environment *working_env, vector *memory);

/* checks if given command @command is runnable. writes error 
 * if itsnot end returns false else returns true. */
size_t isRunnable(listNode *command);


/* checks if given command is system command 
 * (command) which is not defined but hard coded like car cdr 
 * define and so on. if it is, we evaluate it return its value 
 * as result and return 1 in @isSysComm so user know it was system Command
 * if error occured during evaluation null can be returned */
listNode * isSystemCommand(listNode *command, environment *working_env, vector *memory, int *isSysComm);

/* returns pointer to definition of lambda interpretation of function
 * which should be run in @command. if it's not lambda we shearch for 
 * given function name in entire enviorenment  */
listNode *getLambdaDefinition(listNode *command, environment *env);

/* returns list of names of paramethers given 
 * @lambdaDef lambda function is getting as params */
listNode * getParamNames(listNode * lambdaDef);

/* initializes variables of lambda function in enviorenment @env with names 
 * from @lParamNames and values from  @lParamVals  */
int initLambdaParams(listNode *lParamNames, listNode *lparamVals, environment *env, vector *memory);

/* runs lambda function with definition @lambdaDef, in enviorenment @envForLambda */
listNode * runLambdaFunction(listNode *lambdaDef, environment *envForLambda, vector *memory);

/* evaluates given single element, if its string or number 
 * just copies it in heap memory elese we add '<- at start 
 * so we know its immutable string */
void evalSingleElem(void *elemAddr, listNode **node, environment *env, vector *memory);

/* returns node containint information of paramether of given name @name
 * from envioroument e */
listNode *getDefinitionByName(char * name, environment *e);

/* defines given paramether in given environment. with given name @name */
int defineParam(char * name, listNode * node, environment *e);

/* returns 1 if elem is number else 0 if is number it returns in @num*/
int isNumber(void * elem, double * num);

/* returns number of elements for given list */
int countElems(listNode *list);
#endif