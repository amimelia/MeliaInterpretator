/* this is file which initializes and  starts interpreter */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "reader.h"
#include "evaluator.h"
#include "environment.h"

#define VECTOR_INIT_ALLOCATION 300
#define EXIT_COMMAND "exit\n"
#define HELP_COMMAND "help\n"
#define GARBAGE_COMMAND "gb()\n"


void init_environment();
void start_interpreter();
void dispose_environment();
int notSystemCommand(char * commandResult);

environment *global_env;//where global variables and functions are saved 
vector *memory;//where every list node preserves until gb collector deletes them

int main(){
	init_environment();
	start_interpreter();
	dispose_environment();

	return 0;
}

/* initializes environment. gets ready memory and envaironment variables
 * writes greeting messages  */
void init_environment(){
	printf("Welcome to scheme interpeter. have a good time! \n");
	
	global_env = malloc(sizeof(environment));
	EnvironmentNew(global_env, NULL);
	
	memory = malloc(sizeof(vector));
	VectorNew(memory, sizeof(listNode *), freeListNode, VECTOR_INIT_ALLOCATION);
 	
 	//loadDefaultFunctions();
	printf("We are ready to start (type exit() if you want to quit or help()) \n \n \n");
}

/* deleteas all allocated space with memory 
 * (doing this means we have finished working) with interpreter */
void dispose_environment(){
	EnvironmentDispose(global_env);
	free(global_env);

	VectorDispose(memory);
	free(memory);
}

/* starts read/eval/write loop of interpreter */
void start_interpreter(){
	
	char comm[MAX_COMMAND_LENGTH];
	strcpy(comm, "");//init comm to nothing

	while (strcmp(comm, EXIT_COMMAND)){
		readCommand(comm);
		if (notSystemCommand(comm)){
			listNode * curCommand = parseCommand(comm, memory);	
			listNode * commandResult = evalCommand(curCommand, global_env, memory);
			
			if (commandResult == NULL)
				continue;//error happend during parsing
			printf(">: ");
			writeResult(commandResult);
			printf("\n");

		}
	}
}

void gb();
/* returns 1 if given command is not system command like help/exit or 
 * something else returs 0 otherwie  */
int notSystemCommand(char * command){

	if (!strcmp(command, EXIT_COMMAND)){
		//user wants to quit interpreter 
		return 0;
	}else if (!strcmp(command, HELP_COMMAND)){
		//user wants to seee help 
		//displayHelp();
		return 0; 
	}else if (!strcmp(command, GARBAGE_COMMAND)){
		//user wants to run garbage collector
		gb();
		return 0; 
	}
	return 1;
}

/************************************************/
/************garbage collector functions*********/
/************************************************/

/* markes given element as in use */
void mapfn(void *elemAddr, void *auxData){
	listNode * elem = *((listNode **)elemAddr);
   	elem->inUse = 1;
}

/* compares given 2 int numbers */
int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

/* unmarkes nodes which are still in use */
void unMarkGoodNodes(){
	HashSetMap(global_env->env, mapfn, NULL);
}

/* marks all nodes as garbage */
void markAllAsGarbage(){
   int vecSize = VectorLength(memory);
   int i = 0;
   for (; i < vecSize; i++){
   		listNode * elem = *((listNode **)VectorNth(memory, i));
   		elem->inUse = 0;
   }
}

/* deletes nodes which are confirmed as garbage */
void deleteGarbage(){
   int vecSize = VectorLength(memory);
   int i = 0;
   int delNum = 0;
   int del[1000000];
   for (; i < vecSize; i++){
   		listNode * elem = *((listNode **)VectorNth(memory, i));
   		if (elem->inUse == 0){
   			del[delNum] = i;
   			delNum++;
   		}
   }

   qsort(del, delNum, sizeof(int), compare);
   for (i = 0; i < delNum; i++)
      	VectorDelete(memory, del[i] - i);

   printf("GB() finished working. deleted nodes num = %d \n", delNum);
}

/* runs garbage collector */
void gb(){
	printf("garbage Collector lunched !! \n");
	markAllAsGarbage();
	unMarkGoodNodes();
	deleteGarbage();
	
}
