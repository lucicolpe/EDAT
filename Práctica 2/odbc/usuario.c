#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

int insertusu (char *scname, char *fullname) {
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  char y[512];
  char query[512];
  int id;
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }
  
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  
  /*Comprobamos si ya existe un usuario con ese screenname*/
  sprintf(query, "select screenname from usuario where screenname = '%s' AND usuario.borrado = 'FALSE'", scname);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  
  if(SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    printf("Ya existe un usuario con ese screenname \n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
    }
    return -1;
  }
  
  SQLCloseCursor(stmt);
  y[0] = '\0';
  
  /*Creamos el id para el usuario*/
  sprintf(query, "select  MAX(idusuario) from usuario");

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  
  SQLBindCol(stmt, 1, SQL_C_CHAR, y, sizeof(y), NULL);
  
  /* Loop through the rows in the result-set */
  while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
    ret=SQLGetData(stmt, 1, SQL_C_CHAR, y, sizeof(y), NULL);
  }
  id=atoi(y);
  id++;
  y[0] = '\0';
  sprintf(y, "%d", id);
  
  SQLCloseCursor(stmt);
  
  /*Insertamos el nuevo usuario*/
  sprintf(query, "INSERT INTO usuario(idusuario,ccard,fidelizado,screenname,fullname,joindate,expiration,borrado) select '%s', null, TRUE, '%s', '%s', null, null, FALSE",y, scname, fullname);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  
  printf("Se ha añadido el usuario con screenname: %s y fullname: %s \n",scname,fullname);

  SQLCloseCursor(stmt);
  
  SQLFreeHandle(SQL_HANDLE_STMT, stmt);

  /* DISCONNECT */
  ret = odbc_disconnect(env, dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}


int deleteusu (char *scname) {
  SQLHENV env;
  SQLHDBC dbc;
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  SQLCHAR y[512];
  char query[512];
    
  ret = odbc_connect(&env, &dbc);
  if (!SQL_SUCCEEDED(ret)) {
    return EXIT_FAILURE;
  }
  
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  
  /*Comprobamos si existe un usuario con ese screenname*/
  sprintf(query, "select screenname from usuario where usuario.screenname = '%s' AND usuario.borrado = 'FALSE'", scname);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);

  SQLBindCol(stmt, 1, SQL_C_CHAR, y, sizeof(y), NULL);
  
  if(!SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    printf("No existe un usuario con ese screenname \n");
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
      return EXIT_FAILURE;
    }
    return -1;
  }
  
  SQLCloseCursor(stmt);
  
  /*Marcamos el usuario como borrado*/
  sprintf(query, "UPDATE usuario SET borrado=TRUE where usuario.screenname='%s'",scname);

  ret=SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  
  printf("Se ha borrado el usuario con screenname: %s \n",scname);

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
  char op;
  
  if (argc<2) {
    printf("usage: usuario +/- <screenname> <fullname>\n");
    return 1;
  }

  op=argv[1][0];
  
  if(op=='+'){
    insertusu(argv[2], argv[3]);
    return 0;
  }
  else{
    deleteusu(argv[2]);
    return 0;
  }
  
  return 1;
}
