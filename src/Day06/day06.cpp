#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

using namespace std;

// TYPEDEFS & STRUCTS
struct quadIneqSolution
{
  uint64_t lowerLimit;
  uint64_t upperLimit;
};

struct raceStats
{
  uint64_t tRace;
  uint64_t dRecord;
};

// PROTOTYPES

// GLOBALS
int gVerbosity = 0;

// CLASS DEFINITIONS

// FUNCTION DEFINITIONS
void buildRaceInfo(string *pStrTime, string *pStrDist, vector<raceStats> *pRaceStatsVec)
{
  istringstream timeStream, distStream;
  size_t posColonTime, posColonDist;
  string sTmpTime, sTmpDist;
  raceStats tmpStats;

  posColonTime = pStrTime->find(":");
  posColonDist = pStrDist->find(":");

  timeStream.str(pStrTime->substr(posColonTime + 1, pStrTime->length() - posColonTime));
  distStream.str(pStrDist->substr(posColonDist + 1, pStrDist->length() - posColonDist));

  while (timeStream >> sTmpTime)
  {
    distStream >> sTmpDist;
    tmpStats = {stoull(sTmpTime), stoull(sTmpDist)};
    pRaceStatsVec->push_back(tmpStats);
  }
}

void buildRaceInfoBadKerning(string *pStrTime, string *pStrDist, raceStats *pRaceStats)
{
  istringstream timeStream, distStream;
  size_t posColonTime, posColonDist;
  string sSubTmpTime, sSubTmpDist;
  string sTmpTime = "", sTmpDist = "";
  raceStats tmpStats;

  posColonTime = pStrTime->find(":");
  posColonDist = pStrDist->find(":");

  timeStream.str(pStrTime->substr(posColonTime + 1, pStrTime->length() - posColonTime));
  distStream.str(pStrDist->substr(posColonDist + 1, pStrDist->length() - posColonDist));

  while (timeStream >> sSubTmpTime)
  {
    distStream >> sSubTmpDist;
    sTmpTime.append(sSubTmpTime);
    sTmpDist.append(sSubTmpDist);
  }
  pRaceStats->tRace = stoull(sTmpTime);
  pRaceStats->dRecord = stoull(sTmpDist);
}

quadIneqSolution quadraticSolverInt(raceStats stats)
{
  quadIneqSolution out;
  out.lowerLimit = uint64_t(floor(0.5f*(stats.tRace - sqrt(pow(stats.tRace,2)-4*stats.dRecord)))+1);
  out.upperLimit = uint64_t(ceil(0.5f*(stats.tRace + sqrt(pow(stats.tRace,2)-4*stats.dRecord))));
  return out;
}

uint64_t part1(string fName)
{
    string sTime, sDist;
    vector<raceStats> raceStatsVec;
    quadIneqSolution crossers;
    uint32_t nSolutions;
    uint32_t marginOfError = 1;

    // Open file, read two lines for time and distance
    ifstream fsObj(fName);
    getline(fsObj, sTime);
    getline(fsObj, sDist);

    // Get the race info structs and put it into the race stats vector
    buildRaceInfo(&sTime, &sDist, &raceStatsVec);

    // For each race, the range of record-beating charging times is a quadratic inequality, solve them and the limits are the zero-crossers
    for (raceStats &tmpStats : raceStatsVec)
    {
      crossers = quadraticSolverInt(tmpStats);
      nSolutions = crossers.upperLimit-crossers.lowerLimit;
      marginOfError *= nSolutions;
    }

  return marginOfError;
}

uint64_t part2(string fName)
{
    string sTime, sDist;
    raceStats myRaceStats;
    quadIneqSolution crossers;
    uint32_t marginOfError;

    // Open file, read two lines for time and distance
    ifstream fsObj(fName);
    getline(fsObj, sTime);
    getline(fsObj, sDist);

    // Get the race info structs and put it into the race stats vector
    buildRaceInfoBadKerning(&sTime, &sDist, &myRaceStats);

    // For the race, the range of record-beating charging times is a quadratic inequality, solve it and the limits are the zero-crossers
    crossers = quadraticSolverInt(myRaceStats);
    marginOfError = crossers.upperLimit-crossers.lowerLimit;

  return marginOfError;
}

int main()
{
  uint64_t result;
  string outStr1 = "Margin of error in ";
  string outStr2 = "Margin of error in larger race in ";
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