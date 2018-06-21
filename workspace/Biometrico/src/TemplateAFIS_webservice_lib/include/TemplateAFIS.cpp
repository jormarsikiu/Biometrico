/**
* @file TemplateAFIS.cpp
* @brief Clases para manejar las funcionalidades comparar y guardar huella.
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @authors Lcdo. César Cordero. ccordero@simonrodriguez.org.ve.
* @version 1.0
* @date 2014
* This file is part of Sistema de Identificación Biométrica para las OBPP.
*/


#include "TemplateAFIS.h"

/*!
 * Clase Imagen: Almacena las imágenes y variables.
 * Almacena en memoria las imágenes para que puedan ser manipuladas por el webservice.
 *
 */

Imagen::Imagen(string id, unsigned char * imagen, int nWidth, int nHeight, long int nSize, string id_persona, string id_dedo, string mano)
{
this->imagen = imagen;//pImageData;
this->nWidth = nWidth;
this->nHeight = nHeight;
this->nSize = nSize;
this->id = id;
this->id_persona = id_persona;
this->id_dedo = id_dedo;
this->mano = mano;
}

// Destructor
Imagen::~Imagen()
{
}

unsigned char * Imagen::getImagen(unsigned char * &imagen)
{
return this->imagen;
}

int Imagen::getWidth()
{
return this->nWidth;
}

int Imagen::getHeight()
{
return this->nHeight;
}

long int Imagen::getSize()
{
return this->nSize;
}

string Imagen::getId()
{
return this->id;
}

string Imagen::getIdPersona()
{
return this->id_persona;
}

string Imagen::getIdDedo()
{
return this->id_dedo;
}

string Imagen::getMano()
{
return this->mano;
}

long int Imagen::sizeofObject()
{
return this->getSize() + sizeof(nWidth) + sizeof(nHeight) + sizeof(nSize) + id.size() + id_persona.size() + id_dedo.size() + mano.size();
}

/*!
 * Método TemplateAFIS_webservice: Realiza las funciones de Identify,
 * Verify y Enroll, y almacena las huellas en la Base de Datos.
 *
 */

/*!
 * Método convert_raw_jpeg: Convierte un binario de imágen a jpeg.
 *
 */
