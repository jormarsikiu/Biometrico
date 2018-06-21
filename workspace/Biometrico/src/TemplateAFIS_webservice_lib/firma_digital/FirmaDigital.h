/**
 * @file FirmaDigital.h
 * @brief Archivo de cabecera de las clases FirmaDigital
 * @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre, R.S.
 * @date 2014
 * @version 1.0
 *
 * contacto@simonrodriguez.org.ve
 *
 * This file is part of Sistema de Identificación Biométrica para las OBPP.
 */




#include <string.h>

using namespace std;

/**
 * @class FirmaDigital
 * @brief Clase que permite realizar Firma Digital
 */
class FirmaDigital
{
public:
	/*!
	 * @brief clave_privada
	 */
	string clave_privada;
	/**
	 * @brief Constructor
	 * @param clave_privada La clave privada para la firma
	 */
	FirmaDigital(string clave_privada);

	/**
	 * @brief Constructor
	 * @param cadena La cadena a firmar
 	 * @return Obtiene la cadena firmada
	 */
	string firma_srcl(string cadena);
};
