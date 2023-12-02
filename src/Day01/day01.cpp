#include <iostream>
#include <string>
#include <fstream>
#include <array>
// #include <vector>
// #include <list>

using namespace std;

// TYPEDEFS & STRUCTS

// PROTOTYPES

// GLOBALS
int gVerbosity = 0;

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS
uint8_t findBookendDigits(string s)
{
  string code = "";
  uint32_t pos;
  const string possibleDigits = "0123456789";
  pos = s.find_first_of(possibleDigits);
  code.append(s.substr(pos, 1));
  pos = s.find_last_of(possibleDigits);
  code.append(s.substr(pos, 1));
  return stoul(code);
}

uint8_t thoroughBookendNumberFinder(string s)
{
  array<string, 10> spelledNumArr = {"zero", "one", "two", "three", "four",
                                     "five", "six", "seven", "eight", "nine"};
  const string normalDigits = "0123456789";
  size_t pos, lowestPos = string::npos, highestPos = 0;
  string code = "";
  string firstDigit;
  string lastDigit;


  if (gVerbosity > 0)
    cout << "\nString is : " << s << endl;

  // Scan for normal digits finding the local lowest and highest
  pos = s.find_first_of(normalDigits);
  if (pos < lowestPos)
  {
    lowestPos = pos;
    firstDigit = s[pos];
  }
  pos = s.find_last_of(normalDigits);
  if ((pos != string::npos) && (pos >= highestPos))
  {
    highestPos = pos;
    lastDigit = s[pos];
  }

  // Scan through spelled digits, updating the local lowest and highest digit, if applicable
  for (int i = 0; i < 10; i++)
  {
    pos = s.find(spelledNumArr[i]);
    while (pos != string::npos)
    {
      if (pos < lowestPos)
      {
        lowestPos = pos;
        firstDigit = normalDigits[i];
      }
      if ((pos != string::npos) && (pos >= highestPos))
      {
        highestPos = pos;
        lastDigit = normalDigits[i];
      }
      pos = s.find(spelledNumArr[i], pos+1);
    }
  }

  // Build the calibration code
  code = firstDigit + lastDigit;

  if (gVerbosity > 0)
  {
  cout << "First Digit: " << firstDigit << " | Last Digit: " << lastDigit << endl;
  cout << "Code : " << code << endl;
  }

  return stoul(code);
}

uint64_t part1(string fName)
{
  string s;
  uint64_t calibrationSum = 0;

  // Open file, read throguh it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Get the first and last numerical digit of each string, make it an integer, and add it to the cal sum
    calibrationSum += findBookendDigits(s);
  }

  return calibrationSum;
}

uint64_t part2(string fName)
{
  string s;
  uint64_t calibrationSum = 0;

  // Open file, read throguh it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Get the first and last numerical digit of each string, make it an integer, and add it to the cal sum, accounting for spelled numbers
    calibrationSum += thoroughBookendNumberFinder(s);
  }

  return calibrationSum;

}

int main()
{
  uint64_t result;
  string outStr1 = "Sum of all calibration values ";
  string outStr2 = "Sum of all calibration values (correcting for spelled numbers) ";
  string fName;

  // -- PART 1
  cout << "==============PART1==============" << endl;
  // Run on example input
  fName = "example.txt";
  result = part1(fName);
  cout << outStr1 + fName << " is " << result << endl;

  // Run on actual input
  fName = "input.txt";
  result = part1(fName);
  cout << outStr1 + fName << " is " << result << endl;

  // -- PART 2
  cout << endl
       << "==============PART2==============" << endl;
  // Run on example input
  fName = "example2.txt";
  result = part2(fName);
  cout << outStr2 + fName << " is " << result << endl;

  // Run on actual input
  fName = "input.txt";
  result = part2(fName);
  cout << outStr2 + fName << " is " << result << endl;

  return 0;
}