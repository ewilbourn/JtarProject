//Emily Wilbourn
//CSC 310 - Project 2 - jtar implementation
//3/1/2021

#include <iostream> /*standard input/output*/
#include <vector> /*vector object*/
#include <fstream>/*file objects*/
#include <cstring> /*used for converting string to c-string*/
#include <stdio.h> /*for strcpy*/
#include "file.h" /*for the File objects*/
/*these two libraries needed for getting the infromation about each file*/
#include <sys/stat.h>//has the stat struct info
#include <sys/types.h>
#include <sstream>/*for converting int to string*/
#include <experimental/filesystem> /*for recursively iterating through file*/

using namespace std;

//found this here: https://en.cppreference.com/w/cpp/experimental/fs/recursive_directory_iterator 
namespace fs = std::experimental::filesystem;

void helpInfo();
vector<File> storeFileInfo(vector <string> v);
bool fileExist(string fileName);
File parseStat(string filename);
void fillTarFile(string tarfile, vector<File>v);
string getFileContents(string filename);
string int_to_str(int input);

int main(int argc, char *argv[])
{
	string arg1 = argv[1];
	if(arg1 == "--help")
	{
		helpInfo();	
	}
	else if(arg1 == "-cf")
	{
		if (argc >= 4)
		{
			cout << "argv[2]: " << argv[2] << endl;
			string tarfile = argv[2];
			vector <string> files;
			//start the iterator at 3 because argv[2] is the name 
			//of the tarfile (binary file) we're loading info into
			for(int i = 3; i < argc; i++)
			{
				files.push_back(argv[i]);			
			}	
			vector <File> file_objs = storeFileInfo(files);
			fillTarFile(tarfile, file_objs);
		}
		else
		{
			cout << "Error: Only " << argc << " arguments were entered when at least 4 were needed." << endl;
		}
	}
	else if(arg1 == "-tf")
	{

	} 
	else if(arg1 == "-xf")
	{

	}
	return 0;
}

//method to return the information about how the different tar commands work
//precondition: pass nothing in
//postcondition: the information about the different options is printed to the 
//screen
void helpInfo()
{
	//display the contents of the help file that Dr. Digh shared 
	//with us to the screen
	system("cat help");
}

//method to feed all the file protection data into a file
//precondition: pass in a vector of file names (string)
//postcondition: return a vector of File objects
vector<File> storeFileInfo(vector <string> v)
{
	vector <File> file_objs;
	for (int i = 0; i < v.size(); i++)
	{
		if (fileExist(v[i]))
		{
			File f = parseStat(v[i]);	
			file_objs.push_back(f);		
		}
		else
		{
			cout << "jtar: " << v[i] << ": No such file or directory" << endl;
			break;
		}
		
	}
	return file_objs;
}

//method to determine if a file exists
//precondition: pass in file name (string)
//postcondition: true if file is valid, false if file is invalid
bool fileExist(string fileName)
{
        //test to see if the fileName is a valid file
        ifstream test(fileName);

	return (test.is_open() ? true : false);
}

//call the lstat command that will get the stat information for a file - this came
//from Dr. Digh's utility.cpp file
//precondition: pass in a string object that has the filename in it
//postcondition: return a File object 
File parseStat(string filename)
{
	struct stat buf;
	
	//character arrays for our file object	
	char name[81];
	char size[7];
	char pmode[5];
	char stamp[16];
        
	//this makes a call to our file being passed
	lstat (filename.c_str(), &buf);

	//fill up the char arrays for the File object
	
	//copy the string with our filename into a char array
	strcpy(name, filename.c_str());
	
	//copy the size into a string and copy the string into char array	
	string s_size = int_to_str(buf.st_size);
	strcpy(size, s_size.c_str());
	
	//copy the size into a string and copy the string into char array	
	string s_pmode = int_to_str(buf.st_mode & S_IRWXU);
	s_pmode += int_to_str(buf.st_mode & S_IRWXG);
	s_pmode += int_to_str(buf.st_mode & S_IRWXO);
	
	strcpy(pmode, s_pmode.c_str());
	
	//copy the modification time to the stamp char array
	strftime(stamp, 16, "%Y%m%d%H%M.%S", localtime(&buf.st_mtime));
	
	//instantiate a file object with the character arrays
	File f(name, pmode, size, stamp);
	return f;
}

//method to fill our tarfile (binary file)
//precondition: pass in the file name for the tarfile (for creating an
//fstream object) and a vector of Files that we are loading into the tarfile
//postcondition: return nothing, but the tarfile is loaded with all the File objets
void fillTarFile(string tarfile, vector<File>v)
{
	fstream outfile (tarfile, ios::out | ios::binary);
	int numFiles = v.size();
	
	//write out how many files are contained in the tarfile
	outfile.write( (const char *) &numFiles, sizeof (int));
	
	//write out all the File objects to the tarfile
	for (int i = 0; i < v.size(); i++)
	{
		outfile.write( (const char *) &v[i], sizeof (File));	
	
		//code to put the contents of the file into a char string 
		int fileSize = stoi(v[i].getSize());	
		char *str = new char[fileSize];

		//write the char string out to the binary file
		outfile.write( (const char *) &str, sizeof(str));		
	}
}

//a method that passes in the file name and returns a string
//precondition: pass in the filename for our file object (string)
//postcondition: return a string that holds all the contents of the file 
string getFileContents(string filename)
{
	fstream infile (filename, ios::in);

	//string to hold all the contents of the file	
	string fileContents;

	//string to hold each individual line of the file we're grabbing
	//with getline
	string line;
	
	while(infile)
	{
		getline(infile, line);
		fileContents += line;
	}
	return fileContents;	
}

//method to convert a given integer to a string value
//precondition: pass in an integer we want converted to a string
//postcondition: return a string representation of the integer that was passed in
string int_to_str(int input)
{
	stringstream s;

	//read integer into stringstream
	s << input;

	string s_input;

	//read integer in stringstream into a string
	s >> s_input;

	return s_input;
}

