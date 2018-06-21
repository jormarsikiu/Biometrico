/**
* @file TemplateAFIS.h
* @brief Es el archivo de cabecera (header) para TemplateAFIS_webservice.
* Conecta llos archivos de las carpetas de firma_digital,
* Conexion_webservice, y otras cabeceras .h del include.
*
* OPTIMIZAR: Revisar las  bibliotecas, porque es posible que alguna ya no esté en uso.
*
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @authors Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.
* @version 1.0
* @date 2014
* This file is part of Sistema de Identificación Biométrica para las OBPP.
*/

#ifndef _TemplateAFIS_H_
#define _TemplateAFIS_H_

/*Las bibliotecas  de XML-RPC y JPEG que son para conectarse a un webservice y para las imágenes:*/
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <xmlrpc.h>
#include <xmlrpc_cgi.h>
#include <XmlRpcCpp.h>
#include <jpeglib.h>
#include <stdlib.h>

/*Bibliotecas son para la conexión encriptada (por SSL):*/
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <sstream>

/*Cabeceras específicas del captahuellas y archivos generados para el uso del captahuellas:*/
#include "UFMatcher.h"
#include "UFScanner.h"
#include "../firma_digital/FirmaDigital.h"
#include "Utilidades.h"
//#include "../Conexion_c++/Variables.cpp"
#include "../Conexion_webservice/Variables_Webservice_OpenERP.cpp"
#include "ws.h"

/*!
 * @brief MAX_TEMPLATE_SIZE
 */
#define MAX_TEMPLATE_SIZE	       384
/*!
 * @brief BD_SIN_DATOS
 */
#define BD_SIN_DATOS		         -661
/*!
 * @brief BD_COUNT_CERO
 */
#define BD_COUNT_CERO		      -662
/*!
 * @brief BD_COUNT_MAYOR_CERO
 */
#define BD_COUNT_MAYOR_CERO	   -663
/*!
 * @brief BD_TRANSACTION_ERROR
 */
#define BD_TRANSACTION_ERROR	   -664
/*!
 * @brief UFS_POCA_CALIDAD_IMAGEN
 */
#define UFS_POCA_CALIDAD_IMAGEN	-761
/*!
 * @brief UFM_IDENTIFICATION_ERROR
 */
#define UFM_IDENTIFICATION_ERROR -861
/*!
 * @brief UFM_VERIFICATION_ERROR
 */
#define UFM_VERIFICATION_ERROR   -961

/*!
 * @brief NAME
 */
#define NAME       "XML-RPC CFG C++ Client"
/*!
 * @brief VERSION
 */
#define VERSION    "0.1"

/**
 * @class Imagen
 * @brief Clase Imagen: Almacena las imágenes y variables.
 */
class Imagen
{
	public:
		/*!
		* @brief imagen
		*/
		unsigned char * imagen;
		/*!
		* @brief nWidth
		*/
		int nWidth;
		/*!
		* @brief nHeight
		*/
		int nHeight;
		/*!
		* @brief nSize
		*/
		long int nSize;
		/*!
		* @brief id
		*/
		string id;
		/*!
		* @brief id_persona
		*/
		string id_persona;
		/*!
		* @brief id_dedo
		*/
		string id_dedo;
		/*!
		* @brief mano
		*/
		string mano;

		/**
		* @brief Constructor
		*/
		Imagen(string id, unsigned char * imagen, int nWidth, int nHeight, long int nSize, string id_persona, string id_dedo, string mano);

		/**
		* @brief Destructor
		*/
		~Imagen();

		/**
		* @brief Get Imagen
		* @param imagen Una imágen cualquiera (parámetro de prueba)
		* @return Obtiene la imágen. Este es el resultado.
		*/
		unsigned char * getImagen(unsigned char * &imagen);

		/**
		* @brief Get Width
		* @return Obtiene ancho de la imágen.
		*/
		int getWidth();

		/**
		* @brief Get Height
		* @return Obtiene alto de la imágen.
		*/
		int getHeight();

		/**
		* @brief Get Size
		* @return Obtiene tamaño de la imágen.
		*/
		long int getSize();

		/**
		* @brief Get Id
		* @return Obtiene Id de la imágen en la Base de Datos.
		*/
		string getId();

