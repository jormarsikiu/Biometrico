#!/bin/sh

############################################################################
/**																			 
* @file instalar.sh															 
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre	 
* @author Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.    
* @date Abril - Agosto de 2014. 											 
* @brief Script de instalación del webservice.  							 
*/																			 
############################################################################


############################################################################
/** 
 * @brief Script de instalación del webservice. 
 * El primer bloque de if decide para qué arquitectura de Linux debe instalar las dependencias (32 o 64 bits).
 *
 * OPTIMIZAR: 
 * Se debe probar a mano cada despliegue dado que probable que se deban actualizar las librerias.
 * Se debe mejorar la captura de esos errores en la instalación.
 * Las pruebas con HTTP + HTTPS  trajo problemas serios el año 2017, porque los navegadores bloquean silenciosamente el contenido mixto.
 * Probablemente todos los sistemas que tengan captahuellas tendrán que funcionar con HTTPS en todas las conexiones. SINCOOP, por ejemplo, aún es mixto.
 */
############################################################################


############################################################################
/**
 * @brief Antes de la instalación se hace el update, porque a veces han dejado inestable el sistema operativo 
 * o si hay una falla de conexión o si falta alguna firma de seguridad, la instalación no procede bien. 
*/
############################################################################
apt-get update

############################################################################
/**
* @brief Instalación de dependencias según la arquitectura 
*/
############################################################################
apt-get -y install libxmlrpc-c++4 libxmlrpc-core-c3 libcurl3 libcurl3-gnutls \
libc6 openssl libpqxx-3.1 apache2 apache2-mpm-prefork cron postgresql libxmlrpc-c++4-dev \
libxmlrpc-c3-dev libxmlrpc-core-c3-dev g++ libpqxx3-dev libcurl3-gnutls \
curl libcurl4-gnutls-dev libusb-0.1-4 libusb-1.0-0 udev fxload libc6 libjpeg8-dev

############################################################################
/** 
 * @brief Se activa el módulo cgi de Apache (no ha sido probado con Ngnix)
 * y se instala libusb-0.1-4 que es requerido por el captahuellas USB.
*/
############################################################################
a2enmod cgi 
sh compilar.sh

############################################################################
/**
 * @brief Instalacion de los drivers y las reglas para que udev reconozca el captahuellas SUPREMA.
 * Hay un bloque para cada versión con BioMini Plus.
 * El servidor debe llevar un captahuellas para que se pueda hacer la verificación y la identificación.
*/
############################################################################
cp TemplateAFIS_webservice.cgi /usr/lib/cgi-bin/
cp -r TemplateAFIS_webservices_lib /usr/lib/cgi-bin/

############################################################################
/**
 * @brief Se activa el módulo ssl de Apache. SINCO utiliza HTTPS para servir el contenido y también para el sub-sistema de captahuellas. 
 */
############################################################################
service apache2 restart
