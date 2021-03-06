//  Ce fichier est le point d'entrée du moteur ( le "main()" )

#pragma region *** Les includes ***
#include "window.h"
#include "input.h"
#include "timer.h"
#include "ogl.h"
#include "load.h"
#include "camera.h"
#include "maths_3d.h"
#include <iostream>
#pragma endregion

/********************************************************************\
*                                                                    *
* Toutes les variables					                             *
*                                                                    *
\********************************************************************/
#pragma region "... Variables ... "

/****************************************************************************\
*                                                                            *
*                            Variables EXTERNES                              *
*                                                                            *
\****************************************************************************/


/****************************************************************************\
*                                                                            *
*                            Variables GLOBALES                              *
*                                                                            *
\****************************************************************************/
WINDOW    *win = NULL;
MY_INPUT  *inp = NULL;
TIMER     *tim = NULL;
Model3D   *model = NULL;
Texture   *texture_terrain = NULL;
Image     *heightmap = NULL;
CAMERA    *cam = NULL;

//.. Textures des objets cachés dans le labyrinthe ..
Texture   *texture_arbre = NULL;
Texture   *texture_raptor = NULL;
Texture   *texture_logo = NULL;

//.. Les differentes map ..
Image     *heightmap01 = NULL;
Image     *heightmap02 = NULL;
Image     *heightmap03 = NULL;
Image     *heightmap04 = NULL;
Image     *heightmap05 = NULL;

//.. Les differents terrains ..
Texture   *texture_terrain01 = NULL;
Texture   *texture_terrain02 = NULL;
Texture   *texture_terrain03 = NULL;
Texture   *texture_terrain04 = NULL;
Texture   *texture_terrain05 = NULL;

//.. Position et orientation de la camera ..
float posX_camera;
float posY_camera = 30;
float posZ_camera;
float angleX_camera = 0;
float angleY_camera = 0;
float angleZ_camera = 0;

//.. Taille de la carte ..
float size;
float size2;
float hauteurMax;

//.. Position des objets a trouver ..
float posX_arbre;
float posY_arbre;
float posZ_arbre;

float posX_raptor;
float posY_raptor;
float posZ_raptor;

float posX_logo;
float posY_logo;
float posZ_logo;

float posX_pave;
float posY_pave;
float posZ_pave;

float posX_pyramide;
float posY_pyramide;
float posZ_pyramide;

float posX_horloge;
float posY_horloge;
float posZ_horloge;

//.. La couleur du ciel ..
float redSky;
float greenSky;
float blueSky;

//.. Vitesse de déplacement ..
float speed;
float sprint;

//.. Affichage des objets ou non ..
bool arbreInMap;
bool raptorInMap;
bool logoInMap;
bool paveInMap;
bool pyramideInMap;
bool horlogeInMap;

// Nombre de fois qu'on a interrogé l'arbre (c'est pour rendre les gens fou !!)
int nbChanges = 0;

#pragma endregion


/********************************************************************\
*                                                                    *
*  Les valeurs initiales des variables                               *
*                                                                    *
\********************************************************************/
void resetParameters()
{
	#pragma region ".. Position et orientation de la camera .."
		posX_camera = 0;
		posZ_camera = 10;
	#pragma endregion

	#pragma region ".. Taille de la carte .."
		size = 10;
		size2 = 0.3;
		hauteurMax = 10.0f;
	#pragma endregion

	#pragma region ".. Position des objets a trouver .."
		posX_arbre = -30.0f;
		posZ_arbre = 0.0f;

		posX_raptor = 250.0f;
		posZ_raptor = 160.0f;

		posX_logo = 200.0f;
		posZ_logo = -170.0f;

		posX_pave = -150.0f;
		posZ_pave = -200.0f;

		posX_pyramide = -230.0f;
		posZ_pyramide = 40.0f;

		posX_horloge = 40.0f;
		posZ_horloge = -230.0f;
	#pragma endregion

	#pragma region ".. La couleur du ciel .."
		redSky = 0.1f;
		greenSky = 0.3f;
		blueSky = 0.5f;
	#pragma endregion

	#pragma region ".. Vitesse de déplacement .."
		speed = 2.0f;
		sprint = 5.0f;
	#pragma endregion

	#pragma region ".. Affichage des objets ou non .."
		arbreInMap = true;
		raptorInMap = false;
		logoInMap = false;
		paveInMap = false;
		pyramideInMap = false;
		horlogeInMap = false;
	#pragma endregion

}