		/**
		* @brief Get Id Persona
		* @return Obtiene Id de la persona asociada a la imágen en la Base de Datos.
		*/
		string getIdPersona();

		/**
		* @brief Get Id Persona
		* @return Obtiene Id del dedo asociado a la imágen en la Base de Datos.
		*/
		string getIdDedo();

		/**
		* @brief Get Id Persona
		* @return Obtiene mano asociada a la imágen en la Base de Datos.
		*/
		string getMano();

		/**
		* @brief Get Id Persona
		* @return Obtiene el tamaño del objeto Imagen.
		*/
		long int sizeofObject();
};

/**
 * @class TemplateAFIS_webservice
 * Clase TemplateAFIS_webservice: Realiza las funciones de Identify, 
 * Verify y Enroll, y almacena las huellas en la Base de Datos.
 */

class TemplateAFIS_webservice {
    private:
	/*!
	* @brief m_hMatcher
	*/
	HUFMatcher m_hMatcher;
	/*!
	* @brief tipo_template
	*/
	int tipo_template;
	/*!
	* @brief m_nSecurityLevel
	*/
	int m_nSecurityLevel;
	/*!
	* @brief m_bFastMode
	*/
	bool m_bFastMode;
	/*!
	* @brief m_strError[128]
	*/
	char m_strError[128];
	/*!
	* @brief m_nEnrollQuality
	*/
	int m_nEnrollQuality;
	/*!
	* @brief m_nCurScannerIndex
	*/
	int m_nCurScannerIndex;

   /*!
	* @brief base64_chars
	*/
   std::string base64_chars;

   /*!
	* @brief padding
	*/
   int padding;

	/**
	* @brief Convert_raw_jpeg
	* @param filename Donde se guardará la imágen.
	* @param raw Binario a convertir.
	* @param nWidth Ancho de imágen.
	* @param nHeight Alto de imágen.
	* @return Estado de conversión.
	*/
        int convert_raw_jpeg(string filename, unsigned char *raw, int nWidth, int nHeight);

    public:

	/*!
	* @brief firma_figital
	*/
	FirmaDigital * firma_digital;

	/**
	* @brief Constructor
	*/
	TemplateAFIS_webservice(string param1, string param2, string param3, string param4, string param5);

	/**
	* @brief Destructor
	*/
	~TemplateAFIS_webservice();

	/**
	* @brief pElMismo
	* @return Obtiene a si mismo.
	*/
	void* pElMismo();

	/**
	* @brief Set Tipo Template
	* @param tipo Tipo de template
	*/
	void SetTipoTemplate(int tipo);
	/**
	* @brief Save Template2ISO: Salva el Template a ISO. (En pruebas)
	*/
	void SaveTemplate2ISO(); //string FileName
	/**
	* @brief Save ImagenWSQ: Salva el Template a WSQ. (En pruebas)
	*/
	void SaveImagenWSQ();

	/**
	* @brief Get Security Level
	* @return Obtiene el nivel de seguridad para el Matcher, asociado al captahuellas.
	*/
	int GetSecurityLevel();

	/**
	* @brief Get Fast Mode
	* @return Obtiene el nivel de rapidez para el Matcher, asociado al captahuellas.
	*/
	int GetFastMode();

	/**
	* @brief Get Matcher Settings: Muestra la configuración del Matcher (Pruebas)
	*/
	void GetMatcherSettings();

	/**
	* @brief Init Matcher
	* @return Obtiene el estado de inicialización del Matcher. True o False
	*/
	bool initMatcher();

	/**
	* @brief unInit Matcher
	* @return Obtiene el estado de desinicialización del Matcher. True o False
	*/
	bool unInitMatcher();

	/**
	* @brief On Get Parameters
	* @return Muestra los parámetros del Matcher
	*/
	void OnGetParameters();

	/**
	* @brief On Set Template Type
	* @param value Número de tipo de Template. 1 para SUPREMA, 2 para ISO19794_2, 3 para ANSI378
	* @return Estado luego de establecer el tipo de Template.
	*/
	int OnSetTemplateType(int value);

