
void All_Keys_False(){
    KEY1 = false;
    KEY2 = false;
    KEY3 = false;
    KEY4 = false;
}

void print_keys(){
    Serial.print(KEY1);
    Serial.print(KEY2);
    Serial.println(KEY3);
}


//Used to completely redraw the main menu. 
void reDrawMain(){
    
    Serial.println("RedrawMain");
    tft.fillScreen(bg_color);


    tft.setCursor(70,30);
    tft.print("Temperature");
    tft.drawFastHLine(0, 41, 440, 0x0000);
    tft.drawFastHLine(0, 42, 440, 0x0000);
    tft.drawFastHLine(0, 43, 440, 0x0000);
    tft.drawFastHLine(0, 44, 440, 0x0000);
    tft.drawFastHLine(0, 45, 440, 0x0000);
    tft.setCursor(LCol2, LLine1);
    if(Setpoint > 0){
        tft.print(Setpoint, 1);
        tft.setCursor(240,220);
        tft.print("Off");
    }else{
        tft.print("Off");
        tft.setCursor(240,220);
        tft.print("On");
    }
    tft.setCursor(LCol1, LLine1);
    tft.print("Setpoint ");
    tft.setCursor(LCol1, LLine2);
    tft.print("Actual ");
    tft.setCursor(LCol2, LLine2);

    //Up Arrow
    tft.drawFastVLine(70, 200, 30, 0x000);
    tft.drawFastVLine(71, 200, 30, 0x000);
    tft.drawFastVLine(72, 200, 30, 0x000);
    tft.drawFastVLine(73, 200, 30, 0x000);
    tft.drawFastVLine(74, 200, 30, 0x000);
    tft.fillTriangle(72, 190, 60, 210, 84, 210, 0x0000);
  
    //Down Arrow
    tft.drawFastVLine(160, 190, 30, 0x000);
    tft.drawFastVLine(161, 190, 30, 0x000);
    tft.drawFastVLine(162, 190, 30, 0x000);
    tft.drawFastVLine(163, 190, 30, 0x000);
    tft.drawFastVLine(164, 190, 30, 0x000);
    tft.fillTriangle(162, 230, 150, 210, 174, 210, 0x0000);
    tft.setCursor(240, 220);
}

void get_info(){

    tft.setTextSize(1);     // smaller text size
    tft.setCursor(20,30);
    tft.fillScreen(bg_color);

    Analog_Tests();
    
    tft.print("Curr = ");
    tft.setCursor(150,30);
    tft.print(Adj_Current,0);   
    tft.println(" mA");

    tft.setCursor(20,60);
    tft.print("Volts = ");
    tft.setCursor(150,60);
    tft.print(Volts);
    tft.println(" Volts");

    tft.setCursor(20,90);
    tft.print("Power = ");
    tft.setCursor(150,90);
    tft.print(Power);
    tft.println(" Watts");

    tft.setCursor(20,120);
    tft.print("Resistance = ");
    tft.setCursor(150,120);
    tft.print(Resistance);
    tft.println(" Ohms");

    tft.setCursor(20,150);
    tft.print("PID = ");
    tft.setCursor(150,150);
    tft.print(kp,0);
    tft.print(" - ");
    tft.print(ki,0);
    tft.print(" - ");
    tft.print(kd,0);
    tft.print(" ");

    tft.setTextSize(2);

    tft.setCursor(145,210);
    tft.print("Ok");

    while ((digitalRead(PB1) == 0) || (digitalRead(PB2) == 0) || (digitalRead(PB3) == 0) ){  // wait for all keys up
      
    }

     while (true) {
      if (digitalRead(PB2) == 0)   // wait for PB2
      break;
     }
   
}
void wait_4_key_press(){
    while ((digitalRead(PB1) == 0) || (digitalRead(PB2) == 0) || (digitalRead(PB3) == 0) ){ // wait for all keys up
        Run_PID();   // Call PID Loop to prevent runaway while waiting for keys
        strobe_WDT();
    }
    All_Keys_False();
    while (true){
        
        if (digitalRead(PB1) == 0){
            KEY1 = true;
            break;
        }
        if (digitalRead(PB2) == 0){
            KEY2 = true;
            break;
        }
        if (digitalRead(PB3) == 0){
            KEY3 = true;
            break;
        }
        Run_PID();   // Call PID Loop to prevent runaway while waiting for keys
        strobe_WDT();
    }
}



