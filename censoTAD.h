#ifndef __listADT_h_
#define __listADT_h_

typedef struct censoCDT * censoCDT;

censoCDT creaCenso(int cantProvincias);

void freeCenso(censoCDT c);

int agregaElemento(censoCDT c, int estado, char* departamento, char* provincia); //Podria pasar la provincia como el indice del vector. FrontEnd/BackEnd

#endif