int TemplateAFIS_webservice::convert_raw_jpeg(string filename, unsigned char *raw, int nWidth, int nHeight)
{
	int bytes_per_pixel = 1;//3;   /* ó 1 para una imágen a escala de grises */
	J_COLOR_SPACE color_space = JCS_GRAYSCALE;//JCS_RGB; /* ó JCS_GRAYSCALE imágenes a escala de grises */
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	/* Esto es un puntero a una fila de imágenes */
	JSAMPROW row_pointer[1];
	FILE *outfile = fopen( filename.c_str(), "wb" );

	if ( !outfile )
	{
	printf("Error opening output jpeg file %s\n!", filename.c_str() );
	return -1;
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	/* Configurando los parámetros del archivo de salida */
	cinfo.image_width = nWidth;
	cinfo.image_height = nHeight;
	cinfo.input_components = bytes_per_pixel;
	cinfo.in_color_space = color_space;
	/* Compresión de parámetros por defecto, no debemos preocuparnos por esto */
	jpeg_set_defaults( &cinfo );
	/* Ahora se hace  la compresión .. */
	jpeg_start_compress( &cinfo, TRUE );
	/* Como leyendo un archivo, ahora escribiendo una fila por vez */
	while( cinfo.next_scanline < cinfo.image_height )
	{
	row_pointer[0] = &raw[ cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
	jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}
	/* Similar a leer un archivo, limpia después de que comprimamos */
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose( outfile );
	/* El código es 1 si fue exitoso! */
	return 1;
}

/*!
 * Método constructor para la clase TemplateAFIS_webservice.
 * Permite realizar la conexión con el wbservice de Odoo.
 * Se verifica que esté conectado el captahuellas,
 * se genera la firma digital y se encripta una cookie.
 */
TemplateAFIS_webservice::TemplateAFIS_webservice(string param1, string param2, string param3, string param4, string param5)
{
	this->m_hMatcher = NULL;

	this->m_nSecurityLevel = 4;
	this->m_bFastMode = false;
	this->m_nEnrollQuality = 0;

	firma_digital = new FirmaDigital("nonosdevolvemos.2013");

	this->OnSetTemplateType(2);

   this->base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

   this->padding = RSA_PKCS1_PADDING;
} //constructor por defecto

/*!
 * Método desconstructor para la clase TemplateAFIS_webservice.
 *
 */
TemplateAFIS_webservice::~TemplateAFIS_webservice() 
{
} //destructor

/*!
 * Obtener el mismo objeto, a sí mismo.
 *
 */
void* TemplateAFIS_webservice::pElMismo() { return this; }

/*!
 * Método SetTipoTemplate: Establece el tipo de Template.
 *
 */

// GETTER y SETTER
void TemplateAFIS_webservice::SetTipoTemplate(int tipo){tipo_template = tipo;}
void TemplateAFIS_webservice::SaveTemplate2ISO(){} //string FileName
void TemplateAFIS_webservice::SaveImagenWSQ(){}

/*!
 * Método GetSecurityLevel: Establece el nivel de seguridad.
 * Protocolo de SUPREMA indicado en el SDK
 */

int TemplateAFIS_webservice::GetSecurityLevel()
{
	int value;

	// Security level ranges from 1 to 7
	UFM_GetParameter(this->m_hMatcher, UFM_PARAM_SECURITY_LEVEL, &value);
	return value;
}

/*!
 * Método GetFastMode: Obtiene el valor de obtención de la huella, FAST_MODE
 * Protocolo de SUPREMA indicado en el SDK
 */

int TemplateAFIS_webservice::GetFastMode()
{
	int value;

	UFM_GetParameter(this->m_hMatcher, UFM_PARAM_FAST_MODE, &value);

	return value;
}

/*!
 * Método GetMatcherSettings: Configuraciones del Matcher.
 * Protocolo de SUPREMA indicado en el SDK
 */

void TemplateAFIS_webservice::GetMatcherSettings()
{
	GetSecurityLevel();
	GetFastMode();
}

/*!
 * Método initMatcher: Inicializa el Matcher.
 * Protocolo de SUPREMA indicado en el SDK
 */

bool TemplateAFIS_webservice::initMatcher()
{
	/////////////////////////////////////////////////////////////////////////////
	// Crear un matcher
	/////////////////////////////////////////////////////////////////////////////
	UFM_STATUS ufm_res;
	//return true;
	ufm_res = UFM_Create(&this->m_hMatcher);
	if (ufm_res == UFM_OK) {
		//printf("UFM_Create: OK\r\n");
	} else {
		UFM_GetErrorString(ufm_res, m_strError);
		cerr << "UFM_Create: \r\n" << m_strError;
		return false;
		//AddMessage("UFM_Create: %s\r\n", m_strError);
	}

	GetMatcherSettings();
	//GetMatcherSettings(this->m_hMatcher);
	this->OnSetTemplateType(2);
	return true;

}

/*!
 * Método unInitMatcher: Desinicializa el Matcher.
 * Protocolo de SUPREMA indicado en el SDK
 */

bool TemplateAFIS_webservice::unInitMatcher()
{
	/////////////////////////////////////////////////////////////////////////////
	// Delete matcher
	/////////////////////////////////////////////////////////////////////////////
	if (m_hMatcher != NULL) {
		UFM_STATUS ufm_res;

		ufm_res = UFM_Delete(this->m_hMatcher);
		if (ufm_res == UFM_OK) {
			//return true;
			//AddMessage("UFM_Delete: OK\r\n");
		} else {
			return false;
			/*UFM_GetErrorString(ufm_res, m_strError);
			AddMessage("UFM_Delete: %s\r\n", m_strError);*/
		}
		this->m_hMatcher = NULL;
	}

	return true;
}

/*!
 * Método OnGetParameters: Obtener los parámetros del objeto Matcher.
 * Protocolo de SUPREMA indicado en el SDK
 */

void TemplateAFIS_webservice::OnGetParameters()
{
	//GetCurrentScannerSettings();

	if (this->m_hMatcher != NULL) {
		GetMatcherSettings();
	}
}

/*!
 * Método OnSetTemplateType: Establecer los tipos de template.
 * Protocolo de SUPREMA indicado en el SDK
 */

int TemplateAFIS_webservice::OnSetTemplateType(int value)
{
	int estado = 0;

	/*Si el Matcher es NULL devuelve -112, hay que compararlo con UFM_ERR_INVALID_PARAMETERS*/
	if(this->m_hMatcher == NULL)
	{
		//printf("ERROR: Debe inicializar las variables como el Matcher");
		if(!this->initMatcher())
			return -112;
	}

	switch(value){
		case 1:
			//UFS_SetTemplateType(hScanner, UFS_TEMPLATE_TYPE_SUPREMA);
			estado = UFM_SetTemplateType(this->m_hMatcher, UFS_TEMPLATE_TYPE_SUPREMA);
			//printf("template type:suprema");
			break;
		case 2:
			//UFS_SetTemplateType(hScanner, UFS_TEMPLATE_TYPE_ISO19794_2);
			estado = UFM_SetTemplateType(this->m_hMatcher, UFS_TEMPLATE_TYPE_ISO19794_2);
			//printf("template type:iso");
			break;
		case 3:
			//UFS_SetTemplateType(hScanner, UFS_TEMPLATE_TYPE_ANSI378);
			estado = UFM_SetTemplateType(this->m_hMatcher, UFS_TEMPLATE_TYPE_ANSI378);
			//printf("template type:ansi");
			break;
		default:
			//printf("check number..\n");
			estado = -411;
			return estado;
	}

	/*
	 * Si establecer plantillas da error, lanzará -411, hay que compararlo con UFM_ERR_TEMPLATE_TYPE
	 */
	/*
	 * Si no hay errores lanza 0, hay que compararlo con UFM_OK
	 */
	if(estado == -411)
		return estado;
	else
		estado = 0;

	return estado;

}

/*!
 * Método OnEnroll: Guarda la huella en Odoo.
 * Permite relacionar el Captahuellas con Odoo.
 */
int TemplateAFIS_webservice::OnEnroll(string usuario, string password, string id_persona, string id_dedo, string mano, string hash,
		unsigned char * &m_template, int m_template_size, 
		unsigned char* &pImageData, int nWidth, int nHeight, 
		string &id_persona_out, string &id_dedo_out, string &mano_out)
{
    XmlRpcClient::Initialize(NAME, VERSION);
    try {
        if(!comprobar(usuario.c_str(), password.c_str()))
		{
			return -961;
			//return myArrayP;
		}
	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}
    
	int identify = this->OnIdentify(usuario.c_str(), password.c_str(), m_template, m_template_size, id_persona.c_str(), id_persona_out, id_dedo_out, mano_out);

	//int identify = OnIdentifyVerify(usuario.c_str(), password.c_str(), m_template, m_template_size, id_persona.c_str(), id_dedo.c_str(), mano.c_str(), id_persona_out, id_dedo_out, mano_out);

    cerr << "identify " << identify << endl;
	if(identify == 0)
	{
		if((strcmp(id_persona.substr(1).c_str(), id_persona_out.c_str()) != 0) || 
			(strcmp(id_dedo.c_str(), id_dedo_out.c_str()) != 0) || 
			(strcmp(mano.c_str(), mano_out.c_str()) != 0))
		{
			return -680;
		}
	}else if(identify == -664)
	{
		return -664;
	}

	XmlRpcClient::Initialize(NAME, VERSION);

	bool res = true;

	//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));
	try {
		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
			return -664;

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		XmlRpcValue persona = fing->searchPersona(server_search, id_persona.substr(1).c_str());//id_persona);

		if(persona.arraySize() == 0)
		{
			delete fing;
			//XmlRpcClient::Terminate();
			return -664;
		}

		int finger = atoi(id_dedo.c_str());
		long int partner_id = persona.arrayGetItem(0).getInt();
		string hand = mano.c_str();

		XmlRpcValue resultados = fing->searchFingerprint(server_search, finger, hand.c_str(), partner_id);

		if(resultados.arraySize() == 0)
		{
			switch(fing->insertarFingerprintWebService(m_template, hash.c_str(), finger, hand.c_str(), partner_id, m_template_size, nHeight, nWidth, pImageData, nWidth*nHeight*sizeof(unsigned char)))
			{
				case 0:
					delete fing;
					XmlRpcClient::Terminate();
					return 0;
				case -1:
					delete fing;
					XmlRpcClient::Terminate();
					return -1;
				case -2:
					delete fing;
					XmlRpcClient::Terminate();
					return -2;
				case -3:
					delete fing;
					XmlRpcClient::Terminate();
					return -3;
				case -4:
					delete fing;
					XmlRpcClient::Terminate();
					return -4;
				case -5:
					delete fing;
					XmlRpcClient::Terminate();
					return -5;
				case -6:
					delete fing;
					XmlRpcClient::Terminate();
					return -6;
				case -7:
					delete fing;
					XmlRpcClient::Terminate();
					return -7;
				default:
					delete fing;
					XmlRpcClient::Terminate();
					return -664;
			}
		}else
		{
			switch(fing->actualizarFingerprintWebService(m_template, hash.c_str(), finger, hand.c_str(), partner_id, m_template_size, nHeight, nWidth, pImageData, nWidth*nHeight*sizeof(unsigned char)))
			{
				case true:
					delete fing;
					XmlRpcClient::Terminate();
					return 0;
				default:
					delete fing;
					XmlRpcClient::Terminate();
					return -664;
			}
		}

		//free(fing);

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -664;
	}

	return 0;
}

/*!
 * Método OnVerify: Realiza la verificación de una huella.
 * Permite relacionar el Captahuellas con Odoo. Verifica el usuario y aplica las llamadas a SUPREMA
 */

int TemplateAFIS_webservice::OnVerify(string usuario, string password, string id_proyecto, string id_persona, string id_dedo, string mano,
		unsigned char * &Template, int TemplateSize, string &cedula, string &dedo, string &hand, string &situr)
{
    XmlRpcClient::Initialize(NAME, VERSION);
    try {
        if(!comprobar(usuario.c_str(), password.c_str()))
		{
			return -961;
			//return myArrayP;
		}
	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}
    
	/*Si el Matcher es NULL devuelve -112, hay que compararlo con UFM_ERR_INVALID_PARAMETERS*/
	if(this->m_hMatcher == NULL)
	{
		//printf("ERROR: Debe inicializar las variables como el Matcher");
		if(!this->initMatcher())
			return -112;
	}

	UFM_STATUS ufm_res;
	int bVerifySucceed;

	unsigned char * m_template;
	int m_template_size;

	XmlRpcClient::Initialize(NAME, VERSION);

	long int partner_id = 0;

	unsigned char * templ;
	size_t templ_size;

	//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

	try {
		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
			return -961;

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		XmlRpcValue persona = fing->searchPersona(server_search, id_persona.substr(1).c_str());//"J123456789");//id_persona);

		if(persona.arraySize() == 0)
		{
            delete fing;
			XmlRpcClient::Terminate();
			return -664;
		}

		partner_id = persona.arrayGetItem(0).getInt();

		XmlRpcValue get_usuario = fing->readFingerprint(server_search, atoi(id_dedo.c_str()), mano.c_str(), partner_id);

		//Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS
		if(get_usuario.arraySize() == 0)
		{
			//printf("No hay datos.\n");
            delete fing;
			return -661;
		}

		XmlRpcValue estructura = get_usuario.arrayGetItem(0);

		////estructura.structGetValue("template").getBase64(templ, templ_size);

        //XmlRpcValue estructura = get_usuario.arrayGetItem(0);

        string s;

        s = estructura.structGetValue("template").getString();

        base64_decodestring( s, templ, templ_size );

		//temp_size = estructura.structGetValue("template_size").getInt();

        delete fing;

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}

	//cerr << " PASE 2 " << endl;

	XmlRpcClient::Terminate();

	/***result result = modelo_huella->seleccionarHuellaWebservice(partner_id, id_dedo, mano);

	if(result.size() == 0)
	{
		return -661;
	}

	pqxx::binarystring * blob;

	blob = new pqxx::binarystring( result[0]["template"] );***/

	/*m_template = (unsigned char*)malloc(blob->size());
	memset(m_template, 0, blob->size());
	memcpy(m_template, (unsigned char*)blob->data(), blob->size());*/
	m_template = (unsigned char*)malloc(templ_size);
	if(m_template == NULL)
	{
		cerr << "No hay suficiente memoria dinámica" << endl;
		return -961;
	}
    
	////memcpy(m_template, (unsigned char*)blob->data(), blob->size());
    memcpy(m_template, (unsigned char*)templ, templ_size);
	//m_template = (unsigned char*)blob->data();
	//m_template_size = 0;
	m_template_size = templ_size;

	////delete blob;

	ufm_res = UFM_Verify(m_hMatcher, Template, TemplateSize, m_template, m_template_size, &bVerifySucceed);
	free(m_template);
	//Template = NULL;
	if (ufm_res != UFM_OK) {
		UFM_GetErrorString(ufm_res, m_strError);
		//printf("UFM_Verify:%s\r\n",m_strError);
		/*
		 * Si hubo un error en la verificación lanza -961, hay que compararlo con UFM_VERIFICATION_ERROR
		 */
		return -961;
	}

	if (bVerifySucceed) {
		Template = NULL;
		return 0;
		//AddMessage("Verification succeed (No.%d)\r\n", m_nSelectID+1);
	} else {
		/*
		 * Si hubo un error en la verificación lanza -961, hay que compararlo con UFM_VERIFICATION_ERROR
		 */
         
         this->guardarIncidencia(usuario, password, id_persona, id_dedo, mano, id_proyecto);

		//this->OnIdentifyOnVerify(usuario, password, Template, TemplateSize, id_persona, cedula, dedo, hand, situr);
		Template = NULL;
		return -961;
	}

}

int TemplateAFIS_webservice::guardarIncidencia(string usuario, string password, string id_persona, string id_dedo, string mano, string id_proyecto)
{
    //Escribir el nuevo estado.	
    estado_proyecto status;

    Fingerprint * fing = NULL;

    this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);
    
    if(fing == NULL)
        return -961;

    status.set_variables(fing->dbname,usuario.c_str(),password.c_str(),fing->URL,fing->URL);
    int resultado = status.validacion_incidencias_huella(id_persona.c_str(),mano.c_str(),atoi(id_dedo.c_str()),atol(id_proyecto.c_str()));

    return resultado;
}

/*!
 * Método iteracionIdentify: Realiza la iteración de identificación 
 * de una huella, una por una; esto quiere decir un verify a cada huella.
 * Utiliza el método de verificación de BioSuprema.
 *
 */

/*int TemplateAFIS_webservice::iteracionIdentify(bool &estado, long int &i, string &id_persona_result, string &id_dedo_result, string &mano_result, long int &id_partner, string &id_dedo, string &mano, unsigned char * &Template, int &TemplateSize, int &ufm_res, int &bVerifySucceed)
{
	pqxx::result result_huellas = modelo_huella->seleccionarHuellaWebservice(estado, i);

	if(!estado)
	{
		return -664;
	}

	if(result_huellas.size() > 0)
	{
		//s_finger.str("");
		//s_mano.str("");
		//s_partner_id.str("");

		//s_finger << result_huellas[0]["finger"].c_str();
		//s_mano << result_huellas[0]["hand"].c_str();
		//s_partner_id << result_huellas[0]["partner_id"].c_str();

		if(id_partner != atol(result_huellas[0]["partner_id"].c_str()) || 
		strcmp(id_dedo.c_str(), result_huellas[0]["finger"].c_str()) != 0 || 
		strcmp(mano.c_str(), result_huellas[0]["hand"].c_str()) != 0)
		{
			pqxx::binarystring * blob = new pqxx::binarystring( result_huellas[0]["template"] );
	    		//blob = conexion->archivoBlobPuntero(result_huellas[0]["template"]);

			unsigned char * template_verify = (unsigned char*)malloc(blob->size());

			if(template_verify == NULL)
			{
				delete blob;
				cerr << "No hay suficiente memoria dinámica" << endl;
				result_huellas.clear();
				return -664;
			}

			memcpy(template_verify, (unsigned char*)blob->data(), blob->size());
			//m_template_all[i] = (unsigned char*)blob->data();

			int template_verify_size = blob->size();

			delete blob;

			ufm_res = UFM_Verify(m_hMatcher, Template, TemplateSize, template_verify, template_verify_size, &bVerifySucceed);

			free(template_verify);

			if (ufm_res != UFM_OK) {
				UFM_GetErrorString(ufm_res, m_strError);
				//printf("UFM_Verify:%s\r\n",m_strError);
				//
				// Si hubo un error en la verificación lanza -961, hay que compararlo con UFM_VERIFICATION_ERROR
				//
				result_huellas.clear();
				return -961;
			}

			if (bVerifySucceed) {
				if(id_partner != atol(result_huellas[0]["partner_id"].c_str()) || 
				strcmp(id_dedo.c_str(), result_huellas[0]["finger"].c_str()) != 0 || 
				strcmp(mano.c_str(), result_huellas[0]["hand"].c_str()) != 0)
				{
					id_persona_result = result_huellas[0]["partner_id"].c_str();
					id_dedo_result = result_huellas[0]["finger"].c_str();
					mano_result = result_huellas[0]["hand"].c_str();
					result_huellas.clear();
					return 0;
				}
				//break;
			} else {
				//
				// Si hubo un error en la verificación lanza -961, hay que compararlo con UFM_VERIFICATION_ERROR
				//
				//return -961;
			}
		}
		//result_huellas.clear();
	}

	bVerifySucceed = 0;
	result_huellas.clear();
}*/

/*!
 * Método OnIdentifyVerify: Identifica las huellas una por una.
 * Utiliza el método de verificación de BioSuprema.
 */

/*int TemplateAFIS_webservice::OnIdentifyVerify(string usuario, string password, unsigned char * &Template, int TemplateSize, string cedula, string id_dedo, string mano, string &cedula_out, string &dedo_out, string &hand_out)
{
	//Si el Matcher es NULL devuelve -112, hay que compararlo con UFM_ERR_INVALID_PARAMETERS
	if(this->m_hMatcher == NULL)
	{
		//printf("ERROR: Debe inicializar las variables como el Matcher");
		if(!this->initMatcher())
			return -112;
	}

	XmlRpcClient::Initialize(NAME, VERSION);

	long int id_partner = 0;

	//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

	try
	{
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			return -664;
		}

		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
		{
			return -664;
		}

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		XmlRpcValue persona = fing->searchPersona(server_search, cedula.c_str());

		if(persona.arraySize() == 0)
		{
			XmlRpcClient::Terminate();
			delete fing;
			return -664;
		}

		delete fing;

		id_partner = persona.arrayGetItem(0).getInt();

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -664;
	}

	XmlRpcClient::Terminate();

	pqxx::result result = modelo_huella->seleccionarHuellaWebserviceMax();

	//Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS
	if(result.size() == 0)
	{
		return -661;
	}

	//pqxx::binarystring * blob;

	string id_persona_result;
	string id_dedo_result;
	string mano_result;

	bool encontrado = false;

	//long int count = atol(result[0]["count"].c_str());
	long int count = result.size();
	long int max = atol(result[0]["max"].c_str());
	long int min = atol(result[0]["min"].c_str());

	result.clear();

	UFM_STATUS ufm_res;
	int bVerifySucceed = 0;

	bool estado;

	int valor;

	for (long int i = max; i >= min && count > 0; i--) {

		valor = iteracionIdentify(estado, i, id_persona_result, id_dedo_result, mano_result, id_partner, id_dedo, mano, Template, TemplateSize, ufm_res, bVerifySucceed);

		if(valor == -664)
			return -664;
		else if(valor == -961)
			return -961;

		if(bVerifySucceed)
			break;
	}

	if (bVerifySucceed) {

		
		XmlRpcClient::Initialize(NAME, VERSION);

		//Fingerprint * fingerprint = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

		try {
			Fingerprint * fingerprint = NULL;

			this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fingerprint);

			if(fingerprint == NULL)
			{
				cedula_out = "Existe_NoEncontradaPorError";
				dedo_out = id_dedo_result.c_str();
				hand_out = mano_result.c_str();

				XmlRpcClient::Terminate();

				return 0;
			}

			XmlRpcClient server_search (fingerprint->URL + "/xmlrpc/object");

			XmlRpcValue partner = fingerprint->searchPersona(server_search, "", atol(id_persona_result.c_str()));

			XmlRpcValue partner_persona = fingerprint->readVatPersona(server_search, partner);

			delete fingerprint;

			if(partner_persona.arraySize() == 0)
			{
				cedula_out = "Existe_NoEncontradaPorError";
				dedo_out = id_dedo_result.c_str();
				hand_out = mano_result.c_str();

				XmlRpcClient::Terminate();

				return 0;
			}

			cedula_out = partner_persona.arrayGetItem(0).getStruct().structGetValue("identification_id").getString().c_str();
			dedo_out = id_dedo_result.c_str();
			hand_out = mano_result.c_str();

			XmlRpcClient::Terminate();

			return 0;

		} catch (XmlRpcFault& fault) {
			cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
			XmlRpcClient::Terminate();

			cedula_out = "Existe_NoEncontradaPorError";
			dedo_out = id_dedo_result.c_str();
			hand_out = mano_result.c_str();

			return 0;
		}
	} else {
		//printf("Identification failed\r\n");
		//
		// Si hubo un error en la identificación lanza -861, hay que compararlo con UFM_IDENTIFICATION_ERROR
		//
		return -861;
	}

}*/

/*!
 * Método OnIdentify: Identifica una huella contra muchas.
 * Utiliza el método de identificación de BioSuprema.
 * Verifica que el captahuellas esté conectado.
 * Verifica que el usuario de Odoo sea el correcto.
 *
 */

int TemplateAFIS_webservice::OnIdentify(string usuario, string password, unsigned char * Template, int TemplateSize, string cedula_in, string &cedula, string &dedo, string &hand)
{
	/*Si el Matcher es NULL devuelve -112, hay que compararlo con UFM_ERR_INVALID_PARAMETERS*/
	if(this->m_hMatcher == NULL)
	{
		//printf("ERROR: Debe inicializar las variables como el Matcher");
		if(!this->initMatcher())
			return -112;
	}

	int nMaxTemplateNum = 0;
	UFM_STATUS ufm_res;

	/*long int id_partner = 0;

	XmlRpcClient::Initialize(NAME, VERSION);

	try
	{
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			return -664;
		}

		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
		{
			return -664;
		}

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		XmlRpcValue persona = fing->searchPersona(server_search, cedula_in.c_str());

		if(persona.arraySize() == 0)
		{
			XmlRpcClient::Terminate();
			delete fing;
			return -664;
		}

		delete fing;

		id_partner = persona.arrayGetItem(0).getInt();

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -664;
	}*/

	size_t templ_size;
    unsigned char * templ;
    XmlRpcValue get_usuario;

    XmlRpcClient::Initialize(NAME, VERSION);

	try {
		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
			return -961;

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		get_usuario = fing->readFingerprint(server_search);

		//Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS
		if(get_usuario.arraySize() == 0)
		{
			//printf("No hay datos.\n");
			return -661;
		}

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}

	/****pqxx::result result = modelo_huella->getAllHuellaWebservice();****/

	//cerr << " Identify1 " << endl;

	/*Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS*/
	/*if(result.size() == 0)
	{
		//printf("No hay datos.\n");
		return -661;
	}*/

	//cerr << " Identify2 " << endl;

    unsigned char* m_template_all[get_usuario.arraySize()];
    int m_template_all_size[get_usuario.arraySize()];

    //unsigned char *buffer_pt;
    	//size_t len;

    ////pqxx::binarystring * blob;

    string id_persona[get_usuario.arraySize()];
    string id_dedo[get_usuario.arraySize()];
    string mano[get_usuario.arraySize()];

    /*Inicializa todas las variables*/
    	/*for (size_t i = 0; i < result.size(); i++) {
	    m_template_all[i] = (unsigned char*)malloc(blob->size());
	    memset(m_template_all[i], 0, blob->size());
	    m_template_all_size[i] = 0;
    }*/
    
    XmlRpcClient::Initialize(NAME, VERSION);

	try {
	    /*Asigna todas las huellas*/
        for (size_t i = 0; i < get_usuario.arraySize(); i++) {

		    XmlRpcValue estructura = get_usuario.arrayGetItem(i);

            string s;

            s = estructura.structGetValue("template").getString();

            base64_decodestring( s, templ, templ_size );

            stringstream ss;
            ss << estructura.structGetValue("partner_id").arrayGetItem(0).getInt();
		    id_persona[i] = ss.str();
            ss.str("");
            ss << estructura.structGetValue("finger").getInt();
		    id_dedo[i] = ss.str();
            ss.str("");
		    mano[i] = estructura.structGetValue("hand").getString().c_str();

		    m_template_all[i] = (unsigned char*)malloc(templ_size);

		    if(m_template_all[i] == NULL)
		    {
			    cerr << "No hay suficiente memoria dinámica";
			    for (size_t j = 0; j < get_usuario.arraySize(); j++) {
				    if(m_template_all[i] != NULL)
					    m_template_all[i] = NULL;
			    }
			    return -664;
		    }

		    memcpy(m_template_all[i], (unsigned char*)templ, templ_size);

		    m_template_all_size[i] = templ_size;
	    }

    } catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}

	nMaxTemplateNum = get_usuario.arraySize();

	int nMatchIndex;

	/*Realiza el Identify*/
	ufm_res = UFM_Identify(this->m_hMatcher, Template, TemplateSize, m_template_all, m_template_all_size, nMaxTemplateNum, 5000, &nMatchIndex);

	/*Liberando memoria*/
	for (size_t i = 0; i < get_usuario.arraySize(); i++) {
		if(m_template_all[i] != NULL)
			free(m_template_all[i]);
	}

	if (ufm_res != UFM_OK) {
		UFM_GetErrorString(ufm_res, m_strError);
		//printf("UFM_Identify: %s\r\n", m_strError);
		/*
		 * Si hubo un error en la identificación lanza -861, hay que compararlo con UFM_IDENTIFICATION_ERROR
		 */

		return -861;
	}

	if (nMatchIndex != -1) {

		XmlRpcClient::Initialize(NAME, VERSION);

		//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

		try {
			Fingerprint * fing = NULL;

			this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);
			if(fing == NULL)
				return -861;

			XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

			XmlRpcValue partner = fing->searchPersona(server_search, "", atol(id_persona[nMatchIndex].c_str()));

			XmlRpcValue partner_persona = fing->readVatPersona(server_search, partner);

			delete fing;

			if(partner_persona.arraySize() == 0)
			{
				XmlRpcClient::Terminate();
				return -861;
			}

			cedula = partner_persona.arrayGetItem(0).getStruct().structGetValue("identification_id").getString().c_str();

			dedo = id_dedo[nMatchIndex];
			hand = mano[nMatchIndex];

		} catch (XmlRpcFault& fault) {
			cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
			XmlRpcClient::Terminate();

			return -861;
		}

		XmlRpcClient::Terminate();

		//printf("Identification succeed\r\n");
		return 0;
	} else {
		//printf("Identification failed\r\n");
		/*
		 * Si hubo un error en la identificación lanza -861, hay que compararlo con UFM_IDENTIFICATION_ERROR
		 */
		return -861;
	}

}
/**
 * OPTIMIZAR: Revisar Método OnIdentifyOnVerify
 * Revisar Narrativa de SINCO
 */
