#include <iostream>	// pour le cout et le cin
#include <iomanip>	// pour les manipulateurs du cout: fixed, setprecision(), setw(), ...
#include <conio.h>	// pour le _getch() et _getche()
#include <time.h>	// pour la fonction time();
#include <windows.h>
#include "Y:/B21/console(v1.9).h" // pour clrscr(), wherex(), wherey(), gotoxy()

using namespace std; // Pour ne pas être obligé d'écrire std::cout
// CONSTANTES GLOBALES
// *******************

const int NB_COL = 12, NB_LIG = 8; // Nombre de colonnes et de lignes du damier

const int DanierDebutCol =	10, DanierDebutLigne = 2;

const string Message[8] =
{ "Decouvrez et amassez 15 cases ",
"Points: ",
"VICTOIRE !",
"\220CHEC !",
"  Total des points",
" sur un objectif de 15",
"  Total des d\202placements",
"  Temps \202coul\202"
};

const int Longueur = 4, Largeur = 2, EspaceDanier = 1; //les valeurs d'affichages sont changable mais il ne faut pas trop exagerer car ça brise l'affichage...
const char PierreBleu = '\262', PierreMur =' ', PierreRose ='\260', PierreDollar = '$', //les chars des elements visuels du danier sont changable
	curseur[9] = { '\311','\313','\273','\314','\316','\271','\310','\312','\274' };

// DEFINITIONS DES TYPES GLOBAUX
// *****************************

enum Commandes // Code ascii des touches (flèches) de déplacement. Vous devez utiliser ces valeurs symboliques
{
	KEY_DOWN		= 80,
	KEY_UP			= 72,	
	KEY_RIGHT		= 77,
	KEY_LEFT		= 75,
	KEY_UP_RIGHT	= 73,
	KEY_UP_LEFT		= 71,
	KEY_DOWN_LEFT	= 79,
	KEY_DOWN_RIGHT	= 81
};

struct Position // Pour mémoriser une position du curseur sur le damier du jeu
{
	int lig=0; // ligne du damier
	int col=0; // colonne du damier
};

struct Déplacement // Pour mémoriser un déplacement du curseur sur le damier du jeu
{
	Position dep; // case de départ du curseur
	Position arr;// case d'arrivée du curseur
};


enum État { BL, DC, DV, RO, MU }; // États possibles de chacune des cases du damier
/* 
	États possibles			Représentations graphiques

	BL = BLEUE				case bleue
	DC = DOLLARS CACHÉ		case bleue
	DV = DOLLARS VISIBLE	case $$$$ vert
	RO = ROSE				case rose
	MU = MUR				case noire
*/

struct Jeu // Pour mémoriser les différents aspects du jeu
{
	int ptsAccum = 0; // Points accumulés durant la partie. 0 au départ.

	// Damier du jeu avec les états initiaux de chacune des cases
	État damier [NB_LIG][NB_COL] =
	{
		{ BL , BL , BL , BL , BL , BL , BL , BL , MU , BL , BL , DC },
		{ BL , BL , MU , MU , BL , BL , BL , BL , BL , MU , BL , MU },
		{ BL , BL , MU , DC , MU , BL , BL , BL , BL , BL , MU , DC },
		{ BL , BL , MU , DC , MU , BL , BL , MU , MU , BL , MU , DC },
		{ DC , BL , MU , MU , MU , DC , MU , BL , MU , BL , MU , BL },
		{ DC , BL , DC , DC , BL , DC , MU , DC , MU , BL , MU , BL },
		{ DC , BL , DC , BL , BL , BL , MU , MU , MU , BL , MU , BL },
		{ DC , DC , BL , BL , BL , BL , BL , BL , BL , BL , BL , BL }
	};

	Déplacement depl; // Pour conserver le dernier déplacement du curseur dans le damier
};


// VARIABLES GLOBALES
// ******************

Jeu jeu; // Création du jeu en mémoire


