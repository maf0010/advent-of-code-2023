#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
// #include <vector>
#include <list>
#include <algorithm>

using namespace std;

// TYPEDEFS & STRUCTS
struct mapEntry
{
  uint64_t sourceStart;
  uint64_t sourceEnd;
  int64_t conversionFactor;
  mapEntry(uint64_t dest, uint64_t source, uint64_t range)
  {
    this->conversionFactor = dest-source;
    this->sourceStart = source;
    this->sourceEnd = source + range - 1;
  }
  bool operator<(const mapEntry& a) const
  {
    return (sourceStart < a.sourceStart);
  }
  bool operator<=(const mapEntry& a) const
  {
    return (sourceStart <= a.sourceStart);
  }
  bool operator>(const mapEntry& a) const
  {
    return (sourceStart > a.sourceStart);
  }
  bool operator>=(const mapEntry& a) const
  {
    return (sourceStart >= a.sourceStart);
  }
  bool operator==(const mapEntry& a) const
  {
    return (sourceStart == a.sourceStart);
  }
  bool operator!=(const mapEntry& a) const
  {
    return (sourceStart != a.sourceStart);
  }
};

// PROTOTYPES

// GLOBALS
int gVerbosity = 0;

// CLASS DEFINITIONS
class AlmanacMap
{
private:
  list<mapEntry> entryList;
public:
  AlmanacMap(){};
  AlmanacMap(string inputParagraphStr);
  ~AlmanacMap() {};
  uint64_t operator[](uint64_t i);
};

AlmanacMap::AlmanacMap(string inputParagraphStr)
{
  // String will be of format ### ### ###\n### ### ###\n### ### ###\n and so on...
  istringstream stream1(inputParagraphStr);
  istringstream stream2;
  string s;
  uint64_t dest, source, range;

  // Go through each line, getting the information out of each string
  while (true)
  {
    getline(stream1, s);
    if(stream1.eof())
      break;
    
    stream2.clear();
    stream2.str(s);
    stream2 >> dest;
    stream2 >> source;
    stream2 >> range;

    // Use information to make and add a new map entry
    entryList.push_back(mapEntry(dest, source, range));
  }

  // Sort the entry list
  entryList.sort();
}

uint64_t AlmanacMap::operator[](uint64_t i)
{
  bool foundArgValInEntry = false;
  uint64_t outVal;

  // Assumes entry list is sorted, which it does when it creates the list or adds to it
  for (mapEntry &entry : entryList)
  {
    if (i < entry.sourceStart)
      break;
    else if (i > entry.sourceEnd)
      continue;
    else
    {
      foundArgValInEntry = true;
      outVal = i + entry.conversionFactor;
    }    
  }

  // If argValue wasn't found, output is same as argValue
  if (!foundArgValInEntry)
    outVal = i;
  
  // Return the mapped value
  return outVal;
}

// FUNCTION DEFINITIONS

void mySeek(ifstream *pFileStream, string startText)
{
  string s;
  while (true)
  {
    getline(*pFileStream, s);
    if (s.find(startText) == 0)
      break;
  }
}

ostringstream fetchMapParagraph(ifstream *pFileStream)
{
  ostringstream out;
  string s;
  while (true)
  {
    getline(*pFileStream, s);
    if ((s.length() == 0) || (pFileStream->eof()))
      break;
    out << s << endl;
  }
  return out;
}

uint64_t part1(string fName)
{
  string s, s2;
  list<uint64_t> seedList;
  istringstream myStrStrm;
  ostringstream paragraph;
  uint64_t value;
  uint64_t lowestLocation = 0xFFFFFFFFFFFFFFFF;

  // Open file, read through it
  ifstream fsObj(fName);

  // Get seed list
  getline(fsObj, s);
  myStrStrm.clear();
  myStrStrm.str(s.substr(7,s.length()-7)); // 7 is past the ": " in "seeds: "
  while (myStrStrm >> s2)
  {
    seedList.push_back(stoull(s2));
  }

  // Go to the seed-to-soil map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "seed-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapSeed2Soil(paragraph.str());

  // Go to the soil-to-fertilizer map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "soil-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapSoil2Fert(paragraph.str());
  
  // Go to the fertilizer-to-water map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "fertilizer-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapFert2Water(paragraph.str());

  // Go to the water-to-light map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "water-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapWater2Light(paragraph.str());

  // Go to the light-to-temperature map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "light-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapLight2Temp(paragraph.str());

  // Go to the temperature-to-humidity map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "temperature-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapTemp2Humid(paragraph.str());

  // Go to the humidity-to-location map, read in the paragraph, and build the almanac map
  mySeek(&fsObj, "humidity-");
  paragraph = fetchMapParagraph(&fsObj);
  AlmanacMap almMapHumid2Loc(paragraph.str());

  // Scan through the seed list, determining the lowest location
  for (uint64_t &seed : seedList)
  {
    if (gVerbosity > 0)
    {
      cout << "- Seed: " << seed << ", ";
      value = almMapSeed2Soil[seed];
      cout << "Soil: " << value << ", ";
      value = almMapSoil2Fert[value];
      cout << "Fertilizer: " << value << ", ";
      value = almMapFert2Water[value];
      cout << "Water: " << value << ", ";
      value = almMapWater2Light[value];
      cout << "Light: " << value << ", ";
      value = almMapLight2Temp[value];
      cout << "Temperature: " << value << ", ";
      value = almMapTemp2Humid[value];
      cout << "Humidity: " << value << ", ";
      value = almMapHumid2Loc[value];
      cout << "Location: " << value << "." << endl;
    }
    else
    {
      value = almMapHumid2Loc[almMapTemp2Humid[almMapLight2Temp[almMapWater2Light[almMapFert2Water[almMapSoil2Fert[almMapSeed2Soil[seed]]]]]]];
    }
    if (value < lowestLocation)
      lowestLocation = value;
  }  
  return lowestLocation;
}

uint64_t part2(string fName)
{
  string s;

  // Open file, read through it
  ifstream fsObj(fName);
  while (true)
  {
    getline(fsObj, s);
    if (fsObj.eof())
      break;    
  }

  return 0;
}

int main()
{
  uint64_t result;
  string outStr1 = "Lowest Location of all seeds in ";
  string outStr2 = "Total number of scratchcards in ";
  string fName;

  gVerbosity = 1;

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

  // // -- PART 2
  // cout << endl << "==============PART2==============" << endl;
  // // Run on example input
  // fName = "example.txt";
  // result = part2(fName);
  // cout << outStr2 + fName << " is " << result << endl;

  // // Run on actual input
  // fName = "input.txt";
  // result = part2(fName);
  // cout << outStr2 + fName << " is " << result << endl;

  return 0;
}