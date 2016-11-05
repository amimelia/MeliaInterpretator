#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "environment.h"
#include "evaluator.h"

/* File: evaluator.c
 * ------------------
 * Defines the main functions for evaluation scheme commands
 * command is given as list and result is returned as list too
 */

/* evaluates given command @command. in an environment @working_env all 
 * changes done in enviorenment env not outside of it (unless its system command)
 * @memory is memory of nodes where all lists are stored we use it to create new lists
 I during evaluation */

listNode *evalCommand(listNode *command, environment *working_env, vector *memory){
	//case bad command received
	if (!isRunnable(command))
		return NULL;
	
	//case not list we return first elem
	if (!command->isList ){
		listNode * singleNode = NULL;
		evalSingleElem(command->val, &singleNode, working_env, memory); 
		return singleNode;
	}
	
	//case system command received
	command = (listNode *)(command->val); 
	int isSysComm = 0;
	listNode *sysCommandResult = isSystemCommand(command, working_env, memory, &isSysComm);
	
	if (isSysComm){
		return sysCommandResult;
	}
	

	//case when given command is preDefined or something like that
	listNode * lambdaDef = getLambdaDefinition(command, working_env);
	
	if (lambdaDef == NULL)
		return NULL;

	environment lambdaEnv;//create new enviorenment for lamba function 
	EnvironmentNew(&lambdaEnv, working_env);
	
	if (initLambdaParams(getParamNames(lambdaDef), (listNode *)command->next, &lambdaEnv, memory)){
		listNode * finalResult = runLambdaFunction(lambdaDef, &lambdaEnv, memory);
		EnvironmentDispose(&lambdaEnv);
		return finalResult;
	}
	return NULL;
	
}


/* checks if given command @command is runnable. writes error 
 * if itsnot end returns false else returns true. we chech if 
 * given function list command is list with first argument function 
 * or just single element.  */
size_t isRunnable(listNode *command){
	if (command == NULL){
		printf("wrong type of command !");
		return 0;
	}

	//single command as null
	if (!command->isList && command->val == NULL)
		return 0;

	if (command->isList){
		listNode * nextList = (listNode *)(command->val);

		//if its list we check for lambda function
		if (nextList->isList){	 
			listNode *firstElem = (listNode *)(nextList->val);
			if (!strcmp("lambda", (char *)(firstElem->val))){
				return 1;
			}
			else{
				printf("first argument expected function but got nonLambda list %s", (char *)(firstElem->val));
				return 0;
			}
		}else{
			//shouldnot be startging with ' " or be number
			double dummy;
			if ( *(char *)(nextList->val) == '\'' || 
				isNumber(nextList->val, &dummy) || *(char *)(nextList->val) == '\"'){
				printf("given name: %s is not supported for function name : \n",
						 (char *) nextList->val);
				return 0;
			}
		}
	}

	return 1;
}

/* for given string copies its value in heap and adds ' to it */
void * putQuouteToString(char * elem){
	char * retVal = malloc(strlen(elem) + 2);
	*retVal = '\'';
	strcpy(retVal + 1, elem);
	return retVal;
}

/* evaluates given single element, if its string or number 
 * just copies it in heap memory else it considers first elem 
 * as variable and returns its definition */
void evalSingleElem(void *elemAddr, listNode **node, environment *env, vector *memory){
	char * elem = (char *)elemAddr;
	double dummy;
	if (isNumber(elem, &dummy) || (*elem == '\'') || (*elem == '\"')){
		*node = malloc(sizeof(listNode));
		initListNode(*node, NULL, NULL, memory);
		(*node)->val = strdup(elem);
	}else
		*node = getDefinitionByName(elem, env);
}

/**********************************************/
/****** implimentation of lambda function *****/
/**********************************************/

/* this function checks if given list is lambda function */
int isLambdaFunction(listNode *command){
	assert(command != NULL);
	assert(command->isList);

	if (!strcmp("lambda", (char *)(((listNode *)(command->val))->val))){
		return 1;
	}
	return 0;
} 

/* returns pointer to lambda definition list for given list command 
 * first, function element  */
listNode *getLambdaDefinition(listNode *command, environment *env){
	if (command->isList){
		if (!strcmp((char *)(((listNode *) command->val)->val), "lambda")){
			return ((listNode *) command->val);
		}else {
			return NULL;
		}
	}else{
		//search for lambda function in enviorenment
		listNode * defin = getDefinitionByName((char *)command->val, env);
		if (!isLambdaFunction(defin))
			return NULL;
		return (listNode *)(defin->val);
		//printf("not yet implimented");
	}

	return NULL;
}

