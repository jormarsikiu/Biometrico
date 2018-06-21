/**
* @file ws.cpp
* @brief Archivo de los métodos del estado proyecto
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @authors Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.
* @date 2014
* @version 2.0
* This file is part of Sistema de Identificación Biométrica para las OBPP.
*/

#define NAME       "XML-RPC CFG C++ Client"
#define VERSION    "0.1"

#include  "ws.h"
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
#include <iostream>

using namespace std;

estado_proyecto::estado_proyecto()
{
}

estado_proyecto::~estado_proyecto()
{
}

void estado_proyecto::set_variables(string dbname, string user, string pwd, string URL, string URL2)
{
	this->dbname = dbname;
	this->user = user;
	this->pwd = pwd;
	this->URL = URL;
	this->URL2 = URL2;
}

int estado_proyecto::inicializar()
{	
	
	XmlRpcClient::Initialize(NAME, VERSION);
	XmlRpcClient server2 (this->URL + "/xmlrpc/common");
	XmlRpcValue param_array2 = XmlRpcValue::makeArray();
	param_array2.arrayAppendItem(XmlRpcValue::makeString(this->dbname.c_str()));
	param_array2.arrayAppendItem(XmlRpcValue::makeString(this->user.c_str()));
	param_array2.arrayAppendItem(XmlRpcValue::makeString(this->pwd.c_str()));
	XmlRpcValue result2 = server2.call("login", param_array2);
	this->uid = result2.getInt();
	return this->uid;
}

int estado_proyecto::buscar_id_obpp(string cedula)
{
	model = "sinco.obpp.responsible"; //Tabla
	//Consulta SEARCH
	XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

	XmlRpcValue param_array_search = XmlRpcValue::makeArray();
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->dbname.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeInt(this->uid));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->pwd.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

	XmlRpcValue array_comp = XmlRpcValue::makeArray();
	XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("identification_id")); //"Campo"
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString(cedula.substr(1))); // Variable dinamica desde la GUI, Codigo SITUR o OBPP
	array_comp.arrayAppendItem(array_comp_1);

	param_array_search.arrayAppendItem(array_comp);

	XmlRpcValue result_search = server_search.call("execute", param_array_search);
	XmlRpcValue resultados = result_search;

	// READ LECTURA
	XmlRpcClient server_read_2c (this->URL2 + "/xmlrpc/object");
	XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
	param_array_read_2c.arrayAppendItem(resultados);

	XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
	//array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("parents_ids"));
	array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("id"));	
	param_array_read_2c.arrayAppendItem(array_vals_read_2c);
    
	XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);
    
	//this->parent_id = result_read_2c.arrayGetItem(0).structGetValue("parents_ids").arrayGetItem(0).getInt();
	this->id_la_tabla_res_partner = result_read_2c.arrayGetItem(0).structGetValue("id").getInt();
	//cerr << "id_la_tabla_res_partner " << this->id_la_tabla_res_partner << endl;
	return this->parent_id;

}