/********************************************************************\
*                                                                    *
*  Démarre l'application (avant la main_loop). Renvoie false si      *
*  il y a une erreur.                                                *
*                                                                    *
\********************************************************************/
bool start()
{
	printf("Debut Start()");

	// Initialise des variables
	resetParameters();

	#pragma region "... initialisations de base ..."	
	win = new WINDOW();									// prépare la fenêtre
	win->create(800, 600, 16, 60, false);				// crée la fenêtre

	tim = new TIMER();									// crée un timer
	cam = new CAMERA();

	inp = new MY_INPUT(win);							// initialise la gestion clavier souris
	create_context(*win);								// crée le contexte OpenGL sur la fenêtre
	init_font(*win, "Courier");							// initialise la gestion de texte avec OpenGL
	#pragma endregion
	
	#pragma region "... initialisations d'OpenGL ..."
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// set clear color for color buffer (RGBA, black)
	glViewport(0, 0, win->Xres, win->Yres);				// zone de rendu (tout l'écran)
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading -> (pour les normales)
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// mapping quality = best
	glFrontFace(GL_CCW);								// front of face is defined counter clock wise
	glPolygonMode(GL_FRONT, GL_FILL);					// front of a face is filled
	glPolygonMode(GL_BACK, GL_LINE);					// back of a face is made of lines
	glCullFace(GL_BACK);								// cull back face only
	glDisable(GL_CULL_FACE);						    // disable back face culling
	#pragma endregion

	#pragma region "... Chargement des textures ..."

	texture_arbre = new Texture();
	texture_arbre->load_texture("arbre.tga", "arbre_masque.tga");
	glGenTextures(1, texture_arbre->OpenGL_ID);

	model = new Model3D();
	model->load_3d_model("RAPTOR.off");

	texture_raptor = new Texture();
	texture_raptor->load_texture("RAPTOR.tga", NULL);
	glGenTextures(1, texture_raptor->OpenGL_ID);					// cr�e un "nom" de texture (un identifiant associ� � la texture)

	texture_logo = new Texture();
	texture_logo->load_texture("artemis.tga", "artemis_mask.tga");
	glGenTextures(1, texture_logo->OpenGL_ID);

	// Terrain 01
	heightmap01 = new Image();
	heightmap01->load_tga("heightmap01.tga");

	texture_terrain01 = new Texture();
	texture_terrain01->load_texture("terrain01.tga", NULL);

	// Terrain 02
	heightmap02 = new Image();
	heightmap02->load_tga("heightmap02.tga");

	texture_terrain02 = new Texture();
	texture_terrain02->load_texture("terrain02.tga", NULL);

	// Terrain 03
	heightmap03 = new Image();
	heightmap03->load_tga("heightmap03.tga");

	texture_terrain03 = new Texture();
	texture_terrain03->load_texture("terrain03.tga", NULL);

	// Terrain 04
	heightmap04 = new Image();
	heightmap04->load_tga("heightmap04.tga");

	texture_terrain04 = new Texture();
	texture_terrain04->load_texture("terrain04.tga", NULL);

	// Terrain 05
	heightmap05 = new Image();
	heightmap05->load_tga("heightmap05.tga");

	texture_terrain05 = new Texture();
	texture_terrain05->load_texture("terrain05.tga", NULL);


	heightmap = heightmap01;

	texture_terrain = texture_terrain01;
	glGenTextures(1, texture_terrain->OpenGL_ID);

	glAlphaFunc(GL_GREATER, 0.8); // pour la transparence

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#pragma endregion

	#pragma region ".. Update des hauteurs des objets .."
	posY_arbre = size2 * heightmap->data[3 * ((int)(posX_arbre / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_arbre / size + heightmap->leny / 2))];
	posY_raptor = size2 * heightmap->data[3 * ((int)(posX_raptor / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_raptor / size + heightmap->leny / 2))];
	posY_logo = size2 * heightmap->data[3 * ((int)(posX_logo / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_logo / size + heightmap->leny / 2))];
	posY_pave = size2 * heightmap->data[3 * ((int)(posX_pave / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_pave / size + heightmap->leny / 2))];
	posY_pyramide = size2 * heightmap->data[3 * ((int)(posX_pyramide / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_pyramide / size + heightmap->leny / 2))];
	posY_horloge = 30 + size2 * heightmap->data[3 * ((int)(posX_horloge / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_horloge / size + heightmap->leny / 2))];
	#pragma endregion


	win->set_title("BUMILOBO : Find Artemis ! Indice : E-SPACE-R"); 
	// "BUMILOBO" = "EXPLORER" avec un chiffrement de César (décalage de 23)
	// Cela illustre le fait de vouloir chercher, explorer ! Peu d'information est donné au joueur
	// Il ne peut voir que l'objectif "Find Artemis" et des indices correspondant à des touches du clavier.

	printf("Fin Start()");
	return true;
}


