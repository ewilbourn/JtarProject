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
#include <string>/*for find_last_of method*/
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
void readTarfile(string tarfile, string arg);
void extractFile(File f, string fileContents);
void createNewDirectory(string name);
void updateInfo(File f);

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
			string tarfile = argv[2];
			vector <string> files;
			//start the iterator at 3 because argv[2] is the name 
			//of the tarfile (binary file) we're loading info into
			for(int i = 3; i < argc; i++)
			{
				if(fileExist(argv[i]))
                                        files.push_back(argv[i]);
                                else
                                        cout << argv[i] << " does not exist. Exiting program." << endl;
			}	
			vector <File> file_objs = storeFileInfo(files);
			fillTarFile(tarfile, file_objs);
		}
		else
		{
			cout << "Error: Only " << argc << " arguments were entered when at least 4 were needed." << endl;
		}
	}
	else if (arg1 == "-tf" || arg1 == "-xf")
	{
		if (argc >= 3)
		{
			//Pass in the filename and the file option the user passed in.
			//The two file options that we could have are "-tf" or "-xf".
			readTarfile(argv[2], arg1);
		}
		else
		{
			cout << "Error: Only " << argc << " arguments were entered when at least 3 were needed." << endl;
		}
	} 
	else
	{
		cout << "Invalid option entered. Use --help to see valid options." << endl;
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
			//if the file is a directory, then recursively iterate
			//through all the files in the directory
			if (f.isADir())
			{
				string directory = v[i];
				//create a path file object that can be used to iterate over the directory
				fs::path pathObj (directory);

				//for loop to recursively iterate through a given file path
				//and create file objects for each file in a given directory
				for(const auto& p: fs::recursive_directory_iterator(pathObj))
				{
					File f1 = parseStat(p.path());
					file_objs.push_back(f1);
				}	
			}
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
	string s_pmode = int_to_str((buf.st_mode & S_IRWXU) >> 6);
	s_pmode += int_to_str((buf.st_mode & S_IRWXG) >> 3);
	s_pmode += int_to_str(buf.st_mode & S_IRWXO);
	strcpy(pmode, s_pmode.c_str());
	
	//copy the modification time to the stamp char array
	strftime(stamp, 16, "%Y%m%d%H%M.%S", localtime(&buf.st_mtime));

	//instantiate a file object with the character arrays
	File f(name, pmode, size, stamp);

	//if the given file is a directory, then mark it as a directory
	if (S_ISDIR(buf.st_mode))
		f.flagAsDir();
	
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
	
		if(!v[i].isADir())
		{
			//integer to hold integer representation of the fileSize
			int fileSize = stoi(v[i].getSize());	

			//code to put the contents of the file into a char string 
			char *str = new char[fileSize];
		
			//string to hold all the contents of the file	
			string contents = getFileContents(v[i].getName());
			
			strcpy(str, contents.c_str());
				
			//write the char string out to the binary file
			outfile.write(str, stoi(v[i].getSize()));	
		}
	}
}

//method that prints out the information stored in a tar file
//precondition: pass in the name of the binary tarfile and the option
//for the tarfile the user entered on the command line
//postcondition: returns nothing, but the vector with the 
void readTarfile(string tarfile, string arg)
{
	fstream infile(tarfile, ios::in|ios::binary);
		
	if (fileExist(tarfile))
	{
		//extract the number of files in the tarfile so we can 
		//iterate over them
		int numFiles;
		infile.read((char *) &numFiles, sizeof(int));
		//iterate over all the files in the directory
		for(int i = 0; i < numFiles; i++)
		{
			//read in the File object
			File f;
			infile.read((char *) &f, sizeof(File));
		
			//instantiate a c-string to hold the contents of file	
			int len = stoi(f.getSize());
			char* str = new char[len];
			if(!f.isADir())
			{
				//read in the contents of each File	
				infile.read(str, len);
			}
			//command to print out the names of the files stored in the
			//tarfile	
			if(arg == "-tf")
			{
				cout << f.getName() << endl;
			}

			//command to extract all the files from the tarfile
			else if(arg == "-xf")
			{
				if(f.isADir())		
					extractFile(f, "");
				else
				{	
					//cout << "file contents before call: " << str << endl;
					extractFile(f,str);
				}
			}
			delete [] str;
		}
	}	
	else
	{
		cout << tarfile << " does not exist." << endl;
	}	
}
//a method that extracts all files from a tarfile one by one
//precondition: pass in the File object for the file we are extracting
//postcondition: return nothing, but all the files are extracted and put in their 
//proper directories
void extractFile(File f, string fileContents)
{
	if(f.isADir())
	{
		//if the directory does not already exist, then make new directory
		createNewDirectory(f.getName());
	
		//update the time stamp and file permissions for each file being passed in
		updateInfo(f);
		
		//change into the proper directory to create the file
		string cd_command = "cd " + f.getName();
		system(cd_command.c_str());
	}	
	else
	{
		string fileName = f.getName();
		
		//find the position of the last "/" in the file name
		size_t lastSlash = fileName.find_last_of("/");
		
		if(lastSlash <= fileName.size())
		{
			string directory_path = fileName.substr(0, lastSlash);

			//if the directory does not already exist, then make new directory
			createNewDirectory(directory_path);

			//change into the proper directory to create the file
			string cd_command = "cd " + directory_path;
			system(cd_command.c_str());
		}
		string new_filename = fileName.substr(lastSlash+1);
		fstream new_file(fileName, ios::out);
		//if the fileContents grabs more information than just the actual
		//text that was originally in the file, then this prevents that extra
		//garbage from being printed.
		for(int i = 0; i < stoi(f.getSize()); i++)
			new_file.put(fileContents[i]);
		
		//update the time stamp and file permissions for each file being passed in
		updateInfo(f);
	}
	
}
//method to create a new directory if the given directory does not already exist
//precondition: pass in the name of the directory
//postcondition: return nothing, but the new directory is created
void createNewDirectory(string fileName)
{
	//if the directory does not already exist, then make new directory
	if(!fileExist(fileName))
	{
		string command = "mkdir " + fileName;
		system(command.c_str());
	}
}

//method to update the file permission and time stamp for a given file
//precondition: pass in the File object
//postcondition: return nothing, but the file permission and time stamp
//are updated
void updateInfo(File f)
{
	//update the time stamp and file permissions for each file being passed in
	string chmod_command = "chmod " + f.getPmode() + " "+ f.getName();
	system(chmod_command.c_str());
	string touch_command = "touch -t " + f.getStamp() + " " + f.getName();
	system(touch_command.c_str());
}

//a method that passes in the file name and returns a string
//precondition: pass in the filename for our file object (string)
//postcondition: return a string that holds all the contents of the file 
string getFileContents(string filename)
{
	ifstream infile (filename.c_str());

	ostringstream buf;
	char ch;
	
	//read the file character by character into a buffer
	while (buf && infile.get(ch))
	{
		buf.put(ch);
	}
	//turn the buffer into a string and return it
	return buf.str();
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