	/**
	* @brief On Enroll
	* @param usuario Usuario URE
	* @param password Password URE
	* @param id_persona Cédula persona
	* @param id_dedo Dedo
	* @param mano Mano
	* @param hash Hash
	* @param m_template Huella a guardar
	* @param m_template_size Tamaño de huella a guardar
	* @param pImageData Imágen de huella a guardar
	* @param nWidth Ancho de imágen de huella a guardar
	* @param nHeight Alto de imágen de huella a guardar
	* @param id_persona_out Si la huella está repetida, Cédula de la persona donde está repetida.
	* @param id_dedo_out Si la huella está repetida, Dedo de la persona donde está repetida.
	* @param mano_out Si la huella está repetida, Mano de la persona donde está repetida.
	* @return Estado de guardado.
	*/
	int OnEnroll(string usuario, string password, string id_persona, string id_dedo, string mano, string hash,
			unsigned char * &m_template, int m_template_size, 
			unsigned char* &pImageData, int nWidth, int nHeight, 
			string &id_persona_out, string &id_dedo_out, string &mano_out);

	/**
	* @brief On Verify: Verifica una huella. Si la huella no es la verificada, utiliza el Médoto OnIdentifyOnVerify, para obtener donde está, obteniendo cedula, dedo, mano y código Situr
	* @param usuario Usuario URE
	* @param password Password URE
	* @param id_persona Cédula persona
	* @param id_dedo Dedo
	* @param mano Mano
	* @param Template Huella a verificar
	* @param TemplateSize Tamaño de huella a verificar
	* @param cedula Si no es la huella, Cédula de la persona donde está la huella.
	* @param dedo Si no es la huella, Dedo de la persona donde está la huella.
	* @param hand Si no es la huella, Mano de la persona donde está la huella.
	* @param situr Si no es la huella, Situr de la persona donde está la huella.
	* @return Estado de verificacion.
	*/
	int OnVerify(string usuario, string password, string id_proyecto, string id_persona, string id_dedo, string mano,
			unsigned char * &Template, int TemplateSize, string &cedula, string &dedo, string &hand, string &situr);

	/**
	* @brief iteracionIdentify: Identifica usando verificación en cada iteración. Se utiliza con OnIdentifyVerify (No OnIdentifyOnVerify. iteacionIdentify, no se utiliza en esta versión)
	* @param estado Estado de la verificación
	* @param i Id de la huella en la Base de Datos
	* @param id_persona_result Cédula encontrada
	* @param id_dedo_result Dedo encontrado
	* @param mano_result Mano encontrada
	* @param id_partner Id partner a comparar
	* @param id_dedo Dedo a comparar
	* @param mano Mano a comparar
	* @param Template Huella a verificar
	* @param TemplateSize Tamaño de huella a verificar
	* @param ufm_res Estado de la verificación.
	* @param bVerifySucceed Verificación exitosa (o no)
	* @return Estado de verificacion.
	*/
	/*int iteracionIdentify(bool &estado, long int &i, string &id_persona_result, string &id_dedo_result, string &mano_result, long int &id_partner, string &id_dedo, string &mano, unsigned char * &Template, int &TemplateSize, int &ufm_res, int &bVerifySucceed);*/

	/**
	* @brief OnIdentifyVerify: Identifica usando verificación en cada iteración. Se utiliza con iteacionIdentify (OnIdentifyVerify, no se utiliza en esta versión)
	* @param usuario Usuario URE
	* @param password Password URE
	* @param Template Huella a verificar
	* @param TemplateSize Tamaño de huella a verificar
	* @param cedula Cédula de la persona a verificar
	* @param id_dedo Dedo de la persona a verificar
	* @param mano Mano de la persona a verificar
	* @param cedula_out Cédula de la persona encontrada
	* @param dedo_out Dedo de la persona encontrada
	* @param hand_out Mano de la persona encontrada
	* @return Estado de verificacion (Identificación).
	*/
	/*int OnIdentifyVerify(string usuario, string password, unsigned char * &Template, int TemplateSize, string cedula, string id_dedo, string mano, string &cedula_out, string &dedo_out, string &hand_out);*/

