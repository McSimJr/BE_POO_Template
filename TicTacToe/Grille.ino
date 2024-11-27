#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

class box{
    public :
    int i;
    void empty(){}
    char getchar(){return 'o';}
};
class grid{
    private:
        box table[3][3];
    public :
        grid(){
            for (int i=0;i<3;i++){
                for (int j=0;j<3;j++){
                    table[i][j].empty();
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
              lcd.setCursor(j+3,line_corrected);
            }
            lcd.print(table[line][j].getchar());
          }
        }
        void Display_grid(){
          for (int i=0;i<3;i++){
            this->Display_line(i);
          }
        }
        void Empty_grid(){
          for (int i=0;i<3;i++){
            for (int j=0;j<3;j++){
              table[i][j].empty();
            }
          }
        }
        
        int[3]



};


grid grille;

const int colorR = 0;
const int colorG = 0;
const int colorB = 0;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 3);

  lcd.setRGB(colorR, colorG, colorB);

  // Print a message to the LCD.
  //lcd.print("Hello, world!");
  
  delay(1000);
}

void loop() {
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(13, 2);
    // print the number of seconds since reset:
    lcd.print(millis() / 1000);
    grille.Display_grid();
    delay(100);
}
