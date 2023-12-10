#include <iostream>

#include "day03_helpers.hpp"

using namespace std;

// GLOBALS
extern int gVerbosity;

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS

bool isCharANum(char c)
{
  return ( (c >= '0') && ( (c - '0') < 10 ) );
}

bool isCharASymbol(char c)
{
  // Everything else other than '.' and a numerical digit is a symbol
  return ((c != '.') && !(isCharANum(c)));
}

numberSearchResult_t findNumber(string s, size_t startPos)
{
  size_t firstPos = 0; 
  size_t lastPos = s.length()-1;
  bool foundNum = false;
  uint32_t val = 0;
  numberSearchResult_t result;

  // Report inputs
  if (gVerbosity > 0)
  {
    cout << "\nString: " << s << endl;
    cout << "Starting Position=" << startPos << endl;
  }

  // Scan for an integer in the string
  for(int i=startPos; i<s.length(); ++i)
  {
    if (!foundNum)
    {
      if (isCharANum(s[i]))
      {
        firstPos = i;
        foundNum = true;
      }
    }
    else
    {
      if (!isCharANum(s[i]))
      {
        lastPos = i-1;
        break; 
      }
    }
  }

  // Fetch value as uint32 if found
  if (foundNum)
    val = stoul(s.substr(firstPos, lastPos - firstPos + 1));


  // Report outputs
  if (gVerbosity > 0)
  {
    if (foundNum)
    {
      cout << "Number Found: " << val << endl;
      cout << " First Pos=" << firstPos << " | Last Pos=" << lastPos << endl;
    }
    else
    {
      cout << "No Number Found" << endl;
    }
  }

  // Pack results
  result.numberWasFound = foundNum;
  result.firstPos = firstPos;
  result.lastPos = lastPos;
  result.value = val;

  return result;
}

bool isSymbolInString(string s)
{
  bool isSymbolPresent = false; 

  // Report inputs
  if (gVerbosity > 0)
    cout << "isSybolInString():: input string= " << s << endl;
  
  // Rejecting '.' and any numerical digits... everything else is a symbol
  for (int i=0; i<s.length(); ++i)
  {
    isSymbolPresent |= isCharASymbol(s[i]);
    if (isSymbolPresent)
      break;
  }

  // Report outputs
  if (gVerbosity > 0)
  {
    if (isSymbolPresent)
      cout << "-->Symbol found in string: " << s << endl;
    else
      cout << "-->No symbol found in string: " << s << endl;
  }

  return isSymbolPresent;
}

bool isStarInString(string s, uint32_t *pStarPos)
{
  bool isStarPresent = false; 

  // Report inputs
  if (gVerbosity > 0)
    cout << "isStarInString():: input string= " << s << endl;
  
  // Only look for '*'
  for (int i=0; i<s.length(); ++i)
  {
    isStarPresent |= (s[i] == '*');
    if (isStarPresent)
    {
      *pStarPos = i;
      break;
    }
  }

  // Report outputs
  if (gVerbosity > 0)
  {
    if (isStarPresent)
      cout << "-->Star found in string: " << s << endl;
    else
      cout << "-->No Star found in string: " << s << endl;
  }

  return isStarPresent;
}

bool isRegionAdjacentToSymbol( uint32_t rowNum, uint32_t startCol, uint32_t endCol, vector<string> *pInputGrid)
{
  bool isAdjacent=false;

  uint32_t searchStartCol, searchEndCol, searchWidth;

  // Do-While for clean breaking w/o multiple returns
  do
  {
    // If rowNum is invalid, exit now
    if ((rowNum < 0) || (rowNum >= pInputGrid->size()))
      break;

    // Set search colunm start and end
    if (startCol != 0)
      searchStartCol = startCol - 1;
    else
      searchStartCol = startCol;
    if (endCol != (pInputGrid->at(rowNum).length() - 1))
      searchEndCol = endCol + 1;
    else
      searchEndCol = endCol + 1;
    searchWidth = searchEndCol - searchStartCol + 1;

    // Check the relevant columns in the row above the region-under-test
    if (rowNum != 0)
    {
      isAdjacent |= isSymbolInString(pInputGrid->at(rowNum - 1).substr(searchStartCol, searchWidth));
      if (isAdjacent)
        break;
    }

    // Check the relevant columns in the row below the region-under-test
    if (rowNum != (pInputGrid->size() - 1))
    {
      isAdjacent |= isSymbolInString(pInputGrid->at(rowNum + 1).substr(searchStartCol, searchWidth));
      if (isAdjacent)
        break;
    }

    // Check to the left of the region-under-test
    if (startCol != 0)
    {
      isAdjacent |= isSymbolInString(pInputGrid->at(rowNum).substr(searchStartCol, 1));
      if (isAdjacent)
        break;
    }

    // Check to the right of the region-under-test
    if (endCol != (pInputGrid->at(rowNum).length()-1))
    {
      isAdjacent |= isSymbolInString(pInputGrid->at(rowNum).substr(searchEndCol, 1));
      if (isAdjacent)
        break;
    }

  } while (false);
  

  return isAdjacent;
}

