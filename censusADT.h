#ifndef __listADT_h_
#define __listADT_h_

typedef struct censusCDT * censusADT;

censusADT newCensus(int totalProv, char ** provList);

void freeCensus(censusADT c);

int addElem(censusADT c, int state, char *dept, char *prov); //Podria pasar la provincia como el indice del vector. FrontEnd/BackEnd

#endif