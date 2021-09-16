#include "amglib.h"

using namespace std;

namespace amg{

  void SplitStrOnCh(const string& Str, const char& SplitCh, vector <string>& StrV){
    StrV.clear();
    StrV.push_back("");
    for (int cc = 0; cc < int(Str.size()); cc++) {
      if (Str[cc] != SplitCh) {
        StrV.back().push_back(Str[cc]);
      } else {
        StrV.push_back("");
      }
    }
  }

  string JoinStrVToStr(const vector<string>& StrV, const string& SplitStr){
    string JoinStr = "";
    for (auto& StrI : StrV) {
      if (!JoinStr.empty()) { JoinStr.append(SplitStr); }
      JoinStr.append(StrI);
    }
    return JoinStr;
  }

  void SplitStrOnStr(const string& Str, const string& SplitStr, vector <string>& StrV){
    size_t PrevStrEnd = 0;
    size_t NxtStrBeg = 0;
    while ((NxtStrBeg = Str.find(SplitStr, PrevStrEnd)) != string::npos) {
      StrV.push_back(Str.substr(PrevStrEnd, NxtStrBeg - PrevStrEnd));
      PrevStrEnd = NxtStrBeg + SplitStr.size();
    }
    StrV.push_back(Str.substr(PrevStrEnd));
  }

  void SaveSortedStrIntUMap(const unordered_map <string, int>& UMap, const char SplitCh, const string& fNm, const int& MnInt){
    typedef pair<int, string> TIntStrPr;
    vector<TIntStrPr> IntStrPrV;

    for (auto& StrIntI : UMap) {
      IntStrPrV.push_back(TIntStrPr(StrIntI.second, StrIntI.first));
    }
    sort(IntStrPrV.begin(), IntStrPrV.end());
    reverse(IntStrPrV.begin(), IntStrPrV.end());
    ofstream fOut(fNm); 
    IAssert(fOut.is_open());
    int Lns = 0;
    for (int i = 0; i < int(IntStrPrV.size()); i++) {
      Lns++; if (Lns % 100000 == 0) { cout << Lns << '\r'; }
      if ((MnInt == -1) || (IntStrPrV[i].first >= MnInt)) {
        fOut << IntStrPrV[i].first << SplitCh << IntStrPrV[i].second << endl;
      }
    }
  }

  void SaveStrIntUMap(const unordered_map <string, int>& UMap, const char SplitCh, const string& fNm, const int& MnInt){
    ofstream fOut(fNm); IAssert(fOut.is_open());
    int Lns = 0;
    for (auto UMapI = UMap.begin(); UMapI != UMap.end(); UMapI++) {
      Lns++; if (Lns % 100000 == 0) { cout << Lns << '\r'; }
      if ((MnInt == -1) || (UMapI->second >= MnInt)) {
        fOut << UMapI->first << SplitCh << UMapI->second << endl;
      }
    }
  }

  void SaveStrStrUMap(const unordered_map<string, string>& UMap, const char SplitCh, const string& fNm){
    ofstream fOut(fNm); IAssert(fOut.is_open());
    int Lns = 0;
    for (auto UMapI = UMap.begin(); UMapI != UMap.end(); UMapI++) {
      Lns++; if (Lns % 100000 == 0) { cout << Lns << '\r'; }
      fOut << UMapI->first << SplitCh << UMapI->second << endl;
    }
  }

  void LoadStrIntUMap(const string& fNm, unordered_map <string, int>& UMap){
    UMap.clear();
    ifstream fIn(fNm); IAssert(fIn.is_open());
    string Ln;
    while (getline(fIn, Ln)) {
      vector <string> FldStrV;
      SplitStrOnCh(Ln, '\t', FldStrV);
      string Str = FldStrV[0];
      int Int = stoi(FldStrV[1], nullptr);
      UMap[Str] = Int;
    }
  }

  void LoadStrStrStrPrUMap(const string& fNm, const char& SplitChar, unordered_map <string, pair<string, string>>& StrStrStrPrUM){
    ifstream fIn(fNm); IAssert(fIn.is_open());
    string Ln;
    while (getline(fIn, Ln)) {
      vector <string> StrV;
      SplitStrOnCh(Ln, SplitChar, StrV);
      StrStrStrPrUM[StrV[0]].first = StrV[1];
      StrStrStrPrUM[StrV[0]].second = StrV[2];
    }
  }

  void LoadStrStrUMap(const string& fNm, const char& SplitChar, unordered_map <string, string>& StrStrUM){
    ifstream fIn(fNm); IAssert(fIn.is_open());
    string Ln;
    while (getline(fIn, Ln)) {
      vector <string> StrV;
      SplitStrOnCh(Ln, SplitChar, StrV);
      StrStrUM[StrV[0]] = StrV[1];
    }
  }

