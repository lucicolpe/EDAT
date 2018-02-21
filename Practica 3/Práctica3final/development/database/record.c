#include <stdio.h>
#include "record.h"

struct record_ {
    void**values; /* valores */
    int ncols; /* numero de columnas */
    long next; /* posicion del siguiente registro */
};

record_t* record_create(void** values, int ncols, long next) {
    record_t * record= (record_t*)malloc(sizeof(record_t));
    if(record==NULL)
        return NULL;
    record->values=values;
    record->ncols=ncols;
    record->next=next;
    return record;
}

void* record_get(record_t* record, int n) {
    if(record==NULL||n<0||n>record->ncols)
        return NULL;
    return record->values[n];
}


long record_next(record_t* record) {
    if(record==NULL)
        return -1;
    return record->next;
}

void record_free(record_t* record) {
    int i;
    if(record!=NULL){
        for(i=0;i<record->ncols;i++)
            free(record->values[i]);
        free(record->values);
        free(record);
    }
}