/********************************************************************\
*                                                                    *
*  Les fonctions des objets										     *
*                                                                    *
\********************************************************************/
#pragma region "... Fonctions ..."

void updatePosY()
{
	// Position de la camera
	posY_camera = 25 + size2 * heightmap->data[3 * ((int)(posX_camera / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_camera / size + heightmap->leny / 2))];

	posY_arbre = size2 * heightmap->data[3 * ((int)(posX_arbre / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_arbre / size + heightmap->leny / 2))];
	posY_raptor = size2 * heightmap->data[3 * ((int)(posX_raptor / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_raptor / size + heightmap->leny / 2))];
	posY_logo = 30 + size2 * heightmap->data[3 * ((int)(posX_logo / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_logo / size + heightmap->leny / 2))];
	posY_pave = size2 * heightmap->data[3 * ((int)(posX_pave / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_pave / size + heightmap->leny / 2))];
	posZ_pyramide = size2 * heightmap->data[3 * ((int)(posX_pyramide / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posY_pyramide / size + heightmap->leny / 2))];
	posY_horloge = 50 + size2 * heightmap->data[3 * ((int)(posX_horloge / size + heightmap->lenx / 2) + heightmap->lenx * (int)(posZ_horloge / size + heightmap->leny / 2))];
}

// Display les objets
void arbre() {
	
	glGenTextures(1, texture_arbre->OpenGL_ID);							// cr�e un "nom" de texture (un identifiant associ� � la texture)
	glBindTexture(GL_TEXTURE_2D, texture_arbre->OpenGL_ID[0]);			// et on active ce "nom" comme texture courante (d�finie plus bas)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);		// on r�p�te la texture en cas de U,V > 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);		// ou < 0.0
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		// indique qu'il faut m�langer la texture avec la couleur courante

	if (texture_arbre->isRGBA)											// on met de la couleur a l'arbre
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, texture_arbre->img_color->lenx, texture_arbre->img_color->leny, GL_RGBA, GL_UNSIGNED_BYTE, texture_arbre->img_all);
	}
	else
	{
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, texture_arbre->img_color->lenx, texture_arbre->img_color->leny, GL_RGB, GL_UNSIGNED_BYTE, texture_arbre->img_color->data);
	}

	glPushMatrix();
	glTranslatef(posX_arbre, posY_arbre, posZ_arbre);
	glScalef(60, 60, 60);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5, 0, 0);
		glTexCoord2f(0, 1); glVertex3f(-0.5, 2, 0);
		glTexCoord2f(1, 1); glVertex3f(0.5, 2, 0);
		glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0);
	glEnd();



	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5, 0, 0);
		glTexCoord2f(0, 1); glVertex3f(-0.5, 2, 0);
		glTexCoord2f(1, 1); glVertex3f(0.5, 2, 0);
		glTexCoord2f(1, 0); glVertex3f(0.5, 0, 0);
	glEnd();


	glPopMatrix();
	glPolygonMode(GL_FRONT, GL_FILL);					// front of a face is filled
	glPolygonMode(GL_BACK, GL_LINE);
	glDisable(GL_ALPHA_TEST);
	glPopMatrix();
}
void raptor()
{

	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, texture_raptor->OpenGL_ID[0]);		// et on active ce "nom" comme texture courante (d�finie plus bas)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // on répète la texture en cas de U,V > 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // ou < 0.0
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	// indique qu'il faut mélanger la texture avec la couleur courante

	// charge le tableau de la texture en mémoire vidéo et crée une texture mipmap
	if (texture_raptor->isRGBA)
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, texture_raptor->img_color->lenx, texture_raptor->img_color->leny, GL_RGBA, GL_UNSIGNED_BYTE, texture_raptor->img_all);
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, texture_raptor->img_color->lenx, texture_raptor->img_color->leny, GL_RGB, GL_UNSIGNED_BYTE, texture_raptor->img_color->data);

	glPushMatrix();
	glTranslatef(posX_raptor, posY_raptor, posZ_raptor);
	glRotatef(90, 1, 0, 0);
	glRotatef(180, 0, 1, 0);
	glScalef(0.3, 0.3, 0.3);

	glBegin(GL_TRIANGLES);
		for (int i = 0; i<model->nb_triangles; i++) {
			glTexCoord2f(model->points[model->faces[i].a].ucol,	model->points[model->faces[i].a].vcol);
			glVertex3f(model->points[model->faces[i].a].x, model->points[model->faces[i].a].y, model->points[model->faces[i].a].z);

			glTexCoord2f(model->points[model->faces[i].b].ucol, model->points[model->faces[i].b].vcol);
			glVertex3f(model->points[model->faces[i].b].x, model->points[model->faces[i].b].y, model->points[model->faces[i].b].z);

			glTexCoord2f(model->points[model->faces[i].c].ucol, model->points[model->faces[i].c].vcol);
			glVertex3f(model->points[model->faces[i].c].x, model->points[model->faces[i].c].y, model->points[model->faces[i].c].z);
		}
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}
void logo() 
{


	// crée un "nom" de texture (un identifiant associé à la texture)
	glBindTexture(GL_TEXTURE_2D, texture_logo->OpenGL_ID[0]);    // et on active ce "nom" comme texture courante (définie plus bas)
															// indique qu'il faut mélanger la texture avec la couleur courante
																// charge le tableau de la texture en mémoire vidéo et crée une texture mipmap
	glTranslatef(posX_logo, posY_logo, posZ_logo);

	if (texture_logo->isRGBA)
	{
		glDrawPixels(texture_logo->img_color->lenx, texture_logo->img_color->leny, GL_RGBA, GL_UNSIGNED_BYTE, texture_logo->img_all);
	}
	else
	{
		glDrawPixels(texture_logo->img_color->lenx, texture_logo->img_color->leny, GL_RGB, GL_UNSIGNED_BYTE, texture_logo->img_color->data);
	}

	printf(" Gagné !");
}
void pave(float largeurX, float longueurY, float hauteurZ, float colorRED, float colorGREEN, float colorBLUE) {

	glBegin(GL_QUADS);
		//Rectangle 1 (z fixe)
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave + longueurY, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave + longueurY, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave, posZ_pave);

		//Rectangle 2 (x fixe)
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave + longueurY, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave + longueurY, posZ_pave);

		//Rectangle 3 (y fixe)
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave, posZ_pave + hauteurZ);

		//Rectangle 4 (x + dx fixe)
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave + longueurY, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave + longueurY, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave, posZ_pave + hauteurZ);

		//Rectangle 5 (y + dy fixe)
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave + longueurY, posZ_pave);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave + longueurY, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave + longueurY, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave + longueurY, posZ_pave);

		//Rectangle 6 (z + dz fixe)
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave + largeurX, posY_pave + longueurY, posZ_pave + hauteurZ);
		glColor3f(colorRED, colorGREEN, colorBLUE); glVertex3f(posX_pave, posY_pave + longueurY, posZ_pave + hauteurZ);
	glEnd();
}
void pyramide(float h, float c) {

	glBegin(GL_TRIANGLES);
		//Triangle 1
		glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(posX_pyramide - h, posZ_pyramide + h, posY_pyramide);
		glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(posX_pyramide, posZ_pyramide, posY_pyramide + c);
		glColor3f(1.0f, 0.0f, 0.0f); glVertex3f(posX_pyramide + h, posZ_pyramide + h, posY_pyramide);

		//Triangle 2
		glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide + h, posZ_pyramide + h, posY_pyramide);
		glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide, posZ_pyramide, posY_pyramide + c);
		glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide + h, posZ_pyramide - h, posY_pyramide);

		//Triangle 3
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(posX_pyramide + h, posZ_pyramide - h, posY_pyramide);
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(posX_pyramide, posZ_pyramide, posY_pyramide + c);
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(posX_pyramide - h, posZ_pyramide - h, posY_pyramide);

		//Triangle 4
		glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide - h, posZ_pyramide - h, posY_pyramide);
		glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide, posZ_pyramide, posY_pyramide + c);
		glColor3f(1.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide - h, posZ_pyramide + h, posY_pyramide);
	glEnd();

	glBegin(GL_QUADS);
		//Base de la pyramide
		glColor3f(0.0f, 0.0f, 0.0f); glVertex3f(posX_pyramide - h, posZ_pyramide + h, posY_pyramide);
		glColor3f(0.0f, 1.0f, 0.0f); glVertex3f(posX_pyramide + h, posZ_pyramide + h, posY_pyramide);
		glColor3f(0.0f, 0.0f, 1.0f); glVertex3f(posX_pyramide + h, posZ_pyramide - h, posY_pyramide);
		glColor3f(1.0f, 1.0f, 1.0f); glVertex3f(posX_pyramide - h, posZ_pyramide - h, posY_pyramide);
	glEnd();

	glColor3f(1, 1, 1);

}
void horloge() {

	glTranslatef(posX_horloge, posY_horloge, posZ_horloge);

	glPushMatrix();
	float heure = (float)tim->get_heure();
	float angleH = heure / 12 * 360;
	glRotatef(angleH, 0, 0, 1);

	glColor3f(1, 1, 0);
	glBegin(GL_TRIANGLES);
		glVertex3f(0.3, 0, 0);
		glVertex3f(-0.3, 0, 0);
		glVertex3f(0, 7, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	float minute = (float)tim->get_minute();
	float angleM = minute / 60 * 360;
	glRotatef(angleM, 0, 0, 1);
	glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 1);
		glVertex3f(0.3, -0.3, 0);
		glVertex3f(-0.3, 0.3, 0);
		glVertex3f(0, 9, 0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	float seconde = (float)tim->get_seconde();
	float angleS = seconde / 60 * 360;
	glRotatef(angleS, 0, 0, 1);
	glBegin(GL_LINES);
		glColor3f(1, 0.5, 0.5);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 6.75, 0);
	glEnd();
	glPopMatrix();

	for (int i = 0; i < 12; i++) {
		glPushMatrix();
		glRotatef(30 * i, 0, 0, 1);
		glTranslatef(0, 10, 0);
		if (i % 3 == 0)
		{
			glScalef(1, 1, 1);
		}
		else
		{
			glScalef(0.5, 0.5, 1);
		}
		glBegin(GL_QUADS);
			glColor3f(1, 0, 0);
			glVertex3f(-0.5, -1, 0);
			glVertex3f(-0.5, 1, 0);
			glVertex3f(0.5, 1, 0);
			glVertex3f(0.5, -1, 0);
		glEnd();
		glPopMatrix();
	}
	glColor3f(1, 1, 1);
}

