#include <iostream>
#include <stdlib.h>

#include <fstream>
#include <string>


using std::cin;
using std::cout;
using std::ifstream;


int setUpField(int fieldArr[9][9][2], const char* fileName);
int solveField(int fieldArr[9][9][2], int lastX = 9, int lastY = 9, int lastTry = 9);
int analyzeField(int fieldArr[9][9][2]);

int getPossible(int x, int y, int fieldArr[9][9][2]);
int getPossibleHorizontal(int line, int fieldArr[9][9][2]);
int getPossibleVertical(int collumn, int fieldArr[9][9][2]);
int getPossibleCube(int cube, int fieldArr[9][9][2]);

int traceField(int fieldArr[9][9][2], bool inserting = false);

int main(int argc, const char *argv[])
{
	//contains the field data
	system("clear");

	int arrField[9][9][2];
	if (argc == 2) 
	{
		setUpField(arrField, argv[1]);
	}else
	{
		char a;
		int i;
		int j;
		for (i = 0; i < 9; ++i) 
		{
			for (j = 0; j < 9; ++j) 
			{
				arrField[i][j][0] = 10;
			}
		}
		for (i = 0; i < 9; ++i) 
		{
			for (j = 0; j < 9; ++j) 
			{
				traceField(arrField, true);
				cout << "Insert the number on field marked with \"_\" here and press enter: ";
				cin >> a;
				arrField[i][j][0] = a - '0';
				system("clear");
			}
		}
	}

	cout << "Starting with: ";
 	traceField(arrField);
	cout << "\nSolved field: ";
	solveField(arrField);

	cin.get();
	return 0;
}

int setUpField(int fieldArr[9][9][2], const char* fileName) 
{
	ifstream ifFile;
	ifFile.open(fileName);
	char ch;
	int x = 0;
	int y = 0;
	while(!ifFile.eof())
	{
		ifFile.get(ch);
		if (ch == '\n') 
		{
			//go to the new line
			++y;
			if (y > 8) 
				break;
			x = 0;
		} else if (ch != ' ') 
		{
			//ch is a number 0-9
			fieldArr[y][x][0] = ch - '0';
			++x;
		}
	}
	ifFile.close();
	return 0;
} //end of function setUpField



int solveField(int fieldArr[9][9][2], int lastX, int lastY, int lastTry)
{
	int analyzed = analyzeField(fieldArr);
	int solved = 0;
	if (analyzed == 2)
	{
		//if field is solved, it was already traced from the analyzeField function, so just return 2
		return 2;
	}else if (analyzed == 1) 
	{
		//if there is an error, backtrace and use another guess
		return 1;
	}else
	{
		//if field was already analyzed to the maximum, guess the next move
		//get the first next unsolved field
		int i;
		int j;
		int isFree = 9;
		for (i = 0; i < 9; ++i) 
		{
			for (j = 0; j < 9; ++j) 
			{
				if (fieldArr[i][j][0] == 0) 
				{
					isFree = 3;
					break;
				}
			}
			if (isFree != 9) 
				break;
		}
		if (isFree == 9) 
		{
			//if all fields are full, this sudoku is obv solved and we have to trace it to the user.
			traceField(fieldArr);
			return 2;
		}
		//guess the next possible move on this field
		int number = getPossible(j, i, fieldArr);
		for (int k = 0; k < 9; ++k) 
		{
			int temp = number & (1 << k);
			if(temp > 0)
			{
				//copy the field array and at the same time check for the first empty field
				int newField[9][9][2];
				int isFreeX = 9;
				int isFreeY = 9;
				for (int i2 = 0; i2 < 9; ++i2) 
				{
					for (int j2 = 0; j2 < 9; ++j2) 
					{
						newField[i2][j2][0] = fieldArr[i2][j2][0];
						newField[i2][j2][1] = fieldArr[i2][j2][1];
						if (fieldArr[i2][j2][0] == 0 && isFreeX == 9) 
						{
							isFreeX = i2;
							isFreeY = j2;
						}
					}
				}
				newField[isFreeX][isFreeY][0] = k + 1;
				solved = solveField(newField);
				if (solved == 2) 
					return 2;
			}
		}
	}
	return solved;
} //end of function solveField

