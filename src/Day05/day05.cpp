#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
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

enum splitResults_e
{
  SPLIT_RESULT_SET_LESS_THAN_SPLITTER = 0,
  SPLIT_RESULT_SET_SPLIT_BY_SPLITTER = 1,
  SPLIT_RESULT_SET_GREATER_THAN_SPLITTER = 2
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
  void getPiecewiseSetBoundStarts(list<uint64_t> *pList);
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

void AlmanacMap::getPiecewiseSetBoundStarts(list<uint64_t> *pList)
{
  pList->clear();
  for (mapEntry &m : this->entryList)
  {
    pList->push_back(m.sourceStart);
  }
  // Add the end of the highest range (the last set goes from here+1 to infinity)
  pList->push_back(this->entryList.back().sourceEnd+1);
  pList->sort();
}

// FUNCTION DEFINITIONS
splitResults_e splitRangeIfNeeded(array<uint64_t,2> &inRange, uint64_t outRange1[2], uint64_t outRange2[2], uint64_t splitterVal)
{
  splitResults_e result;
  // If the range defined by inRange is split by splitterVal, make new ranges that are split by the old one
  if (inRange[0] < splitterVal)
  {
    if (inRange[1] >= splitterVal)
    {
      result = SPLIT_RESULT_SET_SPLIT_BY_SPLITTER;
      outRange1[0] = inRange[0];
      outRange1[1] = splitterVal - 1;
      outRange2[0] = splitterVal;
      outRange2[1] = inRange[1];
    }
    else
    {
      result = SPLIT_RESULT_SET_LESS_THAN_SPLITTER;
    }
  }
  else
  {
    result = SPLIT_RESULT_SET_GREATER_THAN_SPLITTER;
  }
  return result;
}

void restructureKeyList(list<array<uint64_t, 2>> *pKeyList, AlmanacMap *pAlmMap)
{
  // Key list needs to be amended with the ranges split to account for the almanac piecewise functionality
  list<array<uint64_t, 2>> newList;
  list<uint64_t> almMapBoundaryStarts;
  uint64_t tmpRange1[2], tmpRange2[2];
  array<uint64_t, 2> arrayForInsertion;
  splitResults_e result;

  pAlmMap->getPiecewiseSetBoundStarts(&almMapBoundaryStarts);

  for (list<array<uint64_t, 2>>::iterator itKey = pKeyList->begin(); itKey != pKeyList->end(); ++itKey)
  {
    for (list<uint64_t>::iterator itSplitters = almMapBoundaryStarts.begin(); itSplitters != almMapBoundaryStarts.end(); ++itSplitters)
    {
      // See if the key (set) is split by the splitter and act accordingly
      result = splitRangeIfNeeded(*itKey, tmpRange1, tmpRange2, *itSplitters);

      // If the set is greater than the splitter, move on to next splitter
      if (result == SPLIT_RESULT_SET_GREATER_THAN_SPLITTER)
      {
        continue;
      }
      // If the set was less than the splitter or split by it
      else
      {
        // If it was split, amend the sets in the key list (the current *itKey is changed and a new set is injected after *itKey)
        if (result == SPLIT_RESULT_SET_SPLIT_BY_SPLITTER)
        {
          // Amend the max of the original set
          (*itKey)[1] = tmpRange1[1];

          // Add a new set behind the current place in the list
          itKey++;
          arrayForInsertion = {tmpRange2[0], tmpRange2[1]};
          pKeyList->insert(itKey, arrayForInsertion);
          itKey--;
          itKey--;
        }
        // Regardless if it was less than the splitter or split by it, we'll break this loop and move on to the next set either way
        break;
      }
    }
  }
}

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
  string s, s2, s3;
  list<array<uint64_t,2>> keyList, keyList2;
  array<uint64_t,2> tmpArray;
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
    myStrStrm >> s3;
    tmpArray = {stoull(s2), stoull(s2) + stoull(s3) - 1 }; // Seed list is actually a starting seed # and a range
    keyList.push_back(tmpArray);
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

  // Make a list of pointers to the almanac maps in the sequence they need to be iterated through
  list<AlmanacMap*> almMapPtrList = {&almMapSeed2Soil, &almMapSoil2Fert, &almMapFert2Water, 
                                     &almMapWater2Light, &almMapLight2Temp, &almMapTemp2Humid, 
                                     &almMapHumid2Loc};
  
  // Iterate through the almanac maps, updating the key list (starting with the seed list) with any splits, then translating key-value through the appropriate map and repeating

  for (list<AlmanacMap*>::iterator it=almMapPtrList.begin(); it!= almMapPtrList.end(); ++it)
  {
    if (gVerbosity > 0)
    {
      cout << "\nOriginal Key list of sets/ranges: " << endl;
      for (array<uint64_t, 2> &tmpArray2 : keyList)
      {
        cout << "-->[" << tmpArray2[0] << "," << tmpArray2[1] << "]" << endl;
      }
    }

    // Split the sets/ranges in the key list if needed
    restructureKeyList(&keyList, *it);

    if (gVerbosity > 0)
    {
      cout << "\nNew Key list of sets/ranges: " << endl;
      for (array<uint64_t, 2> &tmpArray2 : keyList)
      {
        cout << "-->[" << tmpArray2[0] << "," << tmpArray2[1] << "]" << endl;
      }
    }

    // Translate the start and end of each range in the key list to values through the maps, making the values the new keys for the next map
    keyList2.clear();
    for (array<uint64_t, 2> &keyRange : keyList)
    {
      tmpArray = {(*(*it))[keyRange[0]], (*(*it))[keyRange[1]]};
      keyList2.push_back(tmpArray);
    }

    // Replace keyList with keyList2
    keyList.swap(keyList2);
  }

  // Iterate through keyList (now has values from the location) and find the lowest value
  for (array<uint64_t, 2> &keyRange : keyList)
  {
    if (keyRange[0] < lowestLocation)
      lowestLocation = keyRange[0];
  }

  return lowestLocation;
}

int main()
{
  uint64_t result;
  string outStr1 = "Lowest Location of all seeds in ";
  string outStr2 = "Total number of scratchcards in ";
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