void afficherBrick(int C, int L,Position danierAffichage, cvmColor color, char affichageElement ) //function qui affiche toute les elements du danier
{
	danierAffichage.col = DanierDebutCol + (Longueur + EspaceDanier) * C; //calcule apartir du danier la position de l'affichage colone et ligne
	danierAffichage.lig = DanierDebutLigne + (Largeur + EspaceDanier) * L;
	cvmSetColor(color);
	for (int La = 1; La <= Largeur; La++) //boucle pour afficher le numbre de character des elements en largeur et longeur
	{
		gotoxy(danierAffichage.col,danierAffichage.lig);
		for (int Lo = 1; Lo <= Longueur; Lo++)
		{
			if (color == JAUNE) //si la color est jaune cela veut dire qu'on affiche le curseur
			{
				if (Lo == 1 && La == 1) // les 9 possibilités de l'affichage du curseur (coins, milieu, ect...)
					cout << curseur[0];
				else if (Lo < Longueur && La == 1)
					cout << curseur[1];
				else if (Lo == Longueur && La == 1)
					cout << curseur[2];
				else if (La < Largeur && Lo == 1)
					cout << curseur[3];
				else if (Lo != Longueur && La != Largeur)
					cout << curseur[4];
				else if (Lo == Longueur && La != Largeur)
					cout << curseur[5];
				else if (La == Largeur && Lo == 1)
					cout << curseur[6];
				else if (Lo < Longueur)
					cout << curseur[7];
				else
					cout << curseur[8];
			}
			else //sinon tout les autres elements n'ont que une possibilité.
			cout << affichageElement;
		}
		danierAffichage.lig++;
	}
}

