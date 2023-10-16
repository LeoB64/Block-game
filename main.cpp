#include <iostream>	// for the cout and cin
#include <iomanip>	// for cout manipulators: fixed, setprecision(), setw(), ...
#include <conio.h>	// for the _getch() and _getche() functions
#include <time.h>	//for the time() function
#include <windows.h>
#include "console(v1.9).h" // for clrscr(), wherex(), wherey(), gotoxy() functions

using namespace std; // replaces std::cout

// DEFINITIONS OF GLOBAL CONSTANT 
// ******************************

const int NB_COL = 12, NB_LIG = 8; // number of columns and lines in the checkerboard
const int DanierDebutCol = 10, DanierDebutLigne = 2;
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

const int Longueur = 4, Largeur = 2, EspaceDanier = 1; 
const char PierreBleu = '\262', PierreMur =' ', PierreRose ='\260', PierreDollar = '$', 
	curseur[9] = { '\311','\313','\273','\314','\316','\271','\310','\312','\274' };

// DEFINITIONS OF GLOBAL VARIABLES 
// *******************************

//  Ascii Code for the keyboard arrowss 
enum Commandes 
{
	KEY_DOWN = 80,
	KEY_UP = 72,	
	KEY_RIGHT = 77,
	KEY_LEFT = 75,
	KEY_UP_RIGHT = 73,
	KEY_UP_LEFT = 71,
	KEY_DOWN_LEFT = 79,
	KEY_DOWN_RIGHT = 81
};

// position in the checkerboard
struct Position
{
	int lig=0; 
	int col=0; 
};

 // for checking a movement in the checkerboard
struct Deplacement
{
	Position dep; // case de depart du curseur
	Position arr;// case d'arrivee du curseur
};

// Possible states of each block in the checkerboard
enum etat { BL, DC, DV, RO, MU }; 
/* 
	etats possibles			Representations graphiques

	BL = BLEUE		blue block
	DC = DOLLARS CACHe	blue block
	DV = DOLLARS VISIBLE	green block ($$$$)
	RO = ROSE		pink block
	MU = MUR		black block
*/

// The game variables
struct Jeu 
{
	// score
	int ptsAccum = 0; 

	// Checkerboard of the game with every initial block state 
	etat damier [NB_LIG][NB_COL] =
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
	// to keep the last movement in the checkerboard
	Deplacement depl; 
};


Jeu jeu; 