int TemplateAFIS_webservice::OnIdentifyOnVerify(string usuario, string password, unsigned char * &Template, int TemplateSize, string cedula_in, string &cedula, string &dedo, string &hand, string &situr)
{
	/*Si el Matcher es NULL devuelve -112, hay que compararlo con UFM_ERR_INVALID_PARAMETERS*/
	if(this->m_hMatcher == NULL)
	{
		//printf("ERROR: Debe inicializar las variables como el Matcher");
		if(!this->initMatcher())
			return -112;
	}

	int nMaxTemplateNum = 0;
	UFM_STATUS ufm_res;

	size_t templ_size;
    unsigned char * templ;
    XmlRpcValue get_usuario;

    XmlRpcClient::Initialize(NAME, VERSION);

	try {
		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
			return -961;

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		get_usuario = fing->readFingerprint(server_search);

		//Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS
		if(get_usuario.arraySize() == 0)
		{
			//printf("No hay datos.\n");
			return -661;
		}

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}

	/****pqxx::result result = modelo_huella->getAllHuellaWebservice();****/

	//cerr << " Identify1 " << endl;

	/*Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS*/
	/*if(result.size() == 0)
	{
		//printf("No hay datos.\n");
		return -661;
	}*/

	//cerr << " Identify2 " << endl;

    unsigned char* m_template_all[get_usuario.arraySize()];
    int m_template_all_size[get_usuario.arraySize()];

    //unsigned char *buffer_pt;
    	//size_t len;

    ////pqxx::binarystring * blob;

    string id_persona[get_usuario.arraySize()];
    string id_dedo[get_usuario.arraySize()];
    string mano[get_usuario.arraySize()];

    /*Inicializa todas las variables*/
    	/*for (size_t i = 0; i < result.size(); i++) {
	    m_template_all[i] = (unsigned char*)malloc(blob->size());
	    memset(m_template_all[i], 0, blob->size());
	    m_template_all_size[i] = 0;
    }*/
    
    XmlRpcClient::Initialize(NAME, VERSION);

	try {
	    /*Asigna todas las huellas*/
        for (size_t i = 0; i < get_usuario.arraySize(); i++) {

		    XmlRpcValue estructura = get_usuario.arrayGetItem(i);

            string s;

            s = estructura.structGetValue("template").getString();

            base64_decodestring( s, templ, templ_size );

            stringstream ss;
            ss << estructura.structGetValue("partner_id").arrayGetItem(0).getInt();
		    id_persona[i] = ss.str();
            ss.str("");
            ss << estructura.structGetValue("finger").getInt();
		    id_dedo[i] = ss.str();
            ss.str("");
		    mano[i] = estructura.structGetValue("hand").getString().c_str();

		    m_template_all[i] = (unsigned char*)malloc(templ_size);

		    if(m_template_all[i] == NULL)
		    {
			    cerr << "No hay suficiente memoria dinámica";
			    for (size_t j = 0; j < get_usuario.arraySize(); j++) {
				    if(m_template_all[i] != NULL)
					    m_template_all[i] = NULL;
			    }
			    return -664;
		    }

		    memcpy(m_template_all[i], (unsigned char*)templ, templ_size);

		    m_template_all_size[i] = templ_size;
	    }

    } catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}

	nMaxTemplateNum = get_usuario.arraySize();

	int nMatchIndex;

	/*Realiza el Identify*/
	ufm_res = UFM_Identify(this->m_hMatcher, Template, TemplateSize, m_template_all, m_template_all_size, nMaxTemplateNum, 5000, &nMatchIndex);

	/*Liberando memoria*/
	for (size_t i = 0; i < get_usuario.arraySize(); i++) {
		if(m_template_all[i] != NULL)
			free(m_template_all[i]);
	}

	if (ufm_res != UFM_OK) {
		UFM_GetErrorString(ufm_res, m_strError);
		//printf("UFM_Identify: %s\r\n", m_strError);
		/*
		 * Si hubo un error en la identificación lanza -861, hay que compararlo con UFM_IDENTIFICATION_ERROR
		 */

		return -861;
	}

	if (nMatchIndex != -1) {

		XmlRpcClient::Initialize(NAME, VERSION);

		//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

		try {
			Fingerprint * fing = NULL;

			this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

			if(fing == NULL)
				return -861;

			XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

			XmlRpcValue partner = fing->searchPersona(server_search, "", atol(id_persona[nMatchIndex].c_str()));

			XmlRpcValue partner_persona = fing->readVatPersona(server_search, partner);

			if(partner_persona.arraySize() == 0)
			{
				delete fing;
				XmlRpcClient::Terminate();
				return -861;
			}

			XmlRpcClient server_search_parent (fing->URL + "/xmlrpc/object");

			XmlRpcValue partner_parent = fing->searchPersona(server_search_parent, "", partner_persona.arrayGetItem(0).getStruct().structGetValue("parents_ids").arrayGetItem(0).getInt());
			
			XmlRpcValue partner_persona_parent = fing->readVatPersona(server_search, partner_parent);

			delete fing;

			if(partner_persona_parent.arraySize() == 0)
			{
				XmlRpcClient::Terminate();
				return -861;
			}

			cedula = partner_persona.arrayGetItem(0).getStruct().structGetValue("identification_id").getString().c_str();
			dedo = id_dedo[nMatchIndex];
			hand = mano[nMatchIndex];

			situr = partner_persona_parent.arrayGetItem(0).getStruct().structGetValue("situr_code").getString().c_str();

		} catch (XmlRpcFault& fault) {
			cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
			XmlRpcClient::Terminate();

			return -861;
		}

		XmlRpcClient::Terminate();

		//printf("Identification succeed\r\n");
		return 0;
	} else {
		//printf("Identification failed\r\n");
		/*
		 * Si hubo un error en la identificación lanza -861, hay que compararlo con UFM_IDENTIFICATION_ERROR
		 */
		return -861;
	}

}

