#include "operation.h"

/* OFFSET OPERATION: operation whose result all but the first N results of another source operation */

typedef struct {
    operation_t* suboperation;
	int offset;
	int n;
} operation_offset_args_t;

void operation_offset_reset(void* vargs) {
    operation_offset_args_t* args = vargs;
	
	operation_reset(args->suboperation);
	args->offset = -1;
}

int operation_offset_next(void* vargs) {
    operation_offset_args_t* args = vargs;
	operation_t* suboperation = args->suboperation;
	int i;
	
	if(args->offset == -1)  {
		args->offset=0;
		for(i=0; i<args->n; i++){
			if (!operation_next(suboperation))
				return 0;
		}
	}

	return operation_next(suboperation);
}

void* operation_offset_get(int col, void* vargs) {
    operation_offset_args_t* args = vargs;
	operation_t* suboperation = args->suboperation;

	return operation_get(col, suboperation);
}

void operation_offset_close(void* vargs) {
    operation_offset_args_t* args = vargs;

	operation_close(args->suboperation);
    free(args);
}

operation_t* operation_offset_create(operation_t* suboperation, int offset) {
    operation_t* operation;
	operation_offset_args_t* args;

	args = malloc(sizeof(operation_offset_args_t));
	args->suboperation = suboperation; 
	args->offset = -1;
	args->n = offset;

	operation = malloc(sizeof (operation_t));
    operation->args = args;
    operation->reset = operation_offset_reset;
    operation->next = operation_offset_next;
    operation->get = operation_offset_get;
    operation->close = operation_offset_close;
    operation->ncols = suboperation->ncols;
    operation->types = malloc(operation->ncols * sizeof (type_t));
    memcpy(operation->types, suboperation->types, operation->ncols * sizeof (type_t));

    return operation;
}

