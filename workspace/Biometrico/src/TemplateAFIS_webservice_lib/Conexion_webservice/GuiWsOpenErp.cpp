/**
* @file GuiWsOpenErp.cpp
* @brief Archivo de cabecera de las clases GuiWsOpenErp
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @author Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.
* @date 2014
* This file is part of Sistema de Identificación Biométrica para las OBPP.
*
*/

/**
 * @def NAME
*/
#define NAME	"XML-RPC CFG C++ Client"

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

using namespace std;

/**
 * @class GuiWsOpenErp
 * @brief Clase que permite realizar la interacción con el Webservice con OpenERP. (No están en uso actualmente.)
 */
class GuiWsOpenErp {
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
	string model;
	/*!
	 * @brief URL
	 */
	string URL;
	/*!
	 * @brief uid
	 */
	XmlRpcValue::int32 uid;
		
public:
        /*!
         * \brief Constructor
         * \param dbname Nombre de la base de datos
         * \param user Nombre de usuario
         * \param pwd Contraseña de user
         * \param URL URL
         */
		GuiWsOpenErp (string dbname, string user, string pwd, string URL)
		{
			this->dbname = dbname;
			this->user = user;
			this->pwd = pwd;
			//this->model = "res.partner";
			this->URL = URL;
		}
        /*!
         * \brief Destructor
         */
		~GuiWsOpenErp()
		{
		}
		
        /*!
         * \brief CargarVariables Seteo de variables
         * \param dbname
         * \param user
         * \param pwd
         * \param URL
         */
		void CargarVariables(string dbname, string user, string pwd, string URL) {
			this->dbname = dbname;
			this->user = user;
			this->pwd = pwd;
			//this->model = "res.partner";
			this->URL = URL;
		}

        /*!
         * \brief Inicializar se encarga de la autentificacion contra el OpenERP
         * \return el id del usuario que se autentifica.
         */
		long int Inicializar() { 
			XmlRpcClient::Initialize(NAME, VERSION);
			XmlRpcClient server2 (this->URL + "/xmlrpc/common");
			XmlRpcValue param_array2 = XmlRpcValue::makeArray();
			param_array2.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array2.arrayAppendItem(XmlRpcValue::makeString(user.c_str()));
			param_array2.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			XmlRpcValue result2 = server2.call("login", param_array2);
			//XmlRpcValue::int32 uid = result2.getInt();
			try
			{
				uid = result2.getInt();
				return uid;
			} catch (XmlRpcFault& fault) {
				uid = result2.getBool();
				return uid;
			}
		}

