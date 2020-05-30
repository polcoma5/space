#include "stdafx.h"

// Se pueden definir SHARED_HANDLERS en un proyecto ATL implementando controladores de vista previa, miniatura
// y filtro de búsqueda, y permiten compartir código de documentos con ese proyecto.
#ifndef SHARED_HANDLERS
#include "EntornVGI.h"
#endif

#include "EntornVGIDoc.h"
#include "EntornVGIView.h"
#include "visualitzacio.h"	// Include funcions de projeció i il.luminació
#include "escena.h"			// Include funcions d'objectes OpenGL
#include <Windows.h>
#include <mmsystem.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////// VGI_Logic_Controller ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor per defecte, per ara fer proves. El constructor que s'utilitzara cridara la funcio:-> VGI_Logic_Controller::getUserConfig();
VGI_Logic_Controller::VGI_Logic_Controller()
{
	// 	
}

// Ctor
VGI_Logic_Controller::VGI_Logic_Controller(int pts, int lvl, int n_col) 
{
	m_Punts = pts;	m_Nivell = lvl;		m_IsRunning = true;
	m_CameraPoint[0] 	= 	1500 / 2;		// x
	m_CameraPoint[1] 	= 	-150;			// y
	m_CameraPoint[2] 	= 	100;			// z
	
	m_ArriboFinal = 1;
	m_VelocityCol = 0.2;
	m_VelocityCamera = 1;
	m_Intents = 10;

	m_iSkin = 1;

	m_CameraIsRunning = false;
}

// Funcio de control
void VGI_Logic_Controller::control(CPunt3D* onEsta, GLfloat *onMira)
{
	if (m_IsRunning)
	{
		if (onEsta->y > 0.0) 
		{
			if (this->isCollide(onEsta, onMira)) 
			{
				m_IsRunning = true; m_CameraIsRunning = true;	
				/*	 Tornem al origen un cop xoquem	*/
				onMira[0] = this->getCameraPX();		onMira[1] = 0;		onMira[2] = this->getCameraPZ();
				onEsta->x = this->getCameraPX();	onEsta->y = this->getCameraPY();		onEsta->z = this->getCameraPZ();
				m_Intents--;
			}
		}
		
		if (this->isFinalMap(onEsta, onMira)) 
		{
			/* Incrementa el nivell */
			m_Nivell++;
			
			if (m_iSkin == 4) m_iSkin = 0;

			/*	Tornem al origen */
			onMira[0] = this->getCameraPX();		onMira[1] = 0;		onMira[2] = this->getCameraPZ();
			onEsta->x = this->getCameraPX();	onEsta->y = this->getCameraPY();		onEsta->z = this->getCameraPZ();

			// TODO: coordinar les velocitats de columna i camera
			
			m_IsRunning = true; 
			m_CameraIsRunning = true;

			switch (m_Nivell) {
			case 0:
				m_VelocityCamera += 0.3;
				m_VelocityCol += 0.01;
				break;
			case 1:
				m_VelocityCamera += 0.33;
				m_VelocityCol += 0.02;
				break;
			case 2:
				m_VelocityCamera += 0.37;
				m_VelocityCol += 0.02;
				break;
			case 3:
				m_VelocityCamera += 0.41;
				m_VelocityCol += 0.03;
				break;
			case 4:
				m_VelocityCamera += 0.45;
				m_VelocityCol += 0.03;
				break;
			case 5:
				m_VelocityCamera += 0.47;
				m_VelocityCol += 0.03;
				break;
			case 6:
				m_VelocityCamera += 0.5;
				m_VelocityCol += 0.04;
				break;
			case 7:
				m_VelocityCamera += 0.53;
				m_VelocityCol += 0.04;
				break;
			case 8:
				m_VelocityCamera += 0.57;
				m_VelocityCol += 0.05;
				break;
			case 9:
				m_VelocityCamera += 0.6;
				m_VelocityCol += 0.05;
				break;
			case 10:
				m_VelocityCamera += 0.65;
				m_VelocityCol += 0.09;
				break;

			default:
				m_VelocityCamera += 0.9;
				m_VelocityCol += 0.2;
				break;

			}
		}

		if (!m_Intents) {
			m_IsRunning = false;
			m_CameraIsRunning = false;
		}
		

		this->printPantalla(onEsta, onMira);
	}
}

// Printa els grafics del jocs. S'ha de cridar en cada frame de rellotge.
void VGI_Logic_Controller::printPantalla(CPunt3D* onEsta, GLfloat *onMira)
{
	// Funció equivalent a dibuixar escena: printa tots els objectes, nau, mapa i columnes.
	// Prèviament s'ha d'haver cridat a la funcio Objecte::move_object(x,y,z) per printar els objectes en unaltre lloc.
	
	Menu m;
	//m.print_object2(m.texturID);
	if (!m_CameraIsRunning) {
		m.print_object(m.texturID);
	}
	m_Mapa->print_object(); // a dins té la funció que printa a les columnes.
	
}

