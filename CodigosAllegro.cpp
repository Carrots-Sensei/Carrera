#include "pch.h" // Inclusi�n de librer�as
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <Windows.h>
#include <time.h>
#include <errno.h>

#include <allegro5/allegro.h> // Allegro en General
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h> // Fuentes de Letras
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h> // Manejo de Im�genes
#include <allegro5/allegro_audio.h> // Manejo de Sonido
#include <allegro5/allegro_acodec.h>

using namespace std;

// Estructuras
typedef struct Vehiculo
{
	/* Estructura que controla el jugador. Almacena en su interior varios de los elementos
	que se imprimen con el dash. Almacena su posici�n en coordenadas. Cuenta con un puntero
	para su respectivo Bitmap. */

	int Vida;
	int Puntos;
	int Municion;
	int Velocidad;
	float Distancia;
	float Temperatura;
	float Gas;
	float Viento;
	int Choques;
	int Nieve;

	int CoordX;
	int CoordY;

	ALLEGRO_BITMAP *Imagen;
}*PtrVehiculo;
typedef struct VehiculoEnemigo
{
	/* Estructura que representa los dem�s veh�culos que aparecen cada cierto tiempo en el
	juego. Almacenan su posici�n en coordenadas. Cuentan con un puntero para su respectivo
	Bitmap. Cuentan con punteros para lista enlazada. El identificador es una llave �nica
	para cada elemento. */

	int Vida;
	int Identificador;

	int CoordX;
	int CoordY;

	ALLEGRO_BITMAP *Imagen;
	VehiculoEnemigo *Anterior;
	VehiculoEnemigo *Siguiente;
}*PtrVehiculoEnemigo;
typedef struct Proyectil
{
	/* Se corresponde con los proyectiles que dispara el jugador. Almacenan su posici�n en
	coordenadas. Cuenta con punteros para lista enlazada. El atributo direcci�n es el
	m�todo empleado para conocer la direcci�n en que viaja el proyectil. El atributo
	identificador es una llave �nica para cada elemento. */

	int CoordX;
	int CoordY;
	int Identificador;

	char Direccion;

	Proyectil *Anterior;
	Proyectil *Siguiente;

	ALLEGRO_BITMAP *Imagen;
}*PtrProyectil;
typedef struct Bonus
{
	/* Se corresponde con todos los dem�s elementos que aparecen en pantalla. Almacenan su
	posici�n en coordenadas. Cuenta con punteros para lista enlazada. El atributo etiqueta
	es el m�todo empleado para diferenciar los distintos tipos de Bonus. El atributo
	identificador es una llave �nica para cada elemento. Posee un puntero para su propio
	Bitmap. */

	char Etiqueta;

	int Identificador;
	int CoordX;
	int CoordY;

	Bonus *Anterior;
	Bonus *Siguiente;

	ALLEGRO_BITMAP *Imagen;
}*PtrBonus;

// Globales
#define mejoresPuntajesJuego 10

enum Direccion { UP, DOWN, RIGHT, LEFT };
enum Teclas { J, I, K, L };

bool teclasDireccion[4] = { false, false, false, false };
bool teclasDisparo[4] = { false, false, false, false };
bool flechasDireccion[4] = { false, false, false, false };

int EnemigosAparecidos = 0;
int EnemigosDestruidos = 0;
int ProyectilesGenerados = 0;
int BonusGenerados = 0;

int GasPresente = 0;
int MunicionPresente = 0;
int VientoPresente = 0;
int VidaPresente = 0;

int MejoresPuntajes[mejoresPuntajesJuego];

FILE *Documento;

// Allegro
ALLEGRO_COLOR Transparente;
ALLEGRO_COLOR Amarillo;
ALLEGRO_COLOR Verde;
ALLEGRO_COLOR Blanco;

ALLEGRO_BITMAP *Carretera;
ALLEGRO_BITMAP *FlechaArriba;
ALLEGRO_BITMAP *FlechaAbajo;
ALLEGRO_BITMAP *FlechaDerecha;
ALLEGRO_BITMAP *FlechaIzquierda;

ALLEGRO_FONT *Texto;
ALLEGRO_FONT *Texto2;
ALLEGRO_FONT *Titulo;
ALLEGRO_FONT *Instru;

#define FPS 30.0

// Funciones de inventario PtrVehiculoEnemigo
void IniciarInventario(PtrVehiculoEnemigo &Lista)
{
	Lista = NULL; // Asigna la direcci�n NULL al puntero
}
void AgregarFinalInventario(PtrVehiculoEnemigo &Lista, PtrVehiculoEnemigo &Nuevo)
{
	if (Lista != NULL)
	{
		PtrVehiculoEnemigo Aux; // Inicializaci�n de un puntero auxiliar
		Aux = Lista; // Se asigna la direcci�n del puntero auxiliar al primer elemento de la colecci�n

		while (Aux->Siguiente != NULL) { // Busca el �ltimo elemento en la colecci�n
			Aux = Aux->Siguiente; // Se asigna la direcci�n del siguiente elemento
		}
		Aux->Siguiente = Nuevo; // La direcci�n del anterior �ltimo elemento cambia
		Nuevo->Anterior = Aux; // El nuevo �ltimo elemento apunta al anterior
	}
	else
	{
		Lista = Nuevo;
	}
}
void AgregarInicioInventario(PtrVehiculoEnemigo &Lista, PtrVehiculoEnemigo &Nuevo)
{
	if (Lista != NULL)
	{
		Nuevo->Siguiente = Lista; // Asigna el valor del nuevo puntero al que posee el puntero de la lista
		Lista->Anterior = Nuevo; // La direcci�n del anterior primer elemento apunta hacia el nuevo
		Lista = Nuevo; // La direcci�n del puntero de la lista es la del nuevo elemento
	}
	else
	{
		Lista = Nuevo;
	}
}

// Funciones de inventario PtrProyectil
void IniciarInventarioProyectil(PtrProyectil &Lista)
{
	Lista = NULL; // Asigna la direcci�n NULL al puntero
}
void AgregarFinalInventarioProyectil(PtrProyectil &Lista, PtrProyectil &Nuevo)
{
	if (Lista != NULL)
	{
		PtrProyectil Aux; // Inicializaci�n de un puntero auxiliar
		Aux = Lista; // Se asigna la direcci�n del puntero auxiliar al primer elemento de la colecci�n

		while (Aux->Siguiente != NULL) { // Busca el �ltimo elemento en la colecci�n
			Aux = Aux->Siguiente; // Se asigna la direcci�n del siguiente elemento
		}
		Aux->Siguiente = Nuevo; // La direcci�n del anterior �ltimo elemento cambia
		Nuevo->Anterior = Aux; // El nuevo �ltimo elemento apunta al anterior
	}
	else
	{
		Lista = Nuevo;
	}
}
void AgregarInicioInventarioProyectil(PtrProyectil &Lista, PtrProyectil &Nuevo)
{
	if (Lista != NULL)
	{
		Nuevo->Siguiente = Lista; // Asigna el valor del nuevo puntero al que posee el puntero de la lista
		Lista->Anterior = Nuevo; // La direcci�n del anterior primer elemento apunta hacia el nuevo
		Lista = Nuevo; // La direcci�n del puntero de la lista es la del nuevo elemento
	}
	else
	{
		Lista = Nuevo;
	}
}

// Funciones de inventario PtrBonus
void IniciarInventarioBonus(PtrBonus &Lista)
{
	Lista = NULL; // Asigna la direcci�n NULL al puntero
}
void AgregarFinalInventarioBonus(PtrBonus &Lista, PtrBonus &Nuevo)
{
	if (Lista != NULL)
	{
		PtrBonus Aux; // Inicializaci�n de un puntero auxiliar
		Aux = Lista; // Se asigna la direcci�n del puntero auxiliar al primer elemento de la colecci�n

		while (Aux->Siguiente != NULL) { // Busca el �ltimo elemento en la colecci�n
			Aux = Aux->Siguiente; // Se asigna la direcci�n del siguiente elemento
		}
		Aux->Siguiente = Nuevo; // La direcci�n del anterior �ltimo elemento cambia
		Nuevo->Anterior = Aux; // El nuevo �ltimo elemento apunta al anterior
	}
	else
	{
		Lista = Nuevo;
	}
}
void AgregarInicioInventarioBonus(PtrBonus &Lista, PtrBonus &Nuevo)
{
	if (Lista != NULL)
	{
		Nuevo->Siguiente = Lista; // Asigna el valor del nuevo puntero al que posee el puntero de la lista
		Lista->Anterior = Nuevo; // La direcci�n del anterior primer elemento apunta hacia el nuevo
		Lista = Nuevo; // La direcci�n del puntero de la lista es la del nuevo elemento
	}
	else
	{
		Lista = Nuevo;
	}
}