string estado_proyecto::buscar_estado_proyecto(int id_proyecto)
{
	model = "sinco.project"; //Tabla
	//Consulta SEARCH
	XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

	XmlRpcValue param_array_search = XmlRpcValue::makeArray();
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->dbname.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeInt(this->uid));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->pwd.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str())); 
	param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

	XmlRpcValue array_comp = XmlRpcValue::makeArray();
	XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
	/*array_comp_1.arrayAppendItem(XmlRpcValue::makeString("partner_id")); //"Campo"
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(id_obpp));*/ // Variable dinamica desde la GUI, Codigo SITUR o OBPP
   array_comp_1.arrayAppendItem(XmlRpcValue::makeString("id")); //"Campo"
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(id_proyecto));
	array_comp.arrayAppendItem(array_comp_1);

	param_array_search.arrayAppendItem(array_comp);

	XmlRpcValue result_search = server_search.call("execute", param_array_search);
	XmlRpcValue resultados = result_search;

    
	
	XmlRpcClient server_read_2c (this->URL2 + "/xmlrpc/object");
	XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
	param_array_read_2c.arrayAppendItem(resultados);

	XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
	array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("state"));
	array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("id"));	
	param_array_read_2c.arrayAppendItem(array_vals_read_2c);		
	XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);

	int total = result_read_2c.arraySize();
	bool aprobados = false;
	string estado = "";
	int actual = 0;
    bool ejecucion = false;
    bool culminados = false;
    bool incidencia = false;

    //implementation

 

	if(total == 0)
	{
		return estado;
	}
    
    estado = result_read_2c.arrayGetItem(0).structGetValue("state").getString();

    /*
	for(int i = 0; i < total; i++)
	{
		estado = result_read_2c.arrayGetItem(i).structGetValue("state").getString();
  
		if(strcmp(estado.c_str(), "approved") == 0)
		{
			aprobados=true;
			actual = i;
			break;
		}else if(strcmp(estado.c_str(), "implementation") == 0)
        {
            ejecucion=true;
			actual = i;
			break;
        }else if(strcmp(estado.c_str(), "finished") == 0)
        {
            culminados=true;
			actual = i;
			break;
        }else if(strcmp(estado.c_str(), "incidencia") == 0)
		{
			incidencia=true;
			actual = i;
			break;
		}
	}

	estado = "";

	if(aprobados)
	{
		estado = "approved";
		//string estado = result_read_2c.arrayGetItem(0).structGetValue("state").getString();
		this->id_del_proyecto = result_read_2c.arrayGetItem(actual).structGetValue("id").getInt();
		//cerr << this->id_del_proyecto << endl;
	}else if(ejecucion)
	{
		estado = "implementation";
		//string estado = result_read_2c.arrayGetItem(0).structGetValue("state").getString();
		this->id_del_proyecto = result_read_2c.arrayGetItem(actual).structGetValue("id").getInt();
		//cerr << this->id_del_proyecto << endl;
	}else if(culminados)
	{
		estado = "finished";
		//string estado = result_read_2c.arrayGetItem(0).structGetValue("state").getString();
		this->id_del_proyecto = result_read_2c.arrayGetItem(actual).structGetValue("id").getInt();
		//cerr << this->id_del_proyecto << endl;
	}else if(incidencia)
	{
		estado = "incidencia";
		//string estado = result_read_2c.arrayGetItem(0).structGetValue("state").getString();
		this->id_del_proyecto = result_read_2c.arrayGetItem(actual).structGetValue("id").getInt();
		//cerr << this->id_del_proyecto << endl;
	}
    */
	
	return estado;
}

int estado_proyecto::buscar_accountability(int id_proyecto)
{
	/* Consulta para ubicar el proyecto:
	* 1- Se establece la tabla Odoo
	* 2- Se arma la consulta XML. Se le pasan los datos de la tabla.
	* 3- (Línea 261) La consulta se añade a un array: param_array_search.arrayAppendItem(array_comp);
	* 4- (Línea 262) Se ejecuta en: XmlRpcValue result_search = server_search.call("execute", param_array_search);
	* 5- (Línea 263) Se asigna a una variable: XmlRpcValue resultados = result_search;
	*
	* Luego en base a ese resultado se hace READ nuevamente al registro solicitando los datos específicos.
	*
	* De no haber rendido cuenta, se retorna cero y no se procede a capturar huellas ni
	* validar ni nada: if(total == 0)*/

	model = "cfg.picp.accountability"; //Tabla	
	//Consulta SEARCH
	XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

	XmlRpcValue param_array_search = XmlRpcValue::makeArray();
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->dbname.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeInt(this->uid));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(this->pwd.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

	XmlRpcValue array_comp = XmlRpcValue::makeArray();
	XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("project_id")); //"Campo"
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(id_proyecto)); // Variable dinamica desde la GUI, Codigo SITUR o OBPP 		
    array_comp.arrayAppendItem(array_comp_1);
    XmlRpcValue array_comp_2 = XmlRpcValue::makeArray();
	array_comp_2.arrayAppendItem(XmlRpcValue::makeString("is_checked")); //"Campo"
	array_comp_2.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_2.arrayAppendItem(XmlRpcValue::makeBool(false)); // Variable dinamica desde la GUI, Codigo SITUR o OBPP 		
	array_comp.arrayAppendItem(array_comp_2);

	param_array_search.arrayAppendItem(array_comp);
	XmlRpcValue result_search = server_search.call("execute", param_array_search);
	XmlRpcValue resultados = result_search;

	XmlRpcClient server_read_2c (this->URL2 + "/xmlrpc/object");
	XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
	param_array_read_2c.arrayAppendItem(resultados);

	XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
	array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("id"));	
	param_array_read_2c.arrayAppendItem(array_vals_read_2c);		
	XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);

	int total = result_read_2c.arraySize();

	if(total == 0)
	{
		return 0;
	}

	int id = result_read_2c.arrayGetItem(0).structGetValue("id").getInt();
	return id;
}



