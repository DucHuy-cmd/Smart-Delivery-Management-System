#ifndef COORD_H
#define COORD_H
#include "models.h"

void dispatchOrders(order **headO, shipper **headS);
void displayMap(char* sCode, double curW, double maxW);
bool find_path(MapPoint start, MapPoint end);
void run_animation(MapPoint start, MapPoint end, char* sCode, double curW, double maxW);
void simulateDeliveryRoute(order **headO, shipper **headS);
int Smart_Coordination(order **headO, shipper **headS);

#endif