void set_Setpoint(){
    int Down = 0;  // Down counter
  
    tft.fillScreen(bg_color);
    tft.setFont(&FreeSans9pt7b);
    tft.setTextColor(txt_clr );  tft.setTextSize(2);
    tft.setCursor(20, 210);
    tft.print("Inc    Dec    OK");
    tft.setCursor(20, 50);
    tft.print("Setpoint:");
    tft.setCursor(180, 50);
    tft.print(Setpoint,1);
    wait_4_key_press();
    while (true) {
        Down = 0;
        Run_PID();
        strobe_WDT();
        while (digitalRead(PB1) == 0){
            if (Setpoint <= 800) {
              strobe_WDT();
                tft.setTextColor(bg_color);
                tft.setCursor(180, 50);
                tft.print(Setpoint++);          // erase old display and increment setpoint
                tft.setTextColor(txt_clr);
                tft.setCursor(180, 50);
                tft.print(Setpoint,1);            // write new setpoint
                if (Down++ < 10)
                    delay_WDT(300);
                else
                    delay (20);
        }
    }
    Down = 0;
    while (digitalRead(PB2) == 0){
        if (Setpoint > 80) {
          strobe_WDT();
            tft.setTextColor(bg_color);
            tft.setCursor(180, 50);
            tft.print(Setpoint--,1);          // erase old display and decrement setpoint
            tft.setTextColor(txt_clr);
            tft.setCursor(180, 50);
            tft.print(Setpoint,1);            // write new setpoint
            if (Down++ < 10)
                delay_WDT(300);
            else
                delay (20);
        }
    }

    Down = 0;
    if (digitalRead(PB3) == 0)
        break;
    }
    while ((digitalRead(PB1) == 0) || (digitalRead(PB2) == 0) || (digitalRead(PB3) == 0) ) // wait for all keys up
    All_Keys_False();
    Write_NVM();  
    return;
}

void set_Kp()
{
    while (true) {
        tft.fillScreen(bg_color);
        tft.setTextColor(txt_clr);  tft.setTextSize(2);
        tft.setCursor(20, 30);
        tft.print("Set Kp");
        tft.setCursor(20, 70);
        tft.print("Kp = ");
        tft.print(kp);
        //Up Arrow
        tft.drawFastVLine(70, 200, 30, 0x000);
        tft.drawFastVLine(71, 200, 30, 0x000);
        tft.drawFastVLine(72, 200, 30, 0x000);
        tft.drawFastVLine(73, 200, 30, 0x000);
        tft.drawFastVLine(74, 200, 30, 0x000);
        tft.fillTriangle(72, 190, 60, 210, 84, 210, 0x0000);
    
        //Down Arrow
        tft.drawFastVLine(160, 190, 30, 0x000);
        tft.drawFastVLine(161, 190, 30, 0x000);
        tft.drawFastVLine(162, 190, 30, 0x000);
        tft.drawFastVLine(163, 190, 30, 0x000);
        tft.drawFastVLine(164, 190, 30, 0x000);
        tft.fillTriangle(162, 230, 150, 210, 174, 210, 0x0000);
    
        tft.setCursor(240, 220);
        tft.print("Ok");
        wait_4_key_press();
        if (KEY1) {
            if (kp <= 200)
                kp = kp + 1;
        }
        if (KEY2) {
            if (kp > 0)
                kp = kp - 1;
        }
        if (KEY3)
            break;
        }
      //  Edit_Parameters();
        Serial.println("KP Exit");
}

void set_Ki(){
    while (true) {
        tft.fillScreen(bg_color);
        tft.setTextColor(txt_clr);  tft.setTextSize(2);
        tft.setCursor(20, 30);
        tft.print("Set Ki");
        tft.setCursor(20, 70);
        tft.print("Ki = ");
        tft.print(ki);
        //Up Arrow
        tft.drawFastVLine(70, 200, 30, 0x000);
        tft.drawFastVLine(71, 200, 30, 0x000);
        tft.drawFastVLine(72, 200, 30, 0x000);
        tft.drawFastVLine(73, 200, 30, 0x000);
        tft.drawFastVLine(74, 200, 30, 0x000);
        tft.fillTriangle(72, 190, 60, 210, 84, 210, 0x0000);
    
        //Down Arrow
        tft.drawFastVLine(160, 190, 30, 0x000);
        tft.drawFastVLine(161, 190, 30, 0x000);
        tft.drawFastVLine(162, 190, 30, 0x000);
        tft.drawFastVLine(163, 190, 30, 0x000);
        tft.drawFastVLine(164, 190, 30, 0x000);
        tft.fillTriangle(162, 230, 150, 210, 174, 210, 0x0000);
    
        tft.setCursor(240, 220);
        tft.print("Ok");
        wait_4_key_press();
        if (KEY1) {
            ki += 1;
        }
        if (KEY2) {
            if (ki > 0)
                ki = ki - 1;
        }
        if (KEY3)
            break;
    }
  //  Edit_Parameters();
    Serial.println("KI Exit");
}