	/**
	* @brief OnIdentify: Identifica usando el método de identificación de BioSuprema
	* @param usuario Usuario URE
	* @param password Password URE
	* @param Template Huella a identificar
	* @param TemplateSize Tamaño de huella a identificar
	* @param cedula_in Cédula de la persona, cuando se va a buscar por estado.
	* @param cedula Cédula de la persona encontrada.
	* @param dedo Dedo de la persona encontrada
	* @param hand Mano de la persona encontrada
	* @return Estado de identificación.
	*/
	int OnIdentify(string usuario, string password, unsigned char * Template, int TemplateSize, string cedula_in, string &cedula, string &dedo, string &hand);

	/**
	* @brief OnIdentifyOnVerify: Identifica usando el método de identificación de BioSuprema. Se utiliza con OnVerify. Si la verificación falla, llama a este método.
	* @param usuario Usuario URE
	* @param password Password URE
	* @param Template Huella a identificar
	* @param TemplateSize Tamaño de huella a identificar
	* @param cedula_in Cédula de la persona, cuando se va a buscar por estado.
	* @param cedula Cédula de la persona encontrada.
	* @param dedo Dedo de la persona encontrada
	* @param hand Mano de la persona encontrada
	* @param situr Código SITUR de la persona encontrada
	* @return Estado de identificación.
	*/
	int OnIdentifyOnVerify(string usuario, string password, unsigned char * &Template, int TemplateSize, string cedula_in, string &cedula, string &dedo, string &hand, string &situr);

	/**
	* @brief Salvar JPEG: Salva una imágen a JPEG.
	* @param usuario Usuario URE
	* @param password Password URE
	* @param id_persona Cédula de la persona
	* @param id_dedo Dedo de la persona
	* @param mano Mano de la persona
	* @param imagen_buffer Buffer donde se guardará la imágen
	* @param width Ancho de Imágen
	* @param height Alto de Imágen
	* @param size Tamaño de Imágen
	* @param id_imagen Id de la Imágen
	* @return Estado.
	*/
	int salvarJPEG(string usuario, string password, string id_persona, string id_dedo, string mano, unsigned char * &imagen_buffer, int &width, int &height, int &size, string &id_imagen);

	/**
	* @brief Comprobar: Comprueba que haga login.
	* @param usuario Usuario URE
	* @param password Password URE
	* @return Estado de login.
	*/
	bool comprobar(string usuario, string password);

	/**
	 * @brief Get Usuario
	 * @param env Variable XMLRPC de ambiente
	 * @param usuario Login
	 * @param password Password
	 * @param myArrayP Variable XMLRPC que volverá con los valores
	 */
	void getUsuario(xmlrpc_env * env, string usuario, string password, xmlrpc_value * &myArrayP);

	/**
	 * @brief Get Datos Huella
	 * @param env Variable XMLRPC de ambiente
	 * @param usuario Login
	 * @param password Password
	 * @param cedula Cédula
	 * @param myArrayP Variable XMLRPC que volverá con los valores
	 */
	void getDatosHuella(xmlrpc_env *env, string usuario, string password, string cedula, xmlrpc_value * &myArrayP);

	/**
	 * @brief Get Datos Persona OBPP
	 * @param env Variable XMLRPC de ambiente
	 * @param usuario Login
	 * @param password Password
	 * @param codigo_obpp Código OBPP
	 * @param myArrayP Variable XMLRPC que volverá con los valores
	 */
	void getDatosPersonaOBPP(xmlrpc_env *env, string usuario, string password, string codigo_obpp, xmlrpc_value * &myArrayP);

	/**
	 * @brief Get Datos Persona
	 * @param env Variable XMLRPC de ambiente
	 * @param usuario Login
	 * @param password Password
	 * @param cedula Cédula
	 * @param myArrayP Variable XMLRPC que volverá con los valores
	 */
	void getDatosPersona(xmlrpc_env *env, string usuario, string password, string cedula, xmlrpc_value * &myArrayP);

	/**
	 * @brief Get Datos Persona Verificacion
	 * @param env Variable XMLRPC de ambiente
	 * @param usuario Login
	 * @param password Password
	 * @param cedula Cédula
	 * @param myArrayP Variable XMLRPC que volverá con los valores
	 */
	void getDatosPersonaVerificacion(xmlrpc_env *env, string usuario, string password, string cedula, xmlrpc_value * &myArrayP);

