#include <iostream>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <unistd.h>
 
sf::Music music;

using namespace std;

static float xAngle = 0.0, yAngle = 0.0;        // Rotação da luz branca
static long font = (long)GLUT_BITMAP_8_BY_13;   // Fonte usada para imprimir na tela
static char theStringBuffer[10];                // String buffer
static float xMouse = 250, yMouse = 250;        // (x,y) do ponteiro do mouse
static float larguraJanela, alturaJanela;       // (w,h) da janela
static bool isLightingOn = false;               // O sistema de iluminação está ligado?
static float anguloEsferaY = 0;                 // Rotação da esfera em torno do eixo y
static float r = 1;
static float translaRot = 0.0;
static int esferaLados = 200;                   // Quantas subdivisões latitudinais/longitudinais da esfera
static bool localViewer = false;
static int marsTexture, sunTexture, mercuryTexture, venusTexture, earthTexture, jupiterTexture, saturnTexture, uranusTexture, neptuneTexture, ringsTexture, ringsTexture2, ringsTexture3, ringsTexture4;
static bool usarTextura = true;
static bool mudarCamera = false;
static bool light0Ligada = 1;                   // Luz branca ligada
static bool light1Ligada = 1;                   // Luz verde ligada
static float d = 1.0;                           // Intensidade da cor difusa da luz branca
static float e = 1.0;                           // Intensidade da cor especular da luz branca
static float m = 0.2;                           // Intensidade da luz ambiente global
static float p = 1.0;                           // A luz branca é posicional?
static float s = 50.0;                          // Expoente especular do material (shininess)
float matShine[] = { s };                       // expoente especular (shininess)

