#include <fstream>
#include <iostream>

using namespace std;

//log error message
void error(string message)
{
  ofstream fout("error.log", ofstream::app);
  fout << "***Error*** " << message;
  fout.close();
  //cout << "***Error*** " << message;
}

//log information message
void info(string message)
{
  ofstream fout("info.log", ofstream::app);
  fout << "***Info*** " << message;
  fout.close();
  //cout << "***Info*** " << message;
}