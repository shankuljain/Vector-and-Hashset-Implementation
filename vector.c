

#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

#define nthelemadd(i) ((char *)v->elem + (i)*v->elemsize) // only internal purpose use to avoid assert fail

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(elemSize >0);
    v->alloclength = initialAllocation;
    v->logilength = 0;
    v->elemsize = elemSize;
    v->freefn = freeFn;
    v->elem = malloc(initialAllocation*elemSize);
    assert(v->elem != NULL);
    
}

void VectorDispose(vector *v){
    if (v->freefn != NULL) {
        for (int i =0; i < v->logilength; i++) {
            v->freefn( nthelemadd(i));
        }
    }
    free(v->elem);
}

int VectorLength(const vector *v)
{ return v->logilength; }

void *VectorNth(const vector *v, int position){
    assert(position >=0 && position < v->logilength);
    return nthelemadd(position);
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position < v->logilength && position >= 0);
    void *temp = nthelemadd(position);
    if (v->freefn != NULL) {
        v->freefn(temp);
    }
    memcpy(temp, elemAddr, v->elemsize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position <= v->logilength && position >= 0);
    
    if (v->logilength == v->alloclength) {
        v->alloclength *= 2;
        v->elem = realloc(v->elem, v->alloclength * v->elemsize);
        assert(v->elem != NULL);
        
    }
    
    size_t size = (v->logilength++ - position) * v->elemsize;
    void *positiionAddress = nthelemadd(position);
    memmove(nthelemadd(position+1), positiionAddress, size);
    memcpy(positiionAddress, elemAddr, v->elemsize);
    
}

void VectorAppend(vector *v, const void *elemAddr){
    if (v->logilength == v->alloclength) {
        v->alloclength *= 2;
        v->elem = realloc(v->elem, v->alloclength * v->elemsize);
        assert(v->elem != NULL);
        
    }
    memcpy(nthelemadd(v->logilength++),elemAddr,v->elemsize);
}

void VectorDelete(vector *v, int position){
    assert(position >= 0 && position < v->logilength);
    void *temp = nthelemadd(position);
    
    if (v->freefn != NULL) {
        v->freefn(temp);
    }
    
    int size = (--v->logilength - position)*v->elemsize;

    memmove(temp, nthelemadd(position+1), size);
    
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
    qsort(v->elem, v->logilength, v->elemsize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn != NULL);
    for (int i =0; i<v->logilength; i++) {
        mapFn(nthelemadd(i),auxData);
    }
}

//static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
    void *add;
    size_t nelem = v->logilength - startIndex;
    if (isSorted) {
        add = bsearch(key,nthelemadd(startIndex), nelem , v->elemsize, searchFn);
    }else{
        add = lfind(key, nthelemadd(startIndex), &nelem , v->elemsize, searchFn);
    }
    
    if(add != NULL){
        return (int) ((char *)add-nthelemadd(startIndex))/v->elemsize;
    }
    
    return -1;
}