/*!
 * Método salvarJPEG: Extrae una imágen de la Base de Datos.
 * Almacena las imagenes jpg de las huellas en la Base de datos de Odoo.
 * Verifica el usuario que realiza la petición y sus permisos,
 * luego busca la huella en la base de datos del usuario.
 * Si la encuentra, la convierte y la almacena en imagen_buffer.
 */
int TemplateAFIS_webservice::salvarJPEG(string usuario, string password, string id_persona, string id_dedo, string mano, unsigned char * &imagen_buffer, int &width, int &height, int &size, string &id_imagen) //(string nombreArchivo)
{

	// variable para nombreArchivo
	string archivo;

	int valido = 0;

	//pqxx::binarystring * blob;

	unsigned char* pImageData = NULL;
    	int nWidth, nHeight, nSize;
	string id;
	long int partner_id = 0;

	XmlRpcClient::Initialize(NAME, VERSION);

    unsigned char * templ;
	size_t templ_size;

	//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

	try {
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			valido = -1;
			//return myArrayP;
		}

		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
		{
			//return NULL;
			valido = -1;
		}

		if(valido == 0)
		{
			XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

			XmlRpcValue persona = fing->searchPersona(server_search, id_persona.c_str());

			if(persona.arraySize() == 0)
			{
				delete fing;
				//return NULL;
				valido = -1;
			}

			if(valido == 0)
				partner_id = persona.arrayGetItem(0).getInt();
		}

		XmlRpcClient::Terminate();

	} catch (XmlRpcFault& fault) {
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		valido = -1001;
	}

    XmlRpcClient::Initialize(NAME, VERSION);

    XmlRpcValue get_usuario;

    try {
		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
			return -961;

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		get_usuario = fing->readFingerprintImage(server_search, atoi(id_dedo.c_str()), mano, partner_id);

		//Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS
		if(get_usuario.arraySize() == 0)
		{
			//printf("No hay datos.\n");
			return -1;
		}

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();

		return -961;
	}

	if(valido == 0)
	{
		/****result result = modelo_huella->seleccionarImagen(partner_id, id_dedo, mano);

		if(result.size() == 0)
		{
			//printf("Hubo un error al mostra la imagen");
			valido = -1;
		}****/
        XmlRpcClient::Initialize(NAME, VERSION);

        try {
		    if(valido == 0)
		    {
                XmlRpcValue estructura = get_usuario.arrayGetItem(0);

                string s;

                s = estructura.structGetValue("image_raw").getString();

                base64_decodestring( s, templ, templ_size );

			    nWidth = estructura.structGetValue("image_width").getInt(); //atoi(result[0]["image_width"].c_str());
			    nHeight = estructura.structGetValue("image_height").getInt(); //atoi(result[0]["image_height"].c_str());
			    nSize = estructura.structGetValue("image_size").getInt(); //atol(result[0]["image_size"].c_str());
                stringstream ss;
                ss << estructura.structGetValue("id").getInt(); //id = result[0]["id"].c_str();
                id = ss.str().c_str();

			    width = nWidth;
			    height = nHeight;

			    id_imagen = id.c_str();

			    //blob = new pqxx::binarystring( result[0]["image"] );

			    if(templ_size*3 > 350000)
				    size = templ_size;
			    else
				    size = templ_size*3;

                size = templ_size;
			    imagen_buffer = (unsigned char *)malloc(size);
			    memcpy(imagen_buffer, (unsigned char*)templ, templ_size);

			    return valido;
		    }
        } catch (XmlRpcFault& fault) {
		    //XmlRpcValue resultBD = XmlRpcValue::makeArray();
		    cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		    XmlRpcClient::Terminate();

		    return -1;
	    }
	}
	return valido;
}

