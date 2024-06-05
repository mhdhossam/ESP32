#include "Display.h"

Display::Display(){
    tft= new TFT_eSPI();
}

Display::~Display(){
   delete tft;
}

void Display::initTFT(){
    tft->init();
    tft->fillScreen(TFT_BLACK);
    tft->setRotation(1);
    tft->setTextColor(TFT_WHITE,TFT_BLACK);
    tft->setTextDatum(MC_DATUM);
    tft->setFreeFont(&Orbitron_Light_24);
}

void Display::centerMsg(String text){
    tft->drawString(text,tft->width()/2, 60);
}

void Display::healthUpdates(String beatsPerMinute,String beatAvg ,String status){
tft->setTextPadding(tft->width());
tft->drawString(beatsPerMinute,tft->width()/2,40);
tft->drawString(beatAvg,tft->width()/2,70);
tft->drawString(status,tft->width()/2,110);
}