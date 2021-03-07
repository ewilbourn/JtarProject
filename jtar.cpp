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
void parseStat(string filename);
vector<string> splitOnWhiteSpace(string input);

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
			parseStat(v[i]);			
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

//call the stat command on a file with the "system" call and iterate through the information
//that is stored from this 
//
//postcondition: return a vector of File objects from this method; or return a 
//vector of file structs that will later be added to file objects?
void parseStat(string filename)
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
	
	//while the file ptr is still alive, keep iterating through the file
	while(stat_file)
	{
		stat_file >> key_word;
		
/*		if(key_word == "Block:")
		{
			getline(stat_file, file_info);
			//split the string of data we got in file_info into a vector
			vector <string> v = splitOnWhiteSpace(file_info);
			//the last index of our vector will tell us if it's a 
			//file or directory
			cout << "v.size(): " << v.size() << endl;
			bool isDirectory = (v[v.size()] == "file" ? false : true);
			cout << "file info: " << v[v.size()] << endl;
			cout << "isDirectory: " << boolalpha << isDirectory << endl;	
			cout << "file_info: " << file_info << endl;		
		}*/
		if (key_word == "Access:")
		{
			times_access_seen += 1;
			
			cout << "key_word: " << key_word << endl;
			getline(stat_file,file_info);
			if (times_access_seen == 1)
			{ 
				cout << "first time seeing access" << endl;
				cout << "file_info: " << file_info << endl;
				
				//get position of the ( in the string
				size_t pos = file_info.find("("); 
				string protection_mode = (file_info.substr(pos+1, 4));
				cout << "protection_mode: " << protection_mode << endl;
				//get position of the / in the string
				pos = file_info.find("/"); 
				string file_or_dir = file_info.substr(pos+1,1);
				
				bool isDirectory = (file_or_dir == "d" ? true : false);
				
				cout << "file_or_dir: " << file_or_dir << endl;
				cout << "isDirectory: " << boolalpha << isDirectory << endl;	
			}

			else if (times_access_seen == 2)
			{
				int dash_pos = file_info.find("-");
				int colon_pos = file_info.find(":");
				string year = file_info.substr(0,dash_pos);
				cout << "year: " << year << endl;

				//dash_pos+1 is the starting place because we want it to 
				//start the character after the dash
				string month = file_info.substr(dash_pos+1,2);
				
				//dash_pos+4 because the the dash position includes the first
				//dash (+1), then two characters for the month (+2), and 1 
				//character for the second dash (+1)
				string day = file_info.substr(dash_pos+4, 2);
				cout << "month: " << month << endl;
				cout << "day: " << day << endl;

				string hour = file_info.substr(colon_pos-2, 2);
				cout << "hour: " << hour << endl;
	

				string minute = file_info.substr(colon_pos+1, 2);
				
				string second = file_info.substr(colon_pos+4, 2);
				cout << "minute: " << minute << endl;
				cout << "second: " << second << endl;
				//cout << "file_info: " << file_info << endl;
					
			}
			cout << "file_info: " << file_info << endl;
		}
	}		
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

