#ifndef WatchdogTimer_h
#define WatchdogTimer_h
  
class WatchdogTimer {
  public:
    WatchdogTimer(int pin);
    void Refresh();
    void Delay(unsigned long milliseconds);
  private:
    int _pin;
    bool _state;
};

#endif
