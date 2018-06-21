/**
* @file Fingerprint.cpp
* @brief Clase que permite realizar la interacción con el Webservice con OpenERP
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @authors Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.
* @date 2014
* This file is part of Sistema de Identificación Biométrica para las OBPP.
*/

/**
 * @def NAME
*/
#define NAME       "XML-RPC CFG C++ Client"
/**
 * @def VERSION
*/
#define VERSION    "0.1"

#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <XmlRpcCpp.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/md5.h>

using namespace std;

/**
 * @class Fingerprint
 * @brief Clase que permite realizar la interacción con el Webservice con OpenERP
 */
class Fingerprint
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
	 * @brief URL
	 */
	string URL;

	/**
	 * @brief Constructor de la clase Fingerprint
	 */
	Fingerprint(string dbname, string user, string pwd, string URL)
	{
		this->dbname = dbname;
		this->user = user;
		this->pwd = pwd;
		this->URL = URL;
	}

	/**
	 * @brief Get UID: Realiza login.
	 * Se crea la matriz (array) con cada parámetro que Odoo necesita para loguear al usuario
	 * (chequea la base de datos, usuario y clave para el logueo).
	 * Todos los valores son procesados por XmlRpcValue.
	 * @return Retorna UID de Usuario URE.Si es 0 devuelve error (credenciales erradas).
	 * Sólo los usuarios con permisos de URE (Coordinadores de las Unidades Receptoras Estadales)
	 * pueden usar el sistema de  huellas.
	 */
	long int getUID()
	{
		XmlRpcClient server2 (this->URL + "/xmlrpc/common");
		XmlRpcValue param_array2 = XmlRpcValue::makeArray();
		param_array2.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array2.arrayAppendItem(XmlRpcValue::makeString(user.c_str()));
		param_array2.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		XmlRpcValue result2 = server2.call("login", param_array2);

		/*XMLRPC_TYPE_INT
		    An integer, 32 bit precision 
		XMLRPC_TYPE_I8
		    An integer, 64 bit precision 
		XMLRPC_TYPE_BOOL
		    A boolean (logical; true/false) value 
		XMLRPC_TYPE_DOUBLE
		    A floating point number 
		XMLRPC_TYPE_DATETIME
		    Identifies a point in time 
		XMLRPC_TYPE_STRING
		    A text string 
		XMLRPC_TYPE_BASE64
		    A byte string 
		XMLRPC_TYPE_NIL
		    A non-value or absence of a value 
		XMLRPC_TYPE_STRUCT
		    A structure, analogous to a C struct. A set of (name, XML-RPC value) pairs. 
		XMLRPC_TYPE_ARRAY
		    An array of XML-RPC values 
		XMLRPC_TYPE_C_PTR
		    A pointer to a C variable. This is meaningful only in the context of a particular
		    running instance of a C program.*/

		if(result2.getType() == XMLRPC_TYPE_BOOL)
		{
			if(!result2.getBool())
				return 0;
        	}

		//else// if(result2.getType() == XMLRPC_TYPE_INT)

		return result2.getInt();
	}

	/**
	 * @brief Is URE: Pregunta si un usuario es URE.
	 * Se buscan la huella y la plantilla a partir de una cédula u otro identificador del vocero.
	 * @param server_search Servidor XML_RPC
	 * @param model es el modelo (tablas de la base de datos) de Odoo.
	 * @return Retorna si es URE o no es URE.
	 * Esta función devuelve los IDs de las huellas que tenga la persona.
	 */
	bool isURE(XmlRpcClient server_search)
	{
		int uid = this->getUID();

		if(uid == 0)
			return false;

		string model = "webservice.methods"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_search = XmlRpcValue::makeArray();
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString("is_ure"));
		XmlRpcValue result_search = server_search.call("execute", param_array_search);
		XmlRpcValue resultados = result_search;
		return resultados.getBool();
	}

	/**
	 * @brief Search Persona: Busca una persona por cédula o partner_id.
	 * OPTIMIZAR: Cambiar todos los search posibles por browse.
	 * @param server_search Servidor XML_RPC
	 * @param cedula Cédula persona
	 * @param partner_id Id Partner
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue searchPersona(XmlRpcClient server_search, string cedula="", long int partner_id = 0)
	{
		long int uid = this->getUID();
		string model = "sinco.obpp.responsible"; //"res.partner"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_search = XmlRpcValue::makeArray();
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		XmlRpcValue array_comp = XmlRpcValue::makeArray();

		//array_comp.arrayAppendItem(XmlRpcValue::makeString("|"));

		if(strcmp(cedula.c_str(), "") != 0)
		{
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("identification_id"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString(cedula.c_str()));
			array_comp.arrayAppendItem(array_comp_1);
		}

		if(partner_id != 0)
		{
			XmlRpcValue array_comp_2 = XmlRpcValue::makeArray();
			array_comp_2.arrayAppendItem(XmlRpcValue::makeString("id"));
			array_comp_2.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_2.arrayAppendItem(XmlRpcValue::makeInt(partner_id));
			array_comp.arrayAppendItem(array_comp_2);
		}

		param_array_search.arrayAppendItem(array_comp);
		XmlRpcValue result_search = server_search.call("execute", param_array_search);
		XmlRpcValue resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Read Vat Persona: Ubica las huellas de la persona en base a su  por partner_id.
	 * @param server_search Servidor XML_RPC
	 * @param partner_id Id Partner
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue readVatPersona(XmlRpcClient server_search, XmlRpcValue partner_id)
	{
		long int uid = this->getUID();
		string model = "sinco.obpp.responsible"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_read = XmlRpcValue::makeArray();
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
		param_array_read.arrayAppendItem(partner_id);
		XmlRpcValue array_vals_read = XmlRpcValue::makeArray();
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("identification_id"));

		//array_vals_read.arrayAppendItem(XmlRpcValue::makeString("parents_ids"));
		//array_vals_read.arrayAppendItem(XmlRpcValue::makeString("situr_code"));

		param_array_read.arrayAppendItem(array_vals_read);
		XmlRpcValue result_search = server_search.call("execute", param_array_read);
		XmlRpcValue resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Search Fingerprint: Busca una huella específica por dedo, mano e Id partner.
	 *
	 * partner_id: identifica el registro de la persona en la base de datos de Odoo.
	 * finger: es el dedo (0 a 4, pulgar, índice, medio, anular y meñique).
	 * hand: es la mano o pie (MD, MI, PD, PI).
	 * @param server_search Servidor XML_RPC
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue searchFingerprint(XmlRpcClient server_search, int finger=0, string hand="", long int partner_id=0)
	{
		long int uid = this->getUID();
		string model = "res.partner.fingerprint"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_search = XmlRpcValue::makeArray();
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		XmlRpcValue array_comp = XmlRpcValue::makeArray();

		//array_comp.arrayAppendItem(XmlRpcValue::makeString("|"));

		if(finger > 0 && strcmp(hand.c_str(), "") != 0 && partner_id > 0)
		{
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("finger"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(finger));
			array_comp.arrayAppendItem(array_comp_1);
			XmlRpcValue array_comp_2 = XmlRpcValue::makeArray();
			array_comp_2.arrayAppendItem(XmlRpcValue::makeString("hand"));
			array_comp_2.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_2.arrayAppendItem(XmlRpcValue::makeString(hand.c_str()));
			array_comp.arrayAppendItem(array_comp_2);
			XmlRpcValue array_comp_3 = XmlRpcValue::makeArray();
			array_comp_3.arrayAppendItem(XmlRpcValue::makeString("partner_id"));
			array_comp_3.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_3.arrayAppendItem(XmlRpcValue::makeInt(partner_id));
			array_comp.arrayAppendItem(array_comp_3);
		}

		param_array_search.arrayAppendItem(array_comp);
		XmlRpcValue result_search = server_search.call("execute", param_array_search);
		XmlRpcValue resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Método readFingerprintImageId: Obtiene el campo image_id de la tabla.
	 *
	 * Sirve para ubicar la imagen que se muestra para cada huella.
	 * De la imagen podemos obtener la plantilla
	 * (template que tiene la información geométrica de las minucias.),
	 * pero en realidad se guarda para mostrarla.
	 * Se hacen las comparaciones en base a las plantillas que genera automáticamente
	 * el captahuellas.
	 *
	 * El captahuellas escanea la huella y mejora brillo y contraste automáticamente.
	 * Almacena un mapa de bits.
	 * El mismo captahuellas procesa la imagen y extrae las minucias según los conceptos de la imagen (islas, bifurcaciones, deltas, vacíos, etc.)
	 * Esas minucias se guardan en un archivo binario de unos 200 - 300 bytes.
	 * Y el mapa de bits también se almacena.
	 * Con ese último método, se busca la imagen de la huella. La comparación en base a las planillas es usada para saber si una huella pertenece a una persona (verificación) o a qué persona pertenece una huella (identificación), se comparan plantillas, no imágenes.
	 * Es para mostrarla en el cliente.
	 *
	 * res.partner.fingerprint fingerprint_id, que es el id de la tabla:
	 * res.partner.fingerprint.
	 * @param server_search Servidor XML_RPC
	 * @param fingerprint_id Id tabla res_partner_fingerprint
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue readFingerprintImageId(XmlRpcClient server_search, XmlRpcValue fingerprint_id)
	{
		long int uid = this->getUID();
		string model = "res.partner.fingerprint"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_read = XmlRpcValue::makeArray();
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
		param_array_read.arrayAppendItem(fingerprint_id);
		XmlRpcValue array_vals_read = XmlRpcValue::makeArray();
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image_id"));
		param_array_read.arrayAppendItem(array_vals_read);
		XmlRpcValue result_search = server_search.call("execute", param_array_read);
		XmlRpcValue resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Método readFingerprint: Es una función polimorfa. 
	 *
	 * Es una función de búsqueda general. Dependiendo del input, hace diferentes búsquedas.
	 * Recibe los parámetros int finger = 0, string hand = "", long int partner_id = 0.
	 * Obtiene todos los campos de la tabla: res.partner.fingerprint.
	 *
	 * OPTIMIZAR: No es seguro que se use actualmente. Es posible que se elimine en la revisión.
	 * @param server_search Servidor XML_RPC
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue readFingerprint(XmlRpcClient server_search, int finger = 0, string hand = "", long int partner_id = 0)
	{
        	//int uid = this->getUID();

		XmlRpcValue resultados_fingerprint_id = XmlRpcValue::makeArray();
		if(finger > 0 && strcmp(hand.c_str(), "") != 0 && partner_id > 0)
			resultados_fingerprint_id = this->searchFingerprint(server_search, finger, hand, partner_id);
		else
			resultados_fingerprint_id = this->searchFingerprint(server_search);

		if(resultados_fingerprint_id.arraySize() == 0)
		{
			XmlRpcValue resultados = XmlRpcValue::makeArray();
			return resultados;
		}
		return readFingerprint(server_search, resultados_fingerprint_id);
	}

	/**
	 * @brief Método readFingerprint: Es una función polimorfa. 
	 * Recibe los parámetros XmlRpcClient server_search, XmlRpcValue fingerprint_id.
	 *
	 * Obtiene todos los campos de la tabla: res.partner.fingerprint.
	 * @param server_search Servidor XML_RPC
	 * @param fingerprint_id Id Fingerprint
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue readFingerprint(XmlRpcClient server_search, XmlRpcValue fingerprint_id)
	{
		long int uid = this->getUID();
		string model = "res.partner.fingerprint"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_read = XmlRpcValue::makeArray();
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
		param_array_read.arrayAppendItem(fingerprint_id);
		XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

		/*array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image_id"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("hash"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("finger"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("template"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("hand"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("partner_id"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("template_size"));*/

		param_array_read.arrayAppendItem(array_vals_read);
		XmlRpcValue result_search = server_search.call("execute", param_array_read);
		XmlRpcValue resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Método readFingerprintImage: Obtiene todos los campos de la tabla 
	 * res.partner.fingerprint.image.
	 *
	 * Es una función de consulta general, pero en base a dedo, mano y el ID de la persona.
	 *
	 * OPTIMIZAR: No se esta usando
	 * @param server_search Servidor XML_RPC
	 * @param finger Finger
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue readFingerprintImage(XmlRpcClient server_search, int finger = 0, string hand = "", long int partner_id = 0)
	{
		long int uid = this->getUID();
		XmlRpcClient server_delete (this->URL + "/xmlrpc/object");
		XmlRpcValue resultados = XmlRpcValue::makeArray();

		if(finger > 0 && strcmp(hand.c_str(), "") != 0 && partner_id > 0)
		{
			XmlRpcValue resultados_fingerprint_id = this->searchFingerprint(server_delete, finger, hand, partner_id);

			if(resultados_fingerprint_id.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			XmlRpcValue readImage = this->readFingerprintImageId(server_delete, resultados_fingerprint_id);

			if(readImage.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			resultados = this->searchFingerprintImage(server_delete, readImage.arrayGetItem(0).getStruct().structGetValue("image_id").arrayGetItem(0).getInt());
		}else
			resultados = this->searchFingerprintImage(server_delete);

		string model = "res.partner.fingerprint.image";

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_read = XmlRpcValue::makeArray();
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
		param_array_read.arrayAppendItem(resultados);
		XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

		/*array_vals_read.arrayAppendItem(XmlRpcValue::makeString("id"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image_height"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image_width"));
		array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image_size"));*/

		param_array_read.arrayAppendItem(array_vals_read);
		XmlRpcValue result_search = server_search.call("execute", param_array_read);
		resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Método searchFingerprintImage:  Obtiene el ID de la tabla:
	 * res.partner.fingerprint.image.
	 *
	 * OPTIMIZAR: No se usa porque ya lo resuelve Odoo
	 * @param server_search Servidor XML_RPC
	 * @param fingerprint_id Id Fingerprint
	 * @return Retorna XmlRpcValue resultante
	 */
	XmlRpcValue searchFingerprintImage(XmlRpcClient server_search, long int fingerprint_id = 0)
	{
		long int uid = this->getUID();
		string model = "res.partner.fingerprint.image"; //res.partner.fingerprint.image

		//XmlRpcClient server_search ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_search = XmlRpcValue::makeArray();
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		XmlRpcValue array_comp = XmlRpcValue::makeArray();

		//array_comp.arrayAppendItem(XmlRpcValue::makeString("|"));

		if(fingerprint_id > 0)
		{
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("id"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(fingerprint_id));
			array_comp.arrayAppendItem(array_comp_1);
		}

		param_array_search.arrayAppendItem(array_comp);
		XmlRpcValue result_search = server_search.call("execute", param_array_search);
		XmlRpcValue resultados = result_search;
		return resultados;
	}

	/**
	 * @brief Método insertarFingerprintImg: Inserta los campos en las tablas:
	 * res.partner.fingerprint.image y res.partner.fingerprint.
	 *
	 * OPTIMIZAR: No se usa en esta versión. Actualmente usa el modelo webservice.methods
	 *
	 * Se sustituyó por el método: int insertarFingerprintWebService(unsigned char * &buffer_template, string hash, int finger, string hand, long int partner_id, int template_size, int image_height, int image_width, unsigned char * &image, long int image_size)
	 *
	 *
	 * @param buffer_template Huella
	 * @param hash Hash
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @param template_size Tamaño Huella
	 * @param image_height Alto imagen
	 * @param image_width Ancho imagen
	 * @param image Imagen
	 * @param image_size Tamaño imagen
	 * @return Retorna Estado de inserción
	 */
	int insertarFingerprintImg(unsigned char *buffer_template, string hash, int finger, string hand, long int partner_id, int template_size, int image_height, int image_width, unsigned char * image, long int image_size)
	{
		long int uid = this->getUID();
		string model;
		XmlRpcClient server_create (this->URL + "/xmlrpc/object");
		model = "res.partner.fingerprint.image";

		//XmlRpcClient server_create ("http://probando.hoatzin.org:8089/xmlrpc/object");

		XmlRpcValue param_array_create = XmlRpcValue::makeArray();
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString("create"));

		//XmlRpcValue array_vals_create = XmlRpcValue::makeArray();

		XmlRpcValue struct_create = XmlRpcValue::makeStruct();
		struct_create.structSetValue("image_height", XmlRpcValue::makeInt(image_height));
		struct_create.structSetValue("image_width", XmlRpcValue::makeInt(image_width));
		struct_create.structSetValue("image", XmlRpcValue::makeBase64(image, image_size));
		struct_create.structSetValue("image_size", XmlRpcValue::makeInt(image_size));
		param_array_create.arrayAppendItem(struct_create);

		XmlRpcValue result = server_create.call("execute", param_array_create);
		long int id_fingerprint_image = result.getInt();
		model = "res.partner.fingerprint";

		param_array_create = XmlRpcValue::makeArray();
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString("create"));

		struct_create = XmlRpcValue::makeStruct();
		struct_create.structSetValue("image_id", XmlRpcValue::makeInt(id_fingerprint_image));
		struct_create.structSetValue("hash", XmlRpcValue::makeString(hash.c_str()));
		struct_create.structSetValue("finger", XmlRpcValue::makeInt(finger));
		struct_create.structSetValue("template", XmlRpcValue::makeBase64(buffer_template, template_size));
		struct_create.structSetValue("hand", XmlRpcValue::makeString(hand.c_str()));
		struct_create.structSetValue("partner_id", XmlRpcValue::makeInt(partner_id));
		struct_create.structSetValue("template_size", XmlRpcValue::makeInt(template_size));

		//array_vals_create.arrayAppendItem(struct_create);

		param_array_create.arrayAppendItem(struct_create);
		result = server_create.call("execute", param_array_create);
		isURE(server_create);
		return result.getInt();
	}

	/**
	 * @brief Método base64: Convierte de unsigned char * a Base64.
	 *
	 * Sirve para convertir texto en UUCODE de base 64,
	 * que se usa para «desbinarizar» la plantilla binaria.
	 * Se desbinariza por compatibilidad con los tipos de datos de XML RPC.
	 * Las imágenes binarias en internet se movían como texto UUEncoded.
	 * XML RPC sólo acepta datos en matrices no-binarias.
	 * Si el argumento de la petición es un template, debemos codificarla a texto.
	 * Odoo admite peticiones a su webservice en XML.
	 * @param input Raw
	 * @param length Tamaño Raw
	 * @return Texto en base64
	 */
	char *base64(unsigned char * &input, long int length)
	{
		BIO *bmem, *b64;
		BUF_MEM *bptr;

		b64 = BIO_new(BIO_f_base64());
		bmem = BIO_new(BIO_s_mem());
		b64 = BIO_push(b64, bmem);
		BIO_write(b64, input, length);
		BIO_flush(b64);
		BIO_get_mem_ptr(b64, &bptr);

		char *buff = (char *)malloc(bptr->length);
		memcpy(buff, bptr->data, bptr->length-1);
		buff[bptr->length-1] = 0;

		BIO_free_all(b64);

		return buff;
	}

	/**
	 * @brief Método base64_decodestring: Decodifica un string Base64 a un 
	 * unsigned char *.
	 * @param text Texto Base64
	 * @return Raw
	 */
	unsigned char * base64_decodestring( std::string text ){
		EVP_ENCODE_CTX ectx;
		unsigned char* out = (unsigned char*)malloc( text.size() );
		int outlen = 0;
		int tlen = 0;

		EVP_DecodeInit( &ectx );
		EVP_DecodeUpdate( &ectx, out, &outlen, (const unsigned char*)text.c_str(), text.size() );
		tlen += outlen;
		EVP_DecodeFinal( &ectx, out+tlen, &outlen );
		tlen += outlen;

		std::string data( (char*)out, tlen );
		//free( out );
		return out;
	}

	/**
	 * @brief Método str2md5: Codifica un string a MD5.
	 *
	 * Sirve para convertir una cadena en MD5.
	 * Se usa para hashear la fecha, la hora y otras cosas, como una forma de crear una
	 * cookie para certificar cada conexión de un cliente con el servidor.
	 * Si la fecha y la hora del cliente están muy lejos de la del servidor, la conexión falla.
	 *
	 * @param str Texto
	 * @param length Tamaño texto
	 * @return Texto en MD5
	 */
	char *str2md5(const char *str, int length) {
		int n;
		MD5_CTX c;
		unsigned char digest[16];
		char *out = (char*)malloc(33);
		MD5_Init(&c);

		while (length > 0) {
			if (length > 512) {
				MD5_Update(&c, str, 512);
			} else {
				MD5_Update(&c, str, length);
			}
			length -= 512;
			str += 512;
		}

		MD5_Final(digest, &c);

		for (n = 0; n < 16; ++n) {
			snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
		}

		return out;
	}

	/**
	 * @brief Método insertarFingerprintWebService: Inserta los campos en las tablas:
	 * res.partner.fingerprint.image y res.partner.fingerprint.
	 *
	 * Es una función para insertar las huellas a Odoo.
	 * Toma los datos del cliente, los arma en una matriz (array) que XML RPC pueda interpretar,
	 * e invoca el método correspondiente en el webservice de Odoo.
	 * Convierte la imagen de binario a texto y de texto a cadena XML RPC.
	 * webservice.methods es el modelo de Odoo que instancia el webservice de Odoo.
	 * Odoo tiene su propio cliente web, y se relaciona con la base de datos mediante ese webservice «interno».
	 * @param buffer_template Huella
	 * @param hash Hash
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @param template_size Tamaño Huella
	 * @param image_height Alto imagen
	 * @param image_width Ancho imagen
	 * @param image Imagen
	 * @param image_size Tamaño imagen
	 * @return Retorna Estado de inserción
	 */

	int insertarFingerprintWebService(unsigned char * &buffer_template, string hash, int finger, string hand, long int partner_id, int template_size, int image_height, int image_width, unsigned char * &image, long int image_size)
	{
		long int uid = this->getUID();
		string model = "webservice.methods";
		XmlRpcClient server_create (this->URL + "/xmlrpc/object");
		XmlRpcValue param_array_create = XmlRpcValue::makeArray();
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString("create_fingerprint"));
		XmlRpcValue fingerprint_image_create = XmlRpcValue::makeStruct();

		fingerprint_image_create.structSetValue("image_height", XmlRpcValue::makeInt(image_height));
		fingerprint_image_create.structSetValue("image_width", XmlRpcValue::makeInt(image_width));

		string image_string = base64(image, image_size); //str2md5(const char *str, int length)
		fingerprint_image_create.structSetValue("image", XmlRpcValue::makeString(image_string.c_str()));
		fingerprint_image_create.structSetValue("image_size", XmlRpcValue::makeInt(image_size));
		param_array_create.arrayAppendItem(fingerprint_image_create);
		XmlRpcValue fingerprint_create = XmlRpcValue::makeStruct();
		string template_string = base64(buffer_template, template_size);

		//hash = str2md5(template_string.c_str(), template_string.length());

		fingerprint_create.structSetValue("hash", XmlRpcValue::makeString(hash.c_str()));
		fingerprint_create.structSetValue("finger", XmlRpcValue::makeInt(finger));
		fingerprint_create.structSetValue("template", XmlRpcValue::makeString(template_string.c_str()));
		fingerprint_create.structSetValue("hand", XmlRpcValue::makeString(hand.c_str()));
		fingerprint_create.structSetValue("partner_id", XmlRpcValue::makeInt(partner_id));
		fingerprint_create.structSetValue("template_size", XmlRpcValue::makeInt(template_size));

		param_array_create.arrayAppendItem(fingerprint_create);
		XmlRpcValue result = server_create.call("execute", param_array_create);

		//if(strcmp(result.getString().substr(0,39).c_str(), "Se crearon correctamente los registros:") == 0)
		//cerr << "Insertó " << result.getInt() << endl;

		return result.getInt();
		/*else
			return -664;*/

		//return result.getInt();
	}

	/**
	 * @brief Método actualizarFingerprintImg: Actualiza un registro en la tabla
	 * res.partner.fingerprint.image.
	 * Es la función para actualizar huellas.
	 * Para actualizar primero se ubica (searchFingerprint) y con ese ID se sobreescribe.
	 *
	 * OPTIMIZAR: * No está activa en esta versión
	 * Se sustituyó por la bool actualizarFingerprintImg(unsigned char *buffer_template, string hash, int finger, string hand, long int partner_id, int template_size, int image_height, int image_width, unsigned char * image, long int image_size)
	 * @param buffer_template Huella
	 * @param hash Hash
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @param template_size Tamaño Huella
	 * @param image_height Alto imagen
	 * @param image_width Ancho imagen
	 * @param image Imagen
	 * @param image_size Tamaño imagen
	 * @return Retorna Estado de actualización
	 */
	bool actualizarFingerprintImg(unsigned char *buffer_template, string hash, int finger, string hand, long int partner_id, int template_size, int image_height, int image_width, unsigned char * image, long int image_size)
	{
		long int uid = this->getUID();
		XmlRpcClient server_edit (this->URL + "/xmlrpc/object");
		XmlRpcValue resultados_fingerprint_id = this->searchFingerprint(server_edit, finger, hand, partner_id);

		if(resultados_fingerprint_id.arraySize() == 0)
			return false;

        //int fingerprint_id = resultados_fingerprint_id.arrayGetItem(0).getInt();

		XmlRpcValue readFingerprint = this->readFingerprint(server_edit, resultados_fingerprint_id);

		if(readFingerprint.arraySize() == 0)
			return false;

		XmlRpcValue resultados = this->searchFingerprintImage(server_edit, readFingerprint.arrayGetItem(0).getStruct().structGetValue("image_id").arrayGetItem(0).getInt());
		string model = "res.partner.fingerprint";
		XmlRpcValue param_array_edit = XmlRpcValue::makeArray();
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString("write"));
		param_array_edit.arrayAppendItem(resultados_fingerprint_id);

		//XmlRpcValue array_vals_create = XmlRpcValue::makeArray();

		XmlRpcValue struct_edit = XmlRpcValue::makeStruct();
		struct_edit.structSetValue("hash", XmlRpcValue::makeString(hash.c_str()));
		struct_edit.structSetValue("finger", XmlRpcValue::makeInt(finger));
		struct_edit.structSetValue("template", XmlRpcValue::makeBase64(buffer_template, template_size));
		struct_edit.structSetValue("hand", XmlRpcValue::makeString(hand.c_str()));
		struct_edit.structSetValue("partner_id", XmlRpcValue::makeInt(partner_id));
		struct_edit.structSetValue("template_size", XmlRpcValue::makeInt(template_size));

		//array_vals_create.arrayAppendItem(struct_create);

		param_array_edit.arrayAppendItem(struct_edit);
		XmlRpcValue result_edit = server_edit.call("execute", param_array_edit);

		if(!result_edit.getBool())
			return result_edit.getBool();

		model = "res.partner.fingerprint.image";
		param_array_edit = XmlRpcValue::makeArray();
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString("write"));
		param_array_edit.arrayAppendItem(resultados);

		//XmlRpcValue array_vals_create = XmlRpcValue::makeArray();

		struct_edit = XmlRpcValue::makeStruct();
		//struct_edit.structSetValue("fingerprint_id", XmlRpcValue::makeInt(fingerprint_id));
		struct_edit.structSetValue("image_height", XmlRpcValue::makeInt(image_height));
		struct_edit.structSetValue("image_width", XmlRpcValue::makeInt(image_width));
		struct_edit.structSetValue("image", XmlRpcValue::makeBase64(image, image_size));
		struct_edit.structSetValue("image_size", XmlRpcValue::makeInt(image_size));

		//array_vals_create.arrayAppendItem(struct_create);

		param_array_edit.arrayAppendItem(struct_edit);
		result_edit = server_edit.call("execute", param_array_edit);
		return result_edit.getBool();
	}

	/**
	 * @brief Método actualizarFingerprintWebService: Actualiza un registro en la tabla:
	 * res.partner.fingerprint.
	 *
	 * Función actual para actualizar huellas usa el browse / read / search
	 * @param buffer_template Huella
	 * @param hash Hash
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @param template_size Tamaño Huella
	 * @param image_height Alto imagen
	 * @param image_width Ancho imagen
	 * @param image Imagen
	 * @param image_size Tamaño imagen
	 * @return Retorna Estado de actualización
	 */
	bool actualizarFingerprintWebService(unsigned char * &buffer_template, string hash, int finger, string hand, long int partner_id, int template_size, int image_height, int image_width, unsigned char * &image, long int image_size)
	{
		long int uid = this->getUID();
		XmlRpcClient server_edit (this->URL + "/xmlrpc/object");
		XmlRpcValue resultados_fingerprint_id = this->searchFingerprint(server_edit, finger, hand, partner_id);

		if(resultados_fingerprint_id.arraySize() == 0)
			return false;

		long int fingerprint_id = resultados_fingerprint_id.arrayGetItem(0).getInt();
		XmlRpcValue readFingerprint = this->readFingerprint(server_edit, resultados_fingerprint_id);

		if(readFingerprint.arraySize() == 0)
			return false;

		long int image_id = readFingerprint.arrayGetItem(0).getStruct().structGetValue("image_id").arrayGetItem(0).getInt();
		string model = "webservice.methods";
		XmlRpcValue param_array_edit = XmlRpcValue::makeArray();
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_edit.arrayAppendItem(XmlRpcValue::makeString("write_fingerprint"));

		XmlRpcValue struct_edit = XmlRpcValue::makeStruct();
		XmlRpcValue struct_fingerprint = XmlRpcValue::makeStruct();
		struct_fingerprint.structSetValue("id", XmlRpcValue::makeInt(image_id));
		XmlRpcValue struct_vals = XmlRpcValue::makeStruct();

		string imagen_string = base64(image, image_size);
		struct_vals.structSetValue("image_height", XmlRpcValue::makeInt(image_height));
		struct_vals.structSetValue("image_width", XmlRpcValue::makeInt(image_width));
		struct_vals.structSetValue("image", XmlRpcValue::makeString(imagen_string.c_str()));
		struct_vals.structSetValue("image_size", XmlRpcValue::makeInt(image_size));
		struct_fingerprint.structSetValue("vals", struct_vals);
		struct_edit.structSetValue("fingerprint_image", struct_fingerprint);
		struct_fingerprint = XmlRpcValue::makeStruct();
		struct_fingerprint.structSetValue("id", XmlRpcValue::makeInt(fingerprint_id));
		struct_vals = XmlRpcValue::makeStruct();

		string template_string = base64(buffer_template, template_size);
		//hash = str2md5(template_string.c_str(), template_string.length());
		struct_vals.structSetValue("hash", XmlRpcValue::makeString(hash.c_str()));
		struct_vals.structSetValue("finger", XmlRpcValue::makeInt(finger));
		struct_vals.structSetValue("template", XmlRpcValue::makeString(template_string.c_str()));
		struct_vals.structSetValue("hand", XmlRpcValue::makeString(hand.c_str()));
		struct_vals.structSetValue("partner_id", XmlRpcValue::makeInt(partner_id));
		struct_vals.structSetValue("template_size", XmlRpcValue::makeInt(template_size));

		struct_fingerprint.structSetValue("vals", struct_vals);
		struct_edit.structSetValue("fingerprint", struct_fingerprint);
		param_array_edit.arrayAppendItem(struct_edit);
		XmlRpcValue result_edit = server_edit.call("execute", param_array_edit);
		return result_edit.getBool();
	}

	/**
	 * @brief Método borrarFingerprintImg: Borrar un registro de la tabla:
	 * res.partner.fingerprint.image.
	 * Función para eliminar huellas
	 *
	 * OPTIMIZAR: Puede que no se use porque usa usa res.partner.fingerprint.image en lugar de webservice.methods.
	 * @param finger Dedo
	 * @param hand Mano
	 * @param partner_id Id Partner
	 * @return Retorna Estado de borrado
	 */
	int borrarFingerprintImg(int finger, string hand, long int partner_id)
	{
		long int uid = this->getUID();

		XmlRpcClient server_delete (this->URL + "/xmlrpc/object");

		XmlRpcValue resultados_fingerprint_id = this->searchFingerprint(server_delete, finger, hand, partner_id);

		if(resultados_fingerprint_id.arraySize() == 0)
			return 0;

        //int fingerprint_id = resultados_fingerprint_id.arrayGetItem(0).getInt();

		XmlRpcValue readFingerprint = this->readFingerprint(server_delete, resultados_fingerprint_id);

		if(readFingerprint.arraySize() == 0)
			return 0;

		XmlRpcValue resultados = this->searchFingerprintImage(server_delete, readFingerprint.arrayGetItem(0).getStruct().structGetValue("image_id").arrayGetItem(0).getInt());

		string model = "res.partner.fingerprint.image";

		XmlRpcValue param_array_delete = XmlRpcValue::makeArray();
		param_array_delete.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_delete.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_delete.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_delete.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_delete.arrayAppendItem(XmlRpcValue::makeString("unlink"));
		param_array_delete.arrayAppendItem(resultados);

		XmlRpcValue result_delete = server_delete.call("execute", param_array_delete);

		return result_delete.getBool();
	}
};