/* initializes variables of lambda function in enviorenment @env with names 
 * from @lParamNames and values from  @lParamVals  */
int initLambdaParams(listNode *lParamNames, listNode *lparamVals, environment *env, vector * memory){
	//case wrong paramether names 
	if (lParamNames == NULL){
		return 0;
	}
	assert(lparamVals != NULL);
	lParamNames = (listNode *)(lParamNames->val);

	while (lParamNames->val != NULL){
		
		 if (lparamVals->val == NULL){
			printf("not enought paramethers supplied for function ");
			return 0;
		}

		if (((listNode *)(lParamNames->next))->isList){
			printf("Error: id = 1001! ");
			return 0;
		}

		if (*(char *)lParamNames->val == '.'){
			//case unspecified paramethers number (like .. in c)
			//we convert left params in list and path it as last paramether 
			if (lParamNames->next == NULL){
				printf("Syntax error please write correct command!");
				return 0;
			}
			
			printf("not implimented error");
			return 1;
		} 

		listNode * res = evalCommand(lparamVals, env, memory);
		if (res == NULL)
			return 0;

		defineParam(lParamNames->val, res, env);
		lParamNames = (listNode *) lParamNames->next;
		lparamVals = (listNode *) lparamVals->next;
	}
	return 1;
}

/* this function evaluates lambda functions eval side after its params are all set in 
 * given enviroument and returns it */
listNode * runLambdaFunction(listNode *lambdaDef, environment *envForLambda, vector *memory){
	assert(lambdaDef != NULL);
	listNode *paramsList = (listNode *)(lambdaDef->next);//next element after lambda is params list
	listNode *evalToList = (listNode *)(paramsList->next);//next element after params list is eval list
	return evalCommand(evalToList, envForLambda, memory);;
}

/* returns lambda function paramethers */
listNode * getParamNames(listNode * lambdaDef){
	listNode * params = (listNode *) lambdaDef->next;
	if (!params->isList){
		printf("bad lambda function found!\n");
		return NULL;
	}else{
		return params;
	}
}

/* returns 1 if given char * is number */
int isNumber(void * elem, double * num){
	assert(elem != NULL);
	char * number = (char *) elem;
	double d = atof(number);
	if (d == 0.0){
		if (*number != '0')
			return 0;
	}
    *num = d;
	return 1;
}

/* returns node containing information of paramether of given name @name
 * from envioroument e */
listNode *getDefinitionByName(char * name, environment *e){
	
	hashSetNode searchNode;
	searchNode.var_name = name;
	hashSetNode * search = &searchNode;

	while (e != NULL){
		void * found = HashSetLookup(e->env, &search);
		if (found != NULL){
			return (listNode*)((*((hashSetNode **) found))->var_value); 
		}
		//dont found in cur env; should search in previous
		e = (environment *)(e->prev);
	}
	return NULL;
}

/* defines given paramether in given environment. with given name @name */
int defineParam(char * name, listNode * node, environment *e){
	hashSetNode *insertNode = malloc(sizeof(hashSetNode));
	assert(insertNode != NULL);
	insertNode->var_name = strdup(name);
	insertNode->var_value =  node;
	HashSetEnter(e->env, &insertNode);
	return 1;
}

/**********************************************/
/****** implimentation of system function *****/
/**********************************************/

/* returns number of elements for given list */
int countElems(listNode *list){
	
	if (list->val == NULL && list->next == NULL)
		return 0;
	else if (list->next == NULL)
		return 1;

	int num = 0;
	while (list->next != NULL){
		num++;
		list = (listNode *) list->next;
	}
	return num;
}

/* writes message when wrong number of paramethers found for function @procName */
void printWrongParamNumError(char *procName, int paramsExp, int paramsFound){
	printf("%s function expected %d paramethers but found %d. \n", procName, paramsExp, paramsFound);
}

/* tries to get number from numElem in @num if it sucess then 1 is returned 
 * else wrong number message is written and 0 is returned */
int getNumberIn(double * num, listNode * numElem){
	
	if ((numElem->val == NULL) || (numElem->isList)
	 || (!isNumber((char *)numElem->val, num))){
		return 0;
	}
	return 1;
}



/* this function returns numbers for sum, minus, divide , and so on 
 * opeartions. if numbers extracted sucessfully 1 is returned else 0 */