void set_Kd(){
    while (true) {
        tft.fillScreen(bg_color);
        tft.setTextColor(txt_clr);  tft.setTextSize(2);
        tft.setCursor(20, 30);
        tft.print("Set Kd");
        tft.setCursor(20, 70);
        tft.print("Kd = ");
        tft.print(kd);
        //Up Arrow
        tft.drawFastVLine(70, 200, 30, 0x000);
        tft.drawFastVLine(71, 200, 30, 0x000);
        tft.drawFastVLine(72, 200, 30, 0x000);
        tft.drawFastVLine(73, 200, 30, 0x000);
        tft.drawFastVLine(74, 200, 30, 0x000);
        tft.fillTriangle(72, 190, 60, 210, 84, 210, 0x0000);
    
        //Down Arrow
        tft.drawFastVLine(160, 190, 30, 0x000);
        tft.drawFastVLine(161, 190, 30, 0x000);
        tft.drawFastVLine(162, 190, 30, 0x000);
        tft.drawFastVLine(163, 190, 30, 0x000);
        tft.drawFastVLine(164, 190, 30, 0x000);
        tft.fillTriangle(162, 230, 150, 210, 174, 210, 0x0000);
    
        tft.setCursor(240, 220);
        tft.print("Ok");
        wait_4_key_press();
        if (KEY1) {
            kd += 1;
        }
        if (KEY2) {
            if (kd > 0)
                kd = kd - 1;
        }
        if (KEY3)
            break;
    }
  //  Edit_Parameters();
    Serial.println("KD Exit");
}

void set_InFilt(){
    while (true) {
        tft.fillScreen(bg_color);
        tft.setTextColor(txt_clr);  tft.setTextSize(2);
        tft.setCursor(20, 10);
        tft.print("Set Input Filter");
        tft.setCursor(20, 50);
        tft.print("InFilt = ");
        tft.print(InFilt_State);
        tft.setCursor(55, 210);
        tft.print("On  Off   Back");
        wait_4_key_press();
        if (KEY1) {
            InFilt_State = 1;
        }
        if (KEY2) {
            InFilt_State = 0;
        }
        if (KEY3)
            break;
    }
}

void set_OutFilt(){
    while (true) {
        tft.fillScreen(bg_color);
        tft.setTextColor(txt_clr);  tft.setTextSize(2);
        tft.setCursor(20, 10);
        tft.print("Set Output Filt");
        tft.setCursor(20, 50);
        tft.print("OutFilt = ");
        tft.print(OutFilt_State);
        tft.setCursor(55, 210);
        tft.print("On  Off   Back");
        wait_4_key_press();
        if (KEY1) {
            OutFilt_State = 1;
        }
        if (KEY2) {
            OutFilt_State = 0;
        }
        if (KEY3)
            break;
    }
}

// These small functions are used to draw and undraw the menu triangles
// The general syntax for a triangle is specifying the position of the 
// three vertices and the color. That is triangle(x0,y0,x1,y1,x2,y2, color)
void triangle1(int color){ tft.fillTriangle(30,40,10,25,10,55, color);}
void triangle2(int color){ tft.fillTriangle(30,65,10,50,10,80, color);}
void triangle3(int color){ tft.fillTriangle(30, 97, 10, 82, 10, 112, color);}
void triangle4(int color){ tft.fillTriangle(30, 135 ,10, 120, 10, 150, color);}

void Edit_Parameters(){
    Serial.println("Edit Param Enter");

EP_Start:    
    void (*fmenuList[4])(int color) = {triangle1, triangle2, triangle3, triangle4};
    tft.setFont(&FreeSans9pt7b);
    tft.setTextSize(2);
    tft.setCursor(20,20);
    tft.fillScreen(bg_color);
    tft.println("Menu");
    tft.setCursor(30, 50);
    (*fmenuList[0])(0x0000);
    //tft.fillCircle(10, 50, 10, 0x0000);
    tft.print("Change Kp");
    tft.setCursor(30, 80);
    tft.print("Change Ki");
    tft.setCursor(30, 110);
    tft.print("Change Kd");
    tft.setCursor(30, 150);
    tft.print("Diagnostics");
    tft.setCursor(30, 210);
    tft.print("Down");
    tft.setCursor(145,210);
    tft.print("Ok");
    tft.setCursor(210,210);
    tft.print("Back");
    while ((digitalRead(PB1) == 0) || (digitalRead(PB2) == 0) || (digitalRead(PB3) == 0) ){ // wait for all keys up
        Run_PID();   // Call PID Loop to prevent runaway while waiting for keys
        strobe_WDT();
    }

    int iconCtrl = 0;
    int iconClean = 3; 
  
    for(;;){
        Run_PID();   // Call PID Loop to prevent runaway while waiting for keys
        strobe_WDT();
  
        if (digitalRead(PB1) == 0){
            iconCtrl = (iconCtrl +  1) % 4;
            iconClean = (iconClean + 1) % 4;
            (*fmenuList[iconCtrl])(0x0000);
            (*fmenuList[iconClean])(bg_color);
                      
            delay_WDT(250);
        }if (digitalRead(PB2) == 0){
            switch(iconCtrl){
                case 0:
                    set_Kp();
                    break;
                case 1:
                    set_Ki();
                    break;
                case 2:
                    set_Kd();
                    break;
                case 3:
                    get_info();
                    break;
                default:
                    //This case should literally never happen as iconCtrl should always be 0,1,2, or 3.
                    //But it is good form to expect the unexpected
                    //If somehow we do end up here, we just exit the secret menu
                    reDrawMain();
                    break;
            }
            goto EP_Start;      // redraw and start over
        }if (digitalRead(PB3) == 0){
            reDrawMain();
            Serial.println("Edit Param Return");
            return;
        }
    }
    Serial.println("Edit Param Exit");
}