// Funció que detecta si la nau ha arribat al final del mapa.
bool VGI_Logic_Controller::isFinalMap(CPunt3D* opvN, GLfloat *n) 
{
	// Comprovem el eix Y
	if (opvN->y >= 1000) {
		Skins *m_Skins = m_Mapa->getSkins(); 
		
		int total_limits = 0;
		int total = 0;

		// Creació d'un nou mapa quan arribem al final.
		// TODO: queda per fer el canvi de colors.
		

		for (int x = 0; x < m_Mapa->getMida(); x++) {
			for (int y = 0; y < m_Mapa->getMida(); y++) {

				// Columnes que es mouen 

				int r, r1, r2, r3, r_tmp, r_off_h;
				int rand_type = rand() % 5;		// 4 tipos, 3 de moviment i alçades diferents i 1 de tamany fixe sense movilitat.+

				m_Mapa->getColumnes()[total].setType(rand_type);
				r_off_h = rand() % 15;

				switch (rand_type)
				{
				case 1:
					
					

					r_tmp = rand() % 100;

					if (r_tmp == 1 || r_tmp == 2) {
						m_Mapa->getColumnes()[total].setIsMove(true);
						m_Mapa->getColumnes()[total].setIsUp(true);

					}
					r3 = rand() % 50;
					m_Mapa->getColumnes()[total].setMaxHeigh(30);
					m_Mapa->getColumnes()[total].setSizes(m_AmpleCol, r3);
					r_off_h = rand() % 25;

					break;

				case 2:
					
					r_tmp = rand() % 100;

					if (r_tmp == 1 || r_tmp == 2 || r_tmp % 3 == 1) {
						m_Mapa->getColumnes()[total].setIsMove(true);
						m_Mapa->getColumnes()[total].setIsUp(true);

					}
					r1 = rand() % 60;
					m_Mapa->getColumnes()[total].setMaxHeigh(60);
					m_Mapa->getColumnes()[total].setSizes(m_AmpleCol, r1);
					


					r_off_h = rand() % 30;

					break;
				case 3:
					
					
					r_tmp = rand() % 100;

					if (r_tmp == 1 || r_tmp == 2) {
						m_Mapa->getColumnes()[total].setIsMove(true);
						m_Mapa->getColumnes()[total].setIsUp(true);

					}
					r2 = rand() % 100;
					m_Mapa->getColumnes()[total].setMaxHeigh(100);
					m_Mapa->getColumnes()[total].setSizes(m_AmpleCol, r2);

					r_off_h = rand() % 10;
					

					break;

				default:
					
					
					int rand_types = rand() % 10;
					if (rand_types % 2 == 0) {
						r = rand() % 20;
						m_Mapa->getColumnes()[total].setMaxHeigh(100);
						m_Mapa->getColumnes()[total].setSizes(m_AmpleCol, r);
						r_off_h = rand() % 10;
						
					}
					else {
						m_Mapa->getColumnes()[total].setIsMove(false);
						m_Mapa->getColumnes()[total].setIsUp(false);
						r = rand() % 100;
						m_Mapa->getColumnes()[total].setMaxHeigh(100);
						m_Mapa->getColumnes()[total].setSizes(m_AmpleCol, r);
						r_off_h = rand() % 100;
						
					}
					break;
				}
				
				m_Mapa->getColumnes()[total].setSkin(m_Skins[m_iSkin]);
				m_Mapa->getColumnes()[total].setPosition(x * m_AmpleCol + m_AmpleCol / 2, y * m_AmpleCol + m_AmpleCol / 2, r_off_h);
				total++;
			
			}
		}
		m_iSkin++;
		return true;
	}
	return false;
}

// Funcio que calcula si hi ha impacte entre la nau i qualsevol columna: TODO
bool VGI_Logic_Controller::isCollide(CPunt3D* onEsta, GLfloat *onMira) 
{
	float x, y;
	int i_y = (onEsta->x) / 10;
	int i_x = (onEsta->y) / 10;
	x = (onEsta->x / 10);
	y = (onEsta->y / 10);

	Columna *tmp_col = &m_Mapa->getColumnes()[i_y * m_NumCol + i_x];

	float dif =  tmp_col->getProfunditat() + tmp_col->getAlt() ;
	float dif2 = tmp_col->getProfunditat() - tmp_col->getAlt();

	if (tmp_col->getIsMove()) {
		if ((onEsta->z) < dif || (onEsta->z - tmp_col->getProfunditat()) < dif2) {
		
			
			return true;
		}
	}
	else {

		if ((onEsta->z ) < dif || (onEsta->z  - tmp_col->getProfunditat()) < dif2) {
			return true;
		}
	}

	return false;
	
}

