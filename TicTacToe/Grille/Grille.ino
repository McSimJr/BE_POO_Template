#include <Wire.h>
#include "rgb_lcd.h"
#include <iostream>
#include <Ultrasonic.h>
#include <cmath>

using namespace std;

#define Circle  1
#define Cross   2
#define Empty   0
const int buttonPin = D6;     // the number of the pushbutton pin
const int ultrasonicPin1 = D7;     // the number of the pushbutton pin
const int ultrasonicPin2 = D8;     // the number of the pushbutton pin


class peripheral{
  protected :
    virtual void init()=0;
};



class ultrasonic_Sensors : public peripheral{
  Ultrasonic * Capteur1;
  Ultrasonic * Capteur2;
  private :
    const float DIST_BETWEEN_SENSORS=9.5; //mesuré à la règle
  public :
    void init(){
      Capteur1=new Ultrasonic(ultrasonicPin1);
      Capteur2=new Ultrasonic(ultrasonicPin2);
    }
    float get_x(){
      long d1_in_cm = Capteur1->MeasureInCentimeters();
      long d2_in_cm = Capteur2->MeasureInCentimeters();
      float Theta= acos((d1_in_cm*d1_in_cm+DIST_BETWEEN_SENSORS*DIST_BETWEEN_SENSORS-d2_in_cm*d2_in_cm)/(2*d1_in_cm*DIST_BETWEEN_SENSORS));
      return (d1_in_cm*cos(Theta)+DIST_BETWEEN_SENSORS*2);
    }    
    float get_y(){
      long d1_in_cm = Capteur1->MeasureInCentimeters();
      long d2_in_cm = Capteur2->MeasureInCentimeters();
      float Theta= acos((d1_in_cm*d1_in_cm+DIST_BETWEEN_SENSORS*DIST_BETWEEN_SENSORS-d2_in_cm*d2_in_cm)/(2*d1_in_cm*DIST_BETWEEN_SENSORS));
      return (d1_in_cm*sin(Theta));
    }


};

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

display lcd;

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
};

class grid{
  private:
    pion table[3][3];
  public :
    grid(){
      for (int i=0;i<3;i++){
        for (int j=0;j<3;j++){
          table[i][j]=i;
        }
      }
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
      
    pion& operator [] (int i){
      return table[i/3][i%3];
    }
      

};


grid grille;
button bouton;
ultrasonic_Sensors sensor;

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  bouton.init();
  sensor.init();

  
  delay(1000);
  grille[7]=Circle;
  // initialize the pushbutton pin as an input:
  
}

void loop() {
    if (bouton.GetPressed()){
      grille[0].SetType((grille[0].GetType()+1)%3);
    }
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(12, 1);
    // print the number of seconds since reset:
    lcd.print_Word(String(sensor.get_x()));
    grille.Display_grid();
    delay(200);
}
