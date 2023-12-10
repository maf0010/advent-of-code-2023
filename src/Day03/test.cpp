#include <catch2/catch_test_macros.hpp>

#include "day03_helpers.hpp"

using namespace std;

int gVerbosity=0;

TEST_CASE("Numbers are extracted", "[findNumber]")
{
  numberSearchResult_t answer = {true, 0, 2, 467};
  REQUIRE(findNumber("467..114..") == answer);
  answer = {true, 5, 7, 114};
  REQUIRE(findNumber("467..114..", 3) == answer);
  answer = {true, 7, 9, 123};
  REQUIRE(findNumber(".......123", 0) == answer);

  numberSearchResult_t result = findNumber("467..114..", 8);
  REQUIRE(result.numberWasFound == false);
}

TEST_CASE("A character symbol can be detected", "[isCharASymbol]")
{
  REQUIRE(isCharASymbol('.')==false);
  REQUIRE(isCharASymbol('0')==false);
  REQUIRE(isCharASymbol('1')==false);
  REQUIRE(isCharASymbol('2')==false);
  REQUIRE(isCharASymbol('3')==false);
  REQUIRE(isCharASymbol('4')==false);
  REQUIRE(isCharASymbol('5')==false);
  REQUIRE(isCharASymbol('6')==false);
  REQUIRE(isCharASymbol('7')==false);
  REQUIRE(isCharASymbol('8')==false);
  REQUIRE(isCharASymbol('9')==false);
  REQUIRE(isCharASymbol('^')==true);
  REQUIRE(isCharASymbol('*')==true);
  REQUIRE(isCharASymbol('p')==true);
  REQUIRE(isCharASymbol('/')==true);
  REQUIRE(isCharASymbol(' ')==true);
}

TEST_CASE("A symbol can be detected in a string", "[isSymbolInString]")
{
  REQUIRE(isSymbolInString("....4321")==false);
  REQUIRE(isSymbolInString("1...4321")==false);
  REQUIRE(isSymbolInString(".")==false);
  REQUIRE(isSymbolInString("a")==true);
  REQUIRE(isSymbolInString("...............164.....-..............532*.................252..........321.......")==true);
  REQUIRE(isSymbolInString(".2.6.5.8./3685..")==true);
}

TEST_CASE("A symbol can be detected adjacent to a region", "[isRegionAdjacentToSymbol]")
{
  vector<string> testVector;
  testVector.push_back("467..114..");
  testVector.push_back("...*......");
  testVector.push_back("..35..6333");
  testVector.push_back("1.....#...");
  testVector.push_back("617*......");
  REQUIRE(isRegionAdjacentToSymbol(0, 0, 2, &testVector) == true);
  REQUIRE(isRegionAdjacentToSymbol(0, 5, 7, &testVector) == false);
  REQUIRE(isRegionAdjacentToSymbol(2, 2, 3, &testVector) == true);
  REQUIRE(isRegionAdjacentToSymbol(2, 6, 9, &testVector) == true);
  REQUIRE(isRegionAdjacentToSymbol(3, 0, 0, &testVector) == false);
  REQUIRE(isRegionAdjacentToSymbol(4, 0, 1, &testVector) == false);
  REQUIRE(isRegionAdjacentToSymbol(4, 0, 2, &testVector) == true);
}