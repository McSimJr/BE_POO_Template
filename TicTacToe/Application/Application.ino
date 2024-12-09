#include <Wire.h>
#include "rgb_lcd.h"
#include <iostream>
#include <Ultrasonic.h>
#include <cmath>

using namespace std;

#define Circle    1
#define Cross     2
#define Empty     0
#define Player_1  1
#define Player_2  2
const int buttonPin = D6;          // Le pin associé au bouton
const int ultrasonicPin1 = D7;     // Le pin associé au premier capteur à ultrasons
const int ultrasonicPin2 = D8;     // Le pin associé au deuxième capteur à ultrasons


class coords{                      //Initialise une classe permettant de transférer x et y en même temps, sans devoir réaliser une fonction qui renvoie x et l'autre y, ce qui les désynchroniserait
  public :
    int x;
    int y;
    coords(){                      //Constructeur vide qui initialise les coordonnées à 0
      x=0;
      y=0;
    }
};

class peripheral{                  //Classe englobant tous les capteurs, pour les forcer à avoir la méthode abstraite init
  protected :
    virtual void init()=0;
};

class button : public peripheral{  //Classe bouton
  private :
    int pin;
  public :
    void init(){                   //Associe le pin au capteur
      pin=buttonPin;
      pinMode(buttonPin, INPUT);   //Met ce pin au mode voulu
    }
    int GetPressed(){              //Renvoie si le bouton est appuyé
      return (digitalRead(pin)==HIGH);
    }
};

button bouton;                     //Instancie un bouton

class display : public peripheral{ //Classe écran
  private :
    const int colorR = 0; //Met le fond de l'écran sans lumière
    const int colorG = 0;
    const int colorB = 0;
    rgb_lcd lcd;
  public :
    void init(){                    //Initialise le format de l'écran et la couleur du fond
      lcd.begin(16, 2);
      lcd.setRGB(colorR, colorG, colorB);
    }
    void setCursor(int line, int column){ //Place le curseur à l'endroit voulu
      lcd.setCursor(line, column);
    }
    
    void print(char character){     //Affiche le caractère passé en argument
      lcd.print(character);
    }
    void print_Word(String word){   //Affiche la chaîne de caractères passée en argument
      lcd.print(word);
    }
};

display lcd;                        //Instancie un écran 


class ultrasonic_Sensors : public peripheral{ //Classe définissant
  Ultrasonic * Capteur1;            //Les deux capteurs
  Ultrasonic * Capteur2;
  private :
    const float DIST_BETWEEN_SENSORS=9.0; //distance entre les deux capteurs mesurée à la règle
    float x1=1000;                  //Initialise les coordonnées de la case centrale à des valeurs aberrantes  pour forcer le programme à rentrer dans la boucle while de calibrate()
    float y1=1000;
  public :
    void calibrate(){               //Calibre les coordonnées associées au point milieu de la grille
      while (!(bouton.GetPressed())){ //Attend que le bouton soit pressé
        lcd.setCursor(2,1);
        float x,y;
        coords point=get_coords();  //Récupère les coordonnées
        x=point.x;
        y=point.y;
        lcd.print_Word("x:"+String(x)+" y:"+String(y)); //Écrit les coordonnées du pion en direct sur la deuxième ligne de l'écran
        delay(200);
      }
      while ((x1<5 || x1>100)&&(y1<5 || y1>100)){ //Récupère les coordonnées jusqu'à obtenir une valeur qui n'est pas aberrante (a du être rajouté à cause de la qualité des mesures)
        coords point = get_coords();
        y1=point.y;
        x1=point.x;
      }   
      while (bouton.GetPressed());  //Attend que le bouton ne soit plus pressé
    }
    int get_diff_x(float x){        //Renvoie la coordonnée en x relative à celle de la case milieu 
      return (x-x1);
    }
    int get_diff_y(float y){        //De même pour y
      return (y-y1);
    }
    void init(){                    //Méthode d'initialisation du capteur
      Capteur1=new Ultrasonic(ultrasonicPin1);  //Insctancie les capteurs à ultrasons en les liant à leur port
      Capteur2=new Ultrasonic(ultrasonicPin2);
      this->calibrate();            //Calibre la grille
    }