// Funcio d'actualitzacio de les posicions de les columnes
void VGI_Logic_Controller::runColumns() 
{
	// Mou les columnes que son active. Si es isUp cap adalt (maxColHeigh) sino cap abaix.
	for (int i = 0; i < m_Mapa->getNumCol(); i++) {
		if (m_Mapa->getColumnes()[i].getIsMove()) {
			float x, y, z;
			x = m_Mapa->getColumnes()[i].getAmplada();
			y = m_Mapa->getColumnes()[i].getAlsada();
			z = m_Mapa->getColumnes()[i].getProfunditat();
			
			switch (m_Mapa->getColumnes()[i].getType()) {
				// case 1: tamany max: 20, moviment lent.
				case 1:
					if (m_Mapa->getColumnes()[i].getIsUp()) {
						if (z < m_Mapa->getColumnes()[i].getMaxHeigh()) {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol);
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol);

						}
						else {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z - m_VelocityCol);
							m_Mapa->getColumnes()[i].setIsUp(false);
						}
					}
					else {
						if (z <= 10.0) {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol);
							m_Mapa->getColumnes()[i].setIsUp(true);
						}
						else {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z - m_VelocityCol);
						}
					}
					break;
					// case 2: tamany max: 60, moviment més rapid.
				case 2:
					if (m_Mapa->getColumnes()[i].getIsUp()) {
						if (z < m_Mapa->getColumnes()[i].getMaxHeigh()) {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol);
						}
						else {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z - m_VelocityCol);
							m_Mapa->getColumnes()[i].setIsUp(false);
						}
					}
					else {
						if (z <= 10) {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol);
							m_Mapa->getColumnes()[i].setIsUp(true);
						}
						else {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z - m_VelocityCol);
						}
					}
					break;
				case 3:

					// fer dos tipos i unes pujen mes rapid amb un rand()

					if (m_Mapa->getColumnes()[i].getIsUp()) {
						if (z < m_Mapa->getColumnes()[i].getMaxHeigh()) {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol + 0.18);
						}
						else {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z -(m_VelocityCol) - 0.15);
							m_Mapa->getColumnes()[i].setIsUp(false);
						}
					}
					else {
						if (z <= 10) {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z + m_VelocityCol + 0.17);
							m_Mapa->getColumnes()[i].setIsUp(true);
						}
						else {
							m_Mapa->getColumnes()[i].setSizes(m_AmpleCol, z - (m_VelocityCol) - 0.2);
						}
					}
					break;
				default:
					break;
			}			
		}
	}
}

// Funció que mou la càmera.
void VGI_Logic_Controller::runCamera(GLfloat &x, GLfloat &y, GLfloat &z, GLfloat *f) 
{
	if (m_CameraIsRunning) {
		y += m_VelocityCamera;
		f[1] += m_VelocityCamera;
		

		/* TODO: moure la nau respecte la camera
		float x, y, z;
		x = m_Nau->getNauX();
		y = m_Nau->getNauY();
		z = m_Nau->getNauZ();

		m_Nau->setPos(x, y, z);*/
	}
}

// Funcio per captar input del teclat i tractar-ho.

bool lock = false;