void main()
{
	int temps, numbreDeplacement=0, pogner, c1, c2;
	char characVariable;
	cvmColor color;
	Position danierAffichage;
	bool bloque = false, deplacementValide, partieGagne = false;

	temps = (unsigned int)time(NULL); //on prend le temps quand le .exe a été ouvert 
	
	cout << Message[0];	 // affichage des conditions de victoire et des points accumulés
	for (int Lo = 0; Lo < Longueur; Lo++)
		cout << PierreDollar;
	gotoxy(70, 0); 
	cout << Message[1] << numbreDeplacement;

	gotoxy(DanierDebutCol, DanierDebutLigne); //affichage du danier complet avec le curseur 
	for (int L = 0; L < NB_LIG; L++)
	{
		for (int C = 0; C < NB_COL; C++)
		{	
			if (L == 0 && C == 0) // le curseur a la possition 0 , 0 du danier
			{
				color = JAUNE; characVariable = 0;
			}
			else 
			{
				switch (jeu.damier[L][C]) // les 2 seules possiblités d'achiffer soit un mur ou une pierre bleu
				{
				case MU: color = BLANC; characVariable = PierreMur; break;
				default: color = BLEU; characVariable = PierreBleu; break;
				}
			}	
			afficherBrick(C, L, danierAffichage, color, characVariable);
		}
	}

	do
	{
		deplacementValide = false;
		do
		{
			c1 = _getch();
			if (c1 == 0 || c1 == 224) // vérification que c'est bien une fleche du clavier
				if (_kbhit())
				{
					c2 = _getch();
					jeu.depl.arr = jeu.depl.dep; 
					switch (c2) // les 8 possibilités de movement avec les fleches
					{
					case KEY_UP_LEFT: jeu.depl.arr.col--; jeu.depl.arr.lig--;	break;
					case KEY_UP: jeu.depl.arr.lig--; break;
					case KEY_UP_RIGHT: jeu.depl.arr.col++; jeu.depl.arr.lig--; 	break;
					case KEY_LEFT:;	jeu.depl.arr.col--;	break;
					case KEY_RIGHT:jeu.depl.arr.col++;	break;
					case KEY_DOWN_LEFT: jeu.depl.arr.col--; jeu.depl.arr.lig++;	break;
					case KEY_DOWN: jeu.depl.arr.lig++;	break;
					case KEY_DOWN_RIGHT: jeu.depl.arr.col++; jeu.depl.arr.lig++;	break;
					}

					if ((jeu.depl.arr.col < NB_COL && jeu.depl.arr.col >= 0) && (jeu.depl.arr.lig < NB_LIG && jeu.depl.arr.lig >= 0)) //verification si le movement ne fait pas parti du danier 
						if (jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] != MU) //verification si le movement ne va pas dans un mur 
							deplacementValide = true; //sinon il est valide
					if (deplacementValide == false) // movement non valide = son d'erreur
						cout << "\a";
				}
		} while (deplacementValide != true); //autant que le movement n'est pas valide on recommence
			
		switch (jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col]) //on change l'element ou le curseur s'est deplacé
		{
		case BL: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = RO; break;
		case DC: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = DV; break;
		case DV: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = RO; 
			jeu.ptsAccum++; gotoxy(78, 0); cout << jeu.ptsAccum;break; //ici on update le numbre de point si on va sur une casse dollar vissible
		case RO: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = MU; break;
		}

		pogner = 0; //ici on regarde si le curseur peut encore bouger dans le danier
		for (int L = -1; L <= 1; L++) //on fait le tour des 8 possition du curseur en ligne colone
		{
			for (int C = -1; C <= 1; C++)
			{
				if (!(L == 0 && C == 0)) //on skip la possition du curseur lui meme
				{
					if (((jeu.depl.arr.lig + L < 0) || (jeu.depl.arr.lig + L > NB_LIG - 1)) //verification si le movement ne fait pas parti du danier 
						|| ((jeu.depl.arr.col + C < 0) || (jeu.depl.arr.col + C > NB_COL - 1)))
						pogner++; //si oui on perd une des 8 possibilités
					else if (jeu.damier[jeu.depl.arr.lig + L][jeu.depl.arr.col + C] == MU) //verification si le movement ne va pas dans un mur 
						pogner++; //si oui on perd une des 8 possibilités
					else
						C = L = 2; // si on se rend ici cela veut dire qu'il reste au moins un movement possible doc on arrete le test
				}
			}
		}
		if (pogner == 8) //si les 8 cases sont inaccessible on est bloque!
			bloque = true;

		switch (jeu.damier[jeu.depl.dep.lig][jeu.depl.dep.col])//on update l'affichage du chagement d'element du depart du curseur
		{
		case BL: color = BLEU; characVariable = PierreBleu; break;
		case DV: color = VERT; characVariable = PierreDollar; break;
		case RO: color = ROSE; characVariable = PierreRose; break;
		case MU: color = BLANC; characVariable = PierreMur; break;
		}
		afficherBrick(jeu.depl.dep.col, jeu.depl.dep.lig, danierAffichage, color, characVariable);
		afficherBrick(jeu.depl.arr.col, jeu.depl.arr.lig, danierAffichage, color=JAUNE, characVariable=0); //ici on update l'affichage de la position du curseur
		cvmSetColor(BLANC);
			
		jeu.depl.dep=jeu.depl.arr; // on garde l'arrivée du curseur comme son depart 
			
		numbreDeplacement++; // on augmente le numbre deplacement

		if (jeu.ptsAccum == 15) //quand on a 15 points on gagne
			partieGagne = true;
	
	} while (partieGagne!=true && bloque != true); // la partie dure autant qu'on gagne ou qu'on est bloqué

	temps -= (unsigned int)time(NULL); // on calcule le temps ecouler depuis le debut de la partie
	clrscr();
	gotoxy(0, 1);

	if (jeu.ptsAccum == 15) //message de victoire ou de défaite
		cout << Message[2];
	else
		cout << Message[3];
	
	cout << endl << endl << Message[4] << right << setw(12) << ": " << left << setw(2) << jeu.ptsAccum << Message[5]; //affichage des "stats"
	cout << endl << endl << Message[6] << right << setw(6) << ": " << numbreDeplacement;
	cout << endl << endl << Message[7] << right << setw(16) << ": " << -temps << " sec";
	
	_getch();
}