  void SaveStrS(const StrS& StrS, const string& OutFNm){
    ofstream fOut(OutFNm); IAssert(fOut.is_open());
    for (auto& StrI : StrS) {
      fOut << StrI << endl;
    }
  }

  void LoadStrV(const string& LoadFNm, StrV& LoadStrV){
    ifstream fIn(LoadFNm); IAssert(fIn.is_open());
    string Ln;
    while (getline(fIn, Ln)) {
      LoadStrV.push_back(Ln);
    }
  }

  void SaveStrV(const StrV& SaveStrV, const string& SaveFNm){
    ofstream fOut(SaveFNm); IAssert(fOut.is_open());
    for (auto& SaveStrI : SaveStrV) {
      fOut << SaveStrI << endl;
    }
  }

  double Sqr(const double& Val){
    return Val * Val;
  }

  void LinearFit(const vector<pair <double, double>>& XYPointV, double& A, double& B){
    int XYPoints = int(XYPointV.size());
    double SumX = 0; double SumY = 0;
    for (int XYPointN = 0; XYPointN < XYPoints; XYPointN++) {
      SumX += XYPointV[XYPointN].first;
      SumY += XYPointV[XYPointN].second;
    }
    double AvgX = SumX / XYPoints;
    double VarX = 0;
    A = 0; B = 0;
    for (int XYPointN = 0; XYPointN < XYPoints; XYPointN++) {
      double XMAvgX = XYPointV[XYPointN].first - AvgX;
      VarX += Sqr(XMAvgX);
      B += XMAvgX * XYPointV[XYPointN].second;
    }
    B = B / VarX;
    A = (SumY - SumX * B) / XYPoints;
  }

  void TxtStrToNGramToNGramFqM(const string& TxtStr, const char SplitCh, const int MxNGramLen, unordered_map<string, int>& NGramToNGramFqM){
    vector<string> WrdStrV;
    SplitStrOnCh(TxtStr, SplitCh, WrdStrV);
    for (size_t WrdN = 0; WrdN<WrdStrV.size(); WrdN++) {
      string NGramStr;
      for (size_t GramN = 0; GramN < MxNGramLen && GramN + WrdN<WrdStrV.size(); GramN++) {
        string& WrdStr = WrdStrV[WrdN + GramN];
        if (GramN > 0) { NGramStr += ' '; }
        NGramStr += WrdStr;
        //if (GramN==MxNGramLen){
        if ((NGramStr == "") || (NGramStr == " ") || (NGramStr == "\n") || (NGramStr == "\t"))continue;//avoid useless NGrams
        NGramToNGramFqM[NGramStr]++;
        //} else if (NGramToNGramFqM.find(NGramStr)==NGramToNGramFqM.end()){
        //  break;
        //}
      }
    }
  }

  void TxtStrToNGramStrV(const string& TxtStr, const char SplitCh, const int MxNGramLen, const int MnNGramFq, unordered_map<string, int>& NGramToNGramFqM, vector<string>& NGramStrV){
    vector<string> WrdStrV;
    SplitStrOnCh(TxtStr, SplitCh, WrdStrV);
    for (size_t WrdN = 0; WrdN<WrdStrV.size(); WrdN++) {
      string NGramStr;
      for (size_t GramN = 0; GramN <= MxNGramLen && GramN + WrdN<WrdStrV.size(); GramN++) {
        string& WrdStr = WrdStrV[WrdN + GramN];
        if (GramN > 0) { NGramStr += ' '; }
        NGramStr += WrdStr;
        if (NGramToNGramFqM[NGramStr] >= MnNGramFq) {
          NGramStrV.push_back(NGramStr);
        }
      }
    }
  }

  void StrToLowerCase(const string& InStr, string& LowerStr){
    for (int cc = 0; cc < int(InStr.size()); cc++) {
      LowerStr.push_back(tolower(InStr[cc]));
    }
  }

  void SaveIntIntPrV(const vector<pair<int, int>>& IntIntPrV, const string& SavefNm){
    ofstream fOut(SavefNm); IAssert(fOut.is_open());
    for (auto IntIntPrI : IntIntPrV) {
      fOut << IntIntPrI.first << "\t" << IntIntPrI.second << endl;
    }
  }

  void SaveStrIntPrV(const vector<StrIntPr>& IntIntPrV, const string& SavefNm, const bool IsAppend, const bool SaveStrFirst){
    ofstream fOut;
    if (IsAppend) {
      fOut.open(SavefNm, ofstream::app); IAssert(fOut.is_open());
    } else {
      fOut.open(SavefNm, ofstream::out); IAssert(fOut.is_open());
    }
    for (auto StrIntPrI : IntIntPrV) {
      if (SaveStrFirst) {
        fOut << StrIntPrI.first << "\t" << StrIntPrI.second << endl;
      } else {
        fOut << StrIntPrI.second << "\t" << StrIntPrI.first << endl;
      }
    }
  }

