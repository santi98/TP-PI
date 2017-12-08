#include <stdio.h>
#include "censusADT.h"

int main(void){
	censusADT c = newCensus();

	addElem(c, 1, "VILLA SANTA ROSA", "CHUBUT");
	addElem(c, 3, "25 DE MAYO", "MENDOZA");
	addElem(c, 4, "9 DE JULIO", "LA PAMPA");
	addElem(c, 2, "HERRERA", "RIO NEGRO");
	addElem(c, 1, "ANCASTI", "CHUBUT");

	freeCensus(c);
}