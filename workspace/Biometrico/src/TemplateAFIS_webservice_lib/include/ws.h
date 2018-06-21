#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <XmlRpcCpp.h>

/**
* @file ws.h
* @brief Archivo de cabecera de las clases estado proyecto
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @authors Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.
* @date 2014
* @version 2.0
* This file is part of Sistema de Identificación Biométrica para las OBPP.
*/

/**
 * @brief NAME
 */
#define NAME       "XML-RPC CFG C++ Client"
/**
 * @brief VERSION
 */
#define VERSION    "2.0"

#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <cctype>

using namespace std;

/**
 * @class estado_proyecto
 * @brief Clase para manipular el estado_proyecto (status)
 */

class estado_proyecto
{

		
public:
    /*!
	* @brief dbname
	*/
	string dbname;
    /*!
	* @brief user
	*/
    string user;
    /*!
	* @brief pwd
	*/
    string pwd;
    /*!
	* @brief model
	*/
    string model;
    /*!
	* @brief URL
	*/
    string URL;
    /*!
	* @brief URL2
	*/
    string URL2;

    /*!
	* @brief parent_id
	*/
	int parent_id;
    /*!
	* @brief id_la_tabla_res_partner
	*/
    int id_la_tabla_res_partner;

	//Las dos variables importantes que se van a insertar en la nueva tabla.
    /*!
	* @brief id_res_partner_fingerprint
	*/
	int id_res_partner_fingerprint;
    /*!
	* @brief id_del_proyecto
	*/
	int id_del_proyecto;

    /*!
	* @brief uid
	*/
	XmlRpcValue::int32 uid;

    /**
	 * @brief Constructor de la clase estado_proyecto()
	 */
	estado_proyecto();

    /**
	 * @brief Destructor de la clase estado_proyecto()
	 */
	~estado_proyecto();

    /**
	* @brief Método set_variables:
	* Se usan para «loguearse» en el webservice de Odoo.
	* Con eso se establece la comunicación entre ambos webservices (captahuellas y Odoo).
	* @param dbname nombre de la base de datos
	* @param user Nombre de usuario
	* @param pwd Contraseña
	* @param URL URL
	* @param URL2 URL2
	*/
	void set_variables(string dbname, string user, string pwd, string URL, string URL2);

    /**
	* @brief Método inicializar:
	* Se define para establecer la conexión entre ambos webservices
	* (Servidor del captahuellas y webservice de Odoo).
	* @param dbname Nombre de la base de datos
	* @param user Nombre del usuario
	* @param pwd Contraseña
	* @param URL URL
	* @param URL2 URL2
	* @return Resultado
	*/
	int inicializar();
	
	/**
	* @brief Método buscar_id_obpp:
	* Permite ubicar el proyecto a partir de la cédula de la persona a quien le vas a capturar
	* la huella, primero se debe buscar la persona en una tabla que relaciona personas con OBPP.
	* Se toma el ID de la tabla de OBPP a la cual pertenece la persona, con un READ en lugar de un BROWSE.
	* Esas funciones son necesarias para la consulta hay que hacer una matriz XML-RPC para pasársela a Odoo.
	*
	* OPTIMIZAR: método
	* @param cedula Número de Cédula
    * @return Resultado
	*/
	int buscar_id_obpp(string cedula);

    /**
	* @brief buscar_estado_proyecto (estatus):
	* Odoo deberá informar el status es si está en «por ir a la cita».
	* La cita es cuando los voceros asisten y se les captan las huellas.
	* Se reliza la búsqueda del proyecto que se arma en las líneas anteriores.
	* Se hace el READ (en lugar de BROWSE) para sacar el estatus del proyecto.
	* Lo que está comentado es una lógica que se aplicó al principio (cuando una BPP tenía más de un proyecto), pero eso se eliminó.
	* Se retorna el estatus y cierra el método.
	* Se extrae el estatus del resultado de la consulta
	*
	* string buscar_estado_proyecto(int id_obpp).
	*
	* Optimizar: El comentado es el codigo “legacy” que ya no se utiliza.
	* Se debe cambiar el nombre de la función de estado a estatus
	* @param id_proyecto Identificador del  Proyecto
    * @return Resultado
	*/

   string buscar_estado_proyecto(int id_proyecto);