// Funciones de colisi�n
bool CalcularSuperposicionBitmaps(int Bit1CoordX, int Bit1CoordY, int Bit2CoordX, int Bit2CoordY, int Modo)
{
	/* Se consider� conveniente disponer de una funci�n que indicara cuando dos bitmaps diferentes
	se han superpuesto uno con otro, evento que entenderemos por colisi�n. Debido a que se manejan
	muchos tipos de datos (estructuras) diferentes la mejor opci�n era la que permitiese resolver
	los casos sin que importase el tipo de dato, y esa es la raz�n por la que �nicamente se
	ingresan valores enteros (de coordenadas) en los par�metros de la funci�n.

	El modo es un m�todo dise�ado para diferenciar el tipo de colisi�n que desea calcularse. Se usa
	para ajustar unos valores internos de la funci�n. */

	bool ResultadoColision = false;

	int AnchoCarro = 13;
	int LargoCarro = 29;

	int ConstX;
	int ConstY;

	switch (Modo) // Define el tipo de colisi�n que se est� analizando
	{
	case 0: // Colisi�n Veh�culo-Veh�culo Enemigo
	{
		ConstX = AnchoCarro - 1;
		ConstY = LargoCarro - 1;
		break;
	}
	case 1: // Colisi�n Veh�culo Enemigo-Blast Vertical
	{
		ConstX = 9;
		ConstY = 15;
		break;
	}
	case 2: // Colisi�n Veh�culo-Blast Horizontal
	{
		ConstX = 15;
		ConstY = 9;
		break;
	}
	case 3: // Colisi�n Gas-Veh�culo
	{
		ConstX = 11;
		ConstY = 15;
		break;
	}
	case 4: // Colisi�n Nitro-Veh�culo
	{
		ConstX = 11;
		ConstY = 15;
		break;
	}
	case 5: // Colisi�n Ammo-Veh�culo
	{
		ConstX = 14;
		ConstY = 15;
		break;
	}
	case 6: // Colisi�n USSR-Veh�culo
	{
		ConstX = 15;
		ConstY = 14;
		break;
	}
	case 7: // Colisi�n Coraz�n-Veh�culo
	{
		ConstX = 15;
		ConstY = 13;
		break;
	}
	case 8: {//Colision Viento-Vehiculo
		ConstX = 30;
		ConstY = 16;
	}

	case 9: { //Colision Charco-Vehiculo
		ConstX = 40;
		ConstY = 50;
	}
	case 10: { // Colision Nieve-Vehiculo
		ConstX = 35;
		ConstY = 35;
	}
	case 11: { //Colision Cono-Vehiculo
		ConstX = 30;
		ConstY = 30;
	}
	}

	// Colisi�n en la esquina superior izquierda del veh�culo
	bool CondicionEjeX = (Bit2CoordX >= Bit1CoordX) && (Bit2CoordX <= Bit1CoordX + ConstX);
	bool CondicionEjeY = (Bit2CoordY >= Bit1CoordY) && (Bit2CoordY <= Bit1CoordY + ConstY);

	// Colisi�n en la esquina inferior derecha del veh�culo
	bool CondicionEjeXCorrida = (Bit2CoordX + ConstX >= Bit1CoordX) && (Bit2CoordX + ConstX <= Bit1CoordX + ConstX);
	bool CondicionEjeYCorrida = (Bit2CoordY + ConstY >= Bit1CoordY) && (Bit2CoordY + ConstY <= Bit1CoordY + ConstY);

	// Cruce de condiciones
	if ((CondicionEjeX && CondicionEjeY) || (CondicionEjeXCorrida && CondicionEjeYCorrida))
	{
		ResultadoColision = true;
	}

	return ResultadoColision;
}
void BorrarEnemigo(PtrVehiculoEnemigo &Lista, PtrVehiculoEnemigo &Borrando)
{
	/* Por tratarse de una lista doblemente enlazada es que se requiere de control adicional
	para las distintas situaciones que puedan presentarse. Hay 5 escenarios cuando de borrar
	un elemento se trata:

	0) Lista vac�a
	1) Lista con un �nico elemento
	2) Primer elemento de la lista
	3) �ltimo elemento de la lista
	4) Un elemento diferente cualquiera

	Para cada caso se requiere ajustar alg�n que otro elemento diferente. Se plantea de esta
	forma para evitar el caso erroneo donde se intenta acceder a NULL. */

	if (Lista != NULL) // Se comprueba si la lista se encuentra vac�a.
	{
		PtrVehiculoEnemigo Aux = Lista;

		while (Aux != NULL) // Se recorre la lista en busca del elemento por borrar.
		{
			if (Aux->Identificador == Borrando->Identificador)
			{
				break;
			}
			else
			{
				Aux = Aux->Siguiente;
			}
		}

		if (Aux != NULL)
		{
			if ((Aux->Anterior == NULL) && (Aux->Siguiente == NULL)) // �nico
			{
				Lista = NULL;
			}
			else if ((Aux->Anterior == NULL) && (Aux->Siguiente != NULL)) // Primero
			{
				Aux->Siguiente->Anterior = NULL;
				Lista = Aux->Siguiente;
			}
			else if ((Aux->Anterior != NULL) && (Aux->Siguiente == NULL)) // �ltimo
			{
				Aux->Anterior->Siguiente = NULL;
			}
			else // Otros
			{
				Aux->Siguiente->Anterior = Aux->Anterior;
				Aux->Anterior->Siguiente = Aux->Siguiente;
			}
			free(Borrando);
			Aux = NULL;
			Borrando = NULL;
		}
	}
}
void BorrarProyectil(PtrProyectil &Lista, PtrProyectil &Borrando)
{
	/* Por tratarse de una lista doblemente enlazada es que se requiere de control adicional
	para las distintas situaciones que puedan presentarse. Hay 5 escenarios cuando de borrar
	un elemento se trata:

	0) Lista vac�a
	1) Lista con un �nico elemento
	2) Primer elemento de la lista
	3) �ltimo elemento de la lista
	4) Un elemento diferente cualquiera

	Para cada caso se requiere ajustar alg�n que otro elemento diferente. Se plantea de esta
	forma para evitar el caso erroneo donde se intenta acceder a NULL. */

	if (Lista != NULL) // Se comprueba si la lista se encuentra vac�a.
	{
		PtrProyectil Aux = Lista;

		while (Aux != NULL) // Se recorre la lista en busca del elemento por borrar.
		{
			if (Aux->Identificador == Borrando->Identificador)
			{
				break;
			}
			else
			{
				Aux = Aux->Siguiente;
			}
		}

		if (Aux != NULL)
		{
			if ((Aux->Anterior == NULL) && (Aux->Siguiente == NULL)) // �nico
			{
				Lista = NULL;
			}
			else if ((Aux->Anterior == NULL) && (Aux->Siguiente != NULL)) // Primero
			{
				Aux->Siguiente->Anterior = NULL;
				Lista = Aux->Siguiente;
			}
			else if ((Aux->Anterior != NULL) && (Aux->Siguiente == NULL)) // �ltimo
			{
				Aux->Anterior->Siguiente = NULL;
			}
			else // Otro
			{
				Aux->Siguiente->Anterior = Aux->Anterior;
				Aux->Anterior->Siguiente = Aux->Siguiente;
			}
			free(Borrando);
			Aux = NULL;
			Borrando = NULL;
		}
	}
}
void BorrarBonus(PtrBonus &Lista, PtrBonus &Borrando)
{
	/* Por tratarse de una lista doblemente enlazada es que se requiere de control adicional
	para las distintas situaciones que puedan presentarse. Hay 5 escenarios cuando de borrar
	un elemento se trata:

	0) Lista vac�a
	1) Lista con un �nico elemento
	2) Primer elemento de la lista
	3) �ltimo elemento de la lista
	4) Un elemento diferente cualquiera

	Para cada caso se requiere ajustar alg�n que otro elemento diferente. Se plantea de esta
	forma para evitar el caso erroneo donde se intenta acceder a NULL. */

	if (Lista != NULL) // Se comprueba si la lista se encuentra vac�a.
	{
		PtrBonus Aux = Lista;

		while (Aux != NULL) // Se recorre la lista en busca del elemento por borrar.
		{
			if (Aux->Identificador == Borrando->Identificador)
			{
				break;
			}
			else
			{
				Aux = Aux->Siguiente;
			}
		}

		if (Aux != NULL)
		{
			if ((Aux->Anterior == NULL) && (Aux->Siguiente == NULL)) // �nico
			{
				Lista = NULL;
			}
			else if ((Aux->Anterior == NULL) && (Aux->Siguiente != NULL)) // Primero
			{
				Aux->Siguiente->Anterior = NULL;
				Lista = Aux->Siguiente;
			}
			else if ((Aux->Anterior != NULL) && (Aux->Siguiente == NULL)) // �ltimo
			{
				Aux->Anterior->Siguiente = NULL;
			}
			else // Otro
			{
				Aux->Siguiente->Anterior = Aux->Anterior;
				Aux->Anterior->Siguiente = Aux->Siguiente;
			}
			free(Borrando);
			Aux = NULL;
			Borrando = NULL;
		}
	}
}
void DiferenciarBonus(PtrVehiculo &Personaje, char Etiqueta)
{
	/* Funci�n que diferencia por su etiqueta el efecto que cada bonus debe tener sobre el
	personaje. */

	switch (Etiqueta)
	{
	case 'G':
	{
		GasPresente--; // Control interno de la cantidad de elementos presentes

		if (Personaje->Gas <= 65)
		{
			Personaje->Gas += 35;
		}
		else // Cuando el nivel de gas�lina es mayor a 65 se asigna en direcamente 100 el resultado.
		{
			Personaje->Gas = 100;
		}
		break;
	}
	case 'A':
	{
		MunicionPresente--; // Control interno de la cantidad de elementos presentes

		if (Personaje->Municion <= 5)
		{
			Personaje->Municion += 5;
		}
		else // Cuando la cantidad de munici�n es mayor a 5 se asigna en direcamente 10 el resultado.
		{
			Personaje->Municion = 10;
		}
		break;
	}
	case 'U':
	{
		Personaje->Puntos += 35;
		break;
	}
	case 'H':
	{
		VidaPresente--; // Control interno de la cantidad de elementos presentes

		if (Personaje->Vida <= 75)
		{
			Personaje->Vida += 25;
		}
		else  // Cuando la cantidad de vida es mayor a 75 se asigna en direcamente 100 el resultado.
		{
			Personaje->Municion = 100;
		}
		break;
	}
	case 'V':
	{
		Personaje->Viento += 10;
		Personaje->Velocidad += 5;
		Personaje->Temperatura -= 2;
		break;
	}
	case 'W':
	{
		Personaje->Velocidad -= 5;
		Personaje->Choques += 1;
		break;
	}
	case 'S':
	{
		if (Personaje->Temperatura >= 25)
		{
			Personaje->Temperatura -= 5;
			Personaje->Nieve += 1;
		}
		else
		{
			Personaje->Temperatura = 20;
			Personaje->Nieve += 1;
		}
		break;
	}
	case 'C':
	{
		Personaje->Velocidad -= 5;
		Personaje->Choques += 1;
		break;
	}
	default:
	{
		break;
	}
	}
}
void DescontarBonus(char Etiqueta)
{
	/* Funci�n que descuenta de los contadores globales los bonus que han salido del mapa. */

	switch (Etiqueta)
	{
	case 'G':
	{
		GasPresente--;
		break;
	}
	case 'A':
	{
		MunicionPresente--;
		break;
	}
	case 'H':
	{
		VidaPresente--;
		break;
	}
	case 'V':
	{
		VientoPresente--;
		break;
	}
	default:
	{
		break;
	}
	}
}

