/**
 * @file FirmaDigital.cpp
 * @brief Se definen los métodos para generar la clave que será la firma de cada paquete XML-RPC
 *
 * Comando de compilación:
 * g++ sha512.cpp firma_srcl.cpp -o firma_srcl && ./firma_srcl
 *
 * @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
 * @date 2014
 * This file is part of Sistema de Identificación Biométrica para las OBPP.
*/


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <ctime>
#include "md5.h"
#include "FirmaDigital.h"

using namespace std;

/**
 * Constructor de la clase FirmaDigital
 * */
FirmaDigital::FirmaDigital(string clave_privada)
{
	this->clave_privada = clave_privada;
}

/**
 * @brief Método firma_srcl:
 * Recibe una cadena generadora y devuelve una clave
 * Algoritmo usado: MD5
 * Se crea una firma digital con una lista de palabras secretas. Se le adiciona la fecha y hora.
 * La palabra secreta depende del día de la semana.
 * Con la palabra secreta se hace un token o cookie para cada sesión cliente-servidor.
 * Luego se le da formato y se calcula el hash MD5.
 * La función del cliente es tomar la fecha y hora, seleccionar una de las palabras de la lista y hashear
 * el resultado. Ese hash se pasa al servidor.
 * En el lado del servidor se hace lo mismo en cada petición.
 * Se comparan los haches y si son iguales, se procesa la petición (comparar, almacenar, borrar, etc.)
*/
string FirmaDigital::firma_srcl(string cadena){
	string clave_privada;
	stringstream ss, sstm;
	string firma;
	string arreglo[7];

	arreglo[0] = "Cabimas";
	arreglo[1] = "Ciudad Ojeda";
	arreglo[2] = "Lagunillas";
	arreglo[3] = "Maracaibo";
	arreglo[4] = "Puerto Caballo";
	arreglo[5] = "Santa Rita";
	arreglo[6] = "Catatumbo";

	struct tm * ptm;
	char comp_mon, comp_day;
	int anio, mes, day, daySem;

	//this->clave_privada = "nonosdevolvemos.2013";

	// Obteniendo la hora
	time_t AHORA = time(0);
	ptm = gmtime(&AHORA);
	anio = ptm->tm_year+1900;
	mes = ptm->tm_mon+1;
	day = ptm->tm_mday;
	daySem = ptm->tm_wday;

	comp_mon = '\0';
	if (mes < 10) comp_mon = '0';

	comp_day = '\0';
	if (day < 10) comp_day = '0';

        // Palabra q cambia por día de la Semana
        string palabraSemana = arreglo[daySem];

	// Formateando
	sstm << anio << "-" << comp_mon << mes << "-" << comp_day << day;

	// Concatenando CADENA SEMILLA + TIEMPO + PALABRASEMANA + CLAVE PRIVADA
	ss << cadena.c_str() << sstm.str() << palabraSemana.c_str() << this->clave_privada.c_str();
	firma = ss.str();

        //cout << "Cadena: " << firma << endl;

	firma = md5(firma);
	//firma = sha512(firma);

	return firma;
}