	/**
	 * @brief Get Datos Id Imagen
	 * @param env Variable XMLRPC de ambiente
	 * @param usuario Login
	 * @param password Password
	 * @param cedula Cédula
	 * @param mano Mano
	 * @param id_dedo Dedo
	 * @param myArrayP Variable XMLRPC que volverá con los valores
	 */
	void getDatosIdImagen(xmlrpc_env *env, string usuario, string password, string cedula, string mano, string id_dedo, xmlrpc_value * &myArrayP);

	/**
	 * @brief Borrar Huella
	 * @param usuario Login
	 * @param password Password
	 * @param cedula Cédula
	 * @param mano Mano
	 * @param id_dedo Dedo
	 */
	bool borrarHuella(string usuario, string password, string cedula, string mano, string id_dedo);

	/**
	 * @brief Archivo Webservice Config
	 * @param directorio Directorio donde está el archivo
	 * @param config_url Nombre de archivo
	 * @param default_url Nombre de archivo
	 * @param params Parámetros que retorna.
	 */
	void archivo_webservice_config(string directorio, string config_url, string default_url, string * &params);

	/**
	 * @brief Get Webservice OpenERP: Objeto que hace conexión con el Webservice de OpenERP.
	 * @param usuario Usuario URE
	 * @param pass Password URE
	 * @param fing Objeto Fingerprint
	 */
	void getWebServiceOpenERP(string usuario, string pass, Fingerprint * &fing);

	/**
	 * @brief Get Webservice GUI OpenERP: Objeto que hace conexión con el Webservice de OpenERP.
	 * @param usuario Usuario URE
	 * @param pass Password URE
	 * @param fing Objeto GuiWsOpenErp
	 */
	void getWebServiceGUIOpenERP(string usuario, string pass, GuiWsOpenErp * &fing);

    /**
	 * @brief base64_decodestring
	 * @param text Texto original
	 * @param out Base64 decode Out
	 * @param tlen largo de Base64
	 */
	void base64_decodestring(std::string text, unsigned char * &out, size_t &tlen);

   /**
	 * @brief is_base64
	 * @param c Caracter base64
	 */
   bool is_base64(unsigned char c);

   /**
	 * @brief base64_decode Para encriptación o desencriptación
	 * @param encoded_string Cadena codificada en base64
	 */
   std::string base64_decode_crypt(std::string const& encoded_string);

   /**
	 * @brief base64_encode Para encriptación o desencriptación
	 * @param bytes_to_encode Unsigned char * en base64
	 * @param in_len Tamaño
	 */
   string base64_encode_crypt(unsigned char const* bytes_to_encode, unsigned int in_len);

   /**
	 * @brief createRSA
	 * @param key Clave
	 * @param publica Si es o no pública
	 */
   RSA * createRSA(unsigned char * key,int publica);

   /**
	 * @brief public_encrypt
	 * @param data Data a encriptar
	 * @param data_len Tamaño de data a encriptar
	 * @param key Llave pública
	 * @param encrypted Resultado de la encriptación
	 */
   int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);

   /**
	 * @brief private_decrypt
	 * @param enc_data Data encriptada
	 * @param data_len Tamaño de data encriptada
	 * @param key Llave privada
	 * @param decrypted Resultado de la desencriptación
	 */
   int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);

   /**
	 * @brief private_encrypt
	 * @param data Data a encriptar
	 * @param data_len Tamaño de data a encriptar
	 * @param key Llave privada
	 * @param encrypted Resultado de la encriptación
	 */
   int private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted);

   /**
	 * @brief public_decrypt
	 * @param enc_data Data encriptada
	 * @param data_len Tamaño de data encriptada
	 * @param key Llave pública
	 * @param decrypted Resultado de la desencriptación
	 */
   int public_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted);

   /**
	 * @brief leerTodoUnArchivo
	 * @param nombre_archivo Nombre de archivo a abrir
	 */
   string leerTodoUnArchivo(string nombre_archivo);

   /**
	 * @brief desencriptarPrivada
	 * @param encriptado_base64 Nombre de archivo a abrir
	 */
   string desencriptarPrivada(string encriptado_base64);
   
   int guardarIncidencia(string usuario, string password, string id_persona, string id_dedo, string mano, string id_proyecto);
};

#endif