int getNumbersForOp(listNode *command, environment *env, 
	vector *memory, double *num1, double *num2){
	int elemNum = countElems(command);
	if (elemNum != 3){
		printWrongParamNumError("operation", 2, elemNum - 1);
		return 0;
	}else{
		listNode *firstNum = evalCommand((listNode *) command->next, env, memory);
		if (firstNum == NULL){
			printf("bad argument number 1! for function");
			return 0;
		}

		listNode *secondNum = evalCommand((listNode *)((listNode *) command->next)->next, env, memory);
		if (secondNum == NULL){
			printf("bad argument number 2! for function");
			return 0;
		} 

		if (!getNumberIn(num1, firstNum) || !getNumberIn(num2, secondNum))
			return 0;
		return 1;
	}
	return 1;
}

/* returns copy of given nonList node without prev and next elems */
listNode *getCopyOfNode(listNode *node, vector *memory){
	listNode *new_node = malloc(sizeof(listNode));
	initListNode(new_node, NULL, NULL, memory);
	if (node->isList){
		new_node->isList = 1;
		new_node->val = (listNode *)(node->val);
	}else{
		new_node->val = strdup(node->val);
	}
	return new_node;
}

/* evaluates sum, minus multiple and divide opeartions */
listNode *numbersOperation(listNode *command, environment *env, vector *memory, char op){

	double num1;
	double num2;
	if (!getNumbersForOp(command, env, memory, &num1, &num2))
		return NULL;
	listNode *node = malloc(sizeof(listNode));
	initListNode(node, NULL, NULL, memory);
	
	char numberToString[100];
	if (op == '+')
	   sprintf(numberToString, "%f", (num1 + num2));
	else if (op == '-')
		sprintf(numberToString, "%f", (num1 - num2));
	else if (op == '*')
		sprintf(numberToString, "%f", (num1 * num2));
	else if (op == '/')
		sprintf(numberToString, "%f", (num1 / num2));

	node->val = strdup(numberToString);
	return node;
}

/* evaluates greater, smaller and equal and divide operations */
listNode *numCompOperation(listNode *command, environment *env, vector *memory, char op){

	double num1;
	double num2;
	if (!getNumbersForOp(command, env, memory, &num1, &num2))
		return NULL;
	listNode *node = malloc(sizeof(listNode));
	initListNode(node, NULL, NULL, memory);
	
	int numToWrite = 0;
	if (op == '<'){
		if(num1 < num2)
			numToWrite = 1;
	}
	else if (op == '>'){
		if(num1 > num2)
			numToWrite = 1;
	}
	else if (op == '='){
		if(num1 == num2)
			numToWrite = 1;
	}
	if (numToWrite)
		node->val = strdup("1");
	else 
		node->val = strdup("0");
	return node;
}

/* inplimentation of built in function quoute. it expects 1 element
 * list, and returns it unchanged */
listNode *operationQuoute(listNode *command){
	int elemNum = countElems(command);
	if (elemNum != 2){
		printWrongParamNumError("quoute", 1, elemNum - 1);
		return 0;
	}else{
		listNode * nextList = (listNode *)(command->next);
		if (!nextList->isList){
			printf("quoute function expects list artgumet\n");
			return NULL;
		}
		return nextList;
	}
}

/* inplimentation of built in function car. it expects 1 element which 
 * can be evaluated in list and returns copy of first element of given list */
listNode *carOperation(listNode *command, environment *working_env, vector *memory){
	int elemNum = countElems(command);
	if (elemNum != 2){
		printWrongParamNumError("car", 1, elemNum - 1);
		return 0;
	}else{
		listNode *nextList = (listNode *)(command->next);
		listNode *evalResult = evalCommand(nextList, working_env, memory);
		if (evalResult == NULL) return NULL;
		if (!evalResult->isList){
			printf("wrong 1 argument for function car \n");
			return NULL;
		}
		return getCopyOfNode((listNode *)(evalResult->val), memory);
	}
}

/* inplimentation of built in function cdr. it expects 1 element which 
 * can be evaluated in list and returns it without first element  */
listNode *cdrOperation(listNode *command, environment *working_env, vector *memory){
	int elemNum = countElems(command);
	if (elemNum != 2){
		printWrongParamNumError("cdr", 1, elemNum - 1);
		return 0;
	}else{
		listNode *nextList = (listNode *)(command->next);
		listNode *evalResult = evalCommand(nextList, working_env, memory);
		if (evalResult == NULL) return NULL;
		
		if (!evalResult->isList){
			printf("wrong 1 argument for function car \n");
			return NULL;
		}

		listNode *new_node = malloc(sizeof(listNode));
		initListNode(new_node, NULL, NULL, memory);
		new_node->isList = 1;
		new_node->val = ((listNode *)(evalResult->val))->next;
		return new_node;
	}
}

/* convert to lambda this function convert define command to lambda function so it 
 * will be saved later as lambda function */