void estado_proyecto::set_id_de_res_partner_fingerprint(string mano, int id_la_tabla_res_partner, int dedo )
{
	model = "res.partner.fingerprint"; //Tabla
	XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

	XmlRpcValue param_array_search = XmlRpcValue::makeArray();
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

	XmlRpcValue array_comp = XmlRpcValue::makeArray();
	XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("hand"));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString(mano)); // Variable Dinamica Mano
	array_comp.arrayAppendItem(array_comp_1);
	array_comp_1 = XmlRpcValue::makeArray();
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("partner_id"));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(id_la_tabla_res_partner)); 
	array_comp.arrayAppendItem(array_comp_1);
	array_comp_1 = XmlRpcValue::makeArray();
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("finger"));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(dedo)); // Variable Dinamica Dedo
	array_comp.arrayAppendItem(array_comp_1);

	param_array_search.arrayAppendItem(array_comp);
	XmlRpcValue result_search = server_search.call("execute", param_array_search);
	XmlRpcValue resultados = result_search;


	// READ LECTURA
	XmlRpcClient server_read_2c (this->URL2 + "/xmlrpc/object");
	XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
	param_array_read_2c.arrayAppendItem(resultados);

	XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();
	array_vals_read_2c.arrayAppendItem(XmlRpcValue::makeString("id"));	
	param_array_read_2c.arrayAppendItem(array_vals_read_2c);		
	XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);

	this->id_res_partner_fingerprint = result_read_2c.arrayGetItem(0).structGetValue("id").getInt();
	//cout << this->id_res_partner_fingerprint << endl;
	

}
/**@brief Proceso para la validacion_verificación:
* -# Se realiza una busqueda en el método estado_proyecto y se ubica el ID de la rendición de cuentas
* -# La variable accountability_id se pasa de argumento este metodo validación_verificación
* -# Se ubica el proyecto que tiene esa rendción de cuenta para saber si hay verificaciones duplicadas
* -# Es para auditar las veces que la gente va a la URE.
* -# El metodo validacion_verificacion verifica si se hizo la rendicion de cuenta del proyecto
* -# La huella evita el repudio de la rendición de cuentas.
* */
bool estado_proyecto::validacion_verificacion(int fingerprint_id, int project_id, string estatus_proyecto, int accountability_id = 0)
{

	model = "res.partner.fingerprint.verification"; //Tabla
	XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

	XmlRpcValue param_array_search = XmlRpcValue::makeArray();
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

	XmlRpcValue array_comp = XmlRpcValue::makeArray();
	XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();

	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("fingerprint_id"));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(fingerprint_id));
	array_comp.arrayAppendItem(array_comp_1);
	array_comp_1 = XmlRpcValue::makeArray();
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("project_id"));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
	array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(project_id)); 
	array_comp.arrayAppendItem(array_comp_1);

   if(strcmp(estatus_proyecto.c_str(), "") != 0)
   {
      array_comp_1 = XmlRpcValue::makeArray();
      array_comp_1.arrayAppendItem(XmlRpcValue::makeString("project_state"));
      array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
      array_comp_1.arrayAppendItem(XmlRpcValue::makeString(estatus_proyecto.c_str()));
      array_comp.arrayAppendItem(array_comp_1);
   }

   if(accountability_id > 0)
   {
      array_comp_1 = XmlRpcValue::makeArray();
      array_comp_1.arrayAppendItem(XmlRpcValue::makeString("accountability_id"));
      array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
      array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(accountability_id));
      array_comp.arrayAppendItem(array_comp_1);
   }

	param_array_search.arrayAppendItem(array_comp);
	XmlRpcValue result_search = server_search.call("execute", param_array_search);
	XmlRpcValue resultados = result_search;

	XmlRpcClient server_read_2c (this->URL2 + "/xmlrpc/object");
	XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
	param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
	param_array_read_2c.arrayAppendItem(resultados);

	XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();	
	param_array_read_2c.arrayAppendItem(array_vals_read_2c);
	XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);
	
	int numero_registros = result_read_2c.arraySize();
	if (numero_registros !=0)
	{
		// Retorna false si el registro esta duplicado.
		return true;
		
	}
	else
	{
		// Retorna true si el registro no esta duplicado.
		return false;
		
	}

}

