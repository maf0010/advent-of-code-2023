#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>
#include <list>

using namespace std;

// TYPEDEFS & STRUCTS
enum handType_e
{
  HT_HIGH_CARD = 0,
  HT_ONE_PAIR = 1,
  HT_TWO_PAIR = 2,
  HT_3_KIND = 3,
  HT_FULL_HOUSE = 4,
  HT_4_KIND = 5,
  HT_5_KIND = 6
};

class RoundData
{
private:
  string sortableHand = "";
  void makeSortableHand(void);
  void determineHandType(void);
  void modifyHandTypeWithWildcard(void);
  map<char,uint8_t> cardCount;
  bool isJokerWild = false;
public:
  string hand = "";
  handType_e type = HT_HIGH_CARD;
  handType_e originalType = HT_HIGH_CARD;
  uint32_t bid = 0;
  RoundData(string inStr, bool isJackAJoker);
  ~RoundData() {};
  bool operator<(const RoundData &a) const;
  bool operator<=(const RoundData &a) const;
  bool operator>(const RoundData &a) const;
  bool operator>=(const RoundData &a) const;
  bool operator==(const RoundData &a) const;
  bool operator!=(const RoundData &a) const;
};

void RoundData::makeSortableHand(void)
{
  static string originals = "TJQKA";
  static string replacements = ":;<=>"; // Increasing ASCII values after '9'
  static string wildReplacements = ":1<=>"; // J is a joker and is valued less than '2'
  // Conduct replacements for Ace, King, Queen, Jack, and Ten so that they can
  //  be easilly sorted the way we want, but using default string sorting
  this->sortableHand = this->hand;
  if (this->isJokerWild)
  {
    for (int i = 0; i < originals.length(); ++i)
    {
      replace(this->sortableHand.begin(), this->sortableHand.end(), originals[i], wildReplacements[i]);
    }
  }
  else
  {
    for (int i = 0; i < originals.length(); ++i)
    {
      replace(this->sortableHand.begin(), this->sortableHand.end(), originals[i], replacements[i]);
    }
  }
}

void RoundData::determineHandType(void)
{
  uint8_t mapSize;
  char testValue;

  // Build a card count map
  this->cardCount.clear();
  for (char &c : this->hand)
  {
    if (this->cardCount.count(c) == 0)
      this->cardCount.emplace(c, 1);
    else
      this->cardCount[c]++;
  }
  // Inspecting the card count map, determine the type of hand that is present
  mapSize = this->cardCount.size();
  // If 5 entries are in this map, all cards are unique this it's a "high card" type
  if (mapSize == 5)
  {
    this->type = HT_HIGH_CARD;
  }
  // If 4 entries are in the map, two cards must form a pair
  else if (mapSize == 4)
  {
    this->type = HT_ONE_PAIR;
  }
  // If 3 entries are in the map, it's either 3 of a kind or two pair
  else if (mapSize == 3)
  {
    // Go through map entries; 3 of a kind has values of 3,1,1 / 2 pair has values of 2,2,1.
    //  While we could check for double entry of '1' as a factor, just scan until a 2 or 3 is
    //  found; the extra potential loop isn't gonna be what bogs this down...
    for (map<char, uint8_t>::iterator it = this->cardCount.begin(); it != this->cardCount.end(); ++it)
    {
      if (it->second == 2)
      {
        this->type = HT_TWO_PAIR;
        break;
      }
      else if (it->second == 3)
      {
        this->type = HT_3_KIND;
        break;
      }
    }
  }
  // If 2 entries are in the map, it's either a full house or a 4 of a kind
  else if (mapSize == 2)
  {
    // Select first entry in the map; if it's value is 4 or 1, then it's 4 of a kind; otherwise (3 or 2), its a full-house
    testValue = this->cardCount.begin()->second;
    if ((testValue == 4) || (testValue == 1))
    {
      this->type = HT_4_KIND;
    }
    else
    {
      this->type = HT_FULL_HOUSE;
    }
  }
  // If only one entry is in the map, it must be a 5 of a kind
  else if (mapSize == 1)
  {
    this->type = HT_5_KIND;
  }
  // If one of the above conditions isn't met, somethis is wrong
  else
  {
    cout << "ERROR in RoundData::determineHandType() : cardCount map size not between 1 and 5" << endl;
    this->type = HT_HIGH_CARD; // Not right, but I don't want to break the application automatically and will do it through inspecting std out
  }
}