// Escreve uma cadeia de caracteres
void escreveTextoNaTela(void *font, char *string)
{
    char *c;
    for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

// Converte um número decimal em string
void floatParaString(char * destStr, int precision, float val)
{
    sprintf(destStr,"%f",val);
    destStr[precision] = '\0';
}

// Escreve as informações variáveis na tela
void informacoesTela(void)
{
    glDisable(GL_LIGHTING); // Desabilita iluminação
    glColor3f(.85f, .85f, .85f);
    glRasterPos3f(-1.0, 1.10, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Camera (1): lateral/vertical");
    glRasterPos3f(-1.0, 1.05, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Desligar fonte de luz (w): ");
    escreveTextoNaTela((void*)font, (char*)(light0Ligada ? "ligada" : "desligada"));
    glRasterPos3f(-1.0, 1.00, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Iluminacao (l): ");
    escreveTextoNaTela((void*)font, (char*)(isLightingOn ? "ligada" : "desligada"));
    floatParaString(theStringBuffer, 4, m);
    glRasterPos3f(-1.0, 0.95, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Aperte as setas para rotacionar a fonte de luz");
    glRasterPos3f(-1.0, 0.90, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Luz ambiente global: ");
    glRasterPos3f(-1.0, 0.85, -2.0);
    escreveTextoNaTela((void*)font, (char*)"  - Intensidade (m/M): ");
    escreveTextoNaTela((void*)font, theStringBuffer);
    glRasterPos3f(-1.0, 0.75, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Luz branca: ");
    glRasterPos3f(-1.0, 0.70, -2.0);
    escreveTextoNaTela((void*)font, (char*)"  - Intensidade difusa (d/D): ");
    floatParaString(theStringBuffer, 4, d);
    escreveTextoNaTela((void*)font, theStringBuffer);
    glRasterPos3f(-1.0, 0.65, -2.0);
    escreveTextoNaTela((void*)font, (char*)"  - Intensidade especular (e/E): ");
    floatParaString(theStringBuffer, 4, e);
    escreveTextoNaTela((void*)font, theStringBuffer);
    glRasterPos3f(-1.0, 0.55, -2.0);
    escreveTextoNaTela((void*)font, (char*)"Material: ");
    glRasterPos3f(-1.0, 0.50, -2.0);
    escreveTextoNaTela((void*)font, (char*)"  - Expoente especular (s/S): ");
    floatParaString(theStringBuffer, 5, s);
    escreveTextoNaTela((void*)font, theStringBuffer);
}

// Callback para setas do teclado
void changeCamera(bool change)
{
    if(!change) 
    {
    // Posiciona a câmera de acordo com posição x,y do mouse na janela
    gluLookAt(5 * (xMouse - larguraJanela / 2) / (larguraJanela / 16), 
                -1 * (yMouse - alturaJanela / 2) / (alturaJanela / 16) + 3, 
                5, 0, 0, 0, 0, 1, 0);
    } 
    else 
    {
    // Posiciona a câmera de acordo com posição x,y do mouse na janela
    gluLookAt(5 * (xMouse - larguraJanela / 2) / (larguraJanela / 16), 
                -1 * (yMouse - alturaJanela / 2) / (alturaJanela / 150) + 3, 
                5, 0, 0, 0, 0, 1, 0);
    }
}

// Configuração inicial do OpenGL e GLUT
void setup(void)
{
    glClearColor(0,0,0, 0.0);                       // fundo preto
    glEnable(GL_DEPTH_TEST);                        // Ativa teste Z

    // Propriedades do material da esfera
    float matAmbAndDif[] = {1.0, 1.0, 1.0, 1.0};    // cor ambiente e difusa: branca
    float matSpec[] = { 1.0, 1.0, 1,0, 1.0 };       // cor especular: branca

    // Definindo as propriedades do material
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Carrega textura sol    
    sunTexture = SOIL_load_OGL_texture
    (
        "sun.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
    );
	// Carrega a textura marte
    marsTexture = SOIL_load_OGL_texture
	(
		"mars.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura mercurio
    mercuryTexture = SOIL_load_OGL_texture
	(
		"mercury.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura venus
    venusTexture = SOIL_load_OGL_texture
	(
		"venus.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura terra
    earthTexture = SOIL_load_OGL_texture
	(
		"earth.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura jupiter
    jupiterTexture = SOIL_load_OGL_texture
	(
		"jupiter.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura saturno
    saturnTexture = SOIL_load_OGL_texture
	(
		"saturn.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura urano
    uranusTexture = SOIL_load_OGL_texture
	(
		"uranus.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura netuno
    neptuneTexture = SOIL_load_OGL_texture
	(
		"neptune.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
    //Carrega textura dos aneis de saturno
    ringsTexture = SOIL_load_OGL_texture
	(
		"rings.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);  
    //Carrega textura dos aneis de jupter
    ringsTexture2 = SOIL_load_OGL_texture
	(
		"rings.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);  

    //Carrega textura dos aneis de uranus
    ringsTexture3 = SOIL_load_OGL_texture
	(
		"Uranus_rings.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);  

    ringsTexture4 = SOIL_load_OGL_texture
	(
		"neptunering.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);  

    if(marsTexture == 0 || sunTexture == 0 || ringsTexture4 == 0 ||mercuryTexture == 0 || ringsTexture3 == 0|| venusTexture == 0 || earthTexture == 0 || ringsTexture2 == 0 || jupiterTexture == 0 || saturnTexture == 0 || uranusTexture == 0 || neptuneTexture == 0 || ringsTexture == 0)
    {
        cout << "Problema ao carregar textura: " << SOIL_last_result() << endl;
    }

    // Não mostrar faces do lado de dentro
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Esconder o ponteiro do mouse quando dentro da janela
    glutSetCursor(GLUT_CURSOR_NONE);
}


void solidRing(float radius,float otherradius, int stacks, int columns)
{

     // cria uma quádrica
    GLUquadric* quadObj = gluNewQuadric();
    // estilo preenchido
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    // chama 01 glNormal para cada vértice.. poderia ser
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    // chama 01 glTexCoord por vértice
    gluQuadricTexture(quadObj, GL_TRUE);
    // cria os vértices de um anel
    glutSolidTorus(radius, otherradius , stacks, columns);
    // limpa as variáveis que a GLU usou para criar a esfera
    gluDeleteQuadric(quadObj);
}

// Desenha uma esfera na origem, com certo raio e subdivisões
// latitudinais e longitudinais
void solidSphere(float radius, int stacks, int columns)
{
    // cria uma quádrica
    GLUquadric* quadObj = gluNewQuadric();
    // estilo preenchido
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    // chama 01 glNormal para cada vértice
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    // chama 01 glTexCoord por vértice
    gluQuadricTexture(quadObj, GL_TRUE);
    // cria os vértices de uma esfera
    gluSphere(quadObj, radius, stacks, columns);
    // limpa as variáveis que a GLU usou para criar a esfera
    gluDeleteQuadric(quadObj);
}

// Callback de desenho
void desenhaCena()
{
    // Propriedades das fontes de luz
    float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
    float lightDif0[] = { d, d, d, 1.0 };
    float lightSpec0[] = { e, e, e, 1.0 };
    float lightPos0[] = { 0.0, 0.0, 3.0, p };
    float globAmb[] = { m, m, m, 1.0 };

    // Propriedades da fonte de luz LIGHT0
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec0);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);        // Luz ambiente global
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);// Enable local viewpoint

    // Ativa as fontes de luz (branca e verde)
    if (light0Ligada) 
    {
        glEnable(GL_LIGHT0);
    } 
    else 
    {
        glDisable(GL_LIGHT0);
    }

    // Limpa a tela e o z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    informacoesTela();

    // Posiciona a câmera de acordo com posição x,y do mouse na janela
    changeCamera(mudarCamera);

    // Desabilita iluminação para desenhar as esferas que representam as luzes
    glDisable(GL_LIGHTING);

    // Light0 e esfera indicativa (ou seta)
    glPushMatrix();
    glRotatef(xAngle, 1.0, 0.0, 0.0); // Rotação no eixo x
    glRotatef(yAngle, 0.0, 1.0, 0.0); // Rotação no eixo y
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
    glColor3f(d, d, d);

    if (light0Ligada)
    {
        if (p) 
        {
            glutWireSphere(0.05, 8, 8); // Esfera indicativa
        }
        else // Seta apontando na direção da fonte de luz direcional
        {
            glLineWidth(3.0);
            glBegin(GL_LINES);
            glVertex3f(0.0, 0.0, 0.25);
            glVertex3f(0.0, 0.0, -0.25);
            glVertex3f(0.0, 0.0, -0.25);
            glVertex3f(0.05, 0.0, -0.2);
            glVertex3f(0.0, 0.0, -0.25);
            glVertex3f(-0.05, 0.0, -0.2);
            glEnd();
            glLineWidth(1.0);
        }
    }
    glPopMatrix();

    // Light1 e sua esfera indicativa
    glPushMatrix();
    glColor3f(0.0, 1.0, 0.0);
    glPopMatrix();
    if (isLightingOn) 
    {
        glEnable(GL_LIGHTING);
    }

    

    // Define (atualiza) o valor do expoente de especularidade
    matShine[0] = s;
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    glColor3f(1, 1, 1);

    //marte
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, marsTexture);
    }
    glPushMatrix();
	glTranslatef((14* cos(2*3.14*r/1.8)),0, (9*sin(2*3.14*r/1.8)));
    glRotatef(anguloEsferaY, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.4, esferaLados, esferaLados);
    glPopMatrix();
    
    //urano
    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, uranusTexture);
    }
    glPushMatrix();
	glTranslatef((23* cos(2*3.14*r/42)),0, (15*sin(2*3.14*r/42)));	
    glRotatef(anguloEsferaY, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.3, esferaLados, esferaLados);   
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //neptune
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, neptuneTexture);
    }
    glPushMatrix();
	glTranslatef((26* cos(2*3.14*r/100)),0, (17*sin(2*3.14*r/100)));
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.3, esferaLados, esferaLados);    
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
 
    //sun
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
    }
    glPushMatrix();
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(2.5, esferaLados, esferaLados);
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //mercury
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, mercuryTexture);
    }
    glPushMatrix();
	glTranslatef((6* cos(2*3.14*r*1.67)),0, (3*sin(2*3.14*r*1.67)));
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.25f, esferaLados, esferaLados);
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //venus
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, venusTexture);
    }
    glPushMatrix();
	glTranslatef((8* cos(2*3.14*r*1.33)),0, (5*sin(2*3.14*r*1.33)));
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.5f, esferaLados, esferaLados);
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //terra
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, earthTexture);
    }
    glPushMatrix();
	glTranslatef((11* cos(2*3.14*r)),0, (7*sin(2*3.14*r)));
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.5f, esferaLados, esferaLados);
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //jupiter
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, jupiterTexture);
    }
    glPushMatrix();
	glTranslatef((17* cos(2 * 3.14 * r / 8)), 0, (11 * sin(2 * 3.14 * r/8)));
    glRotatef(anguloEsferaY * 2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.4, esferaLados, esferaLados);
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //anel de Jupiter
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ringsTexture2);
    }
    glPushMatrix();
	glTranslatef((17* cos(2 * 3.14 * r / 8)), 0, (11 * sin(2 * 3.14 * r/8)));
    glRotatef(anguloEsferaY * 2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidRing(0.02, 0.9, esferaLados, esferaLados);
    solidRing(0.01, 0.8, esferaLados, esferaLados);
    solidRing(0.01, 0.7, esferaLados, esferaLados);
    solidRing(0.02, 0.6, esferaLados, esferaLados);
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //anel de neturno
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ringsTexture4);
    }
    glPushMatrix();
	glTranslatef((26* cos(2*3.14*r/100)),0, (17*sin(2*3.14*r/100)));
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidRing(0.005, 0.5, esferaLados, esferaLados);
    solidRing(0.01, 0.6, esferaLados, esferaLados); 
    solidRing(0.005, 0.7, esferaLados, esferaLados); 
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //saturn
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, saturnTexture);
    }
    glPushMatrix();
	glTranslatef((20* cos(2 * 3.14 * r / 12)), 0, (13 * sin(2 * 3.14 * r / 12)));
    glRotatef(anguloEsferaY * 2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidSphere(0.7, esferaLados, esferaLados);     
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    //anel de uranio
    if (usarTextura) {
        glDisable(GL_TEXTURE_2D);
    }
        // Desenha a esfera grande e bem arredondada
    if (usarTextura) 
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ringsTexture3);
    }
    glPushMatrix();
	glTranslatef((23* cos(2*3.14*r/42)),0, (15*sin(2*3.14*r/42)));	
    glRotatef(anguloEsferaY, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidRing(0.005, 0.5 , 20, 20);   
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }


    //anel de saturno 
    if (usarTextura) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ringsTexture);
    }
    glPushMatrix();
	glTranslatef((20* cos(2*3.14*r/12)),0, (13*sin(2*3.14*r/12)));
    glRotatef(anguloEsferaY*2, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    solidRing(0.05 , 1.3 ,  esferaLados, esferaLados);  
    solidRing(0.05 , 1.2 ,  esferaLados, esferaLados);  
    solidRing(0.05 , 1.5 ,  esferaLados, esferaLados); 
    glPopMatrix();
    if (usarTextura) 
    {
        glDisable(GL_TEXTURE_2D);
    }

    glutSwapBuffers();
}