int estado_proyecto::write_res_partner_fingerprint_verification(int fingerprint_id, int project_id, int accountability_id = 0)
{
		
		XmlRpcValue result;
		XmlRpcClient server_create (this->URL2 + "/xmlrpc/object");
		model = "res.partner.fingerprint.verification";
		
		XmlRpcValue param_array_create = XmlRpcValue::makeArray();
		param_array_create = XmlRpcValue::makeArray();
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeInt(uid));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
		param_array_create.arrayAppendItem(XmlRpcValue::makeString("create"));
		
		XmlRpcValue struct_create = XmlRpcValue::makeStruct();
		struct_create = XmlRpcValue::makeStruct();
		struct_create.structSetValue("fingerprint_id", XmlRpcValue::makeInt(fingerprint_id));
		struct_create.structSetValue("project_id", XmlRpcValue::makeInt(project_id));

        if(accountability_id > 0)
            struct_create.structSetValue("accountability_id", XmlRpcValue::makeInt(accountability_id));

		//array_vals_create.arrayAppendItem(struct_create);

		param_array_create.arrayAppendItem(struct_create);
		result = server_create.call("execute", param_array_create);
		return result.getInt();	
}

//Falta modificar la firma de este metodo, para que se le pasen los valores. MANO Y DEDO, Que se le pasaran por el webservice de cesar, igual que la cedula de identidad.
int estado_proyecto::validacion_del_estado_del_proyecto(string cedula, string mano, int dedo, int id_proy)
{
    bool duplicado;
    string estado_del_proyecto;
    //this->set_variables();
    this->inicializar();
    this->buscar_id_obpp(cedula);
    //Estos dos metodos sirven para buscar los datos que vamos a insertar en la base de datos

    //Cuando se busca el estado del proyecto, de una vez se obtiene el id, del mismo
    //estado_del_proyecto = this->buscar_estado_proyecto(this->parent_id);
    estado_del_proyecto = this->buscar_estado_proyecto(id_proy);
    //Aqui se busca el fingerprint_id, que no es mas que el id de la tabla res_partner_fingerprint
    this->set_id_de_res_partner_fingerprint(mano.c_str(), this->id_la_tabla_res_partner, dedo );
    //cerr << "this->id_res_partner_fingerprint " << this->id_res_partner_fingerprint << " this->id_del_proyecto " << id_proy << " estado_del_proyecto " << estado_del_proyecto << endl;
    duplicado = validacion_verificacion(this->id_res_partner_fingerprint, id_proy, estado_del_proyecto);
    if (duplicado == false)
    {
        //Aqui se envian los 2 registros
        this->write_res_partner_fingerprint_verification(this->id_res_partner_fingerprint, id_proy);
        return 0;
    }
    return 2;

    /*if (estado_del_proyecto == "approved")
    {
        // Si el estado del proyecto es aprobado entonces retorna 0
        
        duplicado = validacion_verificacion(this->id_res_partner_fingerprint, this->id_del_proyecto, estado_del_proyecto);
        if (duplicado == false)
        {
         //Aqui se envian los 2 registros
         this->write_res_partner_fingerprint_verification(this->id_res_partner_fingerprint, this->id_del_proyecto);
         return 0;
        }
        return 2;
    }else if (estado_del_proyecto == "finished")
    {
        // Si el estado del proyecto es culminado entonces retorna 0
        
        duplicado = validacion_verificacion(this->id_res_partner_fingerprint, this->id_del_proyecto, estado_del_proyecto);
        if (duplicado == false)
        {
         //Aqui se envian los 2 registros
         this->write_res_partner_fingerprint_verification(this->id_res_partner_fingerprint, this->id_del_proyecto);
         return 0;
        }
        return 2;
    }else if (estado_del_proyecto == "implementation")
    {
        
      // Si el estado del proyecto es aprobado entonces retorna 0
      int accountability_id = this->buscar_accountability(this->id_del_proyecto);

      if(accountability_id > 0)
      {

         duplicado = validacion_verificacion(this->id_res_partner_fingerprint, this->id_del_proyecto, estado_del_proyecto, accountability_id);
         if (duplicado == false)
         {
            //Aqui se envian los 2 registros
            this->write_res_partner_fingerprint_verification(this->id_res_partner_fingerprint, this->id_del_proyecto, accountability_id);
            return 0;
         }
      }
        return 2;
    }else if (estado_del_proyecto == "incidencia")
    {
        // Si el estado del proyecto es aprobado entonces retorna 0
        
        duplicado = validacion_verificacion(this->id_res_partner_fingerprint, this->id_del_proyecto, estado_del_proyecto);
        if (duplicado == false)
        {
         //Aqui se envian los 2 registros
         this->write_res_partner_fingerprint_verification(this->id_res_partner_fingerprint, this->id_del_proyecto);
         return 0;
        }
        return 2;
    }
    else
    {
        // Si el estado del proyecto es diferente a approved entonces retorna un 1
        // El metodo write_res_partner_fingerprint_verification() no va aqui solo se pone aqui, para probar...
        return 1;
        
        
    }*/

}

