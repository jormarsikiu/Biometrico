/**
* @file Variables_Webservice_OpenERP.cpp
* @brief Archivo de puente entre TemplateAFIS_webservice y, Fingerprint y GuiWsOpenERP.
* Conexión del Webservice con Odoo. Se especifican los parámetros de conexión.
* @author Asociación Cooperativa Simón Rodríguez para el Conocimiento Libre. contacto@simonrodriguez.org.ve
* @author Lcdo. César Cordero, Ing. Servio Zambrano y Ángel Ramírez Isea.
* @date 2014
*
* Para agregar otra variable de conexión, agrega una coma al arreglo y un vector al mismo. De esta forma:
*
* #define WEBSERVICES = {
* {"INTEGRACION-CAPTAHUELLAS", "admin", "123456", "probando.hoatzin.org"},
* {"INTEGRACION-CAPTAHUELLAS-2", "admin", "123456", "probando.hoatzin.org"}
* };
*
*/

#include "Fingerprint.cpp"
#include "GuiWsOpenErp.cpp"

/*!
 * @brief Se define la clase principal WEBSERVICES
 * No cambiar estos valores, a menos que modifiquen el código fuente
 * NUM_CONEXIONES debe ser mayor o igual a 2, nunca menor.
 */
#define WEBSERVICES { {"", ""}, {"", ""} };

/*!
 * @brief NUM_WEBSERVICE
 * NUM_PARAMS debe ser siempre igual a 5
 */
#define NUM_WEBSERVICE 2

/*!
 * @brief NUM_PARAMS_WEBSERVICE
 */
#define NUM_PARAMS_WEBSERVICE 2
