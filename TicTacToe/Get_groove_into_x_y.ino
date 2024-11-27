#include <Ultrasonic.h>
#include <cmath>
/*class obj {
  private :
    int dimension;
    int * measures;
  public : 
    obj(){
      dimension = 0;
      measures = NULL;
    }
    obj(int dim){
      dimension = dim;
      measures = new int(dim);
    }
    obj(int dim, int * tab){
      dimension = dim;
      measures = new int(dim);
      for (int i =0;i<dim;i++){
        measures[i]=tab[i];
      }
    }
     int operator [](int index){
      if (index<dimension){
        return measures[index];
      }
      else{
        int a=0;
      }
     }
};*/
/*
    UltrasonicDisplayOnTerm.ino
    Example sketch for ultrasonic ranger

    Copyright (c) 2012 seeed technology inc.
    Website    : www.seeed.cc
    Author     : LG, FrankieChu
    Create Time: Jan 17,2013
    Change Log :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/


/***************************************************************************/
//	Function: Measure the distance to obstacles in front and print the distance
//			  value to the serial terminal.The measured distance is from
//			  the range 0 to 400cm(157 inches).
//	Hardware: Grove - Ultrasonic Ranger
//	Arduino IDE: Arduino-1.0
/*****************************************************************************/

#include "Ultrasonic.h"

Ultrasonic ultrasonic1(D7),ultrasonic2(D8);

void setup() {
    Serial.begin(9600);
}
void loop() {
  
    long d1_in_cm;
    long d2_in_cm;
    d1_in_cm = ultrasonic1.MeasureInCentimeters();
    d2_in_cm = ultrasonic2.MeasureInCentimeters();
    float distance_target;
    float theta,x,y;
    const float DIST_BETWEEN_SENSORS=9.5; //mesuré à la règle
    theta = acos((d1_in_cm*d1_in_cm+DIST_BETWEEN_SENSORS*DIST_BETWEEN_SENSORS-d2_in_cm*d2_in_cm)/(2*d1_in_cm*DIST_BETWEEN_SENSORS));
    distance_target = d1_in_cm * sin(theta);
    x = d1_in_cm*cos(theta)+DIST_BETWEEN_SENSORS*2;
    y = d1_in_cm*sin(theta);
    Serial.println("x : ");
     // two measurements should keep an interval
    /*Serial.print(distance_target);//0~400cm
    Serial.println(" cm");
    Serial.println("d1 : ");*/
     // two measurements should keep an interval
    Serial.print(x);//0~400cm
    Serial.print(" cm");
    Serial.println("d2 : ");
     // two measurements should keep an interval
    Serial.print(y);//0~400cm
    Serial.println(" cm");
    delay(200);
}
