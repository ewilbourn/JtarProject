//Emily Wilbourn
//CSC 310 - Project 2 - jtar implementation
//3/1/2021

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> /*used for converting string to c-string*/
#include <stdio.h> /*for strcpy*/
#include "file.h" /*for the File objects*/

using namespace std;
void helpInfo();
vector<File> storeFileInfo(vector <string> v);
bool fileExist(string fileName);
File parseStat(string filename);
vector<string> splitOnWhiteSpace(string input);
void fillTarFile(string tarfile, vector<File>v);

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

			//remove the file.txt file from the directory
			system("rm file.txt");
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

//call the stat command on a file with the "system" call and iterate through the information
//that is stored from this 
//precondition: pass in a string object that has the filename in it
//postcondition: return a File object 
File parseStat(string filename)
{
	string command = "stat " + filename + " > stats.txt";
	fstream stat_file("stats.txt", ios::in);
	system(command.c_str());
	//variable to read in key words to find the data we want
	string key_word;

	//variable to store critical info for file class in
	string file_info;
	
	//counter to track the number of times we see the "Access: " key_word
	int times_access_seen;

	//character arrays for our file object	
	char name_array[81];
	char size_array[7];
	char pmode_array[5];
	char stamp_array[16];
	//while the file ptr is still alive, keep iterating through the file
	while(stat_file)
	{
		stat_file >> key_word;
		
		if (key_word == "Access:")
		{
			times_access_seen += 1;
			
			getline(stat_file,file_info);
			//call the method to get the protection mode
			//
			//method to pass in string and return c-string
			if (times_access_seen == 1)
			{ 
				//get position of the ( in the string
				size_t pos = file_info.find("("); 
				string protection_mode = (file_info.substr(pos+1, 4));
				
				//get position of the / in the string
				pos = file_info.find("/"); 
				string file_or_dir = file_info.substr(pos+1,1);
				
				bool isDirectory = (file_or_dir == "d" ? true : false);
				//copy the string with our file info into a char array
				strcpy(pmode_array, protection_mode.c_str());
				
			}
			//call the method to get the time stamp
			//
			//method that passes in a string and returns a c-string
			else if (times_access_seen == 2)
			{
				int dash_pos = file_info.find("-");
				int colon_pos = file_info.find(":");
				string year = file_info.substr(0,dash_pos);

				//dash_pos+1 is the starting place because we want it to 
				//start the character after the dash
				string month = file_info.substr(dash_pos+1,2);
				
				//dash_pos+4 because the the dash position includes the first
				//dash (+1), then two characters for the month (+2), and 1 
				//character for the second dash (+1)
				string day = file_info.substr(dash_pos+4, 2);
				string hour = file_info.substr(colon_pos-2, 2);
				string minute = file_info.substr(colon_pos+1, 2);
				string second = file_info.substr(colon_pos+4, 2);

				string full_date = year+month+day+hour+minute+"."+second;
				//copy the string with our full date into a char array
				strcpy(stamp_array, full_date.c_str());
			}
		}
		else if (key_word == "Size:")
		{
			stat_file >> file_info;
			//copy the string with our file info into a char array
			strcpy(size_array, file_info.c_str());
		}
		
	}		
	//copy the string with our file info into a char array
	strcpy(name_array, filename.c_str());
	
	//instantiate a file object with the character arrays
	File f(name_array, pmode_array, size_array, stamp_array);
	
	//remove the file.txt file from the directory
	system("rm stats.txt");
	return f;
}

//method to split a string on the white space and put all words into a
//vector of strings
vector<string> splitOnWhiteSpace(string input)
{
	cout << "splitting on white space" << endl;
        vector <string> words;
        string local_word;
        for (int i = 0; i < input.size(); i++)
        {
                if(input[i] != ' ')
                {
                        local_word += input[i];
                }
                else
                {
        		cout << "word: " << local_word << endl; 
	                words.push_back(local_word);
                        local_word = "";
                }
        }
	words.push_back(local_word);
  	cout << "stop splitting on white space" << endl;
	return words;
}

//method to fill our tarfile (binary file)
void fillTarFile(string tarfile, vector<File>v)
{
	fstream outfile (tarfile, ios::out | ios::binary);
	for (int i = 0; i < v.size(); i++)
	{
		outfile.write( (const char *) &v[i], sizeof (File));	
	}
}