/*!
 * Método comprobar: Comprueba si un usuario puede hacer login, 
 * en el Webservice de OpenERP.
 *
 * OPTIMIZAR: Método debe borrarse no se utiliza
 *
 */

bool TemplateAFIS_webservice::comprobar(string usuario, string password)
{
	//XmlRpcClient::Initialize(NAME, VERSION);

	try
	{
		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
		{
			return false;
		}

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		if(!fing->isURE(server_search))
		{
			delete fing;
			return false;
		}

		delete fing;

		GuiWsOpenErp * gui_vals = NULL;

		this->getWebServiceGUIOpenERP(usuario.c_str(), password.c_str(), gui_vals);

		if(gui_vals == NULL)
		{
			return false;
		}

		if(!gui_vals->Inicializar())
		{
			delete gui_vals;
			return false;
		}

		delete gui_vals;

		//XmlRpcClient::Terminate();

		return true;
	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		return false;
	}
}

/*!
 * Método getUsuario (XMLRPC): Recibe el usuario y el password. Devuelve el 
 * parámetros verificacion, para validar un usuario.
 *
 * OPTIMIZAR: Método no se utiliza debe borrarse
 *
 */

void TemplateAFIS_webservice::getUsuario(xmlrpc_env * env, string usuario, string password, xmlrpc_value * &myArrayP)
{
	XmlRpcClient::Initialize(NAME, VERSION);

	try
	{
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			myArrayP = xmlrpc_array_new(env);
			goto terminar;
			//return myArrayP;
		}

		GuiWsOpenErp * gui_vals = NULL;

		this->getWebServiceGUIOpenERP(usuario.c_str(), password.c_str(), gui_vals);

		if(gui_vals == NULL)
		{
			myArrayP = NULL;
			goto terminar;
		}

		long int uid_val = gui_vals->Inicializar();

		//myArrayP = (xmlrpc_value *)malloc(2048);//1197);
		xmlrpc_value * myStructP;
		xmlrpc_value * memberValueP;

		myArrayP = xmlrpc_array_new(env);
		myStructP = xmlrpc_struct_new(env);

		memberValueP = xmlrpc_int_new(env, uid_val);//uid_val);

		xmlrpc_struct_set_value(env, myStructP,"verificacion", memberValueP);

		xmlrpc_array_append_item(env, myArrayP, myStructP);

		xmlrpc_DECREF(memberValueP);
		xmlrpc_DECREF(myStructP);

		delete gui_vals;

		//return myArrayP;
	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		myArrayP = NULL;
	}

	terminar:

	XmlRpcClient::Terminate();
}

