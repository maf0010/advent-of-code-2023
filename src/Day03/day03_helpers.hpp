#ifndef DAY03_H
#define DAY03_H

#include <string>
#include <vector>

// TYPEDEFS & STRUCTS
typedef struct numberSearchResult
{
  bool numberWasFound = false;
  size_t firstPos;
  size_t lastPos;
  uint32_t value;
  uint32_t rowNum = 0;
  bool operator==(const numberSearchResult& a) const
  {
    return ((numberWasFound==a.numberWasFound) && (firstPos==a.firstPos) && (lastPos == a.lastPos) && (value == a.value) && (rowNum == a.rowNum));
  }
} numberSearchResult_t;

typedef struct potentialGearLocation
{
  uint32_t column;
  uint32_t row;
  uint32_t associatedValue;
  bool operator==(const potentialGearLocation &a) const
  {
    return ((column == a.column) && (row == a.row));
  }
} potentialGearLocation_t;


// PROTOTYPES
bool isCharANum(char c);
bool isCharASymbol(char c);
numberSearchResult_t findNumber(std::string s, size_t startPos=0);
bool isSymbolInString(std::string s);
bool isStarInString(std::string s, uint32_t *pStarPos);
bool isRegionAdjacentToSymbol( uint32_t rowNum, uint32_t startCol, uint32_t endCol, std::vector<std::string> *pInputGrid);
bool isStarAboveRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, std::vector<std::string> *pInputGrid, potentialGearLocation_t *pPotGearLoc);
bool isStarBelowRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, std::vector<std::string> *pInputGrid, potentialGearLocation_t *pPotGearLoc);
bool isStarLeftOfRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, std::vector<std::string> *pInputGrid, potentialGearLocation_t *pPotGearLoc);
bool isStarRightOfRegion( uint32_t rowNum, uint32_t startCol, uint32_t endCol, std::vector<std::string> *pInputGrid, potentialGearLocation_t *pPotGearLoc);

#endif //DAY03_H
