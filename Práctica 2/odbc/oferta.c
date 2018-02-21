#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int findid(){
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char query[512];
  int id=0;
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }
  
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Creamos el id para la oferta*/
  sprintf(query, "select  MAX(idoferta) from oferta");

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  
  SQLBindCol(stmt, 1, SQL_C_CHAR, &id, sizeof(id), NULL);
  
  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    ret=SQLGetData(stmt, 1, SQL_INTEGER, &id, sizeof(id), NULL);
  }

  id=id+1;

  SQLCloseCursor(stmt);

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

  return id;

}

int insertofer (int id, int descuento, char* de, char* a) {
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char query[512];
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }
  
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Insertamos la oferta*/
  sprintf(query, "INSERT INTO oferta(inicio,fin,idoferta,descuento) select '%s', '%s', %d, %d",de, a, id, descuento);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

	SQLCloseCursor(stmt);
  
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}

int insertoferisbn(int id, char* isbn){
	SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char query[512];
  char titulo[512];
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }
  
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Buscamos el titulo del libro*/
  sprintf(query, "select titulo from libro, tiene, edicion where libro.titulo=tiene.libro AND tiene.isbn=edicion.isbn AND edicion.isbn='%s'",isbn);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_CHAR, titulo, sizeof(titulo), NULL);
  
  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    ret=SQLGetData(stmt, 1, SQL_C_CHAR, titulo, sizeof(titulo), NULL);
  }

  SQLCloseCursor(stmt);

  /*Insertamos el id de la oferta en libro*/	
  sprintf(query, "UPDATE libro SET idoferta='%d'where titulo='%s'",id, titulo);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

	SQLCloseCursor(stmt);

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}



int main(int argc, char **argv) {
  int i=0;
  int descuento;
  int id;
  
  if (argc<2) {
    printf("usage: oferta <descuento> <de> <a> <isbn> <isbn> .... <isbn> \n");
    return 1;
  }

  id=findid();
  descuento=atoi(argv[1]);
  insertofer(id, descuento, argv[2], argv[3]);
  
  for(i=4;i<argc;i++){
    insertoferisbn(id, argv[i]);
  }
  
  return 0;
}