// Funciones de veh�culo
void IniciarPersonaje(PtrVehiculo &Personaje)
{
	/* Inicializa al personaje del juego con las condiciones requeridas para jugar. */

	Personaje->Vida = 100;
	Personaje->Gas = 100;
	Personaje->Velocidad = 0;
	Personaje->Municion = 10;
	Personaje->Puntos = 0;
	Personaje->Temperatura = 22;
	Personaje->Viento = 30;
	Personaje->Distancia = 0;
	Personaje->Choques = 0;
	Personaje->Nieve = 0;

	Personaje->CoordX = 80;
	Personaje->CoordY = 450;

	Personaje->Imagen = al_load_bitmap("Images/Carro_Normal.png");
}
void MoverPersonaje(PtrVehiculo &Personaje, bool CondicionPiloto)
{
	if (Personaje->Gas > 0)
	{
		// M�rgenes del carro dentro del mapa
		int AnchoCarro = 13;
		int LargoCarro = 29;

		// Valores constantes
		int ConstanteMovimientoHorizontal = 2;
		int ConstanteMovimientoVertical = 3;
		int Desaceleracion = 2;

		float Dist = 0.1;
		float Calor = 0.045;
		float GastoGas = 0.1;

		if (teclasDireccion[DOWN])
		{
			if (Personaje->CoordY < (480 - LargoCarro))
			{
				Personaje->CoordY += ConstanteMovimientoVertical;
				if (Personaje->Velocidad >= 0)
				{
					Personaje->Velocidad -= ConstanteMovimientoVertical;
				}
			}
		}
		if (teclasDireccion[UP])
		{
			if (Personaje->CoordY > 0)
			{
				Personaje->CoordY -= ConstanteMovimientoVertical;
				Personaje->Distancia += Dist;

				if (!CondicionPiloto)
				{
					Personaje->Gas -= GastoGas;
					Personaje->Temperatura += Calor;
				}

				if (Personaje->Velocidad <= 110)
				{
					Personaje->Velocidad += ConstanteMovimientoVertical;
				}
			}
		}
		else // Cuando no se presiona la tecla de avanzar, el carro regresa.
		{
			if (Personaje->CoordY < (480 - LargoCarro))
			{
				Personaje->CoordY += Desaceleracion;

				if (Personaje->Velocidad > 0)
				{
					Personaje->Velocidad -= Desaceleracion;
				}
				if (Personaje->Temperatura > 21)
				{
					Personaje->Temperatura -= Calor;
				}
			}
		}
		if (teclasDireccion[RIGHT])
		{
			if (Personaje->CoordX < (720 - (534 + AnchoCarro)))
			{
				Personaje->CoordX += ConstanteMovimientoHorizontal;

				if (!CondicionPiloto)
				{
					Personaje->Gas -= GastoGas;
				}
			}
		}
		if (teclasDireccion[LEFT])
		{
			if (Personaje->CoordX > 50)
			{
				Personaje->CoordX -= ConstanteMovimientoHorizontal;
				if (!CondicionPiloto)
				{
					Personaje->Gas -= GastoGas;
				}
			}
		}
	}
}
void DibujarPersonaje(PtrVehiculo Personaje)
{
	/* Dibuja al personaje en pantalla. */

	int CoordenadaX = Personaje->CoordX;
	int CoordenadaY = Personaje->CoordY;

	al_draw_bitmap(Personaje->Imagen, CoordenadaX, CoordenadaY, NULL);
	al_flip_display();
}