/*!
 * Método getDatosHuella (XMLRPC): Recibe el usuario, password y cédula.
 * Verifica que el usuario tenga permisos, y pide los datos de la huella en base a la mano, dedo, persona.
 * Devuelve el dedo y la mano, de cada huella.
 *
 * OPTIMIZAR: Método no se utiliza debe borrarse
 */

void TemplateAFIS_webservice::getDatosHuella(xmlrpc_env *env, string usuario, string password, string cedula, xmlrpc_value * &myArrayP)
{
	XmlRpcClient::Initialize(NAME, VERSION);

	//GuiWsOpenErp * gui_vals = (GuiWsOpenErp *)malloc(1024*sizeof(GuiWsOpenErp *));

	try
	{

		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			myArrayP = xmlrpc_array_new(env);
			goto terminar;
		}

		GuiWsOpenErp * gui_vals = NULL;

		this->getWebServiceGUIOpenERP(usuario.c_str(), password.c_str(), gui_vals);

		if(gui_vals == NULL)
		{
			myArrayP = NULL;
			goto terminar;
		}

		gui_vals->Inicializar();
		string mano = "I";
		int dedo = 1;
		XmlRpcValue get_usuario = gui_vals->getDatoHuella(cedula, mano, dedo);

		xmlrpc_value * myStructP;
		xmlrpc_value * memberValueP;

		myArrayP = xmlrpc_array_new(env);

    //xmlrpc_value * valueP;

    		for (size_t i = 0; i < get_usuario.arraySize(); i++) {

			myStructP = xmlrpc_struct_new(env);

			XmlRpcValue estructura = get_usuario.arrayGetItem(i);

			int finger;
			string hand;
			if(get_usuario.arraySize() == 1)
			{
				finger = dedo;
				hand = mano;
			}else
			{
				finger = estructura.structGetValue("finger").getInt();
				hand = estructura.structGetValue("hand").getString().c_str();
			}

			memberValueP = xmlrpc_int_new(env, finger);

			xmlrpc_struct_set_value(env, myStructP,"id_dedo", memberValueP);

			memberValueP = xmlrpc_string_new(env, hand.c_str());

			xmlrpc_struct_set_value(env, myStructP,"mano", memberValueP);

			xmlrpc_DECREF(memberValueP);

			xmlrpc_array_append_item(env, myArrayP, myStructP);

			xmlrpc_DECREF(myStructP);

			/*myStructP = NULL;
			memberValueP = NULL;*/
		}

		delete gui_vals;

		//return myArrayP;
	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		myArrayP = NULL;
	}

	terminar:

	XmlRpcClient::Terminate();
}

/*!
 * Método getDatosPersonaOBPP (XMLRPC): Recibe el usuario, 
 * password y codigo_obpp.
 * Devuelve el nombre y la cédula de cada persona perteneciente a esa obpp.
 *
 */

void TemplateAFIS_webservice::getDatosPersonaOBPP(xmlrpc_env *env, string usuario, string password, string codigo_obpp, xmlrpc_value * &myArrayP)
{
	XmlRpcClient::Initialize(NAME, VERSION);

	try
	{
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			myArrayP = xmlrpc_array_new(env);
			goto terminar;
		}

		GuiWsOpenErp * gui_vals = NULL;

		this->getWebServiceGUIOpenERP(usuario.c_str(), password.c_str(), gui_vals);

		if(gui_vals == NULL)
		{
			myArrayP = NULL;
			goto terminar;
		}
		gui_vals->Inicializar();
		XmlRpcValue get_usuario = gui_vals->GetPersonaByOBPP(codigo_obpp);

		xmlrpc_value * myStructP;
		xmlrpc_value * memberValueP;

		myArrayP = xmlrpc_array_new(env);

    		for (size_t i = 0; i < get_usuario.arraySize(); i++) {

			myStructP = xmlrpc_struct_new(env);

			XmlRpcValue estructura = get_usuario.arrayGetItem(i);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("name").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"primer_nombre", memberValueP);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("identification_id").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP, "cedula", memberValueP);

			xmlrpc_DECREF(memberValueP);

			xmlrpc_array_append_item(env, myArrayP, myStructP);

			xmlrpc_DECREF(myStructP);

		}

		delete gui_vals;

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		myArrayP = NULL;
	}

	terminar:

	XmlRpcClient::Terminate();
}

/*!
 * Método getDatosPersona (XMLRPC): Recibe el usuario, password y cédula.
 * Devuelve el nombre y la cédula de la persona.
 *
 */

