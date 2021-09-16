#ifndef amglib_h
#define amglib_h

#include "amglib.h"

//STL
#include <string>
#include <vector>
#include <string>
#include <fstream>
#include <assert.h>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <queue>
#include <deque>
#include <list>
#include <functional> 
#include <stdio.h>
#include <ctype.h>
#include <set>
#include <unordered_set>
#include <list>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <time.h> 
#include <memory>
#include <thread>
#include <chrono>
#include <locale>
#include <codecvt>
#include <functional>
#include <numeric>
#include <mutex>
#include <syncstream>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <random>
using namespace std; 

//Typedefs
typedef long long LL;

typedef pair<int, int> IntIntPr;
typedef pair<int, string> IntStrPr;
typedef pair<string, int> StrIntPr;
typedef pair<string, string> StrStrPr;

typedef vector<int> IntV;
typedef vector<string> StrV;

typedef vector<vector<int>> IntVV;
typedef vector<vector<string>> StrVV;

typedef vector<vector<vector<int>>> IntVVV;
typedef vector<vector<vector<string>>> StrVVV;

typedef unordered_map<string, int> StrToIntM;
typedef unordered_map<int, string> IntToStrM;
typedef unordered_map<string, string> StrToStrM;
typedef unordered_map<int, int> IntToIntM;
typedef unordered_map<string, double> StrToDblM;
typedef unordered_map<double, string> DblToStrM;


typedef set<string> StrS;
typedef set<int> IntS;

namespace amg {

void SplitStrOnCh(const string& Str, const char& SplitCh, vector <string>& StrV);

string JoinStrVToStr(const vector<string>& StrV, const string& SplitStr = "");

void SplitStrOnStr(const string& Str, const string& SplitStr, vector <string>& StrV);

void SaveSortedStrIntUMap(const unordered_map <string, int>& UMap, const char SplitCh, const string& fNm, const int& MnInt=-1);

void SaveStrIntUMap(const unordered_map <string, int>& UMap, const char SplitCh, const string& fNm, const int& MnInt = -1);

void SaveStrStrUMap(const unordered_map <string, string>& UMap, const char SplitCh, const string& fNm);

void LoadStrIntUMap(const string& fNm, unordered_map <string, int>& UMap);

void LoadStrStrStrPrUMap(const string& fNm, const char& SplitChar, unordered_map <string, pair<string, string>>& StrStrStrPrUM);

void LoadStrStrUMap(const string& fNm, const char& SplitChar, unordered_map <string, string>&StrStrUM);

void SaveStrS(const StrS& StrS, const string& OutFNm);

void LoadStrV(const string& LoadFNm, const StrV& LoadStrV);

void SaveStrV(const StrV& SaveStrV, const string& SaveFNm);

double Sqr(const double& Val);

void LinearFit(const vector<pair <double, double>>& XYPointV, double& A, double& B);

void TxtStrToNGramToNGramFqM(const string& TxtStr, const char SplitCh, const int MxNGramLen, unordered_map<string, int>& NGramToNGramFqM);

void TxtStrToNGramStrV(const string & TxtStr, const char SplitCh, const int MxNGramLen, const int MnNGramFq, unordered_map<string, int>& NGramToNGramFqM, vector<string>& NGramStrV);

void StrToLowerCase(const string& InStr, string& LowerStr);

void SaveIntIntPrV(const vector<pair<int, int>>& IntIntPrV, const string& SavefNm);

void SaveStrIntPrV(const vector<StrIntPr>& IntIntPrV, const string& SavefNm, const bool IsAppend, const bool SaveStrFirst = true);

void GetLineSplitV(ifstream& fIn, const char SplitCh, vector<string>& SplitV);

bool IsInt(const string& Str);

string ChToHex(unsigned int n);

double GetXYCoordDist(const double& X1, const double& Y1, const double& X2, const double& Y2);

pair<int, int> GetMovedXYCoordsPos(int MxMove, int SrcX, int SrcY, int DstX, int DstY);

string GetTrimStr(const string& InStr, char TrimCh);

int GetLnsInFile(const string& fNm, const int& OutLns=1000);

void WriteStrToFile(const string& fNm, const string& WriteStr);

void SplitV(StrV& InV, const int& FirstSplitSz, StrV& SplitV1, StrV& SplitV2);
  
string FileToStrFast(const string& fNm);

int GenRandIntUniformDistr(const int& Min, const int& Max);

//Error Handling functions
void SaveToErrLog(const string& MsgStr);

void ExeStop(const string& MsgStr, const string& CondStr, const string& fNm, const int& LnN);

#define IAssert(Cond) \
   ((Cond) ? static_cast<void>(0) : ExeStop("Fail", #Cond, __FILE__, __LINE__))

#define IAssertM(Cond, Msg) \
   ((Cond) ? static_cast<void>(0) : ExeStop(Msg, #Cond, __FILE__, __LINE__))
#define Fail() ExeStop(NULL, "Fail", __FILE__, __LINE__)
#define FailM(Msg) ExeStop(Msg, "Fail", __FILE__, __LINE__)


//Helper Functions
template <class TVVec>
void GenVV(const int& XDim, const int& YDim, TVVec& VVec) {
  VVec.clear(); VVec.resize(XDim);
  for (int X = 0; X < XDim; X++) { VVec[X].resize(YDim); }
}

template <class TMap, class TKey>
bool IsMapKey(const TMap& Map, const TKey& Key) {
  return Map.find(Key) != Map.end();
}

template <class TMap, class TKeyDatPrV>
void GetMapKeyDatPrV(const TMap& Map, TKeyDatPrV& KeyDatPrV) {
  KeyDatPrV.clear();
  for (auto& KeyDatPr : Map) {
    KeyDatPrV.push_back(make_pair(KeyDatPr.first, KeyDatPr.second));
  }
}

template <class TMap, class TKeyV>
void GetMapKeyV(const TMap& Map, TKeyV& KeyV, const bool& IsSorted) {
  KeyV.clear();
  for (auto& KeyDatPr : Map) {
    KeyV.push_back(KeyDatPr.first);
  }
  sort(KeyV.begin(), KeyV.end());
}

template <class TMap, class TDatV>
void GetMapDatV(const TMap& Map, TDatV& DatV) {
  DatV.clear();
  for (auto& KeyDatPr : Map) {
    DatV.push_back(KeyDatPr.second);
  }
}

template<class TKeyDatPrV, class TKey, class TDat>
TDat GetBinKeyDat(const TKeyDatPrV& KeyDatPrV, const TKey& Key, const
  TDat& DfDat) {
  int LValN = 0; int RValN = KeyDatPrV.size() - 1;
  while (LValN <= RValN) {
    int ValN = (LValN + RValN) / 2;
    if (Key == KeyDatPrV[ValN].first) { return KeyDatPrV[ValN].second; }
    if (Key < KeyDatPrV[ValN].first) { RValN = ValN - 1; } else { LValN = ValN + 1; }
  }
  return DfDat;
}

template<typename... Args>
string Fmt(const string& FormatStr, Args... ArgL) {
  size_t StrLen = snprintf(nullptr, 0, FormatStr.c_str(), ArgL...);
  string Str; Str.reserve(StrLen + 1); Str.resize(StrLen);
  snprintf(&Str[0], StrLen + 1, FormatStr.c_str(), ArgL...);
  return Str;
}

}

#endif