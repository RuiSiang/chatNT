#include <fstream>
#include <iostream>

using namespace std;

void error(string message)
{
  //ofstream fout("error.log", ofstream::app);
  //fout << message;
  //fout.close();
  cout << "***Error*** " << message;
}

void info(string message)
{
  //ofstream fout("info.log", ofstream::app);
  //fout << message;
  //fout.close();
  cout << "***Info*** " << message;
}