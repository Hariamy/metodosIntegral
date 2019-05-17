#include "im_gui_openGL/imgui.h"
#include "im_gui_openGL/imgui_impl_glut.h"
#include "im_gui_openGL/imgui_impl_opengl2.h"


#include <iostream>
#include <cstdio>
#include <string>
#include <cmath>
#include <vector>
#include <GL/freeglut.h>

#define MENU     0
#define DERIVADA 1
#define INTEGRAL 2
#define AUTO     3

#define QTD_DIST_UM 0.1

static ImVec4 clear_color = ImVec4(0.095f, 0.095f, 0.095f, 1.00f);
int tipo = MENU;
bool mostrarSobreMenu = false;
static float inter[2] = {-10.0, 10.0};
static std::vector <float> valInterpol;
static std::vector <float> interpol;

inline void pintar ();
inline void menus ();
inline void menuPrincipal ();
inline void sobreMenu ();
inline void menuIntegral ();

inline void mostrarFuncao ();
inline void mostrarArea ();
void mostrarInterpolacao ();

inline void funcaoInterpoladoraNewton ();

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
		
	#ifdef __FREEGLUT_EXT_H__
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	#endif
	
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Metodos numericos 2");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutDisplayFunc(pintar);
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; //Provavelmente, entrada e saída
	io.KeyRepeatRate = 1;
  
	ImGui::StyleColorsDark(); //Cores do imgui
  
   //Faz as ligações do imgui com o glut e o opengl
   ImGui_ImplGLUT_Init();
	ImGui_ImplGLUT_InstallFuncs();
	ImGui_ImplOpenGL2_Init();

	glutMainLoop();
	
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplGLUT_Shutdown();
	ImGui::DestroyContext();
	
	return 0;
}

void pintar () {
   //Cria um novo frame do imgui
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplGLUT_NewFrame();
	
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(inter[0], inter[1], -10, 10, -1.0, 20000.0);
	glMatrixMode(GL_MODELVIEW);
	
	menus();
	
	//Renderiza
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	glutSwapBuffers();
	glutPostRedisplay();
}

void menus () {
   ImGui::GetStyle().WindowRounding = 0.0f;
   
    switch (tipo){
    case MENU:
    	menuPrincipal();
    break;
    case DERIVADA:
    break;
    case INTEGRAL:
      menuIntegral();
    break;
    case AUTO:
      
    break;
    }
   
   if(mostrarSobreMenu){
      sobreMenu();
   }
}

