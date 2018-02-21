#include "operation.h"

/* LIMIT OPERATION: operation whose result is the first N results of another source operation */

typedef struct {
    operation_t* suboperation;
    int limit;
	int n;
} operation_limit_args_t;

void operation_limit_reset(void* vargs) {
    operation_limit_args_t* args = vargs;
	
	operation_reset(args->suboperation);
	args->limit=0;
}

int operation_limit_next(void* vargs) {
    operation_limit_args_t* args = vargs;
	operation_t* suboperation = args->suboperation;

	if(args->limit < args->n) {
		args->limit++;
		return operation_next(suboperation);
	}
	return 0;
}

void* operation_limit_get(int col, void* vargs) {
    operation_limit_args_t* args = vargs;
	operation_t* suboperation = args->suboperation;

	return operation_get(col, suboperation);
}

void operation_limit_close(void* vargs) {
    operation_limit_args_t* args = vargs;

	operation_close(args->suboperation);
    free(args);
}

operation_t* operation_limit_create(operation_t* suboperation, int limit) {
    operation_t* operation;
	operation_limit_args_t* args;

	args = malloc(sizeof(operation_limit_args_t));
	args->suboperation = suboperation; 
	args->limit = 0;
	args->n = limit;

	operation = malloc(sizeof (operation_t));
    operation->args = args;
    operation->reset = operation_limit_reset;
    operation->next = operation_limit_next;
    operation->get = operation_limit_get;
    operation->close = operation_limit_close;
    operation->ncols = suboperation->ncols;
    operation->types = malloc(operation->ncols * sizeof (type_t));
    memcpy(operation->types, suboperation->types, operation->ncols * sizeof (type_t));

    return operation;
}


