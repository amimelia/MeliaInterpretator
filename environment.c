#include "hashset.h"
#include "environment.h"
#include <stdlib.h>
#include <string.h>


/* File: hashtable.c
 * ------------------
 * Defines functions for initializing environment.
 */

//defines how many buckes in hashSet can be
const int NumBucketsForHashSet = 5381;


/******************************************/
/***** functions to init env *************/
/*****************************************/

/* initializes new environment, creates hashSet for its variables with all
 * free, compare and hash functions already defined */
void EnvironmentNew(environment *e, void *prev_env){
	e->prev = prev_env;
	e->env = malloc(sizeof(hashset));
	HashSetNew(e->env, sizeof(hashSetNode *), NumBucketsForHashSet, hashHashSetNode, compareHashSetNodes, FreeHashSetNode);
}

/* deletes all space accociated with environment */
void EnvironmentDispose(environment *e){
	HashSetDispose(e->env);
	free(e->env);
}

/************************************************/
/***** functions for hashSet in env *************/
/************************************************/

/* hashses given (hashSetNode *) wchich is located in @s1. */
int hashHashSetNode(const void *s1, int numBuckets){
	hashSetNode * toHash = *(hashSetNode **) s1;
	return StringHash(toHash->var_name, numBuckets);
} 

/* comares given 2 hashSetNode**  */
int compareHashSetNodes(const void *s1, const void *s2){
	hashSetNode * fa = *(hashSetNode **) s1;
	hashSetNode * sa = *(hashSetNode **) s2;
	return strcmp(fa->var_name, sa->var_name);
}

/* we need to free only place accociated with hashSetNode, we dont free
 * content hashSetNode contains because this information may be shared in other
   hashSetNodes too.  */
void FreeHashSetNode(void *elemAddr){
	free((*(hashSetNode **) elemAddr)->var_name);
	free(*(hashSetNode **) elemAddr);
}

/* function for hashing in hashTable */
int StringHash(char *str, int numBuckets){
    int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return (hash % numBuckets);
}


/**********************************************/
/***** functions to init listNode *************/
/**********************************************/

/* this function initializes list node @node setting its previous and next elements
 * setting ist value copy of @val and puts it in @memory too every created node is 
 * added to memory of listNodes and is preserved there untill garbage collector removes it 
 * 
 * @isList determines if given node is start of new list or just another node of list. if it's start of 
 * new list val should contain pointer of pointer on that list. otherwise it should containt pointer to 
 * string value of value of node   */
void initListNode(listNode * node, void *prev, void *next, vector *memory){
	node->prev = prev;
	node->next = next;
	node->inUse = 0;
	node->val = NULL;
	node->isList = 0;
	VectorAppend(memory, &node);//adding to memory
}

/* this function is called only when gb() decides its time to remove 
 * elementfrom memory;  */
void freeListNode(void * node){
	listNode * lnode = *(listNode **)node;
	
	if (!lnode->isList)
	  free(lnode->val);
	
	free(lnode);
}