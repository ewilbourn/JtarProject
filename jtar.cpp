//Emily Wilbourn
//CSC 310 - Project 2 - jtar implementation
//3/1/2021

#include <iostream>
#include <vector>
#include <fstream>
#include "file.h" /*for the File objects*/

using namespace std;
void helpInfo();
void storeFileInfo(vector <string> v);
bool fileExist(string fileName);

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
			vector <string> files;
			//start the iterator at 3 because argv[2] is the name 
			//of the tarfile (binary file) we're loading info into
			for(int i = 3; i < argc; i++)
			{
				files.push_back(argv[i]);			
			}	
			storeFileInfo(files);
		}
		else
		{
			cout << "Error: Only " << argc << " arguments were entered when at least 4 were needed." << endl;
		}
	} 
	return 0;
}

//method to return the information about how the different tar commands work
//precondition: pass nothing in
//postcondition: the information about the different options is printed to the 
//screen
void helpInfo()
{
/*	cout << "\n\n------------------------OPTIONS--------------------------"<< endl;
	cout << "1. a.out -cf tarfile file1 dir1...\n" << endl;

	cout << "This specifies the executable (a.out) to make a tar file\n"	
		"named tarfile based on the files or directories following\n"
		"the name of the tarfile." << endl; 
	cout << "\n" << endl;	

	cout << "2. a.out -tf tarfile\n" << endl;
	cout << "This lists the names of all the files that have been packed\n"	
		"into a tar file. It does not recreate anything." << endl;
	cout << "\n" << endl;

	cout << "3. a.out -xf tarfile\n" << endl;
	cout << "This reads in a tar file and recreate all the files saves in\n"	
		"that tarfile.." << endl;
	cout << "\n" << endl;*/
	
	//display the contents of the help file that Dr. Digh shared 
	//with us to the screen
	system("cat help");
}

//method to feed all the file protection data into a file
//precondition: pass in a vector of file names (string)
//postcondition: return nothing, but a file called file.txt will have all the 
//file protection information for each file in the vector that was passed in
void storeFileInfo(vector <string> v)
{
	cout << "v.size(): " << v.size() << endl;
	for (int i = 0; i < v.size(); i++)
	{
		if (fileExist(v[i]))
		{
			string command = "ls -l " + v[i] + " > file.txt";
			system(command.c_str());
		
			//create a file object to read in the file information 
			//from file.txt
			fstream file("file.txt", ios::in);
			string line; 
	
			//get the line of information from file.txt and store it
			//in a string so we can grab information from it
			getline(file, line);
			cout << line << endl;
		}
		else
		{
			cout << "jtar: " << v[i] << ": No such file or directory" << endl;
			break;
		}
		
	}
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