// Funciones de veh�culo enemigo
PtrVehiculoEnemigo CrearEnemigo()
{
	/* Inicializa cada veh�culo enemigo con ciertos valores determinados. */

	PtrVehiculoEnemigo CarroEnemigo = new(VehiculoEnemigo);

	CarroEnemigo->Vida = 1;
	CarroEnemigo->Imagen = al_load_bitmap("Images/CarroEnemigo.png");
	CarroEnemigo->CoordY = rand() % 35;

	int Carril = 1 + rand() % 6; // M�todo que selecciona el carril por donde se mover� el carro.

	switch (Carril) // Seg�n sea un carril u otro se especifican ciertas coordenadas de aparici�n. 
	{
	case 1:
	{
		CarroEnemigo->CoordX = 53;
		break;
	}
	case 2:
	{
		CarroEnemigo->CoordX = 75;
		break;
	}
	case 3:
	{
		CarroEnemigo->CoordX = 97;
		break;
	}
	case 4:
	{
		CarroEnemigo->CoordX = 121;
		break;
	}
	case 5:
	{
		CarroEnemigo->CoordX = 143;
		break;
	}
	case 6:
	{
		CarroEnemigo->CoordX = 165;
		break;
	}
	}

	EnemigosAparecidos++;
	CarroEnemigo->Identificador = EnemigosAparecidos;

	CarroEnemigo->Anterior = NULL; // Punteros
	CarroEnemigo->Siguiente = NULL;

	return CarroEnemigo;
}
void DibujarEnemigos(PtrVehiculoEnemigo Lista)
{
	/* Dibuja a los carros enemigos en pantalla. */

	if (Lista != NULL)
	{
		while (Lista != NULL)
		{
			int CoordenadaX = Lista->CoordX;
			int CoordenadaY = Lista->CoordY;

			if (Lista->Imagen != NULL)
			{
				al_draw_bitmap(Lista->Imagen, CoordenadaX, CoordenadaY, NULL);
			}
			Lista = Lista->Siguiente;
		}

		al_flip_display();
	}
}
void MoverEnemigos(PtrVehiculoEnemigo &Lista, PtrVehiculo &Personaje)
{
	if (Lista != NULL)
	{
		PtrVehiculoEnemigo Aux = Lista;
		PtrVehiculoEnemigo Aux2;
		int ConstanteMovimiento = 3;

		while (Aux != NULL) // Se recorre la lista de carros enemigos
		{
			Aux->CoordY += ConstanteMovimiento; // Se desplazan las coordenadas del carro.
			Aux2 = Aux->Siguiente;

			if (CalcularSuperposicionBitmaps(Personaje->CoordX, Personaje->CoordY, Aux->CoordX, Aux->CoordY, 0))
			{
				// Se verifica si ha sucedido una colisi�n al moverse el carro.
				BorrarEnemigo(Lista, Aux);
				Personaje->Vida -= 25;
				Personaje->Gas -= 20;
			}
			else if (Aux->CoordY >= 480) // Verifica si el carro ha salido de pantalla. 
			{
				BorrarEnemigo(Lista, Aux);
			}

			Aux = Aux2;
		}
	}
}

// Funciones de proyectil
PtrProyectil CrearProyectil(PtrVehiculo CarroReferencia, char Direccion)
{
	/* Se inicializan proyectiles con ciertas configuraciones determindas. */

	PtrProyectil Nuevo = new(Proyectil);

	// El proyectil aparece en la posici�n del carro.
	Nuevo->CoordX = CarroReferencia->CoordX + 7;
	Nuevo->CoordY = CarroReferencia->CoordY + 15;

	// Se espec�fica la direcci�n del proyectil
	Nuevo->Direccion = Direccion;

	switch (Direccion) // Se carga un BitMap diferentes seg�n la direcci�n del proyectil.
	{
	case 'I':
	{
		Nuevo->Imagen = al_load_bitmap("Images/BlastUP.png");
		break;
	}
	case 'K':
	{
		Nuevo->Imagen = al_load_bitmap("Images/BlastDown.png");
		break;
	}
	case 'J':
	{
		Nuevo->Imagen = al_load_bitmap("Images/BlastLeft.png");
		break;
	}
	case 'L':
	{
		Nuevo->Imagen = al_load_bitmap("Images/BlastRight.png");
		break;
	}
	}

	Nuevo->Siguiente = NULL; // Punteros
	Nuevo->Anterior = NULL;

	ProyectilesGenerados++; // Identificador
	Nuevo->Identificador = ProyectilesGenerados;

	return Nuevo;
}
void MoverProyectiles(PtrProyectil &ListaProyectiles, PtrVehiculoEnemigo &ListaEnemigos, PtrVehiculo Personaje)
{
	/* Se mueven los proyectiles dentro de un �rea determinada. Seg�n la direcci�n, se mueven en un
	eje u otro. Borra los proyectiles que han salido del mapa. Si detecta una colisi�n con alg�n
	veh�culo, borra ambos elementos.*/

	if (ListaProyectiles != NULL)
	{
		int ConstHor = 4;
		int ConstVer = 5;

		PtrProyectil Aux1 = ListaProyectiles;
		PtrProyectil Aux2;

		while (Aux1 != NULL)
		{
			Aux2 = Aux1->Siguiente;
			switch (Aux1->Direccion)
			{
			case 'I': // Direcci�n: Adelante
			{
				Aux1->CoordY -= ConstVer;

				if (Aux1->CoordY < -9)
				{
					BorrarProyectil(ListaProyectiles, Aux1);
				}

				break;
			}
			case 'K': // Direcci�n: Atr�s
			{
				Aux1->CoordY += ConstVer;

				if (Aux1->CoordY > 480)
				{
					BorrarProyectil(ListaProyectiles, Aux1);
				}

				break;
			}
			case 'J': // Direcci�n: Izquierda
			{
				Aux1->CoordX -= ConstHor;

				if (Aux1->CoordX < 50)
				{
					BorrarProyectil(ListaProyectiles, Aux1);
				}

				break;
			}
			case 'L': // Direcci�n: Derecha
			{
				Aux1->CoordX += ConstHor;

				if (Aux1->CoordX > (720 - 550))
				{
					BorrarProyectil(ListaProyectiles, Aux1);
				}

				break;
			}
			}

			PtrVehiculoEnemigo AuxA = ListaEnemigos;
			PtrVehiculoEnemigo AuxB;

			while ((AuxA != NULL) && (Aux1 != NULL)) // Detecci�n de colisi�n.
			{
				AuxB = AuxA->Siguiente;

				if ((Aux1->Direccion == 'I') || (Aux1->Direccion == 'K')) // Colisi�n vertical
				{
					if (CalcularSuperposicionBitmaps(AuxA->CoordX, AuxA->CoordY, Aux1->CoordX, Aux1->CoordY, 1))
					{
						BorrarEnemigo(ListaEnemigos, AuxA);
						BorrarProyectil(ListaProyectiles, Aux1);
						Personaje->Puntos += 15;
						EnemigosDestruidos += 1;
					}
				}
				else if ((Aux1->Direccion == 'J') || (Aux1->Direccion == 'L')) // Colisi�n horizontal
				{
					if (CalcularSuperposicionBitmaps(AuxA->CoordX, AuxA->CoordY, Aux1->CoordX, Aux1->CoordY, 2))
					{
						BorrarEnemigo(ListaEnemigos, AuxA);
						BorrarProyectil(ListaProyectiles, Aux1);
						Personaje->Puntos += 15;
						EnemigosDestruidos += 1;
					}
				}
				AuxA = AuxB;
			}
			Aux1 = Aux2;
		}
	}
}
void DibujarPoyectil(PtrProyectil Lista)
{
	/* Dibuja los proyectiles presentes en pantalla. */

	if (Lista != NULL)
	{
		while (Lista != NULL)
		{
			int CoordenadaX = Lista->CoordX;
			int CoordenadaY = Lista->CoordY;
			al_draw_bitmap(Lista->Imagen, CoordenadaX, CoordenadaY, NULL);
			Lista = Lista->Siguiente;
		}
		al_flip_display();
	}
}

