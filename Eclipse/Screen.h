#ifndef Screen_h
#define Screen_h

#include "Adafruit_ILI9341.h"
  
class Screen {
  public:

    Screen(int8_t csPin, int8_t dcPin);
    void TurnOff();
    void ShowSplashScreen(void (*delayRoutine)(int));
    void Test();
    void ShowPidMenu();
    void ShowTriangleOne();
    void ShowTriangleTwo();
    void ShowTriangleThree();
    void ShowTriangleFour();
    void ShowPidComponentMenu(const char* pidOption, double pidValue);
    void UpdatePidComponentValue(const char* pidOption, double oldValue, double newValue);
    void ShowSetpointMenu(double setpoint);
    void ShowDiagnostics(double current, double volts, double power, double resistance, double kp, double ki, double kd);
    void ShowMain(double setpoint, double temperature);
    void PrintTftDataToSerial();
    void ShowPolyarcNotFound();
    void ShowScreenTestMenu();
    void ShowHeaterTestMenu();
    void ShowHeaterTestDriveHeader(int drive);
    void ShowHeaterTestDriveResults(int drive, double temperature, double volts, double current, double power, double resistance);
    void ShowWatchdogTestMenu();
    void ShowAliTestMenu();
    void ShowMoreAliTestMenu();
    void ShowGoodbye();
    void ShowTestingHeader();
    void ShowUseLastSetpointQuestion(double setpoint);
    void UpdateSetpoint(double oldSetpoint, double newSetpoint);
    void UpdateTemperature(double oldTemperature, double newTemperature);
    void AddTemperatureStar();
    void RemoveTemperatureStar();
    void Pause();
    void Resume();
    
  private:
    Adafruit_ILI9341 _tft;
    int setpointLocationXY[2];
    void UpdateSetpointLocation(int x, int y);
    void Print(int x, int y, char* value, int color = ILI9341_BLACK);
    void Print(int x, int y, int value, int color = ILI9341_BLACK);
    void Print(int x, int y, double value, int decimalPlaces, int color = ILI9341_BLACK);
    void SetTriangleOneColor(int color);
    void SetTriangleTwoColor(int color);
    void SetTriangleThreeColor(int color);
    void SetTriangleFourColor(int color);
    void DrawUpArrow(int color);
    void DrawDownArrow(int color);
    void DrawNumberedMenu(char* header, char* buttonOne, char* buttonTwo, char* buttonThree);
    unsigned long testFilledRoundRects();
    unsigned long testRoundRects();
    unsigned long testFilledTriangles();
    unsigned long testTriangles();
    unsigned long testCircles(uint8_t radius, uint16_t color);
    unsigned long testFilledCircles(uint8_t radius, uint16_t color);
    unsigned long testFilledRects(uint16_t color1, uint16_t color2);
    unsigned long testRects(uint16_t color);
    unsigned long testFastLines(uint16_t color1, uint16_t color2);
    unsigned long testLines(uint16_t color);
    unsigned long testText();
    unsigned long testFillScreen();
};

#endif