// Display le terrain
void display_terrain()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_terrain->OpenGL_ID[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // on répète la texture en cas de U,V > 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);    // ou < 0.0
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // indique qu'il faut mélanger la texture avec la couleur courante

	if (texture_terrain->isRGBA)
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, texture_terrain->img_color->lenx, texture_terrain->img_color->leny, GL_RGBA, GL_UNSIGNED_BYTE, texture_terrain->img_all);
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, texture_terrain->img_color->lenx, texture_terrain->img_color->leny, GL_RGB, GL_UNSIGNED_BYTE, texture_terrain->img_color->data);


	glBegin(GL_TRIANGLES);
		for (int i = 0; i<(int)heightmap->lenx - 1; i++) {
			for (int j = 0; j<(int)heightmap->leny - 1; j++) {
				float I = i - (int)heightmap->lenx / 2;
				float J = j - (int)heightmap->leny / 2;

				// i, j
				glTexCoord2f((float)i / (float)heightmap->lenx, (float)j / (float)heightmap->leny);
				glVertex3f((I + 0)*size, size2*(float)heightmap->data[3 * (i + heightmap->lenx*(j + 0))], (J + 0)*size);

				// i, j+1
				glTexCoord2f((float)i / (float)heightmap->lenx, (float)(j + 1) / (float)heightmap->leny);
				glVertex3f((I + 0)*size, size2*(float)heightmap->data[3 * (i + heightmap->lenx*(j + 1))], (J + 1)*size);

				// i+1, j
				glTexCoord2f((float)(i + 1) / (float)heightmap->lenx, (float)j / (float)heightmap->leny);
				glVertex3f((I + 1)*size, size2*(float)heightmap->data[3 * (i + 1 + heightmap->lenx*(j + 0))], (J + 0)*size);

				//  i, j+1
				glTexCoord2f((float)i / (float)heightmap->lenx, (float)(j + 1) / (float)heightmap->leny);
				glVertex3f((I + 0)*size, size2*(float)heightmap->data[3 * (i + heightmap->lenx*(j + 1))], (J + 1)*size);

				// i+1, j+1
				glTexCoord2f((float)(i + 1) / (float)heightmap->lenx, (float)(j + 1) / (float)heightmap->leny);
				glVertex3f((I + 1)*size, size2*(float)heightmap->data[3 * (i + 1 + heightmap->lenx*(j + 1))], (J + 1)*size);

				// i+1,j
				glTexCoord2f((float)(i + 1) / (float)heightmap->lenx, (float)j / (float)heightmap->leny);
				glVertex3f((I + 1)*size, size2*(float)heightmap->data[3 * (i + 1 + heightmap->lenx*(j + 0))], (J + 0)*size);

			}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

// Y a-t-il un objet près de nous ?
int isObjectNear()
{
	if (arbreInMap && (abs(posX_camera - posX_arbre) < 50 && abs(posZ_camera - posZ_arbre) < 50) ) {
		return 1;
	}
	if (raptorInMap && (abs(posX_camera - posX_raptor) < 50 && abs(posZ_camera - posZ_raptor) < 50) ) {
		return 2;
	}
	if (horlogeInMap && (abs(posX_camera - posX_horloge) < 50 && abs(posZ_camera - posZ_horloge) < 50) ) {
		return 3;
	}
	if (paveInMap && (abs(posX_camera - posX_pave) < 50 && abs(posZ_camera - posZ_pave) < 50) ) {
		return 4;
	}
	if (logoInMap && (abs(posX_camera - posX_logo) < 200 && abs(posZ_camera - posZ_logo) < 200) ) {
		return 5;
	}
	if (pyramideInMap && (abs(posX_camera - posX_pyramide) < 50 && abs(posZ_camera - posY_pyramide) < 50) ) {
		return 6;
	}
	return 0;
}

// Lorsqu'on interroge un objet
void newParametersArbre() 
{
	resetParameters();
	redSky += 0.1f;
	greenSky -= 0.05f;
	blueSky -= 0.1f;
	updatePosY();
	paveInMap = true;
	raptorInMap = true;

	nbChanges++;
}
void newParametersRaptor()
{
	redSky += 0.3f;
	greenSky -= 0.1f;
	blueSky -= 0.1f;

	size = 9;
	size2 = 0.3;

	heightmap = heightmap03;

	texture_terrain = texture_terrain03;
	glGenTextures(1, texture_terrain->OpenGL_ID);

	
	#pragma region ".. Update des positions des objets a trouver .."
	posX_arbre += -10;
	posZ_arbre -= -30;
	
	posX_raptor -= 10;
	posZ_raptor += 10;
	
	posX_logo -= -40;
	posZ_logo -= 20;
	
	posX_pave += 1;
	posZ_pave -= 5;
	
	posX_horloge -= 14;
	posZ_horloge -= 12;
	#pragma endregion

	updatePosY();

	if (redSky > 0.5)
	{
		paveInMap = false;
	}
}
void newParametersHorloge()
{
	redSky = 0.4f;
	greenSky = 0.4f;
	blueSky = 0.4f;

	size2 = 0.2;

	heightmap = heightmap02;

	texture_terrain = texture_terrain02;
	glGenTextures(1, texture_terrain->OpenGL_ID);

	updatePosY();

	paveInMap = false;
	pyramideInMap = false;
	logoInMap = true;
}
void newParametersPave()
{
	redSky += 0.1f;
	greenSky -= 0.1f;
	blueSky += 0.1f;

	size = 7;
	size2 = 0.8;

	if (greenSky < 0.2)
	{
		heightmap = heightmap04;

		texture_terrain = texture_terrain04;
		glGenTextures(1, texture_terrain->OpenGL_ID);
	}
	else
	{
		heightmap = heightmap01;

		texture_terrain = texture_terrain01;
		glGenTextures(1, texture_terrain->OpenGL_ID);
	}

	updatePosY();

	pyramideInMap = true;
	horlogeInMap = false;
}
void newParametersPyramide()
{
	heightmap = heightmap05;

	texture_terrain = texture_terrain05;
	glGenTextures(1, texture_terrain->OpenGL_ID);

	updatePosY();

	raptorInMap = false;
	horlogeInMap = true;
}

#pragma endregion


/********************************************************************\
*                                                                    *
*  Boucle principale, appelée pour construire une image, gérer les   *
*  entrées.															 *
*                                                                    *
\********************************************************************/
void main_loop()
{
	inp->refresh();
	tim->update_horloge();
	inp->get_mouse_movement();

	if (inp->keys[KEY_CODE_ESCAPE])
	{
		PostMessage(win->handle, WM_CLOSE, 0, 0);			// Stoppe la "pompe à message" en y envoyant le message "QUIT"
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And The Depth Buffer
	glClearColor(redSky, greenSky, blueSky, 1.0f);			// La couleur du ciel
	
	#pragma region ".. Gestion camera .."
	float speedUsed = speed;

	if (inp->keys[KEY_CODE_SPACE])
	{
		speedUsed = sprint;
		posX_raptor += 1.0f;
		posZ_raptor -= 2.0f;
	}

	if (inp->keys[KEY_CODE_R])
	{
		resetParameters();
	}

	// Gestion des touches
	if (inp->keys[KEY_CODE_E]) {
		// interagir avec les objets
		switch (isObjectNear())
		{
			case 0: break;
			case 1: newParametersArbre(); break;
			case 2: newParametersRaptor(); break;
			case 3: newParametersHorloge(); break;
			case 4: newParametersPave(); break;
			case 6: newParametersPyramide();  break;
			default: break;
		}
	}

	

	// Les déplacements - Gestion du clavier
	point dir = cam->direction - cam->position;
	
	float oldPosX_camera = posX_camera;
	float oldPosY_camera = posY_camera;
	float oldPosZ_camera = posZ_camera;

	if (inp->keys[KEY_CODE_UP] || inp->keys[KEY_CODE_Z]) {
			posX_camera += speedUsed * dir.x;
			posZ_camera += speedUsed * dir.z;
	}
	if (inp->keys[KEY_CODE_DOWN] || inp->keys[KEY_CODE_S]) {
			posX_camera -= speedUsed * dir.x;
			posZ_camera -= speedUsed * dir.z;
	}
	if (inp->keys[KEY_CODE_RIGHT] || inp->keys[KEY_CODE_D]) {
			posX_camera += speedUsed * produit_vectoriel(dir, cam->orientation).x;
			posZ_camera += speedUsed * produit_vectoriel(dir, cam->orientation).z;
	}
	if (inp->keys[KEY_CODE_LEFT] || inp->keys[KEY_CODE_Q]) {
			posX_camera -= speedUsed * produit_vectoriel(dir, cam->orientation).x;
			posZ_camera -= speedUsed * produit_vectoriel(dir, cam->orientation).z;
	}

	// L'orientation de la camera - Gestion de la souris
	angleX_camera -= 0.1 *(float)inp->Yrelmouse;
	angleY_camera -= 0.1 * (float)inp->Xrelmouse;

	if (angleX_camera > 60) { angleX_camera = 60; }
	else if (angleX_camera < -60) { angleX_camera = -60; }

	if (angleY_camera > 360) { angleY_camera -= 360; }
	else if (angleY_camera < 0) { angleY_camera += 360; }

	updatePosY();

	// Gestion de la hauteur et des murs infranchissables
	if (posY_camera - oldPosY_camera > hauteurMax)
	{
		posX_camera = oldPosX_camera;
		posY_camera = oldPosY_camera;
		posZ_camera = oldPosZ_camera;
	}



	// Update Camera
	cam->update(point(posX_camera, posY_camera, posZ_camera), angleX_camera, angleY_camera, angleZ_camera);

	#pragma endregion

	#pragma region ".... Preparation affichage ..."

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)win->Xres / (double)win->Yres, 10, 30000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cam->position.x, cam->position.y, cam->position.z,	 //position
		cam->direction.x, cam->direction.y, cam->direction.z,		 // point cible
		cam->orientation.x, cam->orientation.y, cam->orientation.z); //vecteur up
	#pragma endregion

	// Affiche le terrain
	display_terrain();

	// Que peut-on afficher d'autres ?
	if (arbreInMap)
	{
		arbre();
	}
	if (raptorInMap)
	{
		raptor();
	}
	if (logoInMap && isObjectNear() == 5)
	{
		logo();
	}
	if (paveInMap)
	{
		if (horlogeInMap)
		{
			posX_pave += 0.01;
			pave(40, 60, 70, 0.7, 0.2, 0.6);
		}
		else
		{
			pave(30, 30, 15, 0.1, 0.2, 0.2);
		}
		
	}
	if (pyramideInMap)
	{
		pyramide(30,40);
	}
	if (horlogeInMap && nbChanges > 4)
	{
		horloge();
	}
	

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//						ça finit ici															//
	//////////////////////////////////////////////////////////////////////////////////////////////////
	swap_buffer(win);	// affiche l'image composée à l'écran
}