void VGI_Logic_Controller::getKeyword(UINT nChar, UINT nRepCnt, GLfloat *onMira, CPunt3D *onEsta) 
{
	float x, y, z;
	x = this->getCameraPX();
	y = this->getCameraPY();
	z = this->getCameraPZ();
	
	switch (nChar)
	{
		// Tecla cursor amunt
	case VK_UP:
		if (onEsta->z < 120)
		{
			onEsta->z += 1.5;
			onMira[2] += 1.5;
		}
		break;

		// Tecla cursor avall
	case VK_DOWN:
		if (onEsta->z > 80)
		{
			onEsta->z -= 1.5;
			onMira[2] -= 1.5;
		}
		break;

		//tecla 'A' PER ANAR ENDEVANT
	case 0x41:
		onEsta->y += 2;
		onMira[1] += 2;
		break;
		
	case 0x42:
		// tecla 'B' per anar al final
		onEsta->y = 1000;
		onMira[1] = 1020;
		
		
		break;
	case 0x50:
		// tecla 'P' per poausar 
		if (!lock) {
			m_CameraIsRunning = false;
			lock = true;
		}
		else {
			m_CameraIsRunning = true;
			lock = false;
		}

		break;



		// tecla 'Z' PER ANAR ENREDERE
	case 0x5A:
		onEsta->y -= 2;
		onMira[1] -= 2;
		break;

		// Tecla cursor esquerra
	case VK_LEFT:
		if (onEsta->x > m_AmpleCol * 2)
		{
			onEsta->x -= 1.5;
			onMira[0] -= 1.5;
		}
		break;

	case VK_END:
		// acabar am el joc
		break;

		// Tecla cursor dret
	case VK_RIGHT:
		if (onEsta->x < m_AmpleCol*m_Mapa->getMida() - m_AmpleCol * 2) 
		{
			onEsta->x += 1.5;
			onMira[0] += 1.5;
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////// OBJECTES /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CLASSE MARE -> Objecte

// Funcio que incrementa les variables d'amplada, llargada i profunditat dels objectes, per posterior-ment ser pintats.
void Objecte::move_object(int x, int y, int z)
{
	m_x = x;
	m_z = y;
	m_y = z;
}

void Objecte::iluminacio()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_NORMALIZE);
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
}

// CLASSE COLUMNA

/* Abans de cridar aquesta funció, si es vol moure una columna, s'ha d'haver cridat a la funció move_object(x,y,z) abans.
Auesta s'encarrega de canviar els paràmetres que formen la geometria de la columna, de manera que ens permet moure els objectes en l'espai,
en aquest cas la columna.

*/
Columna::Columna() 
{
	Objecte(0, 0, 0);
	setRGB(1, 1, 1);
	m_Type = -1;
	m_Punts.x = 0;
	m_Punts.y = 0;
	m_Punts.z = 0;
	isUp = false;
	isMove = false;
}

// Print el objecte 3D
void Columna::print_object() 
{
	Objecte::iluminacio();
	glColor3f(m_R, m_G, m_B);
	
	glPushMatrix();
		glTranslatef(m_x, m_z, m_y);
		glScalef(1.0f, 1.0f, m_AlsadaCol);
		glutSolidCube(m_AlsadaCol);
		
	
	glPopMatrix();

	
}

// Set the skin colors to the columns depend on the type

void Columna::setSkin(Skins s) {

	switch (m_Type) {
	case 1:
		m_R = s.m_R[0];
		m_G = s.m_G[0];
		m_B = s.m_B[0];
		break;
	case 2:
		m_R = s.m_R[1];
		m_G = s.m_G[1];
		m_B = s.m_B[1];
		break;
	case 3:
		m_R = s.m_R[2];
		m_G = s.m_G[2];
		m_B = s.m_B[2];
		break;
	default:
		if (isMove) {
			m_R = 1;
			m_G = 1;
			m_B = 1;
		}
		else {
			
			m_R = s.m_R[3];
			m_G = s.m_G[3];
			m_B = s.m_B[3];
		}
		
		
		
		
		break;
	}
}

// CLASSE MAPA
Mapa::Mapa(float x,float y, float z, float mida_mapa, float ample_col) 
{
	srand(time(NULL));
	Objecte(x, y, z);
	m_CostatMapa = mida_mapa;
	m_NumCol = m_CostatMapa*m_CostatMapa;
	m_CostatCasella = ample_col;
	m_Columnes = new Columna[m_NumCol];
	m_ColumnesLimit = new Columna[m_CostatMapa * 4];
	
	int total_limits = 0;
	int total = 0;

	m_Skins = new Skins[4];


	{
		//////////////////////////////// SKIN 1 ///////////////////////////////////////////
		
		{
			// tipos columna 2 -> rosa magenta fort

			m_Skins[0].m_R[1] = 1.9f;
			m_Skins[0].m_G[1]= 1.0f;
			m_Skins[0].m_B[1] = 2.5f;

			// tipo columna 3 -> altes que pujen blau fort
			m_Skins[0].m_R[0] = 0.3f;
			m_Skins[0].m_G[0] = 1.2f;
			m_Skins[0].m_B[0] = 2.0f;

			// marro
			m_Skins[0].m_R[3] = 1.2f;
			m_Skins[0].m_G[3] = 1.7f;
			m_Skins[0].m_B[3] = 1.1f;

			// tipo columna 1 -> blau verdós
			m_Skins[0].m_R[2] = 0.7f;
			m_Skins[0].m_G[2] = 1.0f;
			m_Skins[0].m_B[2] = 1.9f;
		}

		//////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////// SKIN 2 //////////////////////////////////////

		{
			// skin 2, tipo columna 1 -> marró granate

			m_Skins[1].m_R[2] = 1.5f;
			m_Skins[1].m_G[2] = 1.2f;
			m_Skins[1].m_B[2] = 1.3f;
			/*
			m_Skins[1].m_R[1] = 2.0f;
			m_Skins[1].m_G[1] = 1.5f;
			m_Skins[1].m_B[1] = 0.2f;
			*/

			// tipo col 3 -> verd

			m_Skins[1].m_R[1] = 3.7f;
			m_Skins[1].m_G[1] = 2.3f;
			m_Skins[1].m_B[1] = 0.4f;

			 //tipo col 2 -> marró granate
			 
			m_Skins[1].m_R[3] = 1;
			m_Skins[1].m_G[3] = 1;
			m_Skins[1].m_B[3] = 1;
			
			
	
			// tronja
			m_Skins[1].m_R[0] = 2.0f;
			m_Skins[1].m_G[0] = 1.5f;
			m_Skins[1].m_B[0] = 0.2f;

			


		}
	
		///////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////// SKIN 3 ////////////////////////////////////////
		{
			


			// tipo columna 3 -> altes que pujen blau fort
			m_Skins[2].m_R[0] = 1.5f;
			m_Skins[2].m_G[0] = 1.0f;
			m_Skins[2].m_B[0] = 3.5f;

			// marro
			m_Skins[2].m_R[2] = 1;
			m_Skins[2].m_G[2] = 1;
			m_Skins[2].m_B[2] = 1;

			// tipo columna 1 -> blau verdós
			m_Skins[2].m_R[3] = 1;
			m_Skins[2].m_G[3] = 1;
			m_Skins[2].m_B[3] = 1;
		
			

			m_Skins[2].m_R[1] = 2.0f;
			m_Skins[2].m_G[1] = 1.5f;
			m_Skins[2].m_B[1] = 0.5f;

			
		}
		{
			m_Skins[3].m_R[0] = 1;
			m_Skins[3].m_G[0] = 1;
			m_Skins[3].m_B[0] = 1;

			m_Skins[3].m_R[1] = 2.0f;
			m_Skins[3].m_G[1] = 1.0f;
			m_Skins[3].m_B[1] = 0.1f;

			m_Skins[3].m_R[2] = 1;
			m_Skins[3].m_G[2] = 1;
			m_Skins[3].m_B[2] = 1;
			
			m_Skins[3].m_R[3] = 0.3f;
			m_Skins[3].m_G[3] = 1.9f;
			m_Skins[3].m_B[3] = 2.0f;
		}

		/*
		m_Skins[0].skin = "blau";
		m_Skins[0].m_R = 0.3f;
		m_Skins[0].m_G = 1.9f;
		m_Skins[0].m_B = 2.0f;

		m_Skins[1].skin = "groc/tronja";
		m_Skins[1].m_R = 2.0f;
		m_Skins[1].m_G = 1.5f;
		m_Skins[1].m_B = 0.2f;

		m_Skins[2].skin = "vermell fosc";
		m_Skins[2].m_R = 1.0f;
		m_Skins[2].m_G = 0.5f;
		m_Skins[2].m_B = 0.2f;

		m_Skins[3].skin = "lila suau";
		m_Skins[3].m_R = 1.5f;
		m_Skins[3].m_G = 1.0f;
		m_Skins[3].m_B = 3.5f;

		m_Skins[4].skin = "verd lima";
		m_Skins[4].m_R = 0.3f;
		m_Skins[4].m_G = 1.9f;
		m_Skins[4].m_B = 2.0f;

		m_Skins[5].skin = "blau molt clar";
		m_Skins[5].m_R = 1.2f;
		m_Skins[5].m_G = 2.6f;
		m_Skins[5].m_B = 2.6f;

		m_Skins[6].skin = "lila";
		m_Skins[6].m_R = 1.5f;
		m_Skins[6].m_G = 1.0f;
		m_Skins[6].m_B = 2.5f;

		m_Skins[7].skin = "marró granate";
		m_Skins[7].m_R = 1;
		m_Skins[7].m_G = 1;
		m_Skins[7].m_B = 1;

		m_Skins[8].skin = "blau verdós";
		m_Skins[8].m_R = 0.5f;
		m_Skins[8].m_G = 1.0f;
		m_Skins[8].m_B = 1.5f;

		m_Skins[9].skin = "rosa magenta fort";
		m_Skins[9].m_R = 1;
		m_Skins[9].m_G = 0;
		m_Skins[9].m_B = 1;*/
	}
	
	for (int x = 0; x < m_CostatMapa; x++) {
		for (int y = 0; y < m_CostatMapa; y++) {

			int r, r1, r2, r3,r_tmp,r_off_h;
			int rand_type = rand() % 4;		

			// 4 tipos, 3 de moviment i alçades diferents i 1 de tamany fixe sense movilitat.+
			// r_tmp serveix per determinar quan una columna es mourà
			// r_off_h serveix per determinar on comença una columna en l'eix z

			m_Columnes[total].setType(rand_type);
			r_off_h = rand() % 15;

			switch (rand_type)
			{
			case 1:
			{
				r_tmp = rand() % 100;

				if (r_tmp == 1 || r_tmp == 2) {
					m_Columnes[total].setIsMove(true);
					m_Columnes[total].setIsUp(true);
				}
				r3 = rand() % 50;
				m_Columnes[total].setMaxHeigh(30);
				m_Columnes[total].setSizes(ample_col, r3);

				r_off_h = rand() % 25;
				
				break;
			}
				

			case 2:
			{
				r_tmp = rand() % 100;

				if (r_tmp == 1 || r_tmp == 2 || r_tmp % 3 == 1) {
					m_Columnes[total].setIsMove(true);
					m_Columnes[total].setIsUp(true);
				}

				r1 = rand() % 60;
				m_Columnes[total].setMaxHeigh(60);
				m_Columnes[total].setSizes(ample_col, r1);

				r_off_h = rand() % 30;
				
				break;
			}
				

			case 3:
			{
				r_tmp = rand() % 60;
				m_Columnes[total].setIsUp(true);

				if (r_tmp == 1 || r_tmp == 2) {
					m_Columnes[total].setIsMove(true);
					r_off_h = rand() % 10;

				}
				else if (r_tmp == 3 || r_tmp == 5) {
					m_Columnes[total].setIsMove(true);
					m_Columnes[total].setIsUp(false);
					r_off_h = rand() % 100;
				}

				r2 = rand() % 100;
				m_Columnes[total].setMaxHeigh(100);
				m_Columnes[total].setSizes(ample_col, r2);
				
				break;
			}
				
			
			default:
			{
				int rand_types = rand() % 10;
				m_Columnes[total].setMaxHeigh(100);

				if (rand_types % 2 == 0) {
					r = rand() % 20;
					r_off_h = rand() % 10;

				}
				else {
					m_Columnes[total].setIsMove(false);
					m_Columnes[total].setIsUp(false);
					r = rand() % 100;

					r_off_h = rand() % 100;
				}
				m_Columnes[total].setSizes(ample_col, r);
			}
				

				// Columnes color blau.
				//m_Columnes[total].setRGB(0.3f, 1.9f, 2.0f);
				
				// groc/trojna
				//m_Columnes[total].setRGB(2.0f, 1.5f, 0.2f);

				// vermell fosc 
				//m_Columnes[total].setRGB(1.0f, 0.5f, 0.2f);

				// lila suau
				//m_Columnes[total].setRGB(1.5f, 1.0f, 3.5f);

				//verd lima
				//m_Columnes[total].setRGB(1.2f,2.6f,0.6f);

				// blau molt clar
				//m_Columnes[total].setRGB(1.2f, 2.6f, 2.6f);

				// lila
				//m_Columnes[total].setRGB(1.5f, 1.0f, 2.5f);

				// marró granate
				//m_Columnes[total].setRGB(1,1, 1);

				// blau verdós
				//m_Columnes[total].setRGB(0.5f, 1.0f, 1.5f);

				// rosa magenta fort
				//m_Columnes[total].setRGB(1, 0, 1);

				//  granate obscur
				//m_Columnes[total].setRGB(1.2,1.2,1.54);

				//m_Columnes[total].setRGB(2.5,2,1);
				
				break;
			}
			m_Columnes[total].setSkin(m_Skins[0]);
			m_Columnes[total].setPunts(x * m_CostatCasella, y * m_CostatCasella, r_off_h);
			m_Columnes[total].setPosition(x * m_CostatCasella + m_CostatCasella / 2, y * m_CostatCasella + m_CostatCasella / 2, r_off_h);
			
			// Primer nivell amb columnes fixes
			//m_Columnes[total].setIsMove(false);

			//	Columnes limitadores del mapa
			/*
			if (x == 0 ||  y == mida_mapa - 1 || x == mida_mapa - 1) 
			{
				m_ColumnesLimit[total_limits].setSizes(ample_col, 1000);
				m_ColumnesLimit[total_limits].setPosition(x * m_CostatCasella + m_CostatCasella / 2, y * m_CostatCasella + m_CostatCasella / 2, 100);
				m_ColumnesLimit[total_limits].setRGB(1, 0, 1);
				m_Columnes[total].setIsMove(false);
				total_limits++;
				// Les columnes que en teoria es mouen pero coincideixen amb el limit, no es mouen.
			}*/
			total++;
		}
	}
}

void Mapa::print_object() 
{
	Objecte::iluminacio();
	
	glPushMatrix();
	glBegin(GL_QUADS);

		glColor3f(0.4f, 1.0f, 1.0f);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(m_CostatCasella * m_CostatMapa, 0.0f);
		glVertex2f(m_CostatCasella * m_CostatMapa, m_CostatCasella * m_CostatMapa);
		glVertex2f(0.0f, m_CostatCasella * m_CostatMapa);

	glEnd();
	glPopMatrix();
	
	
	for (int i = 0; i < m_NumCol; i++) {
		m_Columnes[i].print_object();
	}
	for (int x = 0; x < m_CostatMapa * 3; x++) {
		m_ColumnesLimit[x].print_object();
	}
	
}

void Nau::print_object() {
	


	switch (m_Objecte) {
	case OBJOBJ:
		Objecte::iluminacio();
		//do {
			//glPushMatrix();
		glRotatef(180.0f, 0.0f, 30.0f, 30.0f);
		glScalef(50.0f, 50.0f, 50.0f);
		//glScalef(10.0f, 10.0f, 10.0f);
		glTranslatef(m_x, m_z, m_y);
		glColor3f(0.5f, 1.0f, 0.5f);
		//SeleccionaColor(ref_mat, sw_mat, col_object);
		//glVertex2f(0.0f, 0.0f);
		//glVertex2f(m_AmpleCol * m_mida_mapa, 0.0f);
		//glVertex2f(m_AmpleCol * m_mida_mapa, m_AmpleCol * m_mida_mapa);
		//glVertex2f(0.0f, m_AmpleCol * m_mida_mapa);
		//n.tie();
		selectobj();
		//glPopMatrix();
		//glPushMatrix();
		//posX = posX - 5;
		//glPopMatrix();
	//} while (posX > -1000);
		glPopMatrix();
		break;
	}

}

void Nau::selectobj() 
{

	if (OBJOBJ) {
		glCallList(OBJECTEOBJT);
	}
}

void Nau::tie() {

	glPushMatrix();
	Nau n;
	Objecte o;
	//glTranslatef(o.m_AmpladaObj + 1, o.m_AlsadaObj + 30.5, o.m_ProfunditatObj + 100);
	n.Motor();
	n.Alas();
	n.Canon();
	n.Cuerpo();
	n.Cabina();
	glPopMatrix();

};

void Nau::Alas()
{
	// Ala 1 Parte central

	//Parte Exterior
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslated(25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(10.0, 10.0, 5.0);
	//gluCylinder(AlaparteEx, 5.0f, 5.0f, 0.5f, 6, 1);
	gluCilindre(5.0f, 5.0f, 0.5f, 6, 1);
	glPopMatrix();

	//Tapas
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslated(25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(10.0, 10.0, 5.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslated(27.75f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(10.0, 10.0, 5.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	//Parte Interior
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(2.0, 2.0, 2.0);
	//gluCylinder(AlaparteIn, 5.0f, 5.0f, 2.0f, 6, 1);
	gluCilindre(5.0f, 5.0f, 2.0f, 6, 1);
	glPopMatrix();

	//Tapas
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(2.0, 2.0, 2.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(29.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(2.0, 2.0, 2.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	//Decoracion Exterior
	int j;
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(26.5f, 0.0f, 0.0f);
	glScalef(15.75f, 13.75f, 13.75f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(30, 0, 0, 1);
	for (j = 0; j < 3; j = j + 1)
	{
		glRotatef(60, 0, 0, 1);
		glPushMatrix();
		glScalef(12.7f * 0.583f, 0.1f, 0.3f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}
	glPopMatrix();

	int u;
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	for (u = 0; u < 6; u = u + 1)
	{
		glRotated(60, 1.0, 0.0, 0.0);
		glPushMatrix();
		glTranslated(26.5f, 0, 43.25);
		glRotated(90, 0.0, 0.0, 1.0);
		glScalef(10 * 5.155f, 5.0f, 3.0f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}
	glPopMatrix();

	//Ala 2 Parte central 

	//Parte Exterior
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslated(-27.75f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(10.0, 10.0, 5.0);
	//gluCylinder(AlaparteEx, 5.0f, 5.0f, 0.5f, 6, 1);
	gluCilindre(5.0f, 5.0f, 0.5f, 6, 1);
	glPopMatrix();

	//Tapas
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslated(-27.75f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(10.0, 10.0, 5.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTranslated(-25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(10.0, 10.0, 5.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	//Parte Interior
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(-29.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(2.0, 2.0, 2.0);
	gluCilindre(5.0f, 5.0f, 2.0f, 6, 1);
	glPopMatrix();

	//Tapas
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(-29.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(2.0, 2.0, 2.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(-25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(2.0, 2.0, 2.0);
	gluDisc(0.0f, 5.0f, 6, 1);
	glPopMatrix();

	//Decoracion Exterior
	int w;
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glTranslated(-26.5f, 0.0f, 0.0f);
	glScalef(15.75f, 13.75f, 13.75f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(30, 0, 0, 1);
	for (w = 0; w < 3; w = w + 1)
	{
		glRotatef(60, 0, 0, 1);
		glPushMatrix();
		glScalef(12.7f * 0.583f, 0.1f, 0.3f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}
	glPopMatrix();


	int h;
	glPushMatrix();
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	for (h = 0; h < 6; h = h + 1)
	{
		glRotated(60, 1.0, 0.0, 0.0);
		glPushMatrix();
		glTranslated(-26.5f, 0, 43.25);
		glRotated(90, 0.0, 0.0, 1.0);
		glScalef(10 * 5.155f, 5.0f, 3.0f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}
	glPopMatrix();
};

void Nau::Motor()
{
	//Helices Motor
	int j;
	glPushMatrix();
	glColor4f(0.58f, 0.58f, 0.58f, 0.0f);
	glTranslated(0.0f, -18.0f, 0.0f);
	glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glRotatef(45, 0, 0, 1);
	for (j = 0; j < 6; j = j + 1)
	{
		glRotatef(30, 0, 0, 1);
		glPushMatrix();
		glScalef(12 * 0.583f, 0.1f, 0.3f);
		glutSolidCube(1.0f);
		glPopMatrix();
	}
	glPopMatrix();

	//Soporte Motor
	glPushMatrix();
	glColor4f(0.28f, 0.28f, 0.28f, 0.0f);
	glTranslated(0.0f, -18.0f, 0.0f);
	glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(1.0, 5.0, 20, 20);
	glPopMatrix();
};

void Nau::Canon()
{

	//Cañones

	// Salida cañon 1
	glPushMatrix();
	glColor4f(0.28f, 0.28f, 0.28f, 0.0f);
	glTranslated(5.0f, 8.0f, -10.0f);
	glRotated(-90.0f, 1.0f, 0.0f, 0.0f);
	//gluCylinder(salidacañon1, 1.0f, 0.5f, 5.0f, 60, 1);
	gluCilindre(1.0f, 0.5f, 5.0f, 60, 1);
	glPopMatrix();

	// Salida cañon 2
	glPushMatrix();
	glColor4f(0.28f, 0.28f, 0.28f, 0.0f);
	glTranslated(-5.0f, 8.0f, -10.0f);
	glRotated(-90.0f, 1.0f, 0.0f, 0.0f);
	//gluCylinder(salidacañon2, 1.0f, 0.5f, 5.0f, 60, 1);
	gluCilindre(1.0f, 0.5f, 5.0f, 60, 1);
	glPopMatrix();

	// Cañon 1
	glPushMatrix();
	glColor4f(0.58f, 0.58f, 0.58f, 0.0f);
	glTranslated(5.0f, 10.0f, -10.0f);
	glRotated(-90.0f, 1.0f, 0.0f, 0.0f);
	//gluCylinder(cañon1, 0.35f, 0.35f, 5.0f, 80, 1);
	gluCilindre(0.35f, 0.35f, 5.0f, 80, 1);
	glPopMatrix();

	//Cañon 2
	glPushMatrix();
	glColor4f(0.58f, 0.58f, 0.58f, 0.0f);
	glTranslated(-5.0f, 10.0f, -10.0f);
	glRotated(-90.0f, 1.0f, 0.0f, 0.0f);
	//gluCylinder(cañon2, 0.35f, 0.35f, 5.0f, 80, 1);
	gluCilindre(0.35f, 0.35f, 5.0f, 80, 1);
	glPopMatrix();
}

void Nau::Cuerpo()
{
	//Sujeccion de las Alas

	//Lado2
	glPushMatrix();
	glColor4f(0.16f, 0.16f, 0.16f, 1.0f);
	glRotated(-90.0f, 0.0, 1.0, 0.0);
	glRotated(90.0f, 0.0, 0.0, 1.0);
	glScaled(2.0f, 2.0f, 2.5f);
	gluCilindre(4, 2, 10.25f, 40, 1);
	glPopMatrix();

	//X2
	glPushMatrix();
	glColor4f(0.16f, 0.16f, 0.16f, 1.0f);
	glTranslated(-25.0f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(1.0, 5.0, 20, 20);
	glPopMatrix();

	//Lado 1
	glPushMatrix();
	glColor4f(0.16f, 0.16f, 0.16f, 1.0f);
	glRotated(90.0f, 0.0, 1.0, 0.0);
	glRotated(90.0f, 0.0, 0.0, 1.0);
	glScaled(2.0f, 2.0f, 2.5f);
	gluCilindre(4, 2, 10.25f, 40, 1);
	glPopMatrix();

	//X1
	glPushMatrix();
	glColor4f(0.16f, 0.16f, 0.16f, 1.0f);
	glTranslated(25.25f, 0.0f, 0.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidTorus(1.0, 5.0, 20, 20);
	glPopMatrix();

	//Cuerpo
	glPushMatrix();
	glColor4f(0.16f, 0.16f, 0.16f, 1.0f);
	glScaled(1.75, 1.75, 1.5);
	//glutSolidSphere(10.0f, 80, 80);
	gluEsfera(10.0f, 80, 80);
	glPopMatrix();
}

void Nau::Cabina()
{
	//Tapa Cabina
	glPushMatrix();
	glEnable(GL_BLEND);
	glColor4f(1.0f, 0.058f, 0.058f, 0.5f);
	glTranslated(0.0f, 19.45f, 0.0f);
	glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(1.75f, 1.75f, 1.75f);
	gluDisc(0.0f, 1.5f, 8, 1);
	glPopMatrix();

	//Cristal Cabina
	glPushMatrix();
	glColor4f(1.0f, 0.058f, 0.058f, 0.5f);
	glTranslated(0.0f, 19.45f, 0.0f);
	glRotated(-90.0f, 0.0f, 0.0f, 1.0f);
	glRotated(90.0f, 0.0f, 1.0f, 0.0f);
	glScaled(1.75f, 1.75f, 1.75f);
	gluCilindre(1.5, 4.5f, 2.0f, 8, 1);
	glPopMatrix();
}
void Menu::Init() {
	loadIMA("./textures/craters.bmp", 1);
	loadIMA("./textures/maxresdefault.jpg", 2);

}

void Menu::print_object(GLuint VTextu[NUM_MAX_TEXTURES]) {
	Mapa m;
	Objecte o;
	Objecte::iluminacio();
	Init();
	//m_opciomenu = true;
	float posX = m.m_x + 500;
	float posY = o.m_y - 100;
	float posZ = m.m_z - 100;
	VGI_Logic_Controller v;
	glPushMatrix();
	glTranslatef(-261, -150, -530);
	glRotatef(180.0, 0, 0, posZ);
	glRotatef(180.0f, 0, posY, posZ);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, VTextu[1]);
	glEnable(GL_TEXTURE_2D);
	
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2f(0, 0);
	glTexCoord2i(0, 1); glVertex2f(0.0f, 1150);
	glTexCoord2i(1, 1); glVertex2f(1522, 1150);
	glTexCoord2i(1, 0); glVertex2f(1522, 0.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glPopMatrix();
}

void Menu::print_object2(GLuint VTextu[NUM_MAX_TEXTURES]) {
	Mapa m;
	Objecte o;
	Init();
	Objecte::iluminacio();
	float posX = m.m_x + 500;
	float posY = o.m_y - 100;
	float posZ = m.m_z - 100;
	VGI_Logic_Controller v;
	glPushMatrix();

	glTranslatef(-500, 1000, -100);
	glRotatef(180.0, 0, 0, posZ);
	glRotatef(180.0f, 0, posY, posZ);
	//glRotatef(180.0f, 0, posY, posZ);
	glEnable(GL_TEXTURE_2D);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//gluLookAt(0, 0, 45, 0, 0, 90, 0, 1, 0);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2f(0, 0);
	glTexCoord2i(0, 1); glVertex2f(0.0f, 1500);
	glTexCoord2i(1, 1); glVertex2f(2000, 1500);
	glTexCoord2i(1, 0); glVertex2f(2000, 0.0f);

	//TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glEnd();

	/*glColor3f(0.4f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(m_AmpleCol * m_mida_mapa, 0.0f);
	glVertex2f(m_AmpleCol * m_mida_mapa, m_AmpleCol * m_mida_mapa);
	glVertex2f(0.0f, m_AmpleCol * m_mida_mapa);*/
	glDisable(GL_TEXTURE_2D);
	glEnd();
	glPopMatrix();

}