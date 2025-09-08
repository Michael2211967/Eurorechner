/* mehrwert.c */
#include "mehrwert.h"

float Brutto,Netto,MwSt,Steuer;

float berechne_netto(float Brutto, float MwSt)
{
  float Netto=100*Brutto/(100+MwSt);
  return(Netto);
}