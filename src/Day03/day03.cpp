#include <iostream>
#include <fstream>
#include <algorithm>
#include "day03_helpers.hpp"

using namespace std;

// GLOBALS
int gVerbosity = 0;

uint64_t part1(string fName)
{
  string s;
  uint32_t lineNum = 0;
  size_t scanPos = 0;
  uint32_t partNumberSum = 0;
  vector<string> inputTextGrid;
  vector<numberSearchResult_t> foundNumberVec;
  numberSearchResult_t numSearchRes;
  
  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    // Load input into a vector of strings
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    
    inputTextGrid.push_back(s);
    
    // Get info on the numerical values in each string while we're here
    while(true)
    {
      numSearchRes = findNumber(s, scanPos);
      if (numSearchRes.numberWasFound)
      {
        scanPos = numSearchRes.lastPos + 1;
        numSearchRes.rowNum = lineNum;
        foundNumberVec.push_back(numSearchRes);
      }
      else
      {
        break;
      }
    }
    scanPos=0;
    lineNum++;
  }

  // Go through the list of found numbers, determine if they are adjacent to a symbol, sum the values of the one that are adjacent
  vector<numberSearchResult_t>::iterator it;
  for (it=foundNumberVec.begin(); it!=foundNumberVec.end(); ++it)
  {
    if (isRegionAdjacentToSymbol(it->rowNum, it->firstPos, it->lastPos, &inputTextGrid))
      partNumberSum += it->value;
  }

  return partNumberSum;
}

uint64_t part2(string fName)
{
  uint64_t gearRatio = 0;
  uint64_t gearRatioSum = 0;
  string s;
  uint32_t lineNum = 0;
  size_t scanPos = 0;
  vector<string> inputTextGrid;
  vector<numberSearchResult_t> foundNumberVec;
  numberSearchResult_t numSearchRes;
  potentialGearLocation_t potGearLoc;
  vector<potentialGearLocation_t> potGearLocVec, confirmedGearLocVec;
  
  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    // Load input into a vector of strings
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    
    inputTextGrid.push_back(s);
    
    // Get info on the numerical values in each string while we're here
    while(true)
    {
      numSearchRes = findNumber(s, scanPos);
      if (numSearchRes.numberWasFound)
      {
        scanPos = numSearchRes.lastPos + 1;
        numSearchRes.rowNum = lineNum;
        foundNumberVec.push_back(numSearchRes);
      }
      else
      {
        break;
      }
    }
    scanPos=0;
    lineNum++;
  }

  // Go through the list of found numbers, determine if they are adjacent to a *, add the * locations to the potential gear list
  vector<numberSearchResult_t>::iterator it;
  for (it=foundNumberVec.begin(); it!=foundNumberVec.end(); ++it)
  {
    potGearLoc.associatedValue = it->value;
    if (isStarAboveRegion(it->rowNum, it->firstPos, it->lastPos, &inputTextGrid, &potGearLoc))
      potGearLocVec.push_back(potGearLoc);
    if (isStarBelowRegion(it->rowNum, it->firstPos, it->lastPos, &inputTextGrid, &potGearLoc))
      potGearLocVec.push_back(potGearLoc);
    if (isStarLeftOfRegion(it->rowNum, it->firstPos, it->lastPos, &inputTextGrid, &potGearLoc))
      potGearLocVec.push_back(potGearLoc);
    if (isStarRightOfRegion(it->rowNum, it->firstPos, it->lastPos, &inputTextGrid, &potGearLoc))
      potGearLocVec.push_back(potGearLoc);
  }

  // Go through potential gears, marking the ones that appear exactly twice in the potential list, so it's not double counted
  vector<potentialGearLocation_t>::iterator potGearIt, it3;
  for (potGearIt=potGearLocVec.begin(); potGearIt!=potGearLocVec.end(); ++potGearIt)
  {
    // If it hasn't been confirmed yet
    if (find(confirmedGearLocVec.begin(), confirmedGearLocVec.end(), *potGearIt) == confirmedGearLocVec.end())
    {
      if (count(potGearIt, potGearLocVec.end(), *potGearIt) == 2)
      {
        it3 = find(potGearIt+1, potGearLocVec.end(), *potGearIt);
        gearRatio = potGearIt->associatedValue * it3->associatedValue;
        gearRatioSum += gearRatio;
        confirmedGearLocVec.push_back(*potGearIt);
      }
    }
  }

  // Return the gear ratio sum and be done with this awful daily puzzle, I'm beyong caring how clean this code is
  return gearRatioSum;
}


int main()
{
  uint64_t result;
  string outStr1 = "Sum of all part numbers ";
  string outStr2 = "Sum of all gear ratios ";
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
  cout << endl << "==============PART2==============" << endl;
  // Run on example input
  fName = "example.txt";
  result = part2(fName);
  cout << outStr2 + fName << " is " << result << endl;

  // Run on actual input
  fName = "input.txt";
  result = part2(fName);
  cout << outStr2 + fName << " is " << result << endl;

  return 0;
}