// Funciones de bonus
PtrBonus CrearBonus(int Modo)
{
	/* Se inicializan los bonus. */

	PtrBonus Nuevo = new(Bonus);

	// Se inicia un bonus diferente seg�n su etiqueta.
	switch (Modo)
	{
	case 0: // Gas�lina
	{
		Nuevo->Etiqueta = 'G';
		Nuevo->Imagen = al_load_bitmap("Images/Gas.png");
		GasPresente++;
		break;
	}
	case 1: // Nitro
	{
		Nuevo->Etiqueta = 'N';
		Nuevo->Imagen = al_load_bitmap("Images/Nitro.png");
		break;
	}
	case 2: // Munici�n
	{
		Nuevo->Etiqueta = 'A';
		Nuevo->Imagen = al_load_bitmap("Images/Ammo.png");
		MunicionPresente++;
		break;
	}
	case 3: // Soviet flag
	{
		Nuevo->Etiqueta = 'U';
		Nuevo->Imagen = al_load_bitmap("Images/USSR.png");
		break;
	}
	case 4: // Coraz�n
	{
		Nuevo->Etiqueta = 'H';
		Nuevo->Imagen = al_load_bitmap("Images/Heart.png");
		VidaPresente++;
		break;
	}

	case 5: //Viento
	{
		Nuevo->Etiqueta = 'V';
		Nuevo->Imagen = al_load_bitmap("Images/viento.png");
		VientoPresente++;
		break;
	}
	case 6://Charco
	{
		Nuevo->Etiqueta = 'W';
		Nuevo->Imagen = al_load_bitmap("Images/charco.png");
		break;
	}
	case 7: //Nieve
	{
		Nuevo->Etiqueta = 'S';
		Nuevo->Imagen = al_load_bitmap("Images/nieve.png");
		break;
	}
	case 8: //Cono
	{
		Nuevo->Etiqueta = 'C';
		Nuevo->Imagen = al_load_bitmap("Images/cono.png");
		break;
	}
	}

	BonusGenerados++; // Identificador
	Nuevo->Identificador = BonusGenerados;

	Nuevo->CoordX = 55 + rand() % 110; // Coordenadas
	Nuevo->CoordY = 100 + rand() % 240;

	Nuevo->Siguiente = NULL; // Punteros
	Nuevo->Anterior = NULL;

	return Nuevo;
}
void DibujarBonus(PtrBonus Lista)
{
	/* Dibuja los bonus presentes en pantalla. */

	if (Lista != NULL)
	{
		while (Lista != NULL)
		{
			int CoordenadaX = Lista->CoordX;
			int CoordenadaY = Lista->CoordY;

			al_draw_bitmap(Lista->Imagen, CoordenadaX, CoordenadaY, NULL);

			Lista = Lista->Siguiente;
		}
		al_flip_display();
	}
}
void MoverBonus(PtrBonus &Lista, PtrVehiculo &Personaje)
{
	/* Desplaza los bonus sobre el mapa. Si son tomador por el jugador se hace un llamado a
	DiferenciarBonus(). Si salen del mapa se llama a DescontarBonus(). */

	if (Lista != NULL)
	{
		PtrBonus Aux = Lista;

		int ConstanteMovimiento = 3;

		while (Aux != NULL) // Se recorre la lista de elementos
		{
			int CoordenadaX = Aux->CoordX;
			int CoordenadaY = Aux->CoordY;

			Aux->CoordY += ConstanteMovimiento; // Desplaza los bonus sobre el mapa

			int Modo;

			// Switch para un pr�ximo c�lculo de superposici�n de bitmaps.
			switch (Aux->Etiqueta)
			{
			case 'G': // Gas�lina
			{
				Modo = 3;
				break;
			}
			case 'N': // Nitro
			{
				Modo = 4;
				break;
			}
			case 'A': // Munici�n
			{
				Modo = 5;
				break;
			}
			case 'U': // URSS
			{
				Modo = 6;
				break;

			}
			case 'H': // Vida
			{
				Modo = 7;
				break;
			}
			case 'V': // Viento
			{
				Modo = 8;
				break;
			}
			case 'W': // Charco
			{
				Modo = 9;
				break;
			}
			case 'S':
			{
				Modo = 10;
				break;
			}
			case 'C': // Cono
			{
				Modo = 11;
				break;
			}
			}

			PtrBonus Aux2 = Aux->Siguiente;

			if (CalcularSuperposicionBitmaps(Personaje->CoordX, Personaje->CoordY, CoordenadaX, CoordenadaY, Modo))
			{
				// Colisi�n con el personaje.
				DiferenciarBonus(Personaje, Aux->Etiqueta);
				BorrarBonus(Lista, Aux);
			}
			else if (CoordenadaY >= 480) // Bonus sale de pantalla.
			{
				DescontarBonus(Aux->Etiqueta);
				BorrarBonus(Lista, Aux);
			}

			Aux = Aux2;
		}
	}
}

