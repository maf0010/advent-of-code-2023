#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

// TYPEDEFS & STRUCTS

// PROTOTYPES

// GLOBALS
int gVerbosity = 0;

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS
uint32_t countWinningNumbers(vector<int> *pWinningNumbers, vector<int> *pScratchedNumbers)
{
  uint32_t countSum=0;
  for (int winningNumber : *pWinningNumbers)
    countSum += count(pScratchedNumbers->begin(), pScratchedNumbers->end(), winningNumber);
  return countSum;
}

void stringToIntVector(string inStr, vector<int> *pVec)
{
  istringstream myStrStm(inStr);
  string s;

  while (myStrStm >> s)
  {
    pVec->push_back(stoul(s));
  }
}


uint64_t part1(string fName)
{
  string s;
  string winNumSubStr, scrNumSubStr;
  size_t posColon, posVertBar;
  vector<int> winNumVec, scrNumVec;
  uint32_t occurCount, points, pointSum=0;


  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;
    
    // Exctract winning number and scratched number substrings
    posColon = s.find(":");
    posVertBar = s.find("|");    
    winNumSubStr = s.substr( posColon+1, posVertBar-posColon-1);
    scrNumSubStr = s.substr( posVertBar+1, s.length() - posVertBar);
    
    // Extract values from the substrings
    winNumVec.clear();
    scrNumVec.clear();
    stringToIntVector(winNumSubStr, &winNumVec);
    stringToIntVector(scrNumSubStr, &scrNumVec);

    // Count the occurrances of winning numbers in scratched numbers and add points to total
    occurCount = countWinningNumbers(&winNumVec, &scrNumVec);
    if (occurCount != 0)
      pointSum += 1<<(occurCount-1);

  }

  return pointSum;
}

uint64_t part2(string fName)
{
  string s;
  string winNumSubStr, scrNumSubStr;
  size_t posColon, posVertBar;
  vector<int> winNumVec, scrNumVec;
  uint32_t nWinningNumbers, i;
  vector<int> scratchcardCopiesVec;
  uint32_t currentCard = 0, nextCard = 1;
  int copiesOfCurrentCard, totalScratchcards = 0;


  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // If there's not an entry in the scratchcard copies vector for this card, add it
    currentCard++;
    nextCard++;
    if (scratchcardCopiesVec.size() < currentCard)
      scratchcardCopiesVec.push_back(1);
    
    // Exctract winning number and scratched number substrings
    posColon = s.find(":");
    posVertBar = s.find("|");    
    winNumSubStr = s.substr( posColon+1, posVertBar-posColon-1);
    scrNumSubStr = s.substr( posVertBar+1, s.length() - posVertBar);
    
    // Extract values from the substrings
    winNumVec.clear();
    scrNumVec.clear();
    stringToIntVector(winNumSubStr, &winNumVec);
    stringToIntVector(scrNumSubStr, &scrNumVec);

    // Count the occurrances of winning numbers
    nWinningNumbers = countWinningNumbers(&winNumVec, &scrNumVec);

    // Get copies of the current card
    copiesOfCurrentCard = scratchcardCopiesVec.at(currentCard-1);

    // For however many winning numbers there were, add copies of the next nWinningNumbers scratch cards (taking into account how many copies of the current card will add to those copies)
    for (i=nextCard; i<(nextCard+nWinningNumbers); ++i)
    {
      if (scratchcardCopiesVec.size() < i)
        scratchcardCopiesVec.push_back(1+copiesOfCurrentCard);
      else
        scratchcardCopiesVec[i-1] += copiesOfCurrentCard;
    }

    // Add the number of copies of the current card to the total scratchcard sum
    totalScratchcards += copiesOfCurrentCard;
  }

  return totalScratchcards;
}

int main()
{
  uint64_t result;
  string outStr1 = "Sum of points won in ";
  string outStr2 = "Total number of scratchcards in ";
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