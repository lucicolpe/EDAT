#!/bin/bash

COMMAND="./esecuele"

rm -rf libreria_db

$COMMAND createdb libreria_db

$COMMAND define libreria_db << EOF
TABLE edicion 8 STR STR STR STR STR STR STR LNG
TABLE ventas 4 LNG LNG LNG STR
TABLE usuarios 6 LNG STR STR STR LNG STR 
EOF

$COMMAND insert libreria_db << EOF
COPY edicion ./example_files/edicion.txt
COPY ventas ./example_files/ventas.txt
COPY usuarios ./example_files/usuarios.txt
EOF
# Test the following queries:
# - Lista de libros comprados por "jack"
$COMMAND query libreria_db << EOF
ventas SEQUENTIAL usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT STR O P_COL 1 PROJECT PRODUCT 1 4 C_COLEQCOL SELECT STR 2 P_COL 1 PROJECT edicion SEQUENTIAL PRODUCT 0 8 C_COLEQCOL SELECT STR 2 P_COL 1 PROJECT
EOF

# - Números de libros comprados por "jack"
$COMMAND query libreria_db << EOF
ventas SEQUENTIAL usuarios SEQUENTIAL 1 STR jack C_COLEQCTE SELECT STR O P_COL 1 PROJECT PRODUCT 1 4 C_COLEQCOL SELECT STR 2 P_COL 1 PROJECT edicion SEQUENTIAL PRODUCT 0 8 C_COLEQCOL SELECT STR 2 P_COL 1 PROJECT COUNT
EOF

$COMMAND query libreria_db
