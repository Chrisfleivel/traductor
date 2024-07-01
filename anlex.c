/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Práctica de Programación Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuación para un lenguaje
 * 	con sintaxis tipo JSON.
 *	
 */

/*********** Inclusión de cabecera **************/
#include "anlex.h"


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente JSON
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char lexema[TAMLEX];	// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea

token token_anterior; // token anterior
/*****************Elementos del Traductor*******************/
typedef struct nodo_t {
  char *dato; // Puntero al contenido de la cadena
  struct nodo_t *siguiente; // Puntero al siguiente nodo
} nodo_t;

nodo_t *primero = NULL; // Puntero al primer nodo (cabeza de la cola)
nodo_t *ultimo = NULL; // Puntero al último nodo (cola de la cola)

void agregar_string(char *nuevo_string) {
  nodo_t *nuevo_nodo = malloc(sizeof(nodo_t)); // Reservar memoria para un nuevo nodo

  if (nuevo_nodo == NULL) { // Verificar si la memoria se asignó correctamente
    printf("Error al asignar memoria\n");
    return;
  }

  nuevo_nodo->dato = nuevo_string; // Asignar el contenido de la cadena al nuevo nodo
  nuevo_nodo->siguiente = NULL; // Inicializar el puntero `siguiente` a NULL

  if (primero == NULL) { // Si la cola está vacía
    primero = nuevo_nodo;
    ultimo = nuevo_nodo;
  } else { // Si la cola tiene nodos
    ultimo->siguiente = nuevo_nodo;
    ultimo = nuevo_nodo;
  }
}

void mostrar_cola() {
  nodo_t *actual = primero;

  if (actual == NULL) { // Si la cola está vacía
    printf("La cola está vacía\n");
    return;
  }

  while (actual != NULL) { // Recorrer la cola nodo por nodo
    printf("%s", actual->dato);
    actual = actual->siguiente;
  }
}

char *agregar_caracter_al_principio(char *string_original, char caracter) {
  // Reservar memoria para el nuevo string (tamaño original + 1 para el caracter + 1 para el terminador nulo)
  char *nuevo_string = (char *) malloc(strlen(string_original) + 2);

  // Verificar si la memoria se asignó correctamente
  if (nuevo_string == NULL) {
    printf("Error al asignar memoria\n");
    return NULL;
  }

  // Copiar el caracter al principio del nuevo string
  nuevo_string[0] = caracter;

  // Copiar el string original después del caracter agregado
  int i = 1;
  for (int j = 0; string_original[j] != '\0'; j++) {
    nuevo_string[i] = string_original[j];
    i++;
  }

  // Agregar el terminador nulo al final del nuevo string
  nuevo_string[i] = '\0';

  // Retornar el nuevo string modificado
  return nuevo_string;
}

char *agregar_caracter_al_final(char *string_original, char caracter) {
  // Reservar memoria para el nuevo string (tamaño original + 1 para el caracter + 1 para el terminador nulo)
  char *nuevo_string = (char *) malloc(strlen(string_original) + 2);

  // Verificar si la memoria se asignó correctamente
  if (nuevo_string == NULL) {
    printf("Error al asignar memoria\n");
    return NULL;
  }

  // Copiar el string original al nuevo string
  int i = 0;
  for (int j = 0; string_original[j] != '\0'; j++) {
    nuevo_string[i] = string_original[j];
    i++;
  }

  // Agregar el caracter al final del nuevo string
  nuevo_string[i] = caracter;
  i++;

  // Agregar el terminador nulo al final del nuevo string
  nuevo_string[i] = '\0';

  // Retornar el nuevo string modificado
  return nuevo_string;
}