void TemplateAFIS_webservice::getDatosPersona(xmlrpc_env *env, string usuario, string password, string cedula, xmlrpc_value * &myArrayP)
{
	XmlRpcClient::Initialize(NAME, VERSION);

	try
	{
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			myArrayP = xmlrpc_array_new(env);
			goto terminar;
		}

		GuiWsOpenErp * gui_vals = NULL;

		this->getWebServiceGUIOpenERP(usuario.c_str(), password.c_str(), gui_vals);

		if(gui_vals == NULL)
		{
			myArrayP = NULL;
			goto terminar;
		}
		
		gui_vals->Inicializar();
		XmlRpcValue get_usuario = gui_vals->GetDatoPersona(cedula);

		xmlrpc_value * myStructP;
		xmlrpc_value * memberValueP;

		myArrayP = xmlrpc_array_new(env);

    		for (size_t i = 0; i < get_usuario.arraySize(); i++) {

			myStructP = xmlrpc_struct_new(env);

			XmlRpcValue estructura = get_usuario.arrayGetItem(i);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("name").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"primer_nombre", memberValueP);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("identification_id").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP, "cedula", memberValueP);

			/*memberValueP = xmlrpc_string_new(env, result[i]["primer_nombre"].c_str());
			xmlrpc_struct_set_value(env, myStructP,"primer_nombre", memberValueP);

			memberValueP = xmlrpc_string_new(env, result[i]["segundo_nombre"].c_str());
			xmlrpc_struct_set_value(env, myStructP,"segundo_nombre", memberValueP);

			memberValueP = xmlrpc_string_new(env, result[i]["primer_apellido"].c_str());
			xmlrpc_struct_set_value(env, myStructP,"primer_apellido", memberValueP);

			memberValueP = xmlrpc_string_new(env, result[i]["segundo_apellido"].c_str());
			xmlrpc_struct_set_value(env, myStructP,"segundo_apellido", memberValueP);*/

			xmlrpc_DECREF(memberValueP);

			xmlrpc_array_append_item(env, myArrayP, myStructP);

			xmlrpc_DECREF(myStructP);

			/*myStructP = NULL;
			memberValueP = NULL;*/
		}

		delete gui_vals;

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		myArrayP = NULL;
	}

	terminar:

	XmlRpcClient::Terminate();
}

/*!
 * Método getDatosPersonaVerificacion (XMLRPC): Recibe el usuario, 
 * password y cédula. 
 * Devuelve el nombre obpp a la que pertenece, código SITUR,
 * la cédula de la persona y nombre de la persona.
 *
 */

void TemplateAFIS_webservice::getDatosPersonaVerificacion(xmlrpc_env *env, string usuario, string password, string cedula, xmlrpc_value * &myArrayP)
{
	XmlRpcClient::Initialize(NAME, VERSION);

	try
	{
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			myArrayP = xmlrpc_array_new(env);
			goto terminar;
		}

		GuiWsOpenErp * gui_vals = NULL;

		this->getWebServiceGUIOpenERP(usuario.c_str(), password.c_str(), gui_vals);

		if(gui_vals == NULL)
		{
			myArrayP = NULL;
			goto terminar;
		}

		gui_vals->Inicializar();
		XmlRpcValue get_usuario = gui_vals->getDatoPersonaVerificacion(cedula);

		/*if(result.size() == 0)
		{
			//printf("No hay datos.\n");
			return NULL;
		}*/

		xmlrpc_value * myStructP;
		xmlrpc_value * memberValueP;

		myArrayP = xmlrpc_array_new(env);

		xmlrpc_value * structProyectos;
		xmlrpc_value * memberProyectos;
		xmlrpc_value * arregloProyectos;// = xmlrpc_array_new(env);

		//xmlrpc_value * valueP;

         string executor_type_id;

    	for (size_t i = 0; i < get_usuario.arraySize(); i++) {

			arregloProyectos = xmlrpc_array_new(env);

			myStructP = xmlrpc_struct_new(env);

			XmlRpcValue estructura = get_usuario.arrayGetItem(i);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("name").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"nombre_obpp", memberValueP);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("vat").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"vat", memberValueP);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("situr_code").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"situr_code", memberValueP);

			memberValueP = xmlrpc_string_new(env, estructura.structGetValue("nombre_persona").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"nombre_persona", memberValueP);

         memberValueP = xmlrpc_string_new(env, estructura.structGetValue("tipo_title").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"tipo_title", memberValueP);

         memberValueP = xmlrpc_string_new(env, estructura.structGetValue("tipo_description").getString().c_str());
			xmlrpc_struct_set_value(env, myStructP,"tipo_description", memberValueP);

         for (size_t j = 0; j < estructura.structGetValue("proyectos").getArray().arraySize(); j++) {
            XmlRpcValue estructura_proyectos = estructura.structGetValue("proyectos").arrayGetItem(j);

            structProyectos = xmlrpc_struct_new(env);

            memberProyectos = xmlrpc_string_new(env, estructura_proyectos.structGetValue("description").getString().c_str());
			   xmlrpc_struct_set_value(env, structProyectos, "descripcion_proyecto", memberProyectos);

            memberProyectos = xmlrpc_int_new(env, estructura_proyectos.structGetValue("id").getInt());
			   xmlrpc_struct_set_value(env, structProyectos, "id_proyecto", memberProyectos);

            xmlrpc_DECREF(memberProyectos);

            xmlrpc_array_append_item(env, arregloProyectos, structProyectos);

            xmlrpc_DECREF(structProyectos);
         }

         xmlrpc_struct_set_value(env, myStructP,"arreglo_proyectos", arregloProyectos);

			xmlrpc_DECREF(memberValueP);

			xmlrpc_array_append_item(env, myArrayP, myStructP);

			xmlrpc_DECREF(myStructP);

         xmlrpc_DECREF(arregloProyectos);

			/*myStructP = NULL;
			memberValueP = NULL;*/
		}

		delete gui_vals;
	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		myArrayP = NULL;
	}

	terminar:

	XmlRpcClient::Terminate();
}

/*!
 * Método getDatosIdImagen (XMLRPC): Recibe el usuario, 
 * password, cédula, mano e id_dedo. 
 * Devuelve el ID Imagen.
 *
 */

void TemplateAFIS_webservice::getDatosIdImagen(xmlrpc_env *env, string usuario, string password, string cedula, string mano, string id_dedo, xmlrpc_value * &myArrayP)
{
	XmlRpcClient::Initialize(NAME, VERSION);

		//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

		try
		{
			if(!comprobar(usuario.c_str(), password.c_str()))
			{
				myArrayP = xmlrpc_array_new(env);
				goto terminar;
			}

			Fingerprint * fing = NULL;

			this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

			if(fing == NULL)
			{
				myArrayP = xmlrpc_array_new(env);
				goto terminar;
			}

			XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

			XmlRpcValue persona = fing->searchPersona(server_search, cedula);

			if(persona.arraySize() == 0)
			{
                delete fing;
				myArrayP = xmlrpc_array_new(env);

				goto terminar;
			}

            long int partner_id = persona.arrayGetItem(0).getInt();

			XmlRpcValue get_usuario = fing->readFingerprint(server_search, atoi(id_dedo.c_str()), mano.c_str(), partner_id);

			//Si no hay datos, devuelve -661, hay que compararlo con BD_SIN_DATOS
            delete fing;
			if(get_usuario.arraySize() == 0)
			{
				//printf("No hay datos.\n");
                myArrayP = xmlrpc_array_new(env);
				goto terminar;
			}

			XmlRpcValue estructura = get_usuario.arrayGetItem(0);

            long int s;

            s = estructura.structGetValue("image_id").arrayGetItem(0).getInt();

			xmlrpc_value * myStructP;
			xmlrpc_value * memberValueP;

			myArrayP = xmlrpc_array_new(env);

			myStructP = xmlrpc_struct_new(env);

			//XmlRpcValue estructura = get_usuario.arrayGetItem(i);
			memberValueP = xmlrpc_int_new(env, s);
			xmlrpc_struct_set_value(env, myStructP,"id_imagen", memberValueP);

			xmlrpc_DECREF(memberValueP);

			xmlrpc_array_append_item(env, myArrayP, myStructP);

			xmlrpc_DECREF(myStructP);

		} catch (XmlRpcFault& fault) {
			//XmlRpcValue resultBD = XmlRpcValue::makeArray();
			cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
			XmlRpcClient::Terminate();
			myArrayP = NULL;
		}

		terminar:

		XmlRpcClient::Terminate();
}

/*!
 * Método borrarHuella: Borrar huella.
 *
 */

