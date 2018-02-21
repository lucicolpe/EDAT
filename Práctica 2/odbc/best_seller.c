#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int get_best_seller(int n){
  SQLHENV env;
	SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char query[512];
  char x[512];
  int y=0;
  int i=0;

  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  
  /*Buscamos los libros que mas veces se han vendido*/
  sprintf(query, "select titulo, count(*) from libro, edicion, tiene, ventas where edicion.isbn = tiene.isbn and libro.titulo = 	tiene.libro and ventas.isbn=edicion.isbn group by titulo order by count (*) desc limit(%d)", n);
  
  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

	SQLBindCol(stmt, 1, SQL_C_CHAR, x, sizeof(x), NULL);	
	SQLBindCol(stmt, 2, SQL_INTEGER, &y, sizeof(y), NULL);

	for(i=0; i<n; i++){
		if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {  	
			printf("Best seller numero %d: %s ha sido vendido %d veces\n", i+1, x, y);
  	}
	}

	SQLCloseCursor(stmt);

	SQLFreeHandle(SQL_HANDLE_STMT, stmt);

	/* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

	return 0;

}

int main(int argc, char **argv) {
	int n;
  
  if (argc<2) {
    printf("usage: best_seller <n> \n");
    return 1;
  }

	n = atoi(argv[1]);
	get_best_seller(n);

  return 0;
}