char *eliminar_comillas_dobles(char *string_original) {
  // Check if the original string is NULL or empty
  if (string_original == NULL || strlen(string_original) == 0) {
    return NULL; // Return NULL if the input is invalid
  }

  // Allocate memory for the new string (without double quotes)
  size_t new_string_size = strlen(string_original) - 2; // Remove 2 chars for double quotes
  char *new_string = (char *) malloc(new_string_size + 1); // Add 1 for the null terminator

  // Check if memory allocation was successful
  if (new_string == NULL) {
    printf("Error allocating memory\n");
    return NULL;
  }

  // Initialize variables for tracking indices
  int i = 0; // Index for the original string
  int j = 0; // Index for the new string

  // Skip the first and last characters (double quotes) while copying the string
  for (i = 1; string_original[i] != '\0'; i++) {
    new_string[j] = string_original[i];
    j++;
  }

  // Add the null terminator to the new string
  new_string[j] = '\0';

  // Return the pointer to the new string without double quotes
  return new_string;
}

char *eliminar_comillas_dobles_final(char *string) {
  if (string == NULL || strlen(string) == 0) {
    return NULL; // Return NULL if the input is invalid
  }

  // Initialize variables for indices
  int i = strlen(string) - 1; // Start at the last character

  // Find the last non-double quote character
  while (i >= 0 && string[i] == '"') {
    i--;
  }

  // Check if a non-double quote character was found
  if (i < 0) {
    // No non-double quote characters found, return the original string
    return string;
  }

  // Create a new string with the appropriate size (excluding trailing double quotes)
  char *new_string = (char *) malloc(i + 1); // + 1 for null terminator

  // Check if memory allocation was successful
  if (new_string == NULL) {
    printf("Error allocating memory\n");
    return NULL;
  }

  // Copy characters from the original string to the new string, excluding trailing double quotes
  for (int j = 0; j <= i; j++) {
    new_string[j] = string[j];
  }

  // Add the null terminator to the new string
  new_string[i + 1] = '\0';

  // Return the new string without trailing double quotes
  return new_string;
}

/**************** Funciones **********************/


// Rutinas del analizador lexico


void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
}