void RoundData::modifyHandTypeWithWildcard(void)
{
  uint8_t jokerCount = this->cardCount['J'];
  handType_e originalType = this->type;

  // There are only certain possibilities of type transformations, given original type and Joker count permutations
  // If jokerCount is 0 or 5, then card type should be left as it was orignally, but these scenarios need to be
  //  checked off in the if-else list first for the other statements to hold the assumption that there's at least 1 joker present
  if ((jokerCount == 0) || (jokerCount == 5))
  {
    (void)0; // noop
  }
  // Type of XXXXJ or XXXJJ becomes XXXXX
  else if ((originalType == HT_4_KIND) || (originalType == HT_FULL_HOUSE))
  {
    this->type = HT_5_KIND;
  }
  // Type of XXXYJ or XYJJJ becomes XXXXY
  else if (originalType == HT_3_KIND)
  {
    this->type = HT_4_KIND;
  }
  // Type of XXYYJ becomes XXXYY, Type of XXYJJ becomes XXXXY
  else if (originalType == HT_TWO_PAIR)
  {
    if (jokerCount == 1)
    {
      this->type = HT_FULL_HOUSE;
    }
    else
    {
      this->type = HT_4_KIND;
    }
  }
  // Type of XXYZJ or XYZJJ becomes XXXYZ
  else if (originalType == HT_ONE_PAIR)
  {
    this->type = HT_3_KIND;
  }
  // Type of XYZWJ becomes XXYZW
  else
  {
    this->type = HT_ONE_PAIR;
  }
}

RoundData::RoundData(string inStr, bool isJackAJoker = false)
{
  istringstream iss(inStr);
  string tmpStr;
  this->isJokerWild = isJackAJoker;

  // inStr in format "HAND BID"
  iss >> this->hand;
  iss >> tmpStr;
  this->bid = stoul(tmpStr);

  // Make a sortable version of the hand
  this->makeSortableHand();

  // Determine the hand type
  this->determineHandType();

  // If wildcard joker is used, redetermine hand type, taking it into account
  this->originalType = this->type;
  if (this->isJokerWild)
  {
    this->modifyHandTypeWithWildcard();
  }
}

bool RoundData::operator<(const RoundData &a) const
{
  bool ret = false;
  // Check first by type, then by string hand comparison
  if (this->type < a.type)
  {
    ret = true;
  }
  else if (this->type == a.type)
  {
    if (this->sortableHand < a.sortableHand)
    {
      ret = true;
    }
    // Else will be false
  }
  // No need for else as it will render false, which is default return
  return ret;
}

bool RoundData::operator<=(const RoundData &a) const
{
  bool ret = false;
  // Check first by type, then by string hand comparison (can't go by <= as hands must match too)
  if (this->type < a.type)
  {
    ret = true;
  }
  else if (this->type == a.type)
  {
    if (this->sortableHand <= a.sortableHand)
    {
      ret = true;
    }
    // Else will be false
  }
  // No need for else as it will render false, which is default return
  return ret;
}

bool RoundData::operator>(const RoundData &a) const
{
  bool ret = false;
  // Check first by type, then by string hand comparison
  if (this->type > a.type)
  {
    ret = true;
  }
  else if (this->type == a.type)
  {
    if (this->sortableHand > a.sortableHand)
    {
      ret = true;
    }
    // Else will be false
  }
  // No need for else as it will render false, which is default return
  return ret;
}