void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case 'w':
    case 'W':
        if (light0Ligada) light0Ligada = false;
        else light0Ligada = true;
        break;
    case 'p':
    case 'P':
        if (p) p = 0.0;
        else p = 1.0;
        break;
    case 'd':
        if (d > 0.0) d -= 0.05;
        break;
    case 'D':
        if (d < 1.0) d += 0.05;
        break;
    case 'e':
        if (e > 0.0) e -= 0.05;
        break;
    case 'E':
        if (e < 1.0) e += 0.05;
        break;
    case 's':
        if (s > 5.0) s -= 2.00;
        break;
    case 'S':
        if (s < 100.0) s += 2.00;
        break;
    case 'm':
        if (m > 0.0) m -= 0.05;
        break;
    case 'M':
        if (m < 1.0) m += 0.05;
        break;
    case 'l':
    case 'L':
        isLightingOn = !isLightingOn;
        break;
    
    case 'v':
    case 'V':
        localViewer = !localViewer;
        break;
    case '1':
        mudarCamera = !mudarCamera;

    default:
        break;
    }
    glutPostRedisplay();
}

// Callback para setas do teclado
void specialKeyInput(int key, int x, int y)
{
    if(key == GLUT_KEY_DOWN)
    {
        xAngle++;
        if (xAngle > 360.0) xAngle -= 360.0;
    }
    if(key == GLUT_KEY_UP)
    {
        xAngle--;
        if (xAngle < 0.0) xAngle += 360.0;
    }
    if(key == GLUT_KEY_RIGHT)
    {
        yAngle++;
        if (yAngle > 360.0) yAngle -= 360.0;
    }
    if(key == GLUT_KEY_LEFT)
    {
        yAngle--;
        if (yAngle < 0.0) yAngle += 360.0;
    }
    glutPostRedisplay();
}