        /*!
         * \brief GetUsuario Este metodo se encarga de buscar en la tabla res.users parametrizado con la variable "usuario"
         * \param usuario Viene de la GUI
         * \return id & login.
         */
		XmlRpcValue GetUsuario(string usuario){
			model = "res.users"; //Tabla
			//SEARCH
			XmlRpcClient server_search (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_search = XmlRpcValue::makeArray();
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("login"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString(usuario.c_str())); // Variable que deberia venir de la GUI.

			array_comp.arrayAppendItem(array_comp_1);

			param_array_search.arrayAppendItem(array_comp);

			XmlRpcValue result_search = server_search.call("execute", param_array_search);

			if(result_search.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			XmlRpcValue resultados = result_search;
		
			//READ RESULT
		
			XmlRpcClient server_read (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read = XmlRpcValue::makeArray();
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read.arrayAppendItem(resultados);

			XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("id"));
			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("login"));
			/*array_vals_read.arrayAppendItem(XmlRpcValue::makeString("password"));*/
			param_array_read.arrayAppendItem(array_vals_read);
			XmlRpcValue result_read = server_read.call("execute", param_array_read);

			return result_read;
		}

        /*!
         * \brief GetDatoPersona Este metodo se encarga de buscar en la tabla res.partner
         * \param cedula_identidad Viene de la GUI
         * \return identification_id & name.
         */
		XmlRpcValue GetDatoPersona(string cedula_identidad){
			model = "res.partner"; //Tabla
			//Consulta SEARCH
			XmlRpcClient server_search (this->URL + "/xmlrpc/object");
		
			XmlRpcValue param_array_search = XmlRpcValue::makeArray();
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("identification_id"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString(cedula_identidad.c_str())); // Variable dinamica desde la GUI, cedula de identidad de la persona 
		
			array_comp.arrayAppendItem(array_comp_1);

			param_array_search.arrayAppendItem(array_comp);

			XmlRpcValue result_search = server_search.call("execute", param_array_search);

			if(result_search.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			XmlRpcValue resultados = result_search;

			// READ LECTURA
			XmlRpcClient server_read (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read = XmlRpcValue::makeArray();
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read.arrayAppendItem(resultados);

			XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("identification_id"));
			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("name"));
			param_array_read.arrayAppendItem(array_vals_read);
			XmlRpcValue result_read = server_read.call("execute", param_array_read);

			return result_read;		
		}

        /*!
         * \brief GetPersonaByOBPP Este metodo se encarga de buscar todas las personas a la cual pertene una obpp, para ello se parametriza la busquedad con el cdigo situr.
         * \param situr_code Viene de la GUI
         * \return Valores de la OBPP
         */
		XmlRpcValue /*string ** */ GetPersonaByOBPP(string situr_code /*, int &reg*/){
			XmlRpcClient::Initialize(NAME, VERSION);
			try
			{
				model = "res.partner"; //Tabla
				//Consulta SEARCH
				XmlRpcClient server_search (this->URL + "/xmlrpc/object");
		

				XmlRpcValue param_array_search = XmlRpcValue::makeArray();
				param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
				param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
				param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
				param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
				param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		
				XmlRpcValue array_comp = XmlRpcValue::makeArray();
				XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
				array_comp_1.arrayAppendItem(XmlRpcValue::makeString("situr_code")); //"08-14-01-001-0004"
				array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
				array_comp_1.arrayAppendItem(XmlRpcValue::makeString(situr_code.c_str())); // Variable dinamica desde la GUI, Codigo SITUR o OBPP 		
				array_comp.arrayAppendItem(array_comp_1);

				param_array_search.arrayAppendItem(array_comp);

				XmlRpcValue result_search = server_search.call("execute", param_array_search);

				XmlRpcValue resultados = result_search;
		
				if(resultados.arraySize() == 0)
				{
					XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
					return array_vals_read_2c;
				}
		
				// READ LECTURA
				XmlRpcClient server_read (this->URL + "/xmlrpc/object");
				XmlRpcValue param_array_read = XmlRpcValue::makeArray();
				param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
				param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
				param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
				param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
				param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
				param_array_read.arrayAppendItem(resultados);

				XmlRpcValue array_vals_read = XmlRpcValue::makeArray();
				array_vals_read.arrayAppendItem(XmlRpcValue::makeString("id"));
				//array_vals_read.arrayAppendItem(XmlRpcValue::makeString("child_ids"));
            array_vals_read.arrayAppendItem(XmlRpcValue::makeString("responsibles_ids"));
				param_array_read.arrayAppendItem(array_vals_read);
		
				XmlRpcValue result_read = server_read.call("execute", param_array_read);

            //cerr << "result_read.arraySize() " << result_read.arraySize() << endl;

				if(result_read.arraySize() == 0)
				{
					XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
					return array_vals_read_2c;
				}

				int index = -1;

				for(int i = 0; i < result_read.arraySize(); i++)
				{
					//XmlRpcValue child_ids = result_read.arrayGetItem(i).structGetValue("child_ids").getArray();
               //cerr << "result_read.arrayGetItem(i).structGetValue(child_ids).getArray().arraySize() " << result_read.arrayGetItem(i).structGetValue("child_ids").getArray().arraySize() << endl;
               //cerr << "result_read.arrayGetItem(i).structGetValue(responsibles_ids).getArray().arraySize() " << result_read.arrayGetItem(i).structGetValue("responsibles_ids").getArray().arraySize() << endl;
					if(result_read.arrayGetItem(i).structGetValue("responsibles_ids").getArray().arraySize() > 0)
					{
						index = i;
						break;
					}
				}

				if(index == -1)
				{
					XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
					return array_vals_read_2c;
				}

            XmlRpcValue resultados_2c = result_read.arrayGetItem(index).structGetValue("responsibles_ids").getArray();

				/*long int id_padre = result_read.arrayGetItem(index).structGetValue("id").getInt();
		
				//Ahora que se tiene el ID padre se Buscan los Hijos de la OBPP asociados al ID
				XmlRpcClient server_search_2c (this->URL + "/xmlrpc/object");
				XmlRpcValue param_array_search_2c = XmlRpcValue::makeArray();
				param_array_search_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
				param_array_search_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
				param_array_search_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
				param_array_search_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
				param_array_search_2c.arrayAppendItem(XmlRpcValue::makeString("search"));
		
				XmlRpcValue array_comp_2c = XmlRpcValue::makeArray();
				XmlRpcValue array_comp_1_2c = XmlRpcValue::makeArray();
				array_comp_1_2c.arrayAppendItem(XmlRpcValue::makeString("parent_id"));
				array_comp_1_2c.arrayAppendItem(XmlRpcValue::makeString("="));
				array_comp_1_2c.arrayAppendItem(XmlRpcValue::makeInt(id_padre)); // Variable dinamica desde la GUI, Codigo SITUR o OBPP 		
				array_comp_2c.arrayAppendItem(array_comp_1_2c);

				param_array_search_2c.arrayAppendItem(array_comp_2c);

				XmlRpcValue result_search_2c = server_search_2c.call("execute", param_array_search_2c);

				if(result_search_2c.arraySize() == 0)
				{
					XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
					return array_vals_read_2c;
				}

				XmlRpcValue resultados_2c = result_search_2c;*/
		
				//Ahora que se tienen los id de cada mienbro asociado a la obpp se procede a sacar los datos relevantes de cada uno.
				//READ
				XmlRpcClient server_read_2c (this->URL + "/xmlrpc/object");
				XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
				param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
				param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
				param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
				param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
				param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
				param_array_read_2c.arrayAppendItem(resultados_2c);

				XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
				array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("name"));
				array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("identification_id"));	
				param_array_read_2c.arrayAppendItem(array_vals_read_2c);

				XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);
				return result_read_2c;

			} catch (XmlRpcFault& fault) {
				//XmlRpcValue resultBD = XmlRpcValue::makeArray();
				cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
				XmlRpcClient::Terminate();
				XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
				return array_vals_read_2c;
			}

			XmlRpcClient::Terminate();

		}

        /*!
         * \brief GetDatoidImagen Este metodo se encarga de obtener el id de un registro en la tabla res.partner.fingerprint
         * para ello la busquedad es parametrizada con partner_id, hand y finger.
         * \param partner_id Viene de la GUI
         * \param hand Viene de la GUI
         * \param finger Viene de la GUI
         * \return result_read es un struct XmlRpcValue
         */
		XmlRpcValue GetDatoidImagen(long int partner_id, string hand, string finger){

			model = "res.partner.fingerprint"; //Tabla
			//Consulta SEARCH
			XmlRpcClient server_search (this->URL + "/xmlrpc/object");
		
			XmlRpcValue param_array_search = XmlRpcValue::makeArray();
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("partner_id"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(partner_id)); // Variable Dinamica Cedula de Identidad

			array_comp.arrayAppendItem(array_comp_1);
			array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("hand"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString(hand.c_str())); // Variable Dinamica Mano
			array_comp.arrayAppendItem(array_comp_1);
			array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("finger"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(atoi(finger.c_str()))); // Variable Dinamica Dedo
			array_comp.arrayAppendItem(array_comp_1);
		
			param_array_search.arrayAppendItem(array_comp);

			XmlRpcValue result_search = server_search.call("execute", param_array_search);

			if(result_search.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			XmlRpcValue resultados = result_search;
		
			XmlRpcClient server_read (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read = XmlRpcValue::makeArray();
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read.arrayAppendItem(resultados);

			XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

			//array_vals_read.arrayAppendItem(XmlRpcValue::makeString("image_id"));
			param_array_read.arrayAppendItem(array_vals_read);

			XmlRpcValue result_read = server_read.call("execute", param_array_read);
			//int id = result_read.arrayGetItem(0).structGetValue("id").getInt();
			//cout << id;

			return result_read;
		}

        /*!
         * \brief getDatoPersonaVerificacion Este metodo se encarga de buscar los datos de la persona, a la cual se le verifica la huella.
         * \param cedula Viene de la GUI
         * \return Datos de verificación para una persona
         */
		XmlRpcValue /*string * */ getDatoPersonaVerificacion(string cedula)
		{
			model = "res.partner"; //Tabla
			//SEARCH
			XmlRpcClient server_search (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_search = XmlRpcValue::makeArray();
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("identification_id"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString(cedula.c_str())); // Variable que deberia venir de la GUI.

			array_comp.arrayAppendItem(array_comp_1);

			param_array_search.arrayAppendItem(array_comp);

			XmlRpcValue result_search = server_search.call("execute", param_array_search);

			if(result_search.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			XmlRpcValue resultados = result_search;
			
			//READ
			XmlRpcClient server_read (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read = XmlRpcValue::makeArray();
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read.arrayAppendItem(resultados);

			XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("name"));
			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("parents_ids"));
			param_array_read.arrayAppendItem(array_vals_read);
			XmlRpcValue result_read = server_read.call("execute", param_array_read);

			if(result_read.arraySize() == 0)
			{
				XmlRpcValue result_read = XmlRpcValue::makeArray();
				return result_read;
			}

			// Nombre de La Persona y Parents_ids que se usa para buscar la 
			string nombre_persona = result_read.arrayGetItem(0).structGetValue("name").getString();

			/*long int parents_ids = result_read.arrayGetItem(0).structGetValue("parents_ids").arrayGetItem(0).getInt();

			//SEARCH

			XmlRpcClient server_search_2 (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_search_2  = XmlRpcValue::makeArray();
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp_2 = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1_2 = XmlRpcValue::makeArray();
			array_comp_1_2.arrayAppendItem(XmlRpcValue::makeString("id"));
			array_comp_1_2.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1_2.arrayAppendItem(XmlRpcValue::makeInt(parents_ids)); // Variable que deberia venir de la GUI.

			array_comp_2.arrayAppendItem(array_comp_1_2);

			param_array_search_2.arrayAppendItem(array_comp_2);

			XmlRpcValue result_search_2 = server_search_2.call("execute", param_array_search_2);

			if(result_search_2.arraySize() == 0)
			{
				XmlRpcValue result_search_2 = XmlRpcValue::makeArray();
				return result_search_2;
			}*/

			XmlRpcValue resultados_2 = result_read.arrayGetItem(0).structGetValue("parents_ids").getArray();

			//READ
			XmlRpcClient server_read_2 (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read_2 = XmlRpcValue::makeArray();
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read_2.arrayAppendItem(resultados_2);

			XmlRpcValue array_vals_read_2 = XmlRpcValue::makeArray();

			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("id"));
			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("name"));
			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("vat"));
			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("situr_code"));
         array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("executor_type_id"));
			param_array_read_2.arrayAppendItem(array_vals_read_2);
			XmlRpcValue result_read_2 = server_read_2.call("execute", param_array_read_2);

			if(result_read_2.arraySize() == 0)
			{
				XmlRpcValue result_read_2 = XmlRpcValue::makeArray();
				return result_read_2;
			}

         long int partner_id;

         for(int i = 0; i < result_read_2.arraySize(); i++)
			{
            XmlRpcValue resultados_3 = result_read_2.arrayGetItem(i).structGetValue("executor_type_id").getArray();
            partner_id = result_read_2.arrayGetItem(i).structGetValue("id").getInt();

            XmlRpcValue resultados_4 = XmlRpcValue::makeArray();
            resultados_4.arrayAppendItem(XmlRpcValue::makeInt(resultados_3.arrayGetItem(0).getInt()));

            //READ
            XmlRpcClient server_read_3 (this->URL + "/xmlrpc/object");
            XmlRpcValue param_array_read_3 = XmlRpcValue::makeArray();
            param_array_read_3.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
            param_array_read_3.arrayAppendItem(XmlRpcValue::makeInt(uid));
            param_array_read_3.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
            param_array_read_3.arrayAppendItem(XmlRpcValue::makeString("cfg.picp.executor_type"));
            param_array_read_3.arrayAppendItem(XmlRpcValue::makeString("read"));
            param_array_read_3.arrayAppendItem(resultados_4);
            //param_array_read_3.arrayAppendItem(XmlRpcValue::makeArray());

            XmlRpcValue array_vals_read_3 = XmlRpcValue::makeArray();

            //array_vals_read_3.arrayAppendItem(XmlRpcValue::makeString("title"));
            //array_vals_read_3.arrayAppendItem(XmlRpcValue::makeString("description"));
            param_array_read_3.arrayAppendItem(array_vals_read_3);

            XmlRpcValue result_read_3 = server_read_3.call("execute", param_array_read_3);

            if(result_read_3.arraySize() > 0)
            {
               result_read_2.arrayGetItem(i).structSetValue("tipo_title", XmlRpcValue::makeString(result_read_3.arrayGetItem(0).structGetValue("title").getString().c_str()));
               result_read_2.arrayGetItem(i).structSetValue("tipo_description", XmlRpcValue::makeString(result_read_3.arrayGetItem(0).structGetValue("description").getString().c_str()));
            }else
            {
               result_read_2.arrayGetItem(i).structSetValue("tipo_title", XmlRpcValue::makeString("0"));
               result_read_2.arrayGetItem(i).structSetValue("tipo_description", XmlRpcValue::makeString("0"));
            }

				result_read_2.arrayGetItem(i).structSetValue("nombre_persona", XmlRpcValue::makeString(nombre_persona.c_str()));

            result_read_2.arrayGetItem(i).structSetValue("proyectos", buscarProyecto(partner_id));
			}

			//result_read_2.arrayGetItem(0).structSetValue("nombre_persona", XmlRpcValue::makeString(nombre_persona.c_str()));

			return result_read_2;

			/*string nombre_obpp  = result_read_2.arrayGetItem(0).structGetValue("name").getString();
			string rif  = result_read_2.arrayGetItem(0).structGetValue("vat").getString();
			string codigo_situr  = result_read_2.arrayGetItem(0).structGetValue("situr_code").getString();
			cout << "nombre obpp " << nombre_obpp << " rif " << rif << " codigo " << codigo_situr << "\n";
			
			//Creando la matriz para retonarla a la gui con los datos: Nombre de la persona, nombre_obpp, rif, codigo_situr.
			string * verificacion = new string[4] ;
			verificacion[0] = nombre_persona;
			verificacion[1] = nombre_obpp;
			verificacion[2] = rif;
			verificacion[3] = codigo_situr;
			return verificacion;*/		
		}

      XmlRpcValue buscarProyecto(int id_obpp)
      {
	      model = "project.project"; //Tabla
	      //Consulta SEARCH
         XmlRpcClient server_search (this->URL + "/xmlrpc/object");
	      //XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

	      XmlRpcValue param_array_search = XmlRpcValue::makeArray();
	      param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->dbname.c_str()));
	      param_array_search.arrayAppendItem(XmlRpcValue::makeInt(this->uid));
	      param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->pwd.c_str()));
	      param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	      param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

	      XmlRpcValue array_comp = XmlRpcValue::makeArray();
	      XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
	      array_comp_1.arrayAppendItem(XmlRpcValue::makeString("partner_id")); //"Campo"
	      array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	      array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(id_obpp)); // Variable dinamica desde la GUI, Codigo SITUR o OBPP 		
	      array_comp.arrayAppendItem(array_comp_1);

	      param_array_search.arrayAppendItem(array_comp);

	      XmlRpcValue result_search = server_search.call("execute", param_array_search);
	      XmlRpcValue resultados = result_search;

	      XmlRpcClient server_read_2c (this->URL + "/xmlrpc/object");
	      XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
	      param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	      param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
	      param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	      param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	      param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
	      param_array_read_2c.arrayAppendItem(resultados);

	      XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
	      //array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("state"));
	      array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("description"));
	      array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("id"));	
	      param_array_read_2c.arrayAppendItem(array_vals_read_2c);		
	      XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);
	
	      return result_read_2c;
      }

        /*!
         * \brief getDatoHuella Este metodo se encarga de buscar las huellas que tiene registrado un integrante de la obpp, cuando se va a verificar.
         * \param cedula
         * \param mano
         * \param dedo
         * \return Retorna mano y dedo por referencia. Y datos de la Huella por XmlRpcValue
         */
		XmlRpcValue /*string ** */getDatoHuella(string cedula, string &mano, int &dedo/*, int &tamano_array*/){
			model = "res.partner"; //Tabla
			//SEARCH
			XmlRpcClient server_search (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_search = XmlRpcValue::makeArray();
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("identification_id"));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1.arrayAppendItem(XmlRpcValue::makeString(cedula.c_str())); // Variable que deberia venir de la GUI.

			array_comp.arrayAppendItem(array_comp_1);

			param_array_search.arrayAppendItem(array_comp);

			XmlRpcValue result_search = server_search.call("execute", param_array_search);

			if(result_search.arraySize() == 0)
			{
				XmlRpcValue result_search = XmlRpcValue::makeArray();
				return result_search;
			}

			XmlRpcValue resultados = result_search;

			//READ
			XmlRpcClient server_read (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read = XmlRpcValue::makeArray();
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read.arrayAppendItem(resultados);

			XmlRpcValue array_vals_read = XmlRpcValue::makeArray();

			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("id"));
			array_vals_read.arrayAppendItem(XmlRpcValue::makeString("identification_id"));
			param_array_read.arrayAppendItem(array_vals_read);
			XmlRpcValue result_read = server_read.call("execute", param_array_read);

			if(result_read.arraySize() == 0)
			{
				XmlRpcValue result_read = XmlRpcValue::makeArray();
				return result_read;
			}

			// Nombre de La Persona y Parent_id que se usa para buscar la 
			long int id = result_read.arrayGetItem(0).structGetValue("id").getInt();
			string cedula_identidad = result_read.arrayGetItem(0).structGetValue("identification_id").getString();

			//SEARCH
			model = "res.partner.fingerprint"; //Tabla
			XmlRpcClient server_search_2 (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_search_2  = XmlRpcValue::makeArray();
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_search_2.arrayAppendItem(XmlRpcValue::makeString("search"));
		
			XmlRpcValue array_comp_2 = XmlRpcValue::makeArray();
			XmlRpcValue array_comp_1_2 = XmlRpcValue::makeArray();
			array_comp_1_2.arrayAppendItem(XmlRpcValue::makeString("partner_id"));
			array_comp_1_2.arrayAppendItem(XmlRpcValue::makeString("="));
			array_comp_1_2.arrayAppendItem(XmlRpcValue::makeInt(id)); // Variable que deberia venir de la GUI.

			array_comp_2.arrayAppendItem(array_comp_1_2);

			param_array_search_2.arrayAppendItem(array_comp_2);

			XmlRpcValue result_search_2 = server_search_2.call("execute", param_array_search_2);

			if(result_search_2.arraySize() == 0)
			{
				XmlRpcValue result_search_2 = XmlRpcValue::makeArray();
				return result_search_2;
			}

			XmlRpcValue resultados_2 = result_search_2;

			//READ
			XmlRpcClient server_read_2 (this->URL + "/xmlrpc/object");
			XmlRpcValue param_array_read_2 = XmlRpcValue::makeArray();
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeInt(uid));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
			param_array_read_2.arrayAppendItem(XmlRpcValue::makeString("read"));
			param_array_read_2.arrayAppendItem(resultados_2);

			XmlRpcValue array_vals_read_2 = XmlRpcValue::makeArray();

			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("template"));
			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("hand"));
			array_vals_read_2.arrayAppendItem(XmlRpcValue::makeString("finger"));
			param_array_read_2.arrayAppendItem(array_vals_read_2);
			XmlRpcValue result_read_2 = server_read_2.call("execute", param_array_read_2);

			if(result_read_2.arraySize() == 0)
			{
				XmlRpcValue result_read_2 = XmlRpcValue::makeArray();
				return result_read_2;
			}else 
			if(result_read_2.arraySize() == 1)
			{
				mano = result_read_2.arrayGetItem(0).structGetValue("hand").getString();
				dedo = result_read_2.arrayGetItem(0).structGetValue("finger").getInt();
			}
			return result_read_2;
		}

};
