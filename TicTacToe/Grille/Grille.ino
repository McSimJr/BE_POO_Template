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
const int buttonPin = D6;     // the number of the pushbutton pin
const int ultrasonicPin1 = D7;     // the number of the first ultrasonic sensor pin
const int ultrasonicPin2 = D8;     // the number of the second ultrasonic sensor pin


class coords{
  public :
    int x;
    int y;
    coords(){
      x=0;
      y=0;
    }
};

class peripheral{
  protected :
    virtual void init()=0;
};

class button : public peripheral{
  private :
    int pin;
  public :
    void init(){
      pin=buttonPin;
      pinMode(buttonPin, INPUT);
    }
    int GetPressed(){
      return (digitalRead(pin)==HIGH);
    }
};

button bouton;

class display : public peripheral{
  private :
    const int colorR = 0;
    const int colorG = 0;
    const int colorB = 0;
    rgb_lcd lcd;
  public :
    void init(){
      lcd.begin(16, 2);
      lcd.setRGB(colorR, colorG, colorB);
    }
    void setCursor(int line, int column){
      lcd.setCursor(line, column);
    }
    
    void print(char character){
      lcd.print(character);
    }
    void print_Word(String word){
      lcd.print(word);
    }
};

display lcd;


class ultrasonic_Sensors : public peripheral{
  Ultrasonic * Capteur1;
  Ultrasonic * Capteur2;
  private :
    const float DIST_BETWEEN_SENSORS=9.0; //mesuré à la règle
    float x1=1000;
    float y1=1000;
  public :
    void calibrate(){
      while (!(bouton.GetPressed())){
        lcd.setCursor(2,1);
        float x,y;
        coords point=get_coords();
        x=point.x;
        y=point.y;
        lcd.print_Word("x:"+String(x)+" y:"+String(y));
        delay(200);
      }
      while ((x1<5 || x1>100)&&(y1<5 || y1>100)){
        coords point = get_coords();
        y1=point.y;
        x1=point.x;
      }   
      while (bouton.GetPressed());
    }
    int get_diff_x(float x){
      return (x-x1);
    }
    int get_diff_y(float y){
      return (y-y1);
    }
    void init(){
      Capteur1=new Ultrasonic(ultrasonicPin1);
      Capteur2=new Ultrasonic(ultrasonicPin2);
      this->calibrate();
    }

    coords get_coords(){
      long d1_in_cm=0;
      long d2_in_cm=0;
      for (int i=0;i<10;i++){
        d1_in_cm += Capteur1->MeasureInCentimeters();
        d2_in_cm += Capteur2->MeasureInCentimeters();
        delay(10);
      }
      d1_in_cm = d1_in_cm/10;
      d2_in_cm = d2_in_cm/10;
      float Theta= acos((d1_in_cm*d1_in_cm+DIST_BETWEEN_SENSORS*DIST_BETWEEN_SENSORS-d2_in_cm*d2_in_cm)/(2*d1_in_cm*DIST_BETWEEN_SENSORS));
      coords point;
      point.y=d1_in_cm*sin(Theta);
      point.x=d1_in_cm*cos(Theta)+DIST_BETWEEN_SENSORS*2;
      return (point);
    }
};

class pion{
  private :
    int type;
 public:
    void SetType(int TypeIn){
    type=TypeIn;
    }

    void operator = (int TypeIn){  //Pareil qu'au-dessus mais permet de redéfinir un opérateur
      type=TypeIn;
    }

    int GetType(){
      return type;
    }

