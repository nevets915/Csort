#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

/*
csort.cpp is a sorting program that takes in exactly two arguements.  The second arguement is used to 
determine the sorting order of input from the user.  The sorting order can specify what columns are sorted,
ascending, decending, and sorting by floating point or not.  A column cannot be specified to be sorted more than once

The program will then keep getting input from the user and signify each column separated by commas.  The number
of input columns must be equal to or greater than the largest sorted column specified in the intial arguements.
The program will also check if a inputted column can be sorted by floating point if that column has already been
specified to be sorted by floating point or not.

Lastly, the program sorts the input by row by comparing the row's column in the order specified by the column
sorting order.  If a column's value is equal for two rows, the next portion of the sorting order is used to
specify the column we should sort the two rows by.  After sorting, the program prints out the sorted input to
standard out.

Any errors will be printed to standard error with a message specifying the issue.  
*/


/*Method for trimming off leading and following whitespace from a given string.  

It returns the original content of the string without the leading and following whitespace*/
string trim(string input_str){
	string output_str;
	unsigned i;
	for(i = 0; i < input_str.length(); i++){
		if(input_str[i] != ' '){
			output_str = output_str + input_str[i];
		}
	}
	return output_str;
}

/*Struct which serves as the compare function for sorting the input according to the
command line arguement.

- It gets the container of sorting commands and sorts each row based on each command.  
- Each command can ask to sort by a specific column number, string or floating point.
- If two values are equal, it will use the next sorting command to determine the order
the rows should be in.*/
struct cmp {
	vector<string> commands;
	cmp(const vector<string> str_args) : commands(str_args) {}
	
	bool operator()(const vector<string>& lhs, const vector<string>& rhs)const 
	{
		for(auto &command: commands){
			int col;
			double lhs_num,rhs_num;
			string lhs_str,rhs_str;
			col = abs(std::stoi(command));
			if(command[command.length() - 1] == 'f'){
				lhs_num = stoi(lhs.at(col-1)); 
				rhs_num = stoi(rhs.at(col-1));
				if(lhs_num != rhs_num){
					if(command[0] == '-'){
						return lhs_num > rhs_num;
					}
					else{
						return lhs_num < rhs_num;
					}
				}else{
					continue;
				}
			}
			else{
				lhs_str = trim(lhs[col-1]);
				rhs_str = trim(rhs[col-1]);
				if(lhs_str.compare(rhs_str) != 0){
					if(command[0] == '-'){
						return lhs_str > rhs_str;
					}
					else{
						return lhs_str < rhs_str;
					}
				}else{
					continue;
				}
			}
		}
		return false;
	}
};

/*Method for validating the command line arguements which specify how input should be sorted.

Each command must specify a column number, separated by commas and may specify if it should be 
sorted by a floating point with the character 'f'
This method checks for invalid sorting commands: having a comma at the end, two commas in a row
comma in the beginning, etc.  It will print the specified error to standard error.*/
bool valid_command(const string& input_args, vector<string>& str_args, vector<int>& num_args){
	int col_num;
	size_t i;
	char first_char, second_char;
	string col_str;
	
	if(input_args[input_args.length() - 1] == ','){
		cerr << "Error: Cannot have a comma at the end." << endl;
		return false;
	}
	
	if(input_args[0] == ','){
		cerr <<"Error: Cannot have a comma at the beginning." << endl;
		return false;
	}
	
	for(i = 0;i < input_args.length()-1; i++){
		if(input_args[i] == ',' && input_args[i+1]== ','){
			cerr <<"Error: Cannot have two commas in a row."<< endl;
			return false;
		}
	}
	
	stringstream iss(input_args);
	if(!(iss >> col_num)){
		cerr << "Error: No column number specified." << endl;
		return false;
	}
	
	stringstream ss(input_args);
	while(1){
		if(ss >> col_num){
			if(col_num == 0){
				cerr << "Cannot sort on column 0" << endl;
				return false;
			}else if(ss >> first_char){
				//Determine what character we got
				if(first_char == ','){
					col_str = to_string(col_num);
					str_args.push_back(col_str); //Add the column number to the vector
					num_args.push_back(abs(col_num)); 
				}else if(first_char == 'f'){
					if(ss >> second_char){
						if(second_char==','){
							col_str = to_string(col_num);
							str_args.push_back(col_str + first_char); //Add the column number to the vector
							num_args.push_back(abs(col_num));
						}else{
							cerr << "Error: Incorrect column format." << endl;
							return false;
						}
					}else{
						col_str = to_string(col_num); //Add last arguement condition
						str_args.push_back(col_str + first_char); 
						num_args.push_back(abs(col_num));
						break;
					}
				}else{
					cerr << "Error: Incorrect column format." << endl;
					return false;
				}
			}else{
				col_str = to_string(col_num);//Add last arguement condition
				str_args.push_back(col_str); 
				num_args.push_back(abs(col_num));
				break;
			}
		}
	}
	//Check for duplicate columns sorted.
	sort(num_args.begin(), num_args.end());
	if(unique(num_args.begin(), num_args.end()) != num_args.end()){
		cerr << "Error: Duplicate columns specified." << endl;
		return false;
	}else{
		return true;
	}
}

/*Method for validating the input given by the user.  

It checks whether the user hasn't given enough input columns to be sorted
and checks if the user input can be sorted as a floating point (if specified
by the inital arguements).*/
bool valid_input(const vector<string>& row, vector<string>& str_args, vector<int>& num_args){
	unsigned min_column_size = (unsigned)num_args.back();
	int column_number;
	double test_double;
	
	if(row.size() < min_column_size){
		cerr << "Error: Too few columns specified." << endl;
		return false;
	}
	
	for(auto &raw_column: str_args){
		if(raw_column[raw_column.length() - 1] == 'f'){
			stringstream ss(raw_column);
			ss >> column_number;
			stringstream iss(row[abs(column_number)-1]);
			if(!(iss >> test_double)){
				cerr << "Error: Can't convert specified column into a floating point." << endl;
				return false;
			}
		}
	}
	return true;
}

/*Method for printing the rows after sorting.  

It prints out each section of a row and adds a comma in
between each section in a row.*/
void print(vector<vector<string>> data){
	unsigned i;
	
	for(auto &row: data){
		for(i = 0; i < row.size()-1;i++){
			cout << row.at(i) << ',';
		}
		cout << row.at(i);
		cout << endl;
	}
}

/*Main function for the program.  The program gets 
an arguement string which determines how it should sort
the next inputting data.  If it can no longer get input 
from the user it prints the results after sorting it.*/
int main (int argc, char *argv[] ){
	vector<string> str_args;
	vector<int> num_args;
	string input_row;
	string input_args;
	vector<string> row;
	vector<vector<string>> data;

	if (argc != 2){
		cerr << "Error: Incorrect number of arguements."<< endl;
		return 1;
	}
	
	input_args = argv[1];
	if(!(valid_command(input_args, str_args, num_args))){
		return 1;
	}

	//while we can still get a line from user
	while(getline(cin,input_row)){
		stringstream ss(input_row);
		string row_section;
		row.clear();
		//While that line can be separated by commas
		while(getline(ss,row_section, ',')){
			row.push_back(row_section);
		}
		
		if(valid_input(row, str_args, num_args)){
			data.push_back(row);
		}
	}
	
	sort(data.begin(), data.end(), cmp(str_args));
	print(data);
	
	return 0;
}
