#ifndef Display_H_
#define Display_H_

#include <TFT_eSPI.h>

class Display{
 private:
 TFT_eSPI* tft;

 public:
Display();
~Display();

void initTFT();
void centerMsg(String text);
void healthUpdates(String beatsPerMinute, String beatAvg, String status);
};



#endif