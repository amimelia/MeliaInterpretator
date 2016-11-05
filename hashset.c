#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* initializes space for array of vectors and initializes each vector in sequence */
static void InitSpaceForHashs(hashset *h){
    h->hashs = (vector *) malloc(h->numBuckets * sizeof(vector));

    for (int i = 0; i < h->numBuckets; i++){//init all vectors
        VectorNew(h->hashs + i, h->elemSize, h->freefn, 10);
    }
}

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
    //checking conditionals for exit
    assert(elemSize > 0);
    assert(numBuckets >0);
    assert(hashfn != NULL);
    assert(comparefn !=NULL);
 
    //assigning srating values

    h->elemSize = elemSize;
    h->numBuckets = numBuckets;
    h->comparefn = comparefn;
    h->freefn = freefn;
    h->hashfn = hashfn;
    h->logicalSize = 0;

    //getting enought space for numbulckets vectors
    InitSpaceForHashs(h);
    assert(h->hashs != NULL);
}

void HashSetDispose(hashset *h)
{
    if (h->freefn != NULL){
        //if free function is not null goes over all vectors and disposes them
        for (int i = 0; i < h->numBuckets; i++){
            VectorDispose(h->hashs + i);
        }
    }

    free(h->hashs);//free dinamicly allocated space
}

int HashSetCount(const hashset *h)
{
    return h->logicalSize;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
    for (int i = 0; i < h->numBuckets; i++){
        //iterates over all vectors and lunchs map method
        VectorMap(h->hashs + i, mapfn, auxData);
    }
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
    int hushN = h->hashfn(elemAddr, h->numBuckets);//get vector num from hashfn function
    int foundIndex = VectorSearch(h->hashs + hushN, elemAddr, h->comparefn, 0, false);//search in this vector
    if (foundIndex == -1)
    {
        //this item was not in hasshet so we appent it on vector
        VectorAppend(h->hashs + hushN, elemAddr);
	h->logicalSize++;
    }else
    {
        //this item was in hashset at position @foundIndex
        VectorReplace(h->hashs + hushN, elemAddr, foundIndex);
    }
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{

    int hushN = h->hashfn(elemAddr, h->numBuckets);//get vector num from hashfn function
    int foundIndex = VectorSearch(h->hashs + hushN, elemAddr, h->comparefn, 0, false);//search in this vector
    if (foundIndex == -1) return NULL;

    //case item found we should return pointer on it
    return VectorNth(h->hashs + hushN, foundIndex);
}