// Funciones de chequeo
bool ChequearVidaPersonaje(PtrVehiculo Personaje)
{
	/* Verifica si el personaje se ha quedado sin vida. */

	if (Personaje->Vida <= 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool ChequearGasPersonaje(PtrVehiculo Personaje)
{
	/* Verifica si el personaje se ha quedado sin gas�lina. */

	if (Personaje->Gas <= 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
void ChequearPersonaje(PtrVehiculo Personaje)
{
	/* Verifica si la temperatura del personaje ha excedido un valor l�mite o si el
	veh�culo se ha detenido. En ambos casos se presenta un cambio en la apariencia del
	personaje. */

	if (Personaje->Temperatura > 25) // Temperatura
	{
		Personaje->Imagen = al_load_bitmap("Images/Carro_Llamas.png");
		al_draw_text(Texto, Amarillo, 500, 350, ALLEGRO_ALIGN_LEFT, "BAJE LA VELOCIDAD");
	}
	else if (Personaje->Velocidad <= 0) // Parqueo
	{
		Personaje->Imagen = al_load_bitmap("Images/Carro_Parado.png");
		al_draw_text(Texto, Amarillo, 500, 350, ALLEGRO_ALIGN_LEFT, "El carro freno");
	}
}
bool ChequearEstadoPersonaje(PtrVehiculo Personaje)
{
	// Verifica si alguna de las condiciones que finalizan el juego se han presentado.

	bool CondicionVida = ChequearVidaPersonaje(Personaje);
	bool CondicionGas = ChequearGasPersonaje(Personaje);

	return (CondicionVida && CondicionGas);
}

// Funciones de dash
void MostrarVelocidad(PtrVehiculo Personaje)
{
	/* Muetra en pantalla la velocidad del personaje. */

	char Velocidad[20];
	char buffer[5];

	strcpy_s(Velocidad, "");
	_itoa_s(Personaje->Velocidad, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Velocidad, buffer); // Ingreso de un atributo

	al_draw_text(Texto, Amarillo, 200, 350, ALLEGRO_ALIGN_LEFT, "Velocidad: ");
	al_draw_text(Texto, Amarillo, 280, 350, ALLEGRO_ALIGN_LEFT, Velocidad);
	al_draw_text(Texto, Amarillo, 315, 350, ALLEGRO_ALIGN_LEFT, "km/h");
}
void MostrarGas(PtrVehiculo Personaje)
{
	/* Muetra en pantalla la gas�lina del personaje. */

	char Gas[20];
	char buffer[5];

	strcpy_s(Gas, "");
	_itoa_s(Personaje->Gas, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Gas, buffer); // Ingreso de un atributo

	al_draw_text(Texto, Amarillo, 200, 310, ALLEGRO_ALIGN_LEFT, "Gas: ");
	al_draw_text(Texto, Amarillo, 280, 310, ALLEGRO_ALIGN_LEFT, Gas);
	al_draw_text(Texto, Amarillo, 315, 310, ALLEGRO_ALIGN_LEFT, "%");
}
void MostrarPuntaje(PtrVehiculo Personaje)
{
	/* Muetra en pantalla el puntaje del personaje. */

	char Puntaje[20];
	char buffer[5];

	strcpy_s(Puntaje, "");
	_itoa_s(Personaje->Puntos, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Puntaje, buffer); // Ingreso de un atributo

	al_draw_text(Texto, Amarillo, 200, 370, ALLEGRO_ALIGN_LEFT, "Puntaje: ");
	al_draw_text(Texto, Amarillo, 280, 370, ALLEGRO_ALIGN_LEFT, Puntaje);
}
void MostrarEnemigosDestruidos()
{
	/* Muetra en pantalla la cantidad de enemigos destruidos por el personaje. */

	char NumeroEnemigos[20];
	char buffer[5];

	strcpy_s(NumeroEnemigos, "");
	_itoa_s(EnemigosDestruidos, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(NumeroEnemigos, buffer); // Ingreso de un atributo

	al_draw_text(Texto, Amarillo, 200, 330, ALLEGRO_ALIGN_LEFT, "Enemigos Destruidos: ");
	al_draw_text(Texto, Amarillo, 335, 330, ALLEGRO_ALIGN_LEFT, NumeroEnemigos);
}
void MostrarTemperatura(PtrVehiculo Personaje)
{
	/* Muetra en pantalla la temperatura del personaje. */

	char Temperatura[20];
	char buffer[5];

	strcpy_s(Temperatura, "");
	_itoa_s(Personaje->Temperatura, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Temperatura, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 390, ALLEGRO_ALIGN_LEFT, "Temperatura carro: ");
	al_draw_text(Texto, Amarillo, 310, 390, ALLEGRO_ALIGN_LEFT, Temperatura);
	al_draw_text(Texto, Amarillo, 330, 390, ALLEGRO_ALIGN_LEFT, "�C ");
}
void MostrarViento(PtrVehiculo Personaje) {
	char Viento[20];
	char buffer[5];

	strcpy_s(Viento, "");
	_itoa_s(Personaje->Viento, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Viento, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 410, ALLEGRO_ALIGN_LEFT, "Viento: ");
	al_draw_text(Texto, Amarillo, 310, 410, ALLEGRO_ALIGN_LEFT, Viento);
	al_draw_text(Texto, Amarillo, 330, 410, ALLEGRO_ALIGN_LEFT, "km/h ");
}
void MostrarProyectiles(PtrVehiculo Personaje)
{
	/* Muetra en pantalla la munici�n restante del personaje. */

	char Municion[20];
	char buffer[5];

	strcpy_s(Municion, "");
	_itoa_s(Personaje->Municion, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Municion, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 280, ALLEGRO_ALIGN_LEFT, "Municion: ");
	al_draw_text(Texto, Amarillo, 310, 280, ALLEGRO_ALIGN_LEFT, Municion);
}
void MostrarVida(PtrVehiculo Personaje)
{
	/* Muetra en pantalla la vida del personaje. */

	char Vida[20];
	char buffer[5];
	strcpy_s(Vida, "");
	_itoa_s(Personaje->Vida, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Vida, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 260, ALLEGRO_ALIGN_LEFT, "Puntos de vida: ");
	al_draw_text(Texto, Amarillo, 310, 260, ALLEGRO_ALIGN_LEFT, Vida);
}
void MostrarDistancia(PtrVehiculo Personaje)
{
	/* Muetra en pantalla la distancia recorrida por el personaje. */

	char Distancia[20];
	char buffer[5];
	strcpy_s(Distancia, "");
	_itoa_s(Personaje->Distancia, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Distancia, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 240, ALLEGRO_ALIGN_LEFT, "Distancia recorrida: ");
	al_draw_text(Texto, Amarillo, 310, 240, ALLEGRO_ALIGN_LEFT, Distancia);
	al_draw_text(Texto, Amarillo, 355, 240, ALLEGRO_ALIGN_LEFT, "km");
}
void MostrarChoque(PtrVehiculo Personaje)
{
	char Choques[20];
	char buffer[5];
	strcpy_s(Choques, "");
	_itoa_s(Personaje->Choques, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Choques, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 200, ALLEGRO_ALIGN_LEFT, "Colisiones:");
	al_draw_text(Texto, Amarillo, 310, 200, ALLEGRO_ALIGN_LEFT, Choques);
}
void MostrarNieve(PtrVehiculo Personaje)
{
	char Nieve[20];
	char buffer[5];
	strcpy_s(Nieve, "");
	_itoa_s(Personaje->Nieve, buffer, 10); // Conversi�n de datos de tipo int a str
	strcat_s(Nieve, buffer); // Ingreso de un atributo
	al_draw_text(Texto, Amarillo, 200, 180, ALLEGRO_ALIGN_LEFT, "Nieve:");
	al_draw_text(Texto, Amarillo, 310, 180, ALLEGRO_ALIGN_LEFT, Nieve);
}
void MostrarDireccionales()
{
	/* Muestra en pantalla la direcci�n en que se muevo el personaje. */

	if (flechasDireccion[DOWN])
	{
		al_draw_bitmap(FlechaAbajo, 500, 240, NULL);
	}
	if (flechasDireccion[UP])
	{
		al_draw_bitmap(FlechaArriba, 500, 190, NULL);
	}
	if (flechasDireccion[RIGHT])
	{
		al_draw_bitmap(FlechaDerecha, 530, 210, NULL);
	}
	if (flechasDireccion[LEFT])
	{
		al_draw_bitmap(FlechaIzquierda, 470, 210, NULL);
	}
}
void MostrarInstrucciones() {
	al_draw_text(Titulo, Verde, 400, 50, ALLEGRO_ALIGN_LEFT, "AUTO FANTASTICO");
	//al_draw_text(Instru, Blanco, 400, 80, ALLEGRO_ALIGN_LEFT, "Evita los obst�culos y recolecta los bonus");
}
void MostrarDash(PtrVehiculo Personaje)
{
	/* Muestra en pantalla las condiciones anteriores. */

	MostrarProyectiles(Personaje);
	MostrarVelocidad(Personaje);
	MostrarDistancia(Personaje);
	MostrarPuntaje(Personaje);
	MostrarVida(Personaje);
	MostrarGas(Personaje);
	MostrarEnemigosDestruidos();
	MostrarTemperatura(Personaje);
	MostrarViento(Personaje);
	MostrarChoque(Personaje);
	MostrarNieve(Personaje);
	MostrarInstrucciones();
	MostrarDireccionales();
}

// Funciones de piloto autom�tico
void LimpiarMapa(PtrVehiculoEnemigo &ListaEnemigos, PtrBonus &ListaObstaculos)
{
	/* Para que el piloto autom�tico pueda desempe�arse sin muchas complicaciones se
	vio conveniente que en el mapa hayan solo carros enemigos presentes. Esta funci�n
	reinicia los elementos en el mapa. */

	PtrVehiculoEnemigo Aux = ListaEnemigos;
	PtrVehiculoEnemigo Aux2;
	while (Aux != NULL) // Destruye la lista de enemigos
	{
		Aux2 = Aux->Siguiente;
		free(Aux);
		Aux = Aux2;
	}

	ListaEnemigos = NULL;

	while (ListaObstaculos != NULL) // Destruye la lista de obst�culos
	{
		PtrBonus Aux = ListaObstaculos->Siguiente;
		free(ListaObstaculos);
		ListaObstaculos = Aux;
	}

	ListaObstaculos = NULL;
}
void DeteccionSensor(PtrVehiculo &Personaje, PtrVehiculoEnemigo Enemigo)
{
	/* Se programa un sensor ubicado en el frente del auto. Este sensor mueve el auto cuando
	detecta que un veh�culo enemigo se aproxima por el frente. */
	int CoordenadaX = Personaje->CoordX;
	int CoordenadaY = Personaje->CoordY;

	bool CondicionX = ((CoordenadaX >= Enemigo->CoordX) && (CoordenadaX <= Enemigo->CoordX + 13));
	bool CondicionXDerecha = ((CoordenadaX + 12 >= Enemigo->CoordX) && (CoordenadaX + 12 <= Enemigo->CoordX + 13));

	bool CondicionY = ((CoordenadaY - Enemigo->CoordY < 105) && (CoordenadaY > Enemigo->CoordY));

	if ((CondicionX || CondicionXDerecha) && CondicionY)
	{
		if (CoordenadaX >= 65)
		{
			Personaje->CoordX -= 6;
		}
		else
		{
			Personaje->CoordX += 6;
		}
	}
}
void PilotoAutomatico(PtrVehiculo &Personaje, PtrVehiculoEnemigo ListaEnemigos)
{
	/* Funci�n que invoca una segunda que identifica los autos que se mueven de frente al
	personaje. */
	if (ListaEnemigos != NULL)
	{
		PtrVehiculoEnemigo Aux = ListaEnemigos;

		while (Aux->Siguiente != NULL)
		{
			DeteccionSensor(Personaje, Aux);

			Aux = Aux->Siguiente;
		}
	}
}

// Funciones de menu
void IniciarPuntajes()
{
	/* Inicializa los elementos del arreglo en cero. */

	for (int i = 0; i < mejoresPuntajesJuego; i++)
	{
		MejoresPuntajes[i] = 0;
	}
}
void LimpiarPuntajes()
{
	/* Elimina el contenido de los elementos del arreglo. */

	for (int i = 0; i < mejoresPuntajesJuego; i++)
	{
		MejoresPuntajes[i] = 0;
	}
}
void CargarPuntajes()
{
	/* Toma los valores almacenados en el archivo f�sico. */

	char puntajeTxt[10];
	int punt;
	int aux;

	errno_t pFile;

	pFile = fopen_s(&Documento, "Puntajes.txt", "r");

	if (pFile == NULL)
	{
		while (!feof(Documento)) {
			fgets(puntajeTxt, 10, Documento);
			punt = atoi(puntajeTxt);
			for (int i = 0; i < mejoresPuntajesJuego; i++) {
				if (punt >= MejoresPuntajes[i]) {
					aux = MejoresPuntajes[i];
					MejoresPuntajes[i] = punt;
					punt = aux;
				}
			}
		}
	}
}
void GuardarPuntajes(PtrVehiculo Personaje)
{
	/* Guarda el puntaje de la �ltima partida en el archivo f�sico. */

	errno_t pFile;

	pFile = fopen_s(&Documento, "Puntajes.txt", "a");

	if (pFile == NULL)
	{
		fprintf(Documento, "%i\n", Personaje->Puntos);
		fclose(Documento);
	}
}
void DibujarMejoresPuntajes()
{
	/* Imprime en pantalla los elementos del arreglo de puntajes. */

	char puntajeCadena[10];
	char posicionCadena[10];
	for (int i = 0; i < mejoresPuntajesJuego; i++) {
		sprintf_s(puntajeCadena, "%i", MejoresPuntajes[i]);
		sprintf_s(posicionCadena, "%i", i + 1);

		al_draw_text(Texto2, al_map_rgb(255, 255, 255), 20, i * 30, ALLEGRO_ALIGN_CENTRE, posicionCadena);
		al_draw_text(Texto2, al_map_rgb(255, 255, 255), 35, i * 30, ALLEGRO_ALIGN_CENTRE, " . ");
		al_draw_text(Texto2, al_map_rgb(255, 255, 255), 70, i * 30, ALLEGRO_ALIGN_CENTRE, puntajeCadena);
	}
	al_flip_display();
}
void DibujarMenu()
{
	/* Dibuja en pantalla las opciones del men� de inicio. */

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(Titulo, al_map_rgb(255, 96, 208), 380, 120, ALLEGRO_ALIGN_CENTRE, "AUTO FANTASTICO");
	al_draw_text(Titulo, al_map_rgb(160, 32, 255), 380, 210, ALLEGRO_ALIGN_CENTRE, "1. Jugar");
	al_draw_text(Titulo, al_map_rgb(255, 96, 208), 380, 260, ALLEGRO_ALIGN_CENTRE, "2. Puntajes");
	al_draw_text(Titulo, al_map_rgb(96, 255, 128), 380, 310, ALLEGRO_ALIGN_CENTRE, "3. Salir");

	al_flip_display();
}
// Funciones de cierre
void DestruirtEstructuras(PtrVehiculo &Personaje, PtrVehiculoEnemigo &ListaEnemigos, PtrProyectil &ListaProyectiles, PtrBonus &ListaBonus)
{
	/* Se deshace de todos los elementos cargados en la RAM*/

	al_destroy_bitmap(Personaje->Imagen);

	while (ListaEnemigos != NULL)
	{
		PtrVehiculoEnemigo AuxA = ListaEnemigos->Siguiente;
		al_destroy_bitmap(ListaEnemigos->Imagen);
		free(ListaEnemigos);
		ListaEnemigos = AuxA;
	}

	while (ListaProyectiles != NULL)
	{
		PtrProyectil AuxB = ListaProyectiles->Siguiente;
		al_destroy_bitmap(ListaProyectiles->Imagen);
		free(ListaProyectiles);
		ListaProyectiles = AuxB;
	}

	while (ListaBonus != NULL)
	{
		PtrBonus AuxC = ListaBonus->Siguiente;
		al_destroy_bitmap(ListaBonus->Imagen);
		free(ListaBonus);
		ListaBonus = AuxC;
	}
}
void PantallaFinal(PtrVehiculo Personaje, PtrVehiculoEnemigo ListaA, PtrBonus ListaB, PtrProyectil ListaC)
{
	/* Detalle para dejar una imagen final al momento de finalizar el juego. */

	al_draw_bitmap(Carretera, 50, 0, NULL);
	al_draw_bitmap(Carretera, 118, 0, NULL);

	DibujarBonus(ListaB);
	DibujarEnemigos(ListaA);
	DibujarPoyectil(ListaC);
	DibujarPersonaje(Personaje);
	MostrarDash(Personaje);

	al_draw_text(Texto, Amarillo, 500, 240, ALLEGRO_ALIGN_CENTRE, "FIN DEL JUEGO");

	al_flip_display();

	system("pause");
}

// Rutina Principal
int main(int argc, char **argv)
{
	// Inicializaci�n de Allegro
	ALLEGRO_DISPLAY *display = NULL;
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	display = al_create_display(720, 480);
	al_set_window_title(display, "The Car");
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}

	// Inicializar constructores de Allegro
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(1000);
	al_init_primitives_addon();
	al_install_keyboard();

	// Creaci�n de Timers
	ALLEGRO_TIMER *PrimerTimer = al_create_timer(40 / FPS);
	ALLEGRO_TIMER *SegundoTimer = al_create_timer(55 / FPS);
	ALLEGRO_TIMER *TercerTimer = al_create_timer(65 / FPS);
	ALLEGRO_TIMER *CuartoTimer = al_create_timer(150 / FPS);
	ALLEGRO_TIMER *QuintoTimer = al_create_timer(55 / FPS);
	ALLEGRO_TIMER *SextoTimer = al_create_timer(85 / FPS);
	ALLEGRO_TIMER *SetimoTimer = al_create_timer(100 / FPS);
	ALLEGRO_TIMER *OctavoTimer = al_create_timer(1.0 / FPS);

	// Creaci�n de Cola de Eventos
	ALLEGRO_EVENT_QUEUE *ColaEventos = al_create_event_queue();
	ALLEGRO_EVENT_QUEUE *ColaEventosMenu = al_create_event_queue();

	// Registro de Eventos
	al_register_event_source(ColaEventos, al_get_timer_event_source(PrimerTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(SegundoTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(TercerTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(CuartoTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(QuintoTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(SextoTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(SetimoTimer));
	al_register_event_source(ColaEventos, al_get_timer_event_source(OctavoTimer));
	al_register_event_source(ColaEventos, al_get_keyboard_event_source());

	al_register_event_source(ColaEventosMenu, al_get_keyboard_event_source());

	// Inicializaci�n de Timers
	al_start_timer(PrimerTimer);
	al_start_timer(SegundoTimer);
	al_start_timer(TercerTimer);
	al_start_timer(CuartoTimer);
	al_start_timer(QuintoTimer);
	al_start_timer(SextoTimer);
	al_start_timer(SetimoTimer);
	al_start_timer(OctavoTimer);


	// Inicializa funciones del teclado
	ALLEGRO_KEYBOARD_STATE estadoTeclado;
	al_get_keyboard_state(&estadoTeclado);

	// Estructuras
	PtrVehiculo Personaje = new(Vehiculo);
	IniciarPersonaje(Personaje);

	PtrVehiculoEnemigo ListaEnemigos;
	IniciarInventario(ListaEnemigos);

	PtrProyectil ListaProyectiles;
	IniciarInventarioProyectil(ListaProyectiles);

	PtrBonus ListaBonus;
	IniciarInventarioBonus(ListaBonus);

	// Colores
	Amarillo = al_map_rgb(255, 255, 0);
	Verde = al_map_rgb(60, 179, 13);
	Blanco = al_map_rgb(240, 248, 255);

	// Fonts
	Texto = al_load_font("Fonts/ARCHRISTY.ttf", 14, NULL);
	Texto2 = al_load_font("Fonts/8bitoperator_jve.ttf", 30, NULL);
	Instru = al_load_font("Fonts/ARCHRISTY.ttf", 10, NULL);
	Titulo = al_load_font("Fonts/blox2.ttf", 30, NULL);

	// Bitmaps
	Carretera = al_load_bitmap("Images/Carretera.png");
	FlechaAbajo = al_load_bitmap("Images/FlechaDown.png");
	FlechaArriba = al_load_bitmap("Images/FlechaUp.png");
	FlechaDerecha = al_load_bitmap("Images/FlechaRight.png");
	FlechaIzquierda = al_load_bitmap("Images/FlechaLeft.png");

	// Condicionales
	bool Condicion = true;
	bool Condicion2 = true;
	bool Condicion3 = false;

	bool Menu = true;
	bool Simulacion = true;

	DibujarMenu();
	IniciarPuntajes();

	while (Menu)
	{
		ALLEGRO_EVENT EventosMenu;
		al_wait_for_event(ColaEventosMenu, &EventosMenu);

		if (EventosMenu.type = ALLEGRO_EVENT_KEY_DOWN) { // Jugar

			if (EventosMenu.keyboard.keycode == ALLEGRO_KEY_1)
			{
				Menu = false;
			}
			else if (EventosMenu.keyboard.keycode == ALLEGRO_KEY_2) // Mostrar puntajes hist�ricos
			{
				al_clear_to_color(al_map_rgb(152, 251, 152));
				CargarPuntajes();
				DibujarMejoresPuntajes();
				LimpiarPuntajes();
				al_flip_display();
			}
			else if (EventosMenu.keyboard.keycode == ALLEGRO_KEY_3) // Salir
			{
				Condicion = false;
				Menu = false;
			}
		}
	}

	while (Condicion && Condicion2)
	{
		ALLEGRO_EVENT eventos;
		al_wait_for_event(ColaEventos, &eventos);

		if (eventos.type == ALLEGRO_EVENT_KEY_DOWN) // Presionar una tecla
		{
			int CoordenadaX = Personaje->CoordX;
			int CoordenadaY = Personaje->CoordY;

			bool CondicionTemperatura = false;

			if ((Personaje->Temperatura > 25) || (Personaje->Velocidad <= 0))
			{
				CondicionTemperatura = true;
			}

			switch (eventos.keyboard.keycode)
			{
			case ALLEGRO_KEY_S:
			{
				teclasDireccion[DOWN] = true;
				flechasDireccion[DOWN] = true;
				if (!CondicionTemperatura)
				{
					Personaje->Imagen = al_load_bitmap("Images/Carro_Parado.png");
				}
				break;
			}
			case ALLEGRO_KEY_W:
			{
				flechasDireccion[UP] = true;
				teclasDireccion[UP] = true;
				if (!CondicionTemperatura)
				{
					Personaje->Imagen = al_load_bitmap("Images/Carro_Normal.png");
				}
				break;
			}
			case ALLEGRO_KEY_D:
			{
				teclasDireccion[RIGHT] = true;
				flechasDireccion[RIGHT] = true;
				if (!CondicionTemperatura)
				{
					Personaje->Imagen = al_load_bitmap("Images/Carro_GiroDer.png");
				}
				break;
			}
			case ALLEGRO_KEY_A:
			{
				teclasDireccion[LEFT] = true;
				flechasDireccion[LEFT] = true;
				if (!CondicionTemperatura)
				{
					Personaje->Imagen = al_load_bitmap("Images/Carro_GiroIzq.png");
				}
				break;
			}
			case ALLEGRO_KEY_I:
			{
				if (Personaje->Municion > 0)
				{
					PtrProyectil Nuevo = CrearProyectil(Personaje, 'I');
					AgregarInicioInventarioProyectil(ListaProyectiles, Nuevo);
					Personaje->Municion--;
				}
				break;
			}
			case ALLEGRO_KEY_J:
			{
				if (Personaje->Municion > 0)
				{
					PtrProyectil Nuevo = CrearProyectil(Personaje, 'J');
					AgregarInicioInventarioProyectil(ListaProyectiles, Nuevo);
					Personaje->Municion--;
				}
				break;
			}
			case ALLEGRO_KEY_K:
			{
				if (Personaje->Municion > 0)
				{
					PtrProyectil Nuevo = CrearProyectil(Personaje, 'K');
					AgregarInicioInventarioProyectil(ListaProyectiles, Nuevo);
					Personaje->Municion--;
				}
				break;
			}
			case ALLEGRO_KEY_L:
			{
				if (Personaje->Municion > 0)
				{
					PtrProyectil Nuevo = CrearProyectil(Personaje, 'L');
					AgregarInicioInventarioProyectil(ListaProyectiles, Nuevo);
					Personaje->Municion--;
				}
				break;
			}
			case ALLEGRO_KEY_SPACE:
			{
				Condicion3 = true;
				LimpiarMapa(ListaEnemigos, ListaBonus);
				Personaje->CoordX = 97;
				break;
			}
			case ALLEGRO_KEY_ENTER:
			{
				Condicion3 = false;
				LimpiarMapa(ListaEnemigos, ListaBonus);
				break;
			}
			case ALLEGRO_KEY_ESCAPE:
			{
				Condicion = false;
				break;
			}
			}
		}
		if (eventos.type == ALLEGRO_EVENT_KEY_UP) // Soltar una tecla
		{
			bool CondicionTemperatura = false;

			if ((Personaje->Temperatura > 25) || (Personaje->Velocidad <= 0))
			{
				CondicionTemperatura = true;
			}

			switch (eventos.keyboard.keycode)
			{
			case ALLEGRO_KEY_S:
			{
				teclasDireccion[DOWN] = false;
				flechasDireccion[DOWN] = false;
				break;
			}
			case ALLEGRO_KEY_W:
			{
				teclasDireccion[UP] = false;
				flechasDireccion[UP] = false;
				break;
			}
			case ALLEGRO_KEY_D:
			{
				teclasDireccion[RIGHT] = false;
				flechasDireccion[RIGHT] = false;
				if (!CondicionTemperatura)
				{
					Personaje->Imagen = al_load_bitmap("Images/Carro_Normal.png");
				}
				break;
			}
			case ALLEGRO_KEY_A:
			{
				teclasDireccion[LEFT] = false;
				flechasDireccion[LEFT] = false;
				if (!CondicionTemperatura)
				{
					Personaje->Imagen = al_load_bitmap("Images/Carro_Normal.png");
				}
				break;
			}
			}
		}
		if (eventos.type == ALLEGRO_EVENT_TIMER) {
			if (eventos.timer.source == PrimerTimer) // Spawn de enemigos
			{
				PtrVehiculoEnemigo Nuevo = CrearEnemigo();
				AgregarInicioInventario(ListaEnemigos, Nuevo);
			}
			else if ((eventos.timer.source == SegundoTimer) && (!Condicion3)) // Spawn de gas�lina
			{
				if ((Personaje->Gas <= 65) && ((GasPresente >= 0) && (GasPresente < 2)))
				{
					PtrBonus Nuevo = CrearBonus(0); // Gas
					AgregarInicioInventarioBonus(ListaBonus, Nuevo);
				}
			}
			else if ((eventos.timer.source == TercerTimer) && (!Condicion3)) // Munici�n y Vida
			{
				if ((Personaje->Municion <= 5) && ((MunicionPresente >= 0) && (MunicionPresente < 2)))
				{
					PtrBonus Nuevo = CrearBonus(2); // Munici�n
					AgregarInicioInventarioBonus(ListaBonus, Nuevo);
				}

				if ((Personaje->Vida <= 50) && ((VidaPresente >= 0) && (VidaPresente < 2)))
				{
					PtrBonus Nuevo = CrearBonus(4); // Vida
					AgregarInicioInventarioBonus(ListaBonus, Nuevo);
				}
			}
			else if ((eventos.timer.source == CuartoTimer) && (!Condicion3)) // Spawn de Comunismo
			{
				PtrBonus Nuevo = CrearBonus(3); // URSS
				AgregarInicioInventarioBonus(ListaBonus, Nuevo);
			}
			else if ((eventos.timer.source == QuintoTimer) && (!Condicion3)) // Spawn de Obst�culos
			{
				int NumeroRandom = 7 + rand() % 2;
				PtrBonus Nuevo = CrearBonus(NumeroRandom);
				AgregarInicioInventarioBonus(ListaBonus, Nuevo);
			}
			else if ((eventos.timer.source == SextoTimer) && (!Condicion3)) // Spawn de Viento
			{
				PtrBonus Nuevo = CrearBonus(5); // Viento
				AgregarInicioInventarioBonus(ListaBonus, Nuevo);
			}
			else if ((eventos.timer.source == SetimoTimer) && (!Condicion3)) // Spawn de Viento
			{
				PtrBonus Nuevo = CrearBonus(6); // Viento
				AgregarInicioInventarioBonus(ListaBonus, Nuevo);
			}
			else if (eventos.timer.source == OctavoTimer) // Dibujar elementos en pantalla
			{
				al_draw_bitmap(Carretera, 50, 0, NULL);
				al_draw_bitmap(Carretera, 118, 0, NULL);

				DibujarBonus(ListaBonus);
				DibujarEnemigos(ListaEnemigos);
				DibujarPoyectil(ListaProyectiles);
				DibujarPersonaje(Personaje);

				MostrarProyectiles(Personaje);
				MostrarVelocidad(Personaje);
				MostrarDistancia(Personaje);
				MostrarPuntaje(Personaje);
				MostrarVida(Personaje);
				MostrarGas(Personaje);
				MostrarChoque(Personaje);
				MostrarInstrucciones();

				MostrarEnemigosDestruidos();
				MostrarTemperatura(Personaje);
				MostrarDireccionales();

				MostrarViento(Personaje);
				MostrarNieve(Personaje);

				ChequearPersonaje(Personaje);

				al_flip_display();
			}
		}

		// C�lculo de nuevas condiciones
		MoverEnemigos(ListaEnemigos, Personaje);
		MoverProyectiles(ListaProyectiles, ListaEnemigos, Personaje);
		MoverBonus(ListaBonus, Personaje);
		MoverPersonaje(Personaje, Condicion3);

		Condicion2 = ChequearEstadoPersonaje(Personaje);

		if (Condicion3)
		{
			PilotoAutomatico(Personaje, ListaEnemigos);
		}

		al_clear_to_color(Transparente);
	}

	GuardarPuntajes(Personaje);
	PantallaFinal(Personaje, ListaEnemigos, ListaBonus, ListaProyectiles);
	DestruirtEstructuras(Personaje, ListaEnemigos, ListaProyectiles, ListaBonus);

	// Cierre

	al_destroy_display(display);
	return 0;
}