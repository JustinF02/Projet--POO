#pragma warning(disable : 4996)

#include <iostream>
#include <conio.h> // kbhit() ; getch() ;
#include <windows.h>
#include <vector>
#include <time.h>

HANDLE aff_con = GetStdHandle(STD_OUTPUT_HANDLE);
COORD position;

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class Brique
{
public:
	int pos_br_x;
	int pos_br_y;
	int larg_br;
	std::string cha_br = "#";
	Brique(int, int, int);   //Constructeur d'une brique
};

Brique::Brique(int posX, int posY, int large) {    //brique construite selon sa position horizontale et verticale, et sa taille
	pos_br_x = posX;
	pos_br_y = posY;
	larg_br = large;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CB    //Classe Casse-Brique : Définit la zone de jeu
{
public:
	int xmin = 1;
	int xmax = 20;
	int ymin = 1;
	int ymax = 20;
	int nbBriques = 4;
	std::vector<Brique> tabBrique;                    //Permet d'initialiser un certain nombre de briques dans la zone de jeu

	CB();            //constructeur par défaut qui initialise un vecteur de briques

	void aff_cb();   //Affiche la zone de jeu
	void aff_br();   //Affiche les briques dans la zone de jeu

};

CB::CB() {
	for (int i = 0; i <= nbBriques; i++) {
		Brique br((2 + (3 * i) + 1), 4, 3);     //La position X de chaque brique est la position de la première brique + i fois la largeur d'une brique + 1. Chaque brique sera placée avec un écart de 1 
		tabBrique.push_back(br);                //après la fin de la précédente
	}
}

void CB::aff_cb()                               //Affiche le plateau de jeu
{
	for (int i = xmin; i < xmax; i++) {
		position.X = i;
		position.Y = ymin;
		SetConsoleCursorPosition(aff_con, position);
		std::cout << "-";
	}

	for (int i = ymin; i < ymax; i++) {
		position.X = xmin;
		position.Y = i;
		SetConsoleCursorPosition(aff_con, position);
		std::cout << "|";
		position.X = xmax;
		SetConsoleCursorPosition(aff_con, position);
		std::cout << "|";
	}
}

void CB::aff_br()                              //Affiche et positionne les briques
{
	for (int j = 0; j < nbBriques; j++) {

		position.X = tabBrique.at(j).pos_br_x + j;
		position.Y = tabBrique.at(j).pos_br_y;
		SetConsoleCursorPosition(aff_con, position);
		for (int i = 0; i < tabBrique[j].larg_br; i++) {

			std::cout << tabBrique.at(j).cha_br;
		}
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class Raquette
{
public:
	int pos_r_x = 8;
	int pos_r_y = 19;         //position de départ de la raquette
	int larg_r = 5;
	std::string cha_r = "=";
	void aff_r();
};

void Raquette::aff_r()
{
	for (int i = 0; i < larg_r; i++) {
		position.X = pos_r_x + i;
		position.Y = pos_r_y;
		SetConsoleCursorPosition(aff_con, position);
		std::cout << cha_r;
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class Balle
{
public:
	int pos_b_x;
	int pos_b_y = 15;
	int vitesseHor;                //dirige la balle vers la droite
	int vitesseVert = -1;              //dirige la balle vers le haut
	std::string cha_b = "O";
	void aff_b();
	Balle();                           //Initialise la position horizontale de départ de la balle de manière aléatoire
	bool colMur(Balle, CB);            // Teste si la balle a une collision avec un mur
	bool colPlafond(Balle, CB);        //Teste si la balle a une collision avec le plafond
	bool colBrique(Balle, CB, int);    //Teste si la balle a une collision avec une brique faisant parti du vecteur de Briques de la classe CB 
	bool colRaquette(Balle, Raquette); //Teste si la balle a une collision avec la raquette
};

Balle::Balle() {
	CB cb;
	srand(time(NULL));
	this->pos_b_x = (rand() % cb.xmax - 2) + 2;
	int vitHorinit[2] = { -1, 1 };
	int randNum = rand() % 2;          //retourne 0 ou 1
	this->vitesseHor = vitHorinit[randNum];  //Affecte à la vitesseHor -1 ou 1 selon le tirage de randNum
}

void Balle::aff_b()
{
	pos_b_x += vitesseHor;
	pos_b_y += vitesseVert;
	position.X = pos_b_x;
	position.Y = pos_b_y;
	SetConsoleCursorPosition(aff_con, position);
	std::cout << cha_b;
}

// Teste si la balle a une collision avec un mur 
bool Balle::colMur(Balle b, CB cb) {
	return (b.pos_b_x >= cb.xmax - 1 || b.pos_b_x <= cb.xmin + 1) ? true : false;
}

//Teste si la balle a une collision avec le plafond
bool Balle::colPlafond(Balle b, CB cb) {
	return (b.pos_b_y <= cb.ymin + 1) ? true : false;
}

//Teste si la balle a une collision avec une brique faisant parti du vecteur de Briques de la classe CB 
bool Balle::colBrique(Balle b, CB cb, int index) {

	return ((b.pos_b_x >= cb.tabBrique.at(index).pos_br_x && b.pos_b_x <= cb.tabBrique.at(index).pos_br_x + cb.tabBrique.at(index).larg_br) && (b.pos_b_y == cb.tabBrique.at(index).pos_br_y)) ? true : false;
}

//Teste si la balle a une collision avec la raquette
bool Balle::colRaquette(Balle b, Raquette r) {
	return ((b.pos_b_x >= r.pos_r_x && b.pos_b_x <= (r.pos_r_x + r.larg_r)) && (b.pos_b_y == r.pos_r_y)) ? true : false;
}

/*---------------------------------------------------------------------------------------------MAIN----------------------------------------------------------------------------------------------------------*/

int main()
{
	Balle b;
	Raquette r;
	CB cb;
	bool win = false; bool lose = false;
	int briquesRestante = cb.nbBriques;


	std::cout << "appuyez sur la touche \"entrer\" pour commencer !";
	std::cin.ignore();									//attend l'appuie d'une touche de la part du joueur pour commencer

	while (win == false && lose == false) {
		if (kbhit())
		{
			char ch = getch();                                  //permet de bouger la raquette
			if (ch == 77 && r.pos_r_x + r.larg_r < cb.xmax)     // code 77 = flèche droite, bouge tant que limite gauche pas atteinte
				r.pos_r_x++;
			if (ch == 75 && r.pos_r_x - 1 > cb.xmin)            //code 75 = flèche gauche, bouge tant que limite droite pas atteinte
				r.pos_r_x--;
			if (ch == 27)                                       //code 27 = echap, quitte le jeu
				break;
		}

		if (b.colMur(b, cb))
			b.vitesseHor *= -1;                   //Si la balle touche un mur, il ne faut changer que sa direction horizontale

		if (b.colPlafond(b, cb))
			b.vitesseVert *= -1;                  //Si la balle touche le plafond, c'est sa position verticale qui s'oppose

		if (b.colRaquette(b, r)) {
			b.vitesseVert *= -1;
			if(b.vitesseHor < 2)
				b.vitesseHor *= 2;            //Quand la balle touche la raquette, sa vitesse double
		}

		for (int i = 0; i < cb.nbBriques; i++) {

			if (b.colBrique(b, cb, i)) {
				cb.tabBrique.erase(cb.tabBrique.begin() + i);    //On efface la brique avec laquelle il y a eu collision du vecteur brique 
				cb.nbBriques--;
				briquesRestante--;
				b.vitesseVert *= -1;                             //La balle prends la direction verticale opposée à la direction initiale

				if (b.vitesseHor == 2 || b.vitesseHor == -2)
					b.vitesseHor /= 2;                           //Si la vitesse de la balle a été augmentée, la vitesse est divisée par 2 si elle touche une brique
			}
		}

		if (b.pos_b_y > r.pos_r_y)               // Si la position verticale de la balle est inférieure à la position verticale de la raquette, le joueur a perdu
			lose = true;

		if (briquesRestante == 0)                // S'il ne reste plus de briques, alors le joueur a gagné
			win = true;

		system("cls");

		b.aff_b();
		r.aff_r();
		cb.aff_br();
		cb.aff_cb();
		Sleep(20);
	}
	system("cls");

	/*--------------------------------------------------------------------------------------------------Annonce fin de partie-----------------------------------------------------------------------------------*/

	if (win) {
		std::cout << "========== What a Save ! ==========\n";
		std::cout << "C'est le chemin parcouru et les efforts consentis qui parlent \nle mieux de la valeur du merite et de la justice de la recompense. \nCette victoire personnelle n'est point seulement un don du ciel ... \nElle est avant tout un juste retour et une vrai reconnaissance de vos qualites personnelles. \nMes plus sinceres felicitations pour cette victoire si belle et meritee.\n";
	}
	else {
		if (lose) {
			std::cout << "========== Game Over ==========\n";
			std::cout << "Pour ne pas devoir affronter une situation trop penible, \non s'avoue coupable de tout, c'est-a-dire de rien. \nLa demission devant une situation qui apparait trop effrayante degenere \npeu a peu en une demission devant la vie toute entiere.\n";
			std::cout << "Vous avez perdu, la prochaine fois peut etre !\n";
		}
		else {
			std::cout << "A une prochaine fois peut etre !\n";
		}
	}
	return 0;
}
