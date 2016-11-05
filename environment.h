
#ifndef __environment_
#define __environment_

#include "hashset.h"
#include <stddef.h>

/* File: hashtable.h
 * ------------------
 * Defines the main structures we use in our scheme
 * interpreter. 
 */

/* structure of environment list  */
 typedef struct {
  void *prev;//pointer to previous environment
  hashset *env;//enviornment where variables are saved
} environment;

/* main structure of list, everything in our interpreter is lists. */ 
typedef struct {
  void *prev;//pointer to prev element
  void *next;//pointer to next elem
  void *val;// pointer to value of node
  size_t isList;//integere determining its pointer to next list
  				//or just some node  
  size_t inUse;//integer determining if node should be deleted 
   			   //when garbage collection comes 
} listNode;

/* this structure only helps to make hashSet work as map */
typedef struct {
  char *var_name;// variable name we store in hashset 
  void *var_value;// pointer to that variable 
} hashSetNode;



/*initializes new environment @e and sets its prev as @prev_env */
void EnvironmentNew(environment *e, void *prev_env);
void EnvironmentDispose(environment *e);
int hashHashSetNode(const void *s1, int numBuckets);

/* comares given 2 hashSetNode**  */
int compareHashSetNodes(const void *s1, const void *s2);

/* we need to free only place accociated with hashSetNode, we dont free
 * content hashSetNode contains because this information may be shared in other
   hashSetNodes too.  */
void FreeHashSetNode(void *elemAddr);

/* hushes single c_type string based on numBuchets  */
int StringHash(char *str, int numBuckets);

/* this function is called only when gb() decides its time to remove 
 * elementfrom memory;  */
void freeListNode(void * node);

/* this function initializes list node @node setting its previous and next elements
 * setting ist value copy of @val and puts it in @memory too every created node is 
 * added to memory of listNodes and is preserved there untill garbage collector removes it */
 
void initListNode(listNode * node, void *prev, void *next, vector *memory);

#endif