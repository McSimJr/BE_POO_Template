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

class TicTacToe::grid {
  int is_Winning(){
    int win=0;
    //vérif lignes
    for (int i=0;i<3;i++){
      if (table[i][0].GetChar()==table[i][1].GetChar() == table[i][2].GetChar()) {
        return win=1;
        }
        }
        // Vérifier les colonnes.
      for (int j = 0; j < 3; j++) {
        if (table[0][j].GetChar() == table[1][j].GetChar() == table[2][j].GetChar()) {
          return win=1;
          }
        } 
        // Vérifier la diagonale principale.
        if (table[0][0].GetChar() == table[1][1].GetChar() == table[2][2].GetChar()) {
          return win=1;
          }
       // Vérifier la diagonale secondaire.
       if (table[0][2].GetChar() == table[1][1].GetChar() == table[2][0].GetChar()) {
        return win=1;
      }
      //Pas de victoire
      return win=0;
      }
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