    char GetChar(){
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

    pion(int TypeIn){
      type=TypeIn;
    }
    pion(){
      type=Empty;
    }
    int operator != (int comp){
      return (type != comp);
    }
};

ultrasonic_Sensors sensor;

class grid{
  protected:
    pion table[3][3];
    const float Pion_size=8.; //Les cases font 10 cm de large
  public :
    grid(){
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=Empty;
        }
      }
    }
    void display_player(int player){
      lcd.setCursor(9, 0);
      lcd.print_Word("Player"+ String(player));
    }
    void Toggle_player(int &player){
      static int tab_next[3]={0,2,1};
      player=tab_next[player];
    }
    int get_position(){
      float x = 1000;
      float y = 1000;
      while ((x<5 || x>100)&&(y<5 || y>100)){ //éviter les aberrations/ captage d'autres objets 
        coords point=sensor.get_coords();
        x=point.x;
        y=point.y;
      }

      float relative_x = sensor.get_diff_x(x);
      float relative_y = sensor.get_diff_y(y);
      float position_x = relative_x/Pion_size;
      float position_y = relative_y/Pion_size;
      int Res=5;
      if (position_x<-1){  // de 1 à 2 c'est la ligne du milieu, de 2 à 3 la ligne du bas
        Res-=3;
      }
      else if (position_x>1){
        Res+=3;
      }
       if (position_y<-1){  // de 1 à 2 c'est la ligne du milieu, de 2 à 3 la ligne du bas
        Res-=1;
      }
      else if (position_y>1){
        Res+=1;
      } 
      return Res;
    }
    pion& operator [] (int i){
      return table[(i+1)/3][(i+1)%3];
    }

    void set_pion(int player){
      int position;
      static int x=1;
      static int y=1;
      int init=0;
      while (table[x][y] != Empty || !(init)){
        while (!(bouton.GetPressed())){
          lcd.setCursor(9,1);
          position = this->get_position();
          lcd.print_Word(String(position));
        }
        x=(position-1)/3;
        y=(position-1)%3;
        init=1;
      }
      //((*this)[position]).SetType(player);
      table[x][y].SetType(player);
      delay(200);
      while (bouton.GetPressed());

    }
    void Display_line(int line){
      int line_corrected=line;
      if (line == 2){
        line_corrected = 0;
      }
      for (int j=0;j<3;j++){
        if (line < 2){
          lcd.setCursor(j,line_corrected);
        }else{
          lcd.setCursor(j+4,line_corrected);
        }
        lcd.print(table[line][j].GetChar());
      }
    }
    void Display_grid(){
      for (int i=0;i<3;i++){
        this->Display_line(i);
      }
    }
    void operator =(grid grille2){
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=grille2.table[i][j];
        }
      }
    }
    void Empty_grid(){
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=Empty;
        }
      }
    }    
};


class TicTacToe : public grid {
  public :
  int is_Winning(){
    int win=0;
    //vérif lignes
    for (int i=0;i<3;i++){
      if (table[i][0].GetChar() == table[i][1].GetChar() && table[i][1].GetChar() == table[i][2].GetChar()) {
        return win=table[i][0].GetType();
      }
    }
    // Vérifier les colonnes.
    for (int j = 0; j < 3; j++) {
      if (table[0][j].GetChar() == table[1][j].GetChar() && table[1][j].GetChar() == table[2][j].GetChar()) {
        return win=table[0][j].GetType();
      }
    } 
    // Vérifier la diagonale principale.
    if (table[0][0].GetChar() == table[1][1].GetChar() && table[1][1].GetChar() == table[2][2].GetChar()) {
      return win=table[0][0].GetType();
    }
    // Vérifier la diagonale secondaire.
    if (table[0][2].GetChar() == table[1][1].GetChar() && table[1][1].GetChar() == table[2][0].GetChar()) {
      return win=table[0][2].GetType();
    }
    int product=1;
    for (int i=0;i<3;i++){
      for (int j=0;j<3;j++){
        product=product*table[i][j].GetType();
      }
    }
    if (product == 0){ //S'il reste au moins une case vide
      return win=0;
    }
    else {    // Si la grille est pleine
      return 3;
    }
  }
  void Display_Winner(int player){
    lcd.setCursor(0,0);
    lcd.print_Word("                 ");
    lcd.setCursor(0,1);
    lcd.print_Word("                 "); // clears the screen
    lcd.setCursor(0,0);
    if (player==3){
      lcd.print_Word("That's a draw !");
    }
    else{
      lcd.print_Word("Player "+String(player)+" won !");
    }
  }
  void init_game(){
    lcd.init();
    bouton.init();
    sensor.init();
    lcd.setCursor(2,1);
    lcd.print_Word("              ");
  }
  TicTacToe():grid(){
    init_game();
  }
  void run_game(){
    int player=Player_1;
    int Winner=0;
    this->Empty_grid();
    while (!Winner){
      display_player(player);
      set_pion(player);
      Toggle_player(player);
      Display_grid();
      Winner=is_Winning();
    }
    Display_Winner(Winner);
  }
};

/*grid grille;
int Player=Player_1;

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  bouton.init();
  sensor.init();
  lcd.setCursor(2,1);
  lcd.print_Word("              ");
}

void loop() {
    grille.display_player(Player);
    grille.set_pion(Player);
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    // print the number of seconds since reset:
    grille.Display_grid();

    grille.Toggle_player(Player);
    delay(1000);
}
*/
TicTacToe game;
void setup(){

}
void loop(){
  game.run_game();
  while (!(bouton.GetPressed()));
}