    coords get_coords(){            //Récupère et renvoie les coordonnées x et y
      long d1_in_cm=0;
      long d2_in_cm=0;
      for (int i=0;i<10;i++){       //Réalise la mesure des distances entre l'objet et les capteurs 10 fois, espacées de 10 ms
        d1_in_cm += Capteur1->MeasureInCentimeters();
        d2_in_cm += Capteur2->MeasureInCentimeters();
        delay(10);
      }
      d1_in_cm = d1_in_cm/10;       //Calcule la moyenne de ces distances
      d2_in_cm = d2_in_cm/10;
      float Theta= acos((d1_in_cm*d1_in_cm+DIST_BETWEEN_SENSORS*DIST_BETWEEN_SENSORS-d2_in_cm*d2_in_cm)/(2*d1_in_cm*DIST_BETWEEN_SENSORS));
      coords point;
      point.y=d1_in_cm*sin(Theta);
      point.x=d1_in_cm*cos(Theta)+DIST_BETWEEN_SENSORS*2; //Calcule les coordonnées associées sur un plan parallèle à la table
      return (point);               //Renvoie les coordonnées ainsi obtenues
    }
};

class pion{                         //Classe pion qui représente une case qui peut être soit vide, soit une croix, soit un cercle
  private :
    int type;
 public:
    void SetType(int TypeIn){       //Permet de changer le type de la case
    type=TypeIn;
    }

    void operator = (int TypeIn){   //Pareil qu'au-dessus mais en plus joli ☺☻, et permet de comparer des cases entre elles
      type=TypeIn;
    }

    int GetType(){                  //Récupère le type de la case sélectionnée
    //if (type>2 || type <0){         //Lève une exception si le type est inconnu, mais les exceptions ne sont pas supportées par l'IDE Arduino
    //  throw pion(type);
    //}
      return type;
    }

    char GetChar(){                 //Pareil, mais renvoie le caractère associé pour faciliter l'affichage
      char Resultat;
      if (type==Circle){
        Resultat='o';
      } else if (type==2) {
        Resultat='x';
      } else {
        Resultat=' '; 
      }
      return Resultat;
    }

    pion(int TypeIn){               //Constructeur de pion en sélectionnant le type en argument
      type=TypeIn;
    }
    pion(){                         //Constructeur vide, initialisant le type à 0 par défaut
      type=Empty;
    }
    int operator != (int comp){     //Redéfinit l'opérateur != qui permet de comparer un pion avec un entier, pour vérifier qu'une case n'est pas vide notamment
      return (type != comp);
    }
};

ultrasonic_Sensors sensor;          //Instancie les capteurs à ultrasons

