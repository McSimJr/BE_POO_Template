/*********************************************************************
 * @file  Apllication.cpp
 * @author <mettre l'adresse mail ou nom prenom>
 * @brief Fichier source de l'application
 *********************************************************************/
#include "Application.h"
#define Circle 1
#define Cross 2;
#define Empty 0;

class Pion{
 int type;
 public:
 void SetType(int TypeIn){
  type=TypeIn;
 }

int GetType(){
  return type;
}

char GetChar(){
  char Resultat;
  if (type==Circle){
    Resultat='o';
  } else if (type==Cross) {
    Resultat='x';
  } else {
    Resultat=' '; 
  }
  return Resultat;
}
Pion(int TypeIn);
};


Application::Application()
{
  // Code
  ; 
}
  
Application::~Application()
{
  // Code
  ;
}  

void Application::init(void)
{
  // Code
    ;
}


void Application::run(void)
{
  // Code
    ;
}
