#include <iostream>
#include <fstream>
#include <string>
#include <regex>
// #include <array>
#include <vector>
// #include <list>

using namespace std;

// TYPEDEFS & STRUCTS
typedef struct 
{
  uint32_t nRed = 0;
  uint32_t nGreen = 0;
  uint32_t nBlue = 0;
}drawResults_t;

typedef struct
{
  uint32_t nDraws = 0;
  uint32_t gameId;
  vector<drawResults_t> drawVec;
  vector<string> drawRawStringVec;
  string gameString;
  uint32_t locMaxRed = 0;
  uint32_t locMaxGreen = 0;
  uint32_t locMaxBlue = 0;
  uint32_t power = 0;
}gameResults_t;

// PROTOTYPES

// GLOBALS
int gVerbosity = 0;

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS

drawResults_t parseDraw(string drawString)
{
  static regex eRed ("([\\d]+) red");
  static regex eGreen ("([\\d]+) green");
  static regex eBlue ("([\\d]+) blue");
  smatch smRed, smGreen, smBlue;
  drawResults_t outResults;

  // Verbose
  if (gVerbosity > 0)
    cout << "-->Draw String is: " << drawString << endl;

  // Get Red
  if (regex_search(drawString, smRed, eRed))
    outResults.nRed = stoul(smRed[1]);

  // Get Green
  if (regex_search(drawString, smGreen, eGreen))
    outResults.nGreen = stoul(smGreen[1]);

  // Get Blue
  if (regex_search(drawString, smBlue, eBlue))
    outResults.nBlue = stoul(smBlue[1]);

  // Verbose
  if (gVerbosity > 0)
    cout << "-->Num Red: " << outResults.nRed << " | Num Green: " << outResults.nGreen << " | Num Blue: " << outResults.nBlue << endl;

  return outResults;
}

gameResults_t parseGame(string gameString)
{
  static regex eGame ("Game ([\\d]+)\\:");
  gameResults_t outResults;
  smatch smGame;
  size_t pos1, pos2;
  uint32_t lMxR=0, lMxG=0, lMxB=0;

  // Verbose
  if (gVerbosity > 0)
    cout << "\nGame String is: " << gameString << endl;

  // Store off the game string
  outResults.gameString = gameString;

  // Get the game ID
  if (regex_search(gameString, smGame, eGame))
    outResults.gameId = stoul(smGame[1]);

  // Find the start of the draws in the game string, separate each draw by semicolon, parse each draw
  pos1 = gameString.find(":");
  while (pos1 != string::npos)
  {
    pos2 = gameString.find(";",pos1+1);
    outResults.drawRawStringVec.push_back(gameString.substr(pos1+1, pos2-pos1));
    outResults.nDraws++;
    outResults.drawVec.push_back(parseDraw(outResults.drawRawStringVec.back()));
    drawResults_t &dr = outResults.drawVec.back();
    if (lMxR < dr.nRed)
      lMxR = dr.nRed;
    if (lMxG < dr.nGreen)
      lMxG = dr.nGreen;
    if (lMxB < dr.nBlue)
      lMxB = dr.nBlue;
    pos1 = pos2;
  }

  // Calculate the power of the game and set the local max
  outResults.power = lMxR * lMxG * lMxB;
  outResults.locMaxRed = lMxR;
  outResults.locMaxGreen = lMxG;
  outResults.locMaxBlue = lMxB;

  // Verbose
  if (gVerbosity > 0)
    cout << "-->Number of Draws: " << outResults.nDraws << endl;

  return outResults;
}



uint64_t part1(string fName)
{
  string s;
  const uint32_t maxRed=12, maxGreen=13, maxBlue=14;
  gameResults_t gr;
  drawResults_t dr;
  uint32_t possibleGameSum = 0;
  bool isGamePossible;

  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Parse the game and get data from the draw, determining if the game was possible
    gr = parseGame(s);
    isGamePossible = true;
    for (int i=0; i<gr.nDraws; ++i)
    {
      dr = gr.drawVec[i];
      if ((dr.nRed > maxRed) || (dr.nGreen > maxGreen) || (dr.nBlue > maxBlue))
      {
        isGamePossible = false;
        break;
      }
    }

    // Add game id if the game was possible
    if (isGamePossible)
      possibleGameSum += gr.gameId;
  }

  return possibleGameSum;
}

uint64_t part2(string fName)
{
  string s;
  gameResults_t gr;
  drawResults_t dr;
  uint32_t gamePowerSum = 0;

  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Parse the game and get data from the draw, add the game power to the power sum
    gr = parseGame(s);
    gamePowerSum += gr.power;
  }

  return gamePowerSum;
}

int main()
{
  uint64_t result;
  string outStr1 = "Sum of all possible game IDs ";
  string outStr2 = "Sum of all game powers for the minimum cubes ";
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