void getToken()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{
		
		if (c==' ' || c=='\t')
			continue;	//eliminar espacios en blanco
		else if(c=='\n')
		{
			//incrementar el numero de linea
			numLinea++;
			continue;
		}	
		else if (c=='"')
		{
			//es un identificador 
			i=0;
			do{
				lexema[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
				
			}while(c!='"');
			lexema[i]=c;
			lexema[i+1]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
			c=0;
			t.pe=buscar(lexema); // palabras reservadas 0 id que ya aparecieron antes
			t.compLex=t.pe->compLex;
			strcpy(t.nombre, "STRING");
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,lexema);
				e.compLex=LITERAL_CADENA;
				insertar(e);
				t.pe=buscar(lexema);
				t.compLex=LITERAL_CADENA;
				strcpy(t.nombre, "STRING");
			}
			c=fgetc(archivo);
			break;
		}
		else if (isalpha(c))
		{
			//es un palabra reservada
			i=0;
			do{
				lexema[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c) || isdigit(c));
			lexema[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(lexema);
			t.compLex=t.pe->compLex;
			strcpy(t.nombre, lexema);
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,lexema);
				e.compLex=BOOL;
				insertar(e);
				t.pe=buscar(lexema);
				t.compLex=BOOL;
				strcpy(t.nombre, lexema);
			}
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				lexema[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							lexema[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							lexema[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							lexema[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							lexema[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							lexema[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						lexema[++i]='\0';
						acepto=1;
						t.pe=buscar(lexema);
						if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,lexema);
							e.compLex=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(lexema);
						}
						t.compLex=LITERAL_NUM;
						strcpy(t.nombre, "NUMBER");
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c==':')
		{
			t.compLex=OPASIGNA;
			t.pe=buscar(":");
			strcpy(t.nombre, "DOS_PUNTOS");
			break;
		}
		else if (c==',')
		{
			t.compLex=',';
			t.pe=buscar(",");
			strcpy(t.nombre, "COMA");
			break;
		}
		else if (c=='[')
		{
			t.compLex='[';
			t.pe=buscar("[");
			strcpy(t.nombre, "L_CORCHETE");
			break;
		}
		else if (c==']')
		{
			t.compLex=']';
			t.pe=buscar("]");
			strcpy(t.nombre, "R_CORCHETE");
			break;
		}
		else if (c=='{')
		{
			t.compLex='{';
			t.pe=buscar("{");
			strcpy(t.nombre, "L_LLAVE");
			break;
		}
		else if (c=='}')
		{
			t.compLex='}';
			t.pe=buscar("}");
			strcpy(t.nombre, "R_LLAVE");
			break;
		}		
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		// strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
		strcpy(t.nombre, " ");
	}
	
}

void error_sintactico(char* mensaje)
{
	printf("Lin %d: Error sintáctico:  %s.\n",numLinea,mensaje);	
}

void match(int esperado) {

	
	if(esperado == IDENTIFICADOR && t.compLex == LITERAL_CADENA){
			t.compLex=IDENTIFICADOR;
			strcpy(t.nombre, "IDENTIFICADOR");
	}else{
		if (t.compLex != esperado) {
			error_sintactico("error");
			printf("Se esperaba '%s', pero se encontró '%s'\n", esperado, t.nombre);
		}
	}
	//printf("Match '%s',  '%s'\n", t.pe->lexema, t.nombre);
	//guargarTokenAnterior();
	getToken();
}
void guargarTokenAnterior(){
	token_anterior.pe=t.pe;
	token_anterior.compLex=t.compLex;
	strcpy(token_anterior.nombre, t.nombre);
}


void json(){
	element();	
}

void element(){
	switch (t.compLex) {
		case '{':
			// elige objetos
			objeto();
			break;
	    case '[':
			// elige array
			array();
			break;
	    case EOF:
			break;
	}
}

void array(){
	match('[');
	element_list();
	match(']');	
}

void element_list(){
	agregar_string("<item>  \n");
	element();
	element_list2();
	agregar_string("</item>  \n");
}

void element_list2(){
	switch (t.compLex) {
	case ',':
		//
		match(',');
		element_list();
		element_list2();
		break;
	}
}

void objeto(){
	match('{');
	atribute_list();
	match('}');
}

void atribute_list(){
	
	atribute();
	atribute_list2();
}

void atribute_list2(){
	switch (t.compLex) {
	case ',':
		//
		match(',');
		atribute_list();
		atribute_list2();
		break;
	}
}

void atribute(){
	char * nombre_campo = eliminar_comillas_dobles_final(eliminar_comillas_dobles(t.pe->lexema));
	agregar_string(agregar_caracter_al_final(agregar_caracter_al_principio(nombre_campo,'<'),'>'));
	atribute_name();
	match(OPASIGNA); 
	atribute_value();
	agregar_string(agregar_caracter_al_final(agregar_caracter_al_principio(agregar_caracter_al_principio(nombre_campo,'/'),'<'),'>'));
}

void atribute_name(){
	match(IDENTIFICADOR); 
}

void atribute_value(){
	switch (t.compLex) {
		case '{':
			agregar_string("  \n");
			element();
			break;
		case '[':
			agregar_string("  \n");
			element();
			break;
		case LITERAL_CADENA:
			agregar_string(t.pe->lexema);
			match(LITERAL_CADENA);
			break;
		case LITERAL_NUM:
			agregar_string(t.pe->lexema);
			match(LITERAL_NUM);
			break;
		case BOOL:
			agregar_string(t.pe->lexema);
			match(BOOL);
			break;
		case E_NULL:
			agregar_string(t.pe->lexema);
			match(E_NULL);
			break;
		default:
			error_sintactico("se esperava '{' '[' 'LITERAL_CADENA' 'LITERAL_NUM' 'BOOL' 'E_NULL'");
	}
}

int main(int argc,char* args[])
{
	// inicializar analizador lexico
	initTabla();
	initTablaSimbolos();
	int lineaAnterior = 0;
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			getToken();
			json();
		}
		fclose(archivo);
		printf("\n Analisis Sintactico finalizado\n");
		mostrar_cola();
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}