listNode * convertToLambda(listNode *command, vector *memory){
	command = (listNode *) command->next;
	listNode * lambdaParams = (listNode *) command->val; 
	listNode * evalTo =  (listNode *) command->next;

	listNode *new_node = malloc(sizeof(listNode));
	initListNode(new_node, NULL, NULL, memory);
	new_node->isList = 1;

	listNode *lambda_node = malloc(sizeof(listNode));
	initListNode(lambda_node, NULL, NULL, memory);
	lambda_node->val = strdup("lambda");

	new_node->val = lambda_node; 

	listNode *lambda_params = malloc(sizeof(listNode));
	initListNode(lambda_params, NULL, NULL, memory);
	lambda_params->isList = 1;
	lambda_params->val = lambdaParams->next;

	lambda_node->next = lambda_params;
	lambda_params->next = evalTo;

	return new_node;
}

/* inplimentation of built in function define. it expects 2 element first one 
 * name of variable and second one its definition */
listNode *operationDefine(listNode *command, environment *working_env, vector *memory){
	int elemNum = countElems(command);
	if (elemNum != 3){
		printWrongParamNumError("define", 2, elemNum - 1);
		return 0;
	}else{
		listNode *nextElem = (listNode *)(command->next);
		listNode *finalElem = (listNode *)(nextElem->next);
		if (nextElem->isList){
			listNode *node = convertToLambda(command, memory);
			defineParam((char *)(((listNode *)(((listNode *)(command->next))->val))->val), node, working_env);
		}else{
			listNode *putToVal = finalElem;//case lambda function
			if (!isLambdaFunction(finalElem))
				putToVal = evalCommand(finalElem, working_env, memory);
			if (putToVal == NULL)
				return NULL;
			defineParam((char *)nextElem->val, putToVal, working_env);
		}
	}
	return NULL;
}

/* evaluates built in if operation */
listNode *operationIf(listNode *command, environment *working_env, vector *memory){
	
	int elemNum = countElems(command);
	if (elemNum != 4){
		printWrongParamNumError("define", 3, elemNum - 1);
		return 0;
	}else{
		listNode *cond = (listNode *) command->next;
		listNode *fEval = (listNode *) cond->next;
		listNode *sEval = (listNode *) fEval->next;
		if (!strcmp(evalCommand(cond, working_env, memory)->val, "1")){
			return evalCommand(fEval, working_env, memory);
		}else{
			return evalCommand(sEval, working_env, memory);
		}
	}
}


/*checks if given command is system command or not */
listNode * isSystemCommand(listNode *command, environment *working_env, 
	vector *memory, int *isSysComm){
	if (command->isList){
		*isSysComm = 0;
		return NULL;
	}
	char * opperation = (char *) command->val;
	*isSysComm = 1;
	listNode * retVal;
	if (!strcmp(opperation, "define")){
		retVal = operationDefine(command, working_env, memory);
	}else if (!strcmp(opperation, "if")){
		retVal = operationIf(command, working_env, memory);
	}else if (!strcmp(opperation, "quoute")){
		retVal = operationQuoute(command);
	}else if (!strcmp(opperation, "set")){

	}else if (!strcmp(opperation, "sum")){
		retVal = numbersOperation(command, working_env, memory, '+');
	}else if (!strcmp(opperation, "minus")){
		retVal = numbersOperation(command, working_env, memory, '-');
	}else if (!strcmp(opperation, "mult")){
		retVal = numbersOperation(command, working_env, memory, '*');
	}else if (!strcmp(opperation, "divide")){
		retVal = numbersOperation(command, working_env, memory, '/');
	}else if (!strcmp(opperation, "bigger")){
		retVal = numCompOperation(command, working_env, memory, '>');
	}else if (!strcmp(opperation, "smaller")){
		retVal = numCompOperation(command, working_env, memory, '<');
	}else if (!strcmp(opperation, "equal")){
		retVal = numCompOperation(command, working_env, memory, '=');
	}else if (!strcmp(opperation, "car")){
		retVal = carOperation(command, working_env, memory);
	}else if (!strcmp(opperation, "cdr")){
		retVal = cdrOperation(command, working_env, memory);
	}else if (!strcmp(opperation, "cons")){

	}else if (!strcmp(opperation, "not")){

	}else if (!strcmp(opperation, "append")){

	}else if (!strcmp(opperation, "apply")){

	}else if (!strcmp(opperation, "begin")){

	}else if (!strcmp(opperation, "list?")){

	}else if (!strcmp(opperation, "null?")){

	}else if (!strcmp(opperation, "number?")){

	}else if (!strcmp(opperation, "string?")){

	}else{
		*isSysComm = 0;
		return NULL;
	}
	return retVal;
}