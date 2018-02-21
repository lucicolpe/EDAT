#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

char* findmaxorder(){
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char maxorder[512];
  char query[512];
  int order=0;
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return NULL;
  }
  
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  /*Creamos el id para la oferta*/
  sprintf(query, "select  MAX(cast(\"order\" as integer)) from ventas");

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  
  SQLBindCol(stmt, 1, SQL_C_CHAR, maxorder, sizeof(maxorder), NULL);
  
  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    ret=SQLGetData(stmt, 1, SQL_C_CHAR, maxorder, sizeof(maxorder), NULL);
  }

  order=atoi(maxorder);
  order++;
  maxorder[0]='\0';
  sprintf(maxorder, "%d", order);

  SQLCloseCursor(stmt);

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return NULL;
  }

  return maxorder;

}

int insertcompra (char* order, char* screenname, char* isbn) {
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char id[512];
  char query[512];
  char order_cad[500];
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return 1;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

  sprintf(order_cad, "%s", order); 
  
  /*Buscamos el id del usuario que realiza la compra*/
  sprintf(query, "select idusuario from usuario where screenname = '%s'",screenname);
  
  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_CHAR, id, sizeof(id), NULL);
	
  /* Loop through the rows in the result-set */
 while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {  	
	ret=SQLGetData(stmt, 1, SQL_C_CHAR, id, sizeof(id), NULL);
	
  }

  SQLCloseCursor(stmt);
  
  /*Insertamos la compra*/
  sprintf(query, "INSERT INTO ventas(\"order\",uid,isbn,fecha) select '%s', '%s', '%s', null", order_cad,id,isbn);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

	SQLCloseCursor(stmt);
  
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return 1;
  }

  return 0;

}

int printprecio(char*isbn){
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  SQLREAL precio;
  int z=0;
  char query[512];
  int descuento=0;
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return 1;
  }

  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  
  /*Buscamos el precio del libro*/
  sprintf(query, "select cast (cast (precio as money) as numeric) from edicion where isbn = '%s'",isbn);
  
  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_FLOAT, &precio, sizeof(precio), NULL);
  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {  	
	  ret=SQLGetData(stmt, 1, SQL_C_FLOAT, &precio, sizeof(precio), NULL);
  }

  SQLCloseCursor(stmt);

  /*Miramos si el libro está asociado a alguna oferta*/
  sprintf(query, "select idoferta from edicion, tiene, libro where tiene.isbn=edicion.isbn AND tiene.libro=libro.titulo AND  edicion.isbn='%s'",isbn);
  
  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  SQLBindCol(stmt, 1, SQL_INTEGER, &z, sizeof(z), NULL);
  /* Loop through the rows in the result-set */
  if (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
	  ret=SQLGetData(stmt, 1, SQL_INTEGER, &z, sizeof(z), NULL);
  }
  else printf("El libro con ISBN=%s no tiene oferta\n", isbn);

  SQLCloseCursor(stmt);
  
  if(z!=0){
	  /*Buscamos el descuento que tiene la oferta*/
	  sprintf(query, "select descuento from oferta where idoferta='%d'",z);
  	
  	ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  	SQLBindCol(stmt, 1, SQL_INTEGER, &descuento, sizeof(descuento), NULL);

  	/* Loop through the rows in the result-set */
 	  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
		  ret=SQLGetData(stmt, 1, SQL_INTEGER, &descuento, sizeof(descuento), NULL);
		  precio=precio-((precio*descuento)/100);
		  printf("Precio del libro con ISBN=%s es %.2f euros\n", isbn, precio); 
  	}
	
  }
  /*Aplicamos el 10% de descuento que tienen usuarios fidelizados*/
  else{
	  precio=precio-((precio*10)/100);
	  printf("Precio del libro con ISBN=%s es %.2f euros\n", isbn, precio);
  }

	SQLCloseCursor(stmt);

  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return 1;
  }

  return 0;

}

int main(int argc, char **argv) {
  int i=0;
  char* order;
  
  if (argc<2) {
    printf("usage: compra <screenname> <isbn> <isbn> .... <isbn> \n");
    return 1;
  }
  
  order=findmaxorder();
  
  for(i=2;i<argc;i++){
    insertcompra(order, argv[1], argv[i]);
    printprecio(argv[i]);
  }
  
  return 0;
}
