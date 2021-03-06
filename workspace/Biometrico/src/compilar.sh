#!/bin/sh

############################################################################
/**
* @file compilar.sh
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre	
* @author Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea. 
* @date Abril - Agosto de 2014. 
* @brief Script de instalación del webservice.  
*/
############################################################################

############################################################################
/**
 * Establecer las variables de entorno. Éstas implican haber instalado primero 
 * libtool, automake y autoconf, y compilado desde fuentes: libwww de w3c y     
 * xmlrpc.1.25.26 (en ese orden), habiendo corregido los enlaces simbólicos     
 * faltantes.     
 */                                                              
//############################################################################
CLIENT_CFLAGS=`xmlrpc-c-config c++ libwww-client --cflags`
CLIENT_LIBS=`xmlrpc-c-config c++ libwww-client --libs`
CGI_CFLAGS=`xmlrpc-c-config cgi-server --cflags`
CGI_LIBS=`xmlrpc-c-config cgi-server --libs`

################################################################################
/**
 * Se instala dependencias de compilación. 
 * El comando g++ usualmente indica si nos falta alguna dependencia o si la 
 * actualización de alguna biblioteca choca con el código. 
 * No es común, pero como en los temas de seguridad (bibliotecas SSL) y de 
 * criptografía casi todos los días se descubre un bug o hay un avance, es bueno leer toda la salida.                                             #
*/
################################################################################
g++ -lssl $CGI_CFLAGS $CLIENT_CFLAGS \
  TemplateAFIS_webservices_lib/firma_digital/FirmaDigital.cpp \
  TemplateAFIS_webservices_lib/include/Utilidades.cpp \
  TemplateAFIS_webservices_lib/firma_digital/md5.cpp \
  TemplateAFIS_webservices_lib/include/TemplateAFIS.cpp \
  TemplateAFIS_webservice.cpp TemplateAFIS_webservices_lib/include/ws.cpp -o \
  TemplateAFIS_webservice.cgi -lpqxx -lUFScanner -lUFMatcher \
  $CGI_LIBS $CLIENT_LIBS -L/usr/local/lib -lusb -ljpeg -lcrypto

################################################################################
/**
 * Copiar el binario (.cgi), la licencia y el subdirectorio con dependencias:   
 * Cuando el proceso termina produce TemplateAFIS_webservice.cgi. 
 * Lo mueve a la carpeta desde la cual Apache sirve los binarios
*/
################################################################################
cp TemplateAFIS_webservice.cgi /usr/lib/cgi-bin/sinco/
cp UFLicense.dat /usr/lib/cgi-bin/sinco/
cp -r TemplateAFIS_webservices_lib /usr/lib/cgi-bin/sinco/

################################################################################
/*
 * Otorgar permisos y establecer el propietario de los archivos:                #
*/
################################################################################
chmod 740 /usr/lib/cgi-bin/ -R
chown www-data:www-data /usr/lib/cgi-bin/ -R