int analyzeField(int fieldArr[9][9][2]) 
{
	//analyzes field and puts numbers that are 100% possible in their places
	//calls itself recursively until there are no more certain numbers
	//returns 0 if field is analyzed, 1 if there is an error and 2 if field is complete
	int possibles;
	int temp;
	int used = 0;
	int inserted = 0;
	int currentOnly = 0;
	int isComplete = 1;
	for (int i = 0; i < 9; ++i) 
	{
		for (int j = 0; j < 9; ++j) 
		{
			if (fieldArr[i][j][0] == 0) 
			{
				isComplete = 0;
				possibles = getPossible(j, i, fieldArr);

				used = 0;
				currentOnly = 0;
				temp = 0;
				for (int k = 0; k < 9; ++k) 
				{
					temp = possibles & (1 << k);
					if (temp > 0)
					{
						++used;
						currentOnly = k + 1;
					}
				 	if (used > 1) 
				 		break;
				}
				if (used == 0) 
				{
					// something went wrong at guessing stage
					return 1;
				}else if (used == 1) 
				{
					fieldArr[i][j][0] = currentOnly;
					inserted = 1;
				}
			}
		}
	}
	int analyzed = 0;
	if (isComplete == 1) 
		//traceField(fieldArr);
		return 2;
	if (inserted != 0) 
		//if we have inserted at least one number into the field, analyze it again
		analyzed = analyzeField(fieldArr);
	//error. return 0
	return 0;
} //end of function analyzeField

int getPossible(int x, int y, int fieldArr[9][9][2]) 
{
	int column = getPossibleVertical(x, fieldArr);
	int line = getPossibleHorizontal(y, fieldArr);
	int cube = getPossibleCube(((y / 3) * 3 + (x / 3)), fieldArr);

	//bitwise & -> eliminates every occurance of a 0 in the result and only leaves us the possible numbers
	int result = column & line & cube;

	return result;
} //end of function getPossible

int getPossibleHorizontal(int line, int fieldArr[9][9][2])
{
	//returns int possibilities with bits turned on if number is possible, otherwise bit is turned off (also, we only care about bits on positions < 9)
	//start with all bits turned on, so that we can later turn off the ones that are already used
	unsigned short int possible = ~0;
	unsigned short int currentNumber;
	for (int i = 0; i < 9; ++i) 
	{
		currentNumber = fieldArr[line][i][0];
		if (currentNumber != 0) 
			//toggle the bit on position currentNumber so that it ends up being turned off
			possible = possible ^ (1<<(currentNumber - 1));
	}


	//you dont understand this. stop trying. im gonna help you.
	// so we have number "possible" and after the for loop we end up with something like this (in bits) 1111111111011110
	// the zeros present numbers that were already used in line "line" - 0 on location 5 indicates, that number 6 (5 + 1) was already used in the "line"
	// the above example would mean, that we already used 1 and 6, but other numbers are still available
	// you should think of this as 9 booleans instead of a number. booleans on steroids that is.


	return possible;
} //end of function getPossibleHorizontal

int getPossibleVertical(int collumn, int fieldArr[9][9][2])
{
	//read the description of getPossibleHorizontal
	unsigned short int possible = ~0;
	unsigned short int currentNumber;
	for (int i = 0; i < 9; ++i) 
	{
		currentNumber = fieldArr[i][collumn][0];
		if (currentNumber != 0) 
			//toggle the bit on position currentNumber so that it ends up being turned off
			possible = possible ^ (1<<(currentNumber - 1));
	}
	return possible;
} //end of function getPossibleHorizontal

int getPossibleCube(int cube, int fieldArr[9][9][2]) 
{
	
	//read the description of getPossibleHorizontal
	unsigned short int possible = ~0;
	unsigned short int currentNumber;
	int collumn;
	int line;
	for (int i = 0; i < 9; ++i) 
	{
		collumn = (cube % 3) * 3 + (i % 3);
		line = (cube / 3) * 3 + (i / 3);
		currentNumber = fieldArr[line][collumn][0];
		//cout << line << "x" << collumn << ": " << currentNumber << "\n";
		if (currentNumber != 0) 
			//toggle the bit on position currentNumber so that it ends up being turned off
			possible = possible ^ (1<<(currentNumber - 1));
	}
	return possible;
} //end of function getPossibleCube


int traceField(int fieldArr[9][9][2], bool inserting) 
{
	//traces current field
	int first10 = true;
	for (int i = 0; i < 9; ++i) 
	{
		if (i == 3 || i == 6) 
			cout << "\n-------+-------+-------\n";
		else 
			cout << "\n";
		for (int j = 0; j < 9; ++j) 
		{
			if (j == 3 || j == 6) 
				cout << " | ";
			else 
				cout << " ";
			if (fieldArr[i][j][0] == 10) 
			{
				if (first10 == true) 
				{
					first10 = false;
					cout << "_";
				}else 
					cout << " ";
			}else
				cout << fieldArr[i][j][0];
		}
	}
	cout << "\n";
	return 0;
} //end of function traceField