bool RoundData::operator>=(const RoundData &a) const
{
  bool ret = false;
  // Check first by type, then by string hand comparison (can't go by >= as hands must match too)
  if (this->type > a.type)
  {
    ret = true;
  }
  else if (this->type == a.type)
  {
    if (this->sortableHand >= a.sortableHand)
    {
      ret = true;
    }
    // Else will be false
  }
  // No need for else as it will render false, which is default return
  return ret;
}

bool RoundData::operator==(const RoundData &a) const
{
  bool ret = false;
  // Verify that type and string hand comparisons are equal
  if ((this->type == a.type) && (this->sortableHand == a.sortableHand))
  {
    ret = true;
  }
  // No need for else as it will render false, which is default return
  return ret;
}

bool RoundData::operator!=(const RoundData &a) const
{
  bool ret = true;
  // Verify that type and string hand comparisons are equal
  if ((this->type == a.type) && (this->sortableHand == a.sortableHand))
  {
    ret = false;
  }
  // No need for else as it will render true, which is default return
  return ret;
}

// PROTOTYPES

// GLOBALS
int gVerbosity = 0;

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS

uint64_t part1(string fName)
{
  string s;
  list<RoundData> roundList;
  uint32_t rank = 1;
  uint64_t totalWinnings = 0;
  static map<handType_e, string> handTypeMap = {{HT_5_KIND, "Five of a Kind "}, {HT_4_KIND, "Four of a Kind "},
                                                {HT_3_KIND, "Three Of A Kind"}, {HT_FULL_HOUSE, "Full House     "},
                                                {HT_TWO_PAIR, "Two Pair       "}, {HT_ONE_PAIR, "One Pair       "}, 
                                                {HT_HIGH_CARD, "High Card      "}};

  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Make a new round entry in the list from the recently-read string
    roundList.emplace_back(s);

    // Sort the round list
    roundList.sort();
  }

  if (gVerbosity > 0)
  {
    cout << "Sorted List Details:" << endl;
  }

  // Move through the sorted list, with increasing rank, to tally up the winnings of each hand
  for (list<RoundData>::iterator it = roundList.begin(); it != roundList.end(); ++it)
  {
    if (gVerbosity > 0)
    {
      cout << "-Rank = " << rank << " | ";
      cout << "Type = " << handTypeMap[it->type] << " | ";
      cout << "Hand = " << it->hand << " | ";
      cout << "Bid = " << it->bid << endl;
    }
    totalWinnings += (it->bid * rank);
    rank++;
  }

  return totalWinnings;
}

uint64_t part2(string fName)
{
  string s;
  list<RoundData> roundList;
  uint32_t rank = 1;
  uint64_t totalWinnings = 0;
  static map<handType_e, string> handTypeMap = {{HT_5_KIND, "Five of a Kind "}, {HT_4_KIND, "Four of a Kind "},
                                                {HT_3_KIND, "Three Of A Kind"}, {HT_FULL_HOUSE, "Full House     "},
                                                {HT_TWO_PAIR, "Two Pair       "}, {HT_ONE_PAIR, "One Pair       "}, 
                                                {HT_HIGH_CARD, "High Card      "}};

  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;

    // Make a new round entry in the list from the recently-read string
    roundList.emplace_back(s, true);

    // Sort the round list
    roundList.sort();
  }

  if (gVerbosity > 0)
  {
    cout << "Sorted List Details:" << endl;
  }

  // Move through the sorted list, with increasing rank, to tally up the winnings of each hand
  for (list<RoundData>::iterator it = roundList.begin(); it != roundList.end(); ++it)
  {
    if (gVerbosity > 0)
    {
      cout << "-Rank = " << rank << " | ";
      cout << "Type = " << handTypeMap[it->type] << " | ";
      cout << "Original Type = " << handTypeMap[it->originalType] << " | ";
      cout << "Hand = " << it->hand << " | ";
      cout << "Bid = " << it->bid << endl;
    }
    totalWinnings += (it->bid * rank);
    rank++;
  }

  return totalWinnings;
}

int main()
{
  uint64_t result;
  string outStr1 = "Total winnings in ";
  string outStr2 = "Total winnings with joker wildcard in ";
  string fName;

  // gVerbosity = 1;

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