class grid{                         //La grille du jeu de morpion
  protected:
    pion table[3][3];               //Tableau de pion qui représente la grille
    const float Pion_size=8.;       //Les cases sont des carrés de 8 cm de large
  public :
    grid(){                         //Constructeur vide qui crée un tableau de pion tous initialisés à vide
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=Empty;
        }
      }
    }
    void display_player(int player){ //Affiche le numéro du joueur qui doit jouer en haut à gauche de l'écran
      lcd.setCursor(9, 0);
      lcd.print_Word("Player"+ String(player));
    }
    void Toggle_player(int &player){  //Si le joueur est égal à 1, passe à 2, et inversement
      static int tab_next[3]={0,2,1};
      player=tab_next[player];
    }
    int get_position(){             //Fonction pour récupérer la position du pion
      float x = 1000;               //Initialise x et y à des valeurs aberrantes afin de rentrer dans la boucle while au moins une fois, aurait pu être géré avec un do while
      float y = 1000;
      while ((x<5 || x>100)&&(y<5 || y>100)){ //éviter les aberrations/captage d'autres objets trop loin 
        coords point=sensor.get_coords(); //Récupère les coordonnées x et y
        x=point.x;
        y=point.y;
      }

      float relative_x = sensor.get_diff_x(x);
      float relative_y = sensor.get_diff_y(y);
      float position_x = relative_x/Pion_size;
      float position_y = relative_y/Pion_size;
      int Res=5;                    //Le calcul de position se fait relativement à la case du milieu
      if (position_x<-1){           //Si le pion est décalé d'une fois le pas sélectionné verticalement, vers le haut, le numéro de la ligne diminue de 1
        Res-=3;
      }
      else if (position_x>1){       //Et inversement 
        Res+=3;
      }
       if (position_y<-1){          //Si le pion est décalé d'une fois le pas sélectionné horizontalement, vers la gauche, le numéro de la colonne diminue de 1
        Res-=1;
      }
      else if (position_y>1){       //Et inversement
        Res+=1;
      } 
      return Res;
    }
    pion& operator [] (int i){
      return table[(i+1)/3][(i+1)%3];
    }

    void set_pion(int player){      //Permet de placer une croix ou un cercle, ce que nous avons décidé d'appeler un pion
      int position;                 //On associe une position à chaque case  (première ligne : 1 2 3 deuxième ligne : 4 5 6 troisième ligne : 7 8 9)
      static int x=1;               //On initialise les coordonnées à des valeurs cohérentes pour ne pas que la boucle dépasse du tableau si ces valeurs s'initialisaient toutes seules à des valeurs indéfinies
      static int y=1;
      int init=0;                   //On initialise une variable pour être sûr de passer dans la boucle au moins une fois (aurait pu être géré avec un do while)
      while (table[x][y] != Empty || !(init)){
        while (!(bouton.GetPressed())){ //Attend que le bouton soit pressé
          lcd.setCursor(9,1);
          position = this->get_position();
          lcd.print_Word(String(position)); //Récupère la position et l'affiche en direct sur la droite de l'écrane, sur la ligne du dessous
        }
        x=(position-1)/3; //On récupère les valeurs finales de x et y en fonction de position
        y=(position-1)%3;
        init=1;                     //Init est mise à 1 pour pouvoir sortir de la boucle
      }
      table[x][y].SetType(player);  //Change le type du pion de la case sélectionnée
      delay(200);                   //Attend pour éviter de potentielles erreurs liées au switch 
      while (bouton.GetPressed());  //Attend le relâchement du bouton

    }
    void Display_line(int line){    //Affiche la ligne souhaitée de la grille à l'écran
      int line_corrected=line;
      if (line == 2){               //S'il s'agit de la troisième ligne, on l'écrit sur la première ligne de l'écran
        line_corrected = 0;
      }
      for (int j=0;j<3;j++){        //On parcourt les éléments de la ligne souhaitée et on les affiche un par un l'un à côté de l'autre
        if (line < 2){
          lcd.setCursor(j,line_corrected);
        }else{
          lcd.setCursor(j+4,line_corrected);//S'il s'agit de la troisième ligne, on l'écrit décalée des autres sur l'écran (Qui est malheureusement de dimension 2 x 16)
        }
        lcd.print(table[line][j].GetChar());
      }
    }
    void Display_grid(){            //Affiche la grille sur l'écran
      for (int i=0;i<3;i++){
        this->Display_line(i);
      }
    }
    void operator =(grid grille2){  //Redéfinis l'opérateur = pour pouvoir faire une affectation terme à terme d'une grille à l'autre facilement
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=grille2.table[i][j];
        }
      }
    }
    void Empty_grid(){              //Parcourt la grille et vide toutes les cases
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=Empty;
        }
      }
    }    
};