    /**
	* @brief Método validacion_del_estado_del_proyecto:
	* Devuelve un estatus del proyecto al obtener la cédula del vocero, mano, dedo e id del proyecto.
	* Busca los estatus en SINCO
	* @param cedula Número de Cédula
	* @param mano Nombre de la Mano
	* @param dedo Número de Dedo
	* @param id_proyecto Identificador del Proyecto
    * @return Resultado
	*/
	int validacion_del_estado_del_proyecto(string cedula, string mano, int dedo, int id_proyecto);

    /**
	* @brief Método set_id_de_res_partner_fingerprint:
	* Permite identificar la huela en base a la persona (partner), el dedo y la mano especificados.
	* @param mano Nombre de Mano
	* @param id_la_tabla_res_partner Identificador de la tabla res_partner
	* @param dedo Número de Dedo
	*/
	void set_id_de_res_partner_fingerprint(string mano, int id_la_tabla_res_partner, int dedo);

    /**
	* @brief Método write_res_partner_fingerprint_verification:
	* Escribe cuando se hace la verificación de huella.
	* Sirve para llevar la cuenta y poder auditar las validaciones que se hacen con las huellas.
	* Esas verificaciones siempre se relacionan con el proyecto y/o con una rendición de cuentas.
	* if(accountability_id > 0):
	* El write permite validar la carga del proyecto, o para validar la rendición
	* de cuentas de ese proyecto.
	* @param fingerprint_id Identificador-Fingerprint
	* @param project_id Identificador del Proyecto
	* @param accountability_id Identificador Accountability
    * @return Resultado
	*/
	int write_res_partner_fingerprint_verification(int fingerprint_id, int project_id, int accountability_id);

    /**
	* @brief Método validacion_verificacion:
	* Realiza la búsqueda de la huella, en res.partner.fingerprint.verification donde se registran
	* las veces que se validaron las huellas (luego de haber sido registradas).
	* @param fingerprint_id Identificador-Fingerprint
	* @param project_id Identificador del Proyecto
	* @param accountability_id Identificador Accountability
    * @return Resultado
	*/
	bool validacion_verificacion(int fingerprint_id, int project_id, string estatus_proyecto, int accountability_id);

    /**
	* @brief Método buscar_accountability:
	* Busca las rendiciones de cuentas (los eventos de rendición de cuenta o los documentos que se entregan
	* para rendir cuentas). Se capturan huellas al momento de rendir cuentas.
	* Es una forma de que no puedan repudiar el reporte del uso de los fondos.
	* La huellas capturadas se requieren cuando van a la cita a solicitar el proyecto y para
	* certificar al usuario que reportó el uso de los fondos. Es la «no repudiabilidad».
	*
	* OPTIMIZACIÓN: Usar el BROWSE y especificar qué datos son necesarios. Actualmente trae todos los datos.
	* Usa mucha memoria
	*
	* @param id_proyecto Identificador del proyecto
    * @return Resultado
	*/
	int buscar_accountability(int id_proyecto);
    
    int write_res_partner_fingerprint_verification_incidence(long int fingerprint_id, long int project_id, string state, string incidence_state);
    
    /**
    * @brief Método validacion_incidencia:
 	* La incidencia es un booleano que se activa cuando ocurre algo en la validación.
 	* Si hay una incidencia se muestra en error XML-RPC
 	* Luego se cierra la comunicación con el cliente.
 	* @param fingerprint_id Identificador-fingerprint
 	* @param project_id Identificador del proyecto
 	* @param state Estado(estatus) del proyecto
 	* @param incidence_state Estatus de incidencia del proyecto
    */

    bool validacion_incidencia(long int fingerprint_id, long int project_id, string state, string incidence_state);
    


    /**
    * @brief Método validacion_incidencias_huella:
    * Es un método para detectar la falla a nivel de red:
    * Si la falla es a nivel del webservice se atrapa con el Odoo, sino la falla es de a nivel
    * de la red y se verifica si se realizó un registro adecuado en la base de datos.
    * Este método llama al método write cuando se da cuenta que hubo una falla en la validación
    * y luego verifica si se escribió en el registro, sino se escribió nada se corta al cliente.
 	* @param cedula Cédula
 	* @param mano Nombre de la mano
 	* @param dedo Número del dedo
 	* @param id_proy Identificador del proyecto
    */
    int validacion_incidencias_huella(string cedula, string mano, int dedo, long int id_proy);
};