void menuPrincipal () {
	ImVec2 tamanho((ImGui::GetIO().DisplaySize.x*3.0)/4.0, ((ImGui::GetIO().DisplaySize.y)*5.0)/6.0);
	
	ImGui::SetWindowSize("Botoes", tamanho);
	ImGui::SetNextWindowPosCenter();

	ImGui::Begin("Botoes", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImVec2 tamBotoes((tamanho.x*3.0)/4.0, tamanho.y/5.0);
		float distancia = (tamanho.x/2) - (tamBotoes.x/2);

		ImGui::SameLine(distancia);
		if(ImGui::Button("Derivada", tamBotoes)) { tipo = DERIVADA; }

		ImGui::NewLine();
		ImGui::SameLine(distancia);
		if(ImGui::Button("Integral", tamBotoes)) { tipo = INTEGRAL; }

		ImGui::NewLine();
		ImGui::SameLine(distancia);
		if(ImGui::Button("Autovetor e Autovalor", tamBotoes)) { tipo = AUTO; }

		ImGui::NewLine();
		ImGui::SameLine(distancia);
		if(ImGui::Button("Problema do valor inicial", tamBotoes)) { }

		ImGui::NewLine();
		ImGui::SameLine(distancia);
		if(ImGui::Button("Problema de valor de contorno", tamBotoes)) { }
	ImGui::End();

   ImGui::BeginMainMenuBar();
		if(ImGui::BeginMenu("Arquivo")){
			if(ImGui::MenuItem("Derivada"))                  { tipo = DERIVADA; }
			if(ImGui::MenuItem("Integral"))                  { tipo = INTEGRAL; }
			if(ImGui::MenuItem("Autovetores e autovalores")) { tipo = AUTO; }
			ImGui::MenuItem("Sair");
			
			ImGui::EndMenu();
		}
		
		if(ImGui::BeginMenu("Ajuda")){
			mostrarSobreMenu |= ImGui::MenuItem("Sobre");
			
			ImGui::EndMenu();
		}
	ImGui::EndMainMenuBar();
}

void sobreMenu () {
	ImVec2 tamanho(200, 100), posicao((ImGui::GetWindowWidth()), (ImGui::GetWindowHeight()));
	
	ImGui::SetWindowSize("Sobre", tamanho);
	ImGui::SetNextWindowPosCenter();
    ImGui::Begin("Sobre", &mostrarSobreMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);
		ImGui::PushTextWrapPos(200);
    	ImGui::Text("Métodos Numéricos 2");
		ImGui::Separator();
		ImGui::AlignFirstTextHeightToWidgets();
		ImGui::Text("Programa de métodos numéricos criado para a cadeira de métodos 2");
		ImGui::PopTextWrapPos();
    ImGui::End();
}

void menuIntegral () {
	static char tt[20], *item[] = {"", "Newton Cotes", "Gauss Legendre", "Exponencial"};
	ImVec2 tamanho(250, 170);
	static int qtd = 0, val = 0;
	static bool func = false, area = false, interpolacao = false; 
	static std::string resposta;
	
	if(area){
		mostrarArea();
	}
	if(func){
		mostrarFuncao();
	}
	if(interpolacao){
		funcaoInterpoladoraNewton();
		mostrarInterpolacao();
	}

	ImGui::SetWindowSize("Integral", tamanho);
	ImGui::Begin("Integral", NULL, ImGuiWindowFlags_NoResize);
		ImGui::Checkbox("Função", &func);
		ImGui::SameLine();
		ImGui::Checkbox("Área", &area);
		ImGui::SameLine();
		ImGui::Checkbox("Interpolação", &interpolacao);

		ImGui::InputText("Equação", tt, 20);
		ImGui::InputFloat2("Intervalo", inter);
		ImGui::Combo("Método", &qtd, item, IM_ARRAYSIZE(item));

		switch (qtd){
			case 1:
				ImGui::InputInt("Grau", &val);
			break;
			case 2:
				ImGui::InputInt("Nº pontos", &val);
			break;
			case 3:

			break;
		}

		ImGui::Button("Calcular");
		ImGui::SameLine();
		ImGui::Text(resposta.data());

	ImGui::End();
}

void mostrarFuncao () {
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for(float i = inter[0]; i < inter[1]; i += QTD_DIST_UM){
		glVertex2f(i, std::sin(i));
	}
	glEnd();
}

void mostrarArea () {
	float fim = inter[0] - QTD_DIST_UM;

	glBegin(GL_TRIANGLE_STRIP);
		glColor4f(1.0, 0.7, 0.3, 0.85);
		
		for(float i = inter[1]; i >= fim; i -= QTD_DIST_UM){
			glVertex2f(i, std::sin(i));
			glVertex2f(i, 0);
		}
	glEnd();
}

void mostrarInterpolacao () {
	int tam = interpol.size();
	float dist = (inter[1] - inter[0]) / 3,
	      i0 = inter[0], i1 = i0 + dist,
	      i2 = i1 + dist, i3 = inter[1],
				f0 = std::sin(inter[0]), f1 = std::sin(i1),
				f2 = std::sin(i2), f3 = std::sin(i3);

	glBegin(GL_LINE_STRIP);
		glColor3f(1.0, 0.0, 0.0);
		for(int i = 0; i < tam; i++){
			glVertex2f(valInterpol[i], interpol[i]);
		}
	glEnd();

	glPointSize(10.0);
	glBegin(GL_POINTS);
		glColor3f(1.0, 1.0, 0.0);
		
		glVertex2f(i0, f0);
		glVertex2f(i1, f1);
		glVertex2f(i2, f2);
		glVertex2f(i3, f3);
	glEnd();
}

void funcaoInterpoladoraNewton () {
	valInterpol.clear();
	interpol.clear();
	
	float dist = (inter[1] - inter[0]) / 3,
	      i0 = inter[0], i1 = i0 + dist,
	      i2 = i1 + dist, i3 = inter[1],
				f0 = std::sin(inter[0]), f1 = std::sin(i1),
				f2 = std::sin(i2), f3 = std::sin(i3),
				s, s2, s3;
	//std::cout << dist << "  " << i0 << "  " << i1 << "  " << i2 << "  " << i3 << '\n';
	for(float i = inter[0]; i < inter[1]; i += QTD_DIST_UM){
		s = (i - inter[0]) / dist;
		s2 = s * s;
		s3 = s2 * s;

		valInterpol.push_back(i);
		interpol.push_back(f0 +
		                   (s * (f1 - f0)) +
											 (((s2 - s) / 2) * (f2 - (2*f1) + f0)) +
											 (((s3 - (3 * s2) + (2 * s)) / 6) * (f3 - (3 * f2) + (3 * f1) - f0)));
	}
}