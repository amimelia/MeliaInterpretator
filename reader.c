#include <assert.h>

#include "reader.h"
/* File: reader.c
 * ------------------
 * Defines the main file which defines functions for reading command from user
 * and processing them if they are system files 
 */

int lineID = 0;
#define  SINGLE_TOKEN_SIZE 1024//max size single token can be 

/* reads command from user if its some kind of syntax to read more commands next line 
 * or read from file, performs action and then returns it in @command */
void readCommand(char *command){

	printf("Line N:%d ", lineID);
	
	char in[MAX_COMMAND_LENGTH];
	fgets(in, MAX_COMMAND_LENGTH, stdin);
	if (isSchemeCommand(in)){
		lineID++;
		strcpy(command, in);
		return;
	}
	//case when given command is just request for more

}

/* checks if given command is shechema command
 * or some kind of request for reading from file  */
int isSchemeCommand(char * comm){
	return 1;
}

/****************************************************/
/********** parsing given commands in list **********/
/****************************************************/

/* parses given scheme command in list and returns it as pointer to
 * very start of that list */
listNode * parseCommand(char *command, vector *memory){
	if (isSyntaxWrong(command)){
		return NULL;
	}

	if (notListCommand(command)){
		listNode * start_node = malloc(sizeof(listNode));
		initListNode(start_node, NULL, NULL, memory);
		char c[1000];
		getFirstToken(command, c);
		start_node->val = strdup(c);
		return start_node;
	}

	listNode * startNode = malloc(sizeof(listNode));
	initListNode(startNode, NULL, NULL, memory);
	startNode->isList = 1;
	startNode->val = getListFromCommand(&command, memory);

	
	return startNode;
}

/* returns firt node of parced list from char ** command 
 * command is pointer on pointer of char which is start of list "(" */
listNode *getListFromCommand(char **command, vector *memory){
	char * curCommand = *command;
	curCommand = curCommand + 1;
	//init start point of list
	listNode * start_node = malloc(sizeof(listNode));
	initListNode(start_node, NULL, NULL, memory);

	listNode * prev_node = start_node;

	skipDelimethers(&curCommand);
	while(*curCommand != ')'){
		//create new end point and change prev end point
		listNode * new_node = malloc(sizeof(listNode));
		initListNode(new_node, NULL, NULL, memory);
		prev_node->next = new_node;
		new_node->prev = prev_node;

		if (*curCommand == '('){
			//start new recursive call
			prev_node->isList = 1;
			prev_node->val = getListFromCommand(&curCommand, memory);

		}else if (*curCommand == '\'' && *(curCommand + 1) == '('){
			//replace ' with (quoute)

		}else{
			//we know it's banch of symbols
			prev_node->isList = 0;
			prev_node->val = getDataValueFrom(&curCommand);
		}
		prev_node = new_node;
		skipDelimethers(&curCommand);
	}

	*command = curCommand + 1;//move cursor from already parced area
	return start_node;
}

/* by counting "(" and ")" checks if given string 
 * accepts syntax requieremnts */
size_t isSyntaxWrong(char * command){
	int deg = 0;

	while (*command != '\0'){
		if (*command == ')'){
			deg--;
		}else if (*command == '('){
			deg++;
		}

		if (deg < 0){//check if wrong syntax
			break;	
		}
		command = command + 1;
	}

	if (deg != 0){
		printf("Syntax Error: parsing scheme command, check all brackets");
		printf(" are correclty placed and run command again. \n\n");
		return 1;
	}

	return 0;
}

/* check if given string can be parced in list or not */
size_t notListCommand(char * command){
	if (*command != '('){
		return 1;
	}
	return 0;
}

/* gets first token from command and writes it in placeToWrite */
void getFirstToken(char *command, char *placeToWrite){
	char delimethers[] = "\t\n() ";
	char * commandCopy = strdup(command);

    char * pch;
	pch = strtok (commandCopy, delimethers);

	assert(pch != NULL);
    if (pch != NULL)
    {
    	strcpy(placeToWrite, pch);
    }

	free(commandCopy);
}

/* moves (*command) <- this pointer, makes him to 
 * skip every delimeter like " " "\n" or so on so we dont 
 * reed them */
void skipDelimethers(char **command){
	char delimethers[] = "\t\n ";
	char * curCommand = *command;

	while (strchr(delimethers, *curCommand) != NULL){
		curCommand = curCommand + 1;
	}

	*command = curCommand;
}

/* returns copy of first token from command
 * this copy should be placed in heap memory and moves 
 * cursor of *(char **)  on end of token  */
void * getDataValueFrom(char **command){
	char delimethers[] = "\t\n()' ";
	char * curCommand = *command;
	
	int count = 1;
	while (strchr(delimethers, *curCommand) == NULL){
		curCommand = curCommand + 1;
		count++;
	}

	void * retValue = malloc(count);
	memcpy(retValue, *command, count - 1);
	
	//we mark last element of copyed data es end of string
	char * lastElem = (char *)retValue + (count - 1);
	*lastElem = '\0'; 

	*command = curCommand;

	return retValue;
}


/****************************************************/
/********** displaying result list as output ********/
/****************************************************/

/* writes result */
void writeResult(listNode *commandResult){
	assert(commandResult != NULL);

	//if (commandResult->next == NULL){
		//case single output
		if (commandResult->isList){
			printf("(");
			writeListResult((listNode *) commandResult->val);
			printf("\n");
		}else{
			//answer is single element
			printf("%s\n", (char *)commandResult->val);
		}
	//}else{
		//result is not single command
	//	printf("\n\nError id: 1000; Unexpected Type of Command! \n\n");
	//}	
}

/* writes list elements after "(" is written */
void writeListResult(listNode *commandResult){
	listNode *curNode = commandResult;
	while (curNode->next != NULL){
		if (curNode->isList){
			printf("(");
			writeListResult((listNode *) curNode->val);
		}else{
			printf("%s", (char *)curNode->val);
			//check if space is needed
			listNode *tmp = curNode->next;
			if ((tmp->next != NULL) || (tmp->val != NULL))
				printf(" ");
		}
		curNode = curNode->next;
	}

	//case when given command can be
	//is single symbols or something like that  
	if (curNode->val == NULL)
		printf(") ");
	else
		printf("\n\nError id: 1001; Unexpected Type of Result! \n\n");
}