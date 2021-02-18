using namespace std;
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>

class File
{
   public :
       File (const File& otherFile);//copy constructor (deep copy)
       File (const char name[], const char pmode[],
             const char size[], const char stamp[]);
       File ();//default constructor
       File & operator = (const File& otherFile);//operator overloading with deep copy

       //getters
       string getSize() const;
       string getName() const;
       string getPmode() const;
       string getStamp() const;
       int recordSize() const;

       //marks something as a directory and not a file
       void flagAsDir();
       bool isADir() const;

   private :
       char name[81], pmode[5], size[7], stamp[16];//arrays are pointers in disguise...
       bool ADir;
};
