#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "table.h"
#include "../type/type.h"

struct table_ {
    FILE* file;
    int ncols;
    type_t* types;
    int first_pos;
    int last_pos;
};

void table_create(char* path, int ncols, type_t* types) {
    FILE *f;
    if(path==NULL||ncols<0||types==NULL)
        return;
    f =fopen(path,"w");
    if(f==NULL){
        return;
    }
    fwrite(&ncols,sizeof(int),1,f);
    fwrite(types,sizeof(type_t),ncols,f);
    fclose(f);
    return;
}

table_t* table_open(char* path) {
    table_t* table;
    if(path==NULL)
	return NULL;
    table=(table_t *)malloc(sizeof(table_t));
    if(table==NULL)
        return NULL;
    table->file=fopen(path, "r+");
    if(table->file==NULL){
	free(table);
	return NULL;
    }
    fread(&table->ncols,sizeof(int),1,table->file);
    table->types=(type_t*)malloc(table->ncols*sizeof(type_t));
    if(table->types==NULL){
	free(table);
	return NULL;
    }
    fread(table->types,sizeof(type_t),table->ncols,table->file);
    table->first_pos=ftell(table->file);
    fseek(table->file,0, SEEK_END);
    table->last_pos=ftell(table->file);
    return table;
}

void table_close(table_t* table) {
    if(table !=NULL){
        fclose(table->file);
        free(table->types);
        free(table);
    }
}

int table_ncols(table_t* table) {
    if(table == NULL)
        return -1;
    return table->ncols;
}

type_t* table_types(table_t* table) {
    if(table == NULL)
        return NULL;
    return table->types;
}

long table_first_pos(table_t* table) {
    if(table == NULL)
        return -1;
    return table->first_pos;
}

long table_last_pos(table_t* table) {
    if(table == NULL)
        return -1;
    return table->last_pos;
}
/*te lee desde una determinada posicion*/
record_t* table_read_record(table_t* table, long pos) {
    size_t size;
    long next;
    int i;
    void ** values;
    record_t * record;
    if(table ==NULL|| table->last_pos == pos)
        return NULL;
    fseek(table->file, pos, SEEK_SET);
    values=(void**)malloc(table->ncols*sizeof(void*));
    if(values==NULL)
        return NULL;
    for(i=0;i<table->ncols;i++){
        fread(&size,sizeof(size_t),1,table->file);
        values[i]=(void*)malloc(size);
        if (values[i] == NULL){
			for ( ; i >= 0; i--)
				free (values[i]);
			free (values);
			return NULL;
		}
        fread(values[i], size, 1, table->file);
    }
    next=ftell(table->file);
    record=record_create(values, table->ncols, next);
    if(record==NULL){
        for(i=0;i<table->ncols;i++)
            free(values[i]);
        free(values);
        return NULL;
    }
    return record;
    
}

void table_insert_record(table_t* table, void** values) {
    int i;
    size_t len;
	if (table == NULL || values == NULL) return;
	fseek(table->file,table->last_pos, SEEK_SET);
    for(i=0;i<table->ncols;i++){
        len=value_length(table->types[i], values[i]);
        fwrite(&len,sizeof(size_t),1,table->file);
        fwrite(values[i], len, 1, table->file);
    }
    table->last_pos=ftell(table->file);
    return;
}