bool isStarAboveRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, vector<string> *pInputGrid, potentialGearLocation_t *pPotGearLoc)
{
bool isPresent=false;

  uint32_t searchStartCol, searchEndCol, searchWidth;
  uint32_t starPositionInSubString;

  // Do-While for clean breaking w/o multiple returns
  do
  {
    // If rowNum is invalid, exit now
    if ((rowNum < 0) || (rowNum >= pInputGrid->size()))
      break;

    // Set search colunm start and end
    if (startCol != 0)
      searchStartCol = startCol - 1;
    else
      searchStartCol = startCol;
    if (endCol != (pInputGrid->at(rowNum).length() - 1))
      searchEndCol = endCol + 1;
    else
      searchEndCol = endCol + 1;
    searchWidth = searchEndCol - searchStartCol + 1;

    // Check the relevant columns in the row above the region-under-test
    if (rowNum != 0)
    {
      isPresent |= isStarInString(pInputGrid->at(rowNum - 1).substr(searchStartCol, searchWidth), &starPositionInSubString);
      if (isPresent)
      {
        // Record location
        pPotGearLoc->row = rowNum - 1;        
        pPotGearLoc->column = searchStartCol  + starPositionInSubString;
      }
    }
  } while (false);

  return isPresent;
}

bool isStarBelowRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, vector<string> *pInputGrid, potentialGearLocation_t *pPotGearLoc)
{
bool isPresent=false;

  uint32_t searchStartCol, searchEndCol, searchWidth;
  uint32_t starPositionInSubString;

  // Do-While for clean breaking w/o multiple returns
  do
  {
    // If rowNum is invalid, exit now
    if ((rowNum < 0) || (rowNum >= pInputGrid->size()))
      break;

    // Set search colunm start and end
    if (startCol != 0)
      searchStartCol = startCol - 1;
    else
      searchStartCol = startCol;
    if (endCol != (pInputGrid->at(rowNum).length() - 1))
      searchEndCol = endCol + 1;
    else
      searchEndCol = endCol + 1;
    searchWidth = searchEndCol - searchStartCol + 1;

    // Check the relevant columns in the row below the region-under-test
    if (rowNum != (pInputGrid->size() - 1))
    {
      isPresent |= isStarInString(pInputGrid->at(rowNum + 1).substr(searchStartCol, searchWidth), &starPositionInSubString);
      if (isPresent)
      {
        // Record location
        pPotGearLoc->row = rowNum + 1;        
        pPotGearLoc->column = searchStartCol  + starPositionInSubString;
      }
    }
  } while (false);

  return isPresent;
}

bool isStarLeftOfRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, vector<string> *pInputGrid, potentialGearLocation_t *pPotGearLoc)
{
bool isPresent=false;

  uint32_t searchStartCol;
  uint32_t starPositionInSubString;

  // Do-While for clean breaking w/o multiple returns
  do
  {
    // If rowNum is invalid, exit now
    if ((rowNum < 0) || (rowNum >= pInputGrid->size()))
      break;

    // Set search colunm start and end
    if (startCol != 0)
      searchStartCol = startCol - 1;
    else
      searchStartCol = startCol;

    // Check to the left of the region-under-test
    if (startCol != 0)
    {
      isPresent |= isStarInString(pInputGrid->at(rowNum).substr(searchStartCol, 1), &starPositionInSubString);
      if (isPresent)
      {
        // Record location
        pPotGearLoc->row = rowNum;        
        pPotGearLoc->column = searchStartCol;
      }
    }
  } while (false);

  return isPresent;
}

bool isStarRightOfRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, vector<string> *pInputGrid, potentialGearLocation_t *pPotGearLoc)
{
bool isPresent=false;

  uint32_t searchEndCol;
  uint32_t starPositionInSubString;

  // Do-While for clean breaking w/o multiple returns
  do
  {
    // If rowNum is invalid, exit now
    if ((rowNum < 0) || (rowNum >= pInputGrid->size()))
      break;

    // Set search colunm start and end
    if (endCol != (pInputGrid->at(rowNum).length() - 1))
      searchEndCol = endCol + 1;
    else
      searchEndCol = endCol + 1;

    // Check to the right of the region-under-test
    if (endCol != (pInputGrid->at(rowNum).length()-1))
    {
      isPresent |= isStarInString(pInputGrid->at(rowNum).substr(searchEndCol, 1), &starPositionInSubString);
      if (isPresent)
      {
        // Record location
        pPotGearLoc->row = rowNum;
        pPotGearLoc->column = searchEndCol;
      }
    }
  } while (false);

  return isPresent;
}
