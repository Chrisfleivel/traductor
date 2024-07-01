/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos

#define E_NULL			256
#define LITERAL_NUM		257
//STRING
#define LITERAL_CADENA	258 
#define BOOL			259
#define OPASIGNA		260
#define IDENTIFICADOR		261
#define VALOR_IDENTIFICADOR		262
//#define E_ARRAY			263
//#define E_OBJECT			264

// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101
#define TAMNOMBRE 	20


/************* Estructuras ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...
	
} entrada;

typedef struct {
	int compLex;
	entrada *pe;
	char nombre[TAMNOMBRE];
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void getToken();
void json();
void element();
void array();
void element_list();
void element_list2();
void objeto();
void atribute_list();
void atribute_list2();
void atribute();
void atribute_name();
void atribute_value();
