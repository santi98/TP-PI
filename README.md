# Census Processing Program

Reads from the standard input until EOF, processing census data and outputs 3 files

Country.csv  -  Province.csv  - Department.csv

Each line should hold 4 comma separated values, them being:

Economic status Indicator

     #0: No data
  
     #1: Occupied
  
     #2: Unoccupied
  
     #3: Inactive
  

Unique HOME ID

     Should be a number of type unsigned long int.

Department name

     A string formed by standard ASCII characters.
  
Province name

     A string formed by standard ASCII characters.
  
  
Example line:
```
3,654085,Cruz del Eje,Cordoba
```


### Installing

To compile:

```
gcc censusADT.c main.c -o censusProgram -Wall -pedantic -std=c99 -fsanitize=address
```

To run:

```
./censusProgram
```
and data should be entered manually.
or
```
./censusProgram < census.csv
```
where census.csv is a properly formatted .csv file.