/********************************************************************\
*                                                                    *
* Arrête l'application                                               *
*                                                                    *
\********************************************************************/
void stop()
{
	delete inp;
	delete tim;

	if (win)
	{
		kill_font();
		kill_context(*win);
		delete win;
	}
}


/********************************************************************\
*                                                                    *
* Point d'entrée de notre programme pour Windows ("WIN32")           *
*                                                                    *
\********************************************************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	MSG  msg;

	win = NULL;
	inp = NULL;
	tim = NULL;

	if (start() == false)								// initialise la fenêtre et OpenGL
	{
		debug("start() : échec !");
		stop();
		return 1;
	}

	// main loop //
	// --------- //
	//   __
	//  /  \_
	//  |  \/ 
	//  \__/

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))	// s'il y a un message, appelle WndProc() pour le traiter
		{
			if (!GetMessage(&msg, NULL, 0, 0))				// "pompe à message"
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			main_loop();								// sinon, appelle main_loop()
		}
	}


	stop();												// arrête OpenGL et ferme la fenêtre

	return 0;
}



/********************************************************************\
*                                                                    *
*  Boucle des messages                                               *
*                                                                    *
\********************************************************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{

	case WM_MOVE:        win->Xpos = (dword)LOWORD(lParam);
		win->Ypos = (dword)HIWORD(lParam);
		return 0;

	case WM_CLOSE:       PostQuitMessage(0); // dit à GetMessage() de renvoyer 0
		return 0;

	case WM_SYSCOMMAND:  // évite l'économiseur d'écran
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
		/*
		case WM_CHAR:        la touche est traduite dans ce msg
		return 0;
		*/

	case WM_KEYDOWN:     inp->set_key_down((dword)wParam);
		return 0;

	case WM_KEYUP:       inp->set_key_up((dword)wParam);
		return 0;

	case WM_LBUTTONDOWN: inp->set_mouse_left_down();
		return 0;

	case WM_LBUTTONUP:   inp->set_mouse_left_up();
		return 0;

	case WM_RBUTTONDOWN: inp->set_mouse_right_down();
		return 0;

	case WM_RBUTTONUP:   inp->set_mouse_right_up();
		return 0;

	case WM_MBUTTONDOWN: inp->set_mouse_middle_down();
		return 0;

	case WM_MBUTTONUP:   inp->set_mouse_middle_up();
		return 0;

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