class TicTacToe : public grid {
  public :
  int is_Winning(){       //Méthode vérifiant si le jeu est terminé
    int win=0;
    //Vérifie les lignes
    for (int i=0;i<3;i++){
      if (table[i][0].GetChar() == table[i][1].GetChar() && table[i][1].GetChar() == table[i][2].GetChar()) {
        return win=table[i][0].GetType(); //Si gagnant il y a, on renvoie qui l'est
      }
    }
    // Vérifie les colonnes
    for (int j = 0; j < 3; j++) {
      if (table[0][j].GetChar() == table[1][j].GetChar() && table[1][j].GetChar() == table[2][j].GetChar()) {
        return win=table[0][j].GetType(); //Si gagnant il y a, on renvoie qui l'est
      }
    } 
    // Vérifier la diagonale principale.
    if (table[0][0].GetChar() == table[1][1].GetChar() && table[1][1].GetChar() == table[2][2].GetChar()) {
      return win=table[0][0].GetType(); //Si gagnant il y a, on renvoie qui l'est
    }
    // Vérifier la diagonale secondaire.
    if (table[0][2].GetChar() == table[1][1].GetChar() && table[1][1].GetChar() == table[2][0].GetChar()) {
      return win=table[0][2].GetType(); //Si gagnant il y a, on renvoie qui l'est
    }
    int product=1;
    for (int i=0;i<3;i++){           //On multiplie la valeur de chacune des cases (0 pour les vides 1 pour les croix et 2 pour les cercles) si le produit est nul, il reste encore au moins une case vide
      for (int j=0;j<3;j++){
        product=product*table[i][j].GetType();
      }
    }
    if (product == 0){              //S'il reste au moins une case vide
      return win=0;
    }
    else {                          // Si la grille est pleine
      return 3;
    }
  }
  void Display_Winner(int player){
    lcd.setCursor(0,0);
    lcd.print_Word("                 ");
    lcd.setCursor(0,1);
    lcd.print_Word("                 "); // Efface l'affichage
    lcd.setCursor(0,0);
    if (player==3){                 //S'il y a égalité
      lcd.print_Word("That's a draw !"); //Affiche l'écran d'égalité
    }
    else{                           //Si quelqu'un a gagné, affiche son numéro de joueur
      lcd.print_Word("Player "+String(player)+" won !");
    }
  }
  void init_game(){
    lcd.init();                     //initialise l'écran
    bouton.init();                  //initialise le bouton
    sensor.init();                  //initialise les capteurs à ultrason
    lcd.setCursor(2,1);
    lcd.print_Word("              "); //efface la deuxième ligne de l'écran
  }
  TicTacToe():grid(){
    init_game();                   //Réalise toutes les étapes de l'initialisation et crée une grille vide
  }
  void run_game(){
    int player=Player_1;          //On initialise le joueur à joueur 1
    int Winner=0;                 //Variable qui indiquera qui est le gagnant, égale à zéro tant qu'il n'y en a pas
    this->Empty_grid();           //Nettoie l'écran
    while (!Winner){              //Tant qu'il n'y a pas de vainqueur ou d'égalité
      display_player(player);     //Affiche le joueur à qui c'est le tour de jouer
      set_pion(player);           //Le joueur positionne son pion
      Toggle_player(player);      //Change le joueur à qui c'est le tour de jouer
      Display_grid();             //Affiche la grille avec le pion qui vient d'être joué
      Winner=is_Winning();        //Vérifie s'il y a un vainqueur
    }
    Display_Winner(Winner);       //Affiche le vainqueur une fois le jeu terminé
  }
};


TicTacToe game;
void setup(){

}
void loop(){
  //try{
    game.run_game();                //Lance une partie
  /*}
  catch (int err){
    lcd.setCursor(0,0);
    lcd.print_Word("                 ");
    lcd.setCursor(0,1);
    lcd.print_Word("                 "); // Efface l'affichage
    lcd.setCursor(0,0);
      lcd.print_Word("Type inconnu"); //Affiche l'écran d'erreur
  }*/
  while (!(bouton.GetPressed())); //Attend que le bouton soit pressé
}