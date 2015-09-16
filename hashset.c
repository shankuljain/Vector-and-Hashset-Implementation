

#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
    assert(elemSize>0 && numBuckets>0);
    assert(hashfn != NULL && comparefn!= NULL);
    
    h->elemsize = elemSize;
    h->numbuckets = numBuckets;
    h->elemcount = 0;
    h->hashfn = hashfn;
    h->cmpfn = comparefn;
    h->freefn = freefn;
    h->buckets = malloc(numBuckets*sizeof(vector));
    
    assert(h->buckets != NULL);
    
    for (int i =0; i<numBuckets; i++) {
        VectorNew(h->buckets+i, elemSize, freefn, 4);
    }
}

void HashSetDispose(hashset *h){
    for (int i=0; i<h->numbuckets; i++) {
        VectorDispose(h->buckets+i);
    }
    free(h->buckets);
}

int HashSetCount(const hashset *h)
{   assert(h != NULL);
    return h->elemcount; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
    assert(h != NULL);
    assert(mapfn != NULL);
    for (int i=0; i<h->numbuckets; i++) {
        VectorMap(h->buckets+i, mapfn, auxData);
    }
}

void HashSetEnter(hashset *h, const void *elemAddr){
    assert(elemAddr != NULL);
    int bucket = h->hashfn(elemAddr, h->numbuckets);
    assert(bucket < h->numbuckets && bucket >=0);
    int index = VectorSearch((h->buckets+bucket), elemAddr, h->cmpfn, 0, false);
    if (index != -1) {
        VectorReplace(h->buckets+bucket, elemAddr, index);
    }else{
        VectorAppend((h->buckets+bucket), elemAddr);
        h->elemcount++;
    }
    
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
    assert(elemAddr != NULL);
    int bucket = h->hashfn(elemAddr, h->numbuckets);
    int index = VectorSearch((h->buckets+bucket), elemAddr, h->cmpfn, 0, false);
    if(index != -1){
        return VectorNth(h->buckets+bucket, index);
    }
    return NULL;
}