// Function used to display any block in the checkerboard
void afficherBrick(int C, int L, Position danierAffichage, cvmColor color, char affichageElement ) 
{
	danierAffichage.col = DanierDebutCol + (Longueur + EspaceDanier) * C;
	danierAffichage.lig = DanierDebutLigne + (Largeur + EspaceDanier) * L;
	cvmSetColor(color);
	for (int La = 1; La <= Largeur; La++) 
	{
		gotoxy(danierAffichage.col,danierAffichage.lig);
		for (int Lo = 1; Lo <= Longueur; Lo++)
		{
			if (color == JAUNE) 
			{
				if (Lo == 1 && La == 1) 
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
			else //sinon tout les autres elements n'ont que une possibilite.
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

	//the timer start when the .exe is open
	temps = (unsigned int)time(NULL);

	//display the win conditions and the score
	cout << Message[0];	
	for (int Lo = 0; Lo < Longueur; Lo++)
		cout << PierreDollar;
	gotoxy(70, 0); 
	cout << Message[1] << numbreDeplacement;

	//display the full checkerboard
	gotoxy(DanierDebutCol, DanierDebutLigne);
	for (int L = 0; L < NB_LIG; L++)
	{
		for (int C = 0; C < NB_COL; C++)
		{	
			//updates the logics of the game
			if (L == 0 && C == 0) 
			{
				color = JAUNE; characVariable = 0;
			}
			else 
			{
				switch (jeu.damier[L][C]) 
				{
				case MU: color = BLANC; characVariable = PierreMur; break;
				default: color = BLEU; characVariable = PierreBleu; break;
				}
			}
			//updates the display of the game
			afficherBrick(C, L, danierAffichage, color, characVariable);
		}
	}

	do
	{
		deplacementValide = false;
		do
		{
			c1 = _getch();
			// validate if the input is an arrow key
			if (c1 == 0 || c1 == 224)
				if (_kbhit())
				{
					c2 = _getch();
					jeu.depl.arr = jeu.depl.dep; 
					// the 8 movement possible with the arrow keys
					switch (c2) 
					{
						case KEY_UP_LEFT: jeu.depl.arr.col--; jeu.depl.arr.lig--;	break;
						case KEY_UP: jeu.depl.arr.lig--; break;
						case KEY_UP_RIGHT: jeu.depl.arr.col++; jeu.depl.arr.lig--; 	break;
						case KEY_LEFT:	jeu.depl.arr.col--;	break;
						case KEY_RIGHT:jeu.depl.arr.col++;	break;
						case KEY_DOWN_LEFT: jeu.depl.arr.col--; jeu.depl.arr.lig++;	break;
						case KEY_DOWN: jeu.depl.arr.lig++;	break;
						case KEY_DOWN_RIGHT: jeu.depl.arr.col++; jeu.depl.arr.lig++;	break;
					}
					//validate if the movement is inside the checkerboard
					if ((jeu.depl.arr.col < NB_COL && jeu.depl.arr.col >= 0) && (jeu.depl.arr.lig < NB_LIG && jeu.depl.arr.lig >= 0)) 
						//validate if the movement doesn't go into a wall 
						if (jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] != MU)
							deplacementValide = true; 
					if (deplacementValide == false) 
						cout << "\a";
				}
		} while (deplacementValide != true); 

		//changes the state of the block where the player moved
		switch (jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col]) 
		{
			case BL: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = RO; break;
			case DC: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = DV; break;
			case DV: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = RO; 
				jeu.ptsAccum++; gotoxy(78, 0); cout << jeu.ptsAccum;break;
			case RO: jeu.damier[jeu.depl.arr.lig][jeu.depl.arr.col] = MU; break;
		}

		//checks if the player can still move
		pogner = 0; 
		//check the 8 positions around the player
		for (int L = -1; L <= 1; L++) 
		{
			for (int C = -1; C <= 1; C++)
			{
				if (!(L == 0 && C == 0))
				{
					if (((jeu.depl.arr.lig + L < 0) || (jeu.depl.arr.lig + L > NB_LIG - 1))
						|| ((jeu.depl.arr.col + C < 0) || (jeu.depl.arr.col + C > NB_COL - 1)))
						pogner++; 
					else if (jeu.damier[jeu.depl.arr.lig + L][jeu.depl.arr.col + C] == MU)
						pogner++; 
					else
						C = L = 2; 
				}
			}
		}

		//if the 8 positions around the player are inaccessible, the game is over
		if (pogner == 8) {
			bloque = true;
		}

		switch (jeu.damier[jeu.depl.dep.lig][jeu.depl.dep.col])//on update l'affichage du chagement d'element du depart du curseur
		{
			case BL: color = BLEU; characVariable = PierreBleu; break;
			case DV: color = VERT; characVariable = PierreDollar; break;
			case RO: color = ROSE; characVariable = PierreRose; break;
			case MU: color = BLANC; characVariable = PierreMur; break;
		}
		afficherBrick(jeu.depl.dep.col, jeu.depl.dep.lig, danierAffichage, color, characVariable);
		afficherBrick(jeu.depl.arr.col, jeu.depl.arr.lig, danierAffichage, color=JAUNE, characVariable=0);
		cvmSetColor(BLANC);
			
		jeu.depl.dep=jeu.depl.arr; 
		
		numbreDeplacement++; 

		//check if the winning condition is met
		if (jeu.ptsAccum == 15) 
			partieGagne = true;
		
	// the game last until the player wins or loses
	} while (partieGagne!=true && bloque != true);

	//clears the screen to show the game results
	temps -= (unsigned int)time(NULL); 

	clrscr();
	gotoxy(0, 1);
	
	
	if (jeu.ptsAccum == 15)
		cout << Message[2];
	else
		cout << Message[3];
	
	cout << endl << endl << Message[4] << right << setw(12) << ": " << left << setw(2) << jeu.ptsAccum << Message[5]; //affichage des "stats"
	cout << endl << endl << Message[6] << right << setw(6) << ": " << numbreDeplacement;
	cout << endl << endl << Message[7] << right << setw(16) << ": " << -temps << " sec";
	
	_getch();
}