  void GetLineSplitV(ifstream& fIn, const char SplitCh, vector<string>& SplitV){
    string Ln;
    getline(fIn, Ln);
    SplitStrOnCh(Ln, SplitCh, SplitV);
  }

  bool IsInt(const string& Str){
    for (auto ChI : Str) {
      if (!isdigit(ChI)) { return false; }
    }
    return true;
  }

  string ChToHex(unsigned int n){
    string res;
    do {
      res += "0123456789ABCDEF"[n % 16];
      n >>= 4;
    } while (n);

    return std::string(res.rbegin(), res.rend());
  }

  double GetXYCoordDist(const double& X1, const double& Y1, const double& X2, const double& Y2){
    return sqrt(Sqr(X1 - X2) + Sqr(Y1 - Y2));
  }

  pair<int, int> GetMovedXYCoordsPos(int MxMove, int SrcX, int SrcY, int DstX, int DstY){
    //XDirection
    if (MxMove > max(SrcX, DstX) - min(SrcX, DstX)) {
      MxMove -= max(SrcX, DstX) - min(SrcX, DstX);
      SrcX = DstX;
    } else {
      if (SrcX > DstX)SrcX -= MxMove;
      else SrcX += MxMove;
      return make_pair(SrcX, SrcY);
    }

    if (MxMove > max(SrcY, DstY) - min(SrcY, DstY)) {
      MxMove -= max(SrcY, DstY) - min(SrcY, DstY);
      SrcY = DstY;
    } else {
      if (SrcY > DstY)SrcY -= MxMove;
      else SrcY += MxMove;
      return make_pair(SrcX, SrcY);
    }

    return make_pair(SrcX, SrcY);
  }

  string GetTrimStr(const string& InStr, char TrimCh){
    const auto Beg = InStr.find_first_not_of(TrimCh);
    if (Beg == std::string::npos) {
      return ""; // no content
    }
    const auto End = InStr.find_last_not_of(TrimCh);
    const auto Range = End - Beg + 1;
    return InStr.substr(Beg, Range);
  }

  int GetLnsInFile(const string& fNm, const int& OutLns){
    cout << "counting number of lines in file..." << endl;
    ifstream fIn(fNm);
    IAssertM(fIn.is_open(), "AMGLib GetLnsInFile: fIn not open");
    string Ln;
    int LnN = 0;
    while (getline(fIn, Ln)) {
      LnN++; if (LnN % OutLns == 0) { cout << LnN << '\r'; }
    }
    cout << "Done." << endl;
    return LnN;
  }

  void WriteStrToFile(const string& fNm, const string& WriteStr){
    ofstream fOut(fNm); IAssertM(fOut.is_open(), "WriteStrToFile: fOut not open");
    fOut << WriteStr;
  }

  void SplitV(StrV& InV, const int& FirstSplitSz, StrV& SplitV1, StrV& SplitV2){
    IAssertM(InV.size() > FirstSplitSz, "SplitV: Input vector not big enough for split");
    copy(InV.begin(), InV.begin() + FirstSplitSz, back_inserter(SplitV1));
    copy(InV.begin() + FirstSplitSz + 1, InV.end(), back_inserter(SplitV2));
  }

  string FileToStrFast(const string& fNm){
    cout << "Reading " << fNm << "..." << endl;
    ifstream fIn(fNm);
    fIn.seekg(0, std::ios::end);
    size_t size = fIn.tellg();
    string InStr(size, ' ');
    fIn.seekg(0);
    fIn.read(&InStr[0], size);
    fIn.close();
    return InStr;
    cout << "Done.";
  }

  int GenRandIntUniformDistr(const int& Min, const int& Max){
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_int_distribution<int>  distr(Min, Max);
    return distr(generator);
  }

  ///////////////////////////ERROR HANDLING FUNCTIONS///////////////////////
  void SaveToErrLog(const string& MsgStr){
    string fNm = "RunTimeErrors.Log";
    ofstream fOut(fNm, ofstream::app); if (!fOut.good()) { return; }
    fOut << "--------[[[\r\n" << fNm << "\r\n" << MsgStr <<
      "\r\n]]]---------";
    fOut.close();
  }

  void ExeStop(const string& MsgStr, const string& CondStr, const string& fNm, const int& LnN){
    // construct full message
    ostringstream sOut;
    sOut << "Execution stopped {";
    if (!MsgStr.empty()) { sOut << endl << "  [Message: " << MsgStr << "]"; }
    if (!CondStr.empty()) {
      sOut << endl << "  [Condition: " << CondStr <<
        "]";
    }
    if (!fNm.empty()) {
      sOut << endl << "  [File: " << fNm << " (Line:" <<
        LnN << ")" << "]";
    }
    sOut << "}" << endl;
    // report full message to console & log file
    cout << sOut.str();
    SaveToErrLog(sOut.str());
    // finish execution
    exit(EXIT_FAILURE);
  }

  ////////////////HELPER FUNCTIONS////////////////////////////


}