// Callback de redimensionamento
void resize(int w, int h)
{
    larguraJanela = w;
    alturaJanela = h;
    glViewport (0, 0, w, h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, ((float)w / (float)h), 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void rotacionaEsfera() 
{
	r += 0.002f;
    translaRot += 2.2f;
    if( translaRot == 360)
    {
	translaRot =0;
    }
    anguloEsferaY += .1f;
    glutPostRedisplay();
}

// Imprime a ajuda no console
void imprimirAjudaConsole(void)
{
    cout << "Ajuda:" << endl;
    cout << "  Aperte 'l' para ligar/desligar a iluminacao do OpenGL." << endl
         << "  Aperte 'w' para ligar/desligar a fonte de luz." << endl
         << "  Aperte 'd/D' para aumentar/reduzir a intensidade difusa da luz branca." << endl
         << "  Aperte 'e/E' para aumentar/reduzir a intensidade especular da luz branca." << endl
         << "  Aperte 'm/M' para aumentar/reduzir a intensidade da luz ambiente global." << endl
         << "  Aperte 's/S' para aumentar/reduzir o expoente especular do material." << endl
         << "  Aperte 'p' para alternar entre fonte posicional ou direcional." << endl
         << "  Aperte as setas para rotacionar a fonte de luz" << endl;
}

int main(int argc, char *argv[])
{
   if(!music.openFromFile("menu.wav"))
    {
        printf("ERRO");    
    }
    music.setLoop(true);
    music.setVolume(70);    
    music.play();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1600, 1200);
    glutInitWindowPosition (100, 100);
    glutCreateWindow("Trabalho Pratico 2");
    glutDisplayFunc(desenhaCena);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);
    glutIdleFunc(rotacionaEsfera);
    setup();
    glutMainLoop();
}