bool estado_proyecto::validacion_incidencia(long int fingerprint_id, long int project_id, string state, string incidence_state)
{
    XmlRpcClient::Initialize(NAME, VERSION);
    try {
        model = "res.partner.fingerprint.verification.incidence"; //Tabla
        XmlRpcClient server_search (this->URL2 + "/xmlrpc/object");

        XmlRpcValue param_array_search = XmlRpcValue::makeArray();
        param_array_search.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
        param_array_search.arrayAppendItem(XmlRpcValue::makeInt(uid));
        param_array_search.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
        param_array_search.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
        param_array_search.arrayAppendItem(XmlRpcValue::makeString("search"));

        XmlRpcValue array_comp = XmlRpcValue::makeArray();
        XmlRpcValue array_comp_1 = XmlRpcValue::makeArray();
        
        cerr << "fingerprint_id " << fingerprint_id << " project_id " << project_id << " state " << state << " incidence_state " << incidence_state << endl;

        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("fingerprint_id"));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(fingerprint_id));
        array_comp.arrayAppendItem(array_comp_1);
        array_comp_1 = XmlRpcValue::makeArray();
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("project_id"));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeInt(project_id));
        array_comp.arrayAppendItem(array_comp_1);
        array_comp_1 = XmlRpcValue::makeArray();
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("project_state"));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString(state)); 
        array_comp.arrayAppendItem(array_comp_1);
        array_comp_1 = XmlRpcValue::makeArray();
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("incidence_state"));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString("="));
        array_comp_1.arrayAppendItem(XmlRpcValue::makeString(incidence_state));
        array_comp.arrayAppendItem(array_comp_1);

        param_array_search.arrayAppendItem(array_comp);
        XmlRpcValue result_search = server_search.call("execute", param_array_search);
        XmlRpcValue resultados = result_search;

        XmlRpcClient server_read_2c (this->URL2 + "/xmlrpc/object");
        XmlRpcValue param_array_read_2c = XmlRpcValue::makeArray();
        param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
        param_array_read_2c.arrayAppendItem(XmlRpcValue::makeInt(uid));
        param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
        param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
        param_array_read_2c.arrayAppendItem(XmlRpcValue::makeString("read"));
        param_array_read_2c.arrayAppendItem(resultados);

        XmlRpcValue array_vals_read_2c = XmlRpcValue::makeArray();	
        param_array_read_2c.arrayAppendItem(array_vals_read_2c);
        XmlRpcValue result_read_2c = server_read_2c.call("execute", param_array_read_2c);
        
        int numero_registros = result_read_2c.arraySize();
        if (numero_registros > 0)
        {
            // Retorna true si el registro no esta duplicado.
            return true;
            
        }
        else
        {
            // Retorna false si el registro esta duplicado.
            return false;
            
        }

    } catch (XmlRpcFault& fault) {
        //XmlRpcValue resultBD = XmlRpcValue::makeArray();
        cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
        XmlRpcClient::Terminate();

        return true;
    }
}

int estado_proyecto::write_res_partner_fingerprint_verification_incidence(long int fingerprint_id, long int project_id, string state, string incidence_state)
{
		XmlRpcClient::Initialize(NAME, VERSION);
        try {
            XmlRpcValue result;
            XmlRpcClient server_create (this->URL2 + "/xmlrpc/object");

            model = "res.partner.fingerprint.verification.incidence";
            
            XmlRpcValue param_array_create = XmlRpcValue::makeArray();
            param_array_create = XmlRpcValue::makeArray();
            param_array_create.arrayAppendItem(XmlRpcValue::makeString(dbname.c_str()));
            param_array_create.arrayAppendItem(XmlRpcValue::makeInt(uid));
            param_array_create.arrayAppendItem(XmlRpcValue::makeString(pwd.c_str()));
            param_array_create.arrayAppendItem(XmlRpcValue::makeString(model.c_str()));
            param_array_create.arrayAppendItem(XmlRpcValue::makeString("create_verification"));
            
            XmlRpcValue struct_create = XmlRpcValue::makeStruct();
            struct_create = XmlRpcValue::makeStruct();
            struct_create.structSetValue("fingerprint_id", XmlRpcValue::makeInt(fingerprint_id));
            struct_create.structSetValue("project_id", XmlRpcValue::makeInt(project_id));
            struct_create.structSetValue("project_state", XmlRpcValue::makeString(state));
            struct_create.structSetValue("incidence_state", XmlRpcValue::makeString(incidence_state));

            //array_vals_create.arrayAppendItem(struct_create);

            param_array_create.arrayAppendItem(struct_create);
            result = server_create.call("execute", param_array_create);

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
                A pointer to a C variable. This is meaningful only in the context of a particular running instance of a C program.*/
            if(result.getType() != XMLRPC_TYPE_INT)
                return -664;
            if(result.getInt() < 0)
                return -664;
                
            return result.getInt();
        } catch (XmlRpcFault& fault) {
            //XmlRpcValue resultBD = XmlRpcValue::makeArray();
            cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
            XmlRpcClient::Terminate();

            return -664;
        }
}

//Falta modificar la firma de este metodo, para que se le pasen los valores. MANO Y DEDO, Que se le pasaran por el webservice de cesar, igual que la cedula de identidad.
int estado_proyecto::validacion_incidencias_huella(string cedula, string mano, int dedo, long int id_proy)
{	
	bool duplicado;
	string estado_del_proyecto;
	//this->set_variables();
	this->inicializar();
	this->buscar_id_obpp(cedula);
    string incidence_state = "in_process";
	//Estos dos metodos sirven para buscar los datos que vamos a insertar en la base de datos
	
	//Cuando se busca el estado del proyecto, de una vez se obtiene el id, del mismo
	//estado_del_proyecto = this->buscar_estado_proyecto(this->parent_id);
    estado_del_proyecto = this->buscar_estado_proyecto(id_proy);
	//Aqui se busca el fingerprint_id, que no es mas que el id de la tabla res_partner_fingerprint
	this->set_id_de_res_partner_fingerprint(mano.c_str(), this->id_la_tabla_res_partner, dedo );
    //cerr << "estado_del_proyecto " << estado_del_proyecto << endl;    
    duplicado = validacion_incidencia(this->id_res_partner_fingerprint, id_proy, estado_del_proyecto, incidence_state);
    if (duplicado == false)
    {
        //Aqui se envian los 2 registros
        int valor = this->write_res_partner_fingerprint_verification_incidence(this->id_res_partner_fingerprint, id_proy, estado_del_proyecto, incidence_state);
        if(valor == -664)
        {
            return -664;
        }   
        return 0;
    }
    return 2;
}