bool TemplateAFIS_webservice::borrarHuella(string usuario, string password, string cedula, string mano, string id_dedo)
{
	XmlRpcClient::Initialize(NAME, VERSION);

	//Fingerprint * fing = (Fingerprint *)malloc(1024*sizeof(Fingerprint *));

	try {
		if(!comprobar(usuario.c_str(), password.c_str()))
		{
			XmlRpcClient::Terminate();
			return false;
		}

		Fingerprint * fing = NULL;

		this->getWebServiceOpenERP(usuario.c_str(), password.c_str(), fing);

		if(fing == NULL)
		{
			XmlRpcClient::Terminate();
			return false;
		}

		XmlRpcClient server_search (fing->URL + "/xmlrpc/object");

		int finger = atoi(id_dedo.c_str());//, partner_id = 67; //fing.getUID();
		string hand = mano;

		XmlRpcValue persona = fing->searchPersona(server_search, cedula.c_str());//"J123456789");//id_persona);

		if(persona.arraySize() == 0)
		{
			delete fing;

			XmlRpcClient::Terminate();
			return false;
		}

		fing->borrarFingerprintImg(finger, hand.c_str(), persona.arrayGetItem(0).getInt());

		delete fing;

	} catch (XmlRpcFault& fault) {
		//XmlRpcValue resultBD = XmlRpcValue::makeArray();
		cerr << ": XML-RPC fault #" << fault.getFaultCode() << ": " << fault.getFaultString() << endl;
		XmlRpcClient::Terminate();
		return false;
	}

	XmlRpcClient::Terminate();

	return true;
}

/*!
 * Método archivo_webservice_config: Archivo de Configuración del Webservice.
 *
 */

void TemplateAFIS_webservice::archivo_webservice_config(string directorio, string config_url, string default_url, string * &params)
{
	string line;
	stringstream ss;
	ss << directorio << config_url;
	string dir = ss.str();
	ifstream myfile( dir.c_str() );
	params = new string[5];
	int leidos = 0;
	if (myfile)
	{
		while (getline( myfile, line ))
		{
			line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

			if (strcmp(line.c_str(), "") != 0)
			{
				std::string delimiter = "=";
				std::string token = line.substr(0, line.find(delimiter));
				if(strcmp(token.c_str(), "BD") == 0)
				{
					params[0] = line.substr(line.find(delimiter)+1, line.length()-1);
					leidos++;
				}
				else if(strcmp(token.c_str(), "HOST") == 0)
				{
					params[1] = line.substr(line.find(delimiter)+1, line.length()-1);
					leidos++;
				}
			}
		}
		myfile.close();
	}

	if(leidos < 2)
	{
		params = new string[5];
		string line = "";
		stringstream ss;
		ss << directorio << default_url;
		string dir = ss.str();
		ifstream myfile_default( dir.c_str() );
		if (myfile_default)
		{
			while (getline( myfile_default, line ))
			{
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

				if (strcmp(line.c_str(), "") != 0)
				{
					std::string delimiter = "=";
					std::string token = line.substr(0, line.find(delimiter));
					if(strcmp(token.c_str(), "BD") == 0)
						params[0] = line.substr(line.find(delimiter)+1, line.length()-1);
					else if(strcmp(token.c_str(), "HOST") == 0)
						params[1] = line.substr(line.find(delimiter)+1, line.length()-1);
				}
			}
			myfile_default.close();
		}
	}

	//return params;
}

/*!
 * Método getWebServiceOpenERP: Obtiene un objeto para realizar 
 * transacciones XMLRPC.
 *
 */

void TemplateAFIS_webservice::getWebServiceOpenERP(string usuario, string pass, Fingerprint * &fing)
{
	if(NUM_WEBSERVICE < 2)
	{
		cerr << "ERROR al inicializar el webservice" << endl;

		fing = NULL;
		//exit(0);
	}else
	{

	//string conn[NUM_WEBSERVICE][NUM_PARAMS_WEBSERVICE] = WEBSERVICES;

		string * params;
		archivo_webservice_config("TemplateAFIS_webservices_lib/config/ws_config/", "config", "default", params);

		//Fingerprint * fing;

		fing = new Fingerprint(params[0].c_str(), usuario.c_str(), pass.c_str(), params[1].c_str());

		delete []params;

	    	//return fing;
	}
}

/*!
 * Método getWebServiceGUIOpenERP: Obtiene un objeto para realizar 
 * transacciones XMLRPC.
 *
 */

void TemplateAFIS_webservice::getWebServiceGUIOpenERP(string usuario, string pass, GuiWsOpenErp * &fing)
{
	if(NUM_WEBSERVICE < 2)
	{
		cerr << "ERROR al inicializar el webservice" << endl;

		fing = NULL;
		//exit(0);
	}else
	{

	//string conn[NUM_WEBSERVICE][NUM_PARAMS_WEBSERVICE] = WEBSERVICES;

		string * params;

		archivo_webservice_config("TemplateAFIS_webservices_lib/config/ws_config/", "config", "default", params);

		//GuiWsOpenErp * fing;

		fing = new GuiWsOpenErp(params[0].c_str(), usuario.c_str(), pass.c_str(), params[1].c_str());

		delete []params;
	    	//return fing;
	}
}

void TemplateAFIS_webservice::base64_decodestring( std::string text, unsigned char * &out, size_t &tlen ){

   EVP_ENCODE_CTX ectx;
	out = (unsigned char*)malloc( text.size() );
	int outlen = 0;
	tlen = 0;

	EVP_DecodeInit( &ectx );
	EVP_DecodeUpdate( &ectx, out, &outlen, (const unsigned char*)text.c_str(), text.size() );
	tlen += outlen;
	EVP_DecodeFinal( &ectx, out+tlen, &outlen );
	tlen += outlen;

	std::string data( (char*)out, tlen );
    //cerr << endl << "Data: " << data.c_str() << endl;
    //out = (unsigned char *)data.c_str();
    memcpy(out, (unsigned char *)data.c_str(), data.size());
    tlen = data.size();
	//free( out );
	//return out;
}

bool TemplateAFIS_webservice::is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string TemplateAFIS_webservice::base64_decode_crypt(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::string ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret += char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
  }

  return ret;
}

std::string TemplateAFIS_webservice::base64_encode_crypt(unsigned char const* bytes_to_encode, unsigned int in_len) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

RSA * TemplateAFIS_webservice::createRSA(unsigned char * key,int publica)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(publica)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
    if(rsa == NULL)
    {
        printf( "Failed to create RSA");
    }
 
    return rsa;
}
 
int TemplateAFIS_webservice::public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,this->padding);
    RSA_free(rsa);
    return result;
}
int TemplateAFIS_webservice::private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,this->padding);
    RSA_free(rsa);
    return result;
}

int TemplateAFIS_webservice::private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,0);
    int result = RSA_private_encrypt(data_len,data,encrypted,rsa,this->padding);
    RSA_free(rsa);
    return result;
}
int TemplateAFIS_webservice::public_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,1);
    int  result = RSA_public_decrypt(data_len,enc_data,decrypted,rsa,this->padding);
    RSA_free(rsa);
    return result;
}

string TemplateAFIS_webservice::leerTodoUnArchivo(string nombre_archivo)
{
   ifstream archivo( nombre_archivo.c_str() );
   string line;
   stringstream strings;
   strings.str("");

   if (archivo)
	{
		while (getline( archivo, line ))
		{
			strings << line.c_str() << endl;
		}
		archivo.close();
	}

   return strings.str();
}

string TemplateAFIS_webservice::desencriptarPrivada(string encriptado_base64)
{
   string privateKey=leerTodoUnArchivo("TemplateAFIS_webservices_lib/cert/rsa_2048_priv.pem");
   string text = base64_decode_crypt(encriptado_base64.c_str());

   unsigned char * encrypted=(unsigned char*)malloc(4098*sizeof(unsigned char));
   memset(encrypted,0,4098);
   size_t encrypted_length;
   unsigned char * decrypted=(unsigned char*)malloc(4098*sizeof(unsigned char));
   memset(decrypted,0,4098);
   int decrypted_length;
   memcpy(encrypted, (unsigned char *)text.c_str(), text.size());
   encrypted_length = text.size();

   decrypted_length = private_decrypt(encrypted,encrypted_length,(unsigned char *)privateKey.c_str(), decrypted);

   free(encrypted);

   if(decrypted_length == -1)
   {
      free(decrypted);
      printf("Private Decrypt failed ");
      return "";
   }

   stringstream ss;
   ss.str("");
   ss << decrypted;

   free(decrypted);

   return ss.str();
}