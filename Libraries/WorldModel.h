#ifndef WorldMd_h
#define WorldMd_h

#include "amglib.h"

using namespace amg;

//forward decalarations
class TWorldMd;

class TAgent;
typedef shared_ptr<TAgent> PAgent;

typedef unordered_map<string, PAgent> NmToPAgentM;

//////Possible Property Values
//InInventoryS: cloak, basket
//WantS: see grandma, 
//AwareOfS: [agent], [agent] + [attribute/attribute group]
////ex. grandma's location

///////////////////////////
// attribue values
enum class TAType{
  Undef, Bool, Int, Dbl, Str, Vec, Set, Map
};

class TAVal{
private:
  TAType Type;
  bool Bool;
  int Int;
  double Dbl;
  string Str;
  vector<TAVal> Vec;
  multiset<TAVal> Set;
  unordered_map<string, TAVal> Map;
public:
  //always call bool & int & double constructor as they're primitive and have no default constructors
  TAVal(const TAType& _Type = TAType::Undef):
    Type(_Type), Bool(false), Int(0), Dbl(0.0), Str(), /*AValPr(),*/ Vec(), Set(), Map(){}
  TAVal(const TAVal& CopyTAVal){
    Type = CopyTAVal.Type;
    Bool = CopyTAVal.Bool;
    Int = CopyTAVal.Int;
    Dbl = CopyTAVal.Dbl;
    Str = CopyTAVal.Str;
    Vec = CopyTAVal.Vec;
    Set = CopyTAVal.Set;
    Map = CopyTAVal.Map;
  }

  /*static TAVal GetNewAVal(const TAVal& OldAVal){
    //TAVal NewAVal = TAVal(OldAVal);
    shared_ptr<TAVal> PNewAVal(TAVal(OldAVal));
    return PNewAVal.get();
  }*/

  static TAVal GenUndef(){ return TAVal(TAType::Undef); }
  static TAVal GenBool(const bool& _Bool){ TAVal AVal(TAType::Bool); AVal.SetBool(_Bool); return AVal; }
  static TAVal GenInt(const int& _Int){ TAVal AVal(TAType::Int); AVal.SetInt(_Int); return AVal; }
  static TAVal GenDbl(const double& _Dbl){ TAVal AVal(TAType::Dbl); AVal.SetDbl(_Dbl); return AVal; }
  static TAVal GenStr(const string& _Str){ TAVal AVal(TAType::Str); AVal.SetStr(_Str); return AVal; }

  bool operator==(const TAVal& AVal) const;
  bool operator<(const TAVal& AVal) const;

  TAType GetType() const{ return Type; }
  bool GetBool() const{ IAssert(Type == TAType::Bool); return Bool; }
  int GetInt() const{ IAssert(Type == TAType::Int); return Int; }
  double GetDbl() const{ IAssert(Type == TAType::Dbl); return Dbl; }
  string GetStr() const{ IAssert(Type == TAType::Str); return Str; }
  vector<TAVal> GetVec() const{ IAssert(Type == TAType::Vec); return Vec; }
  multiset<TAVal> GetSet() const{ IAssert(Type == TAType::Set); return Set; }
  unordered_map<string, TAVal> GetMap() const{ IAssert(Type == TAType::Map); return Map; }

  void SetType(const TAType& _Type){
    Type = _Type; Bool = false; Int = 0; Dbl = 0; Str = ""; Vec.clear(); Set.clear(); Map.clear();
  }
  TAVal& SetBool(const bool& _Bool){ Type = TAType::Bool; Bool = _Bool; return *this; }
  TAVal& SetInt(const int& _Int){ Type = TAType::Int; Int = _Int; return *this; }
  TAVal& SetDbl(const double& _Dbl){ Type = TAType::Dbl; Dbl = _Dbl; return *this; }
  TAVal& SetStr(const string& _Str){ Type = TAType::Str; Str = _Str; return *this; }
  TAVal& SetVec(const vector<TAVal>& _Vec){ Type = TAType::Vec; Vec = _Vec; return *this; }
  TAVal& SetSet(const multiset<TAVal>& _Set){ Type = TAType::Set; Set = _Set; return *this; }
  TAVal& SetMap(const unordered_map<string, TAVal>& _Map){ Type = TAType::Map; Map = _Map; return *this; }

  //vec
  void ClearVec(){ IAssert(Type == TAType::Vec); Vec.clear(); };
  int GetVecSz() const{ IAssert(Type == TAType::Vec); return int(Vec.size()); }
  int GetAValN(const TAVal& AVal) const{
    IAssert(Type == TAType::Vec);
    for (int AValN = 0; AValN < GetVecSz(); AValN++) {
      if (Vec[AValN] == AVal) { return AValN; }
    }
    return -1;
  };
  const TAVal& GetVecAVal(const int& AValN) const{ IAssert(Type == TAType::Vec); return Vec[AValN]; }
  void SetVecAVal(const int& AValN, const TAVal& AVal){
    IAssert(Type == TAType::Vec); Vec[AValN] = AVal;
  }
  void AddVecAVal(const TAVal& AVal){ IAssert(Type == TAType::Vec); Vec.push_back(AVal); }
  void DelVecAVal(const TAVal& AVal){
    IAssert(Type == TAType::Vec); Vec.erase(Vec.begin() + GetAValN(AVal));
  };

  //set
  void ClearSet(){ IAssert(Type == TAType::Set); Set.clear(); }
  int GetSetSz() const{ return int(Set.size()); }
  bool IsSetAVal(const TAVal& AVal) const{
    IAssert(Type == TAType::Set); return (Set.find(AVal) != Set.end());
  }
  void AddSetAVal(const TAVal& AVal);
  void DelSetAVal(const TAVal& AVal);

  string GetValStr() const;

  //map
  void ClearMap(){ IAssert(Type == TAType::Map); Map.clear(); }
  int GetMapSz() const{ return int(Map.size()); }
  bool IsMapKey(const string& KeyStr) const{
    IAssert(Type == TAType::Map); return (Map.find(KeyStr) != Map.end());
  }
  TAVal& GetOrSetMapAVal(const string& KeyStr, const TAVal& AVal);
  TAVal& GetMapAVal(const string& KeyStr){ IAssert(Type == TAType::Map); return Map[KeyStr]; }
  TAVal& SetMapAVal(const string& KeyStr, const TAVal& AVal){ IAssert(Type == TAType::Map); Map[KeyStr] = AVal; return GetMapAVal(KeyStr); }
  void DelMapAVal(const string& KeyStr){ IAssert(Type == TAType::Map); Map.erase(KeyStr); }
};

class TAValBs{
private:
  unordered_map<string, TAVal> AttrNmToValM;
public:
  TAValBs(){}
  TAValBs(const TAValBs& CopyAValBs){
    for (auto& CopyAttrNmToValI : CopyAValBs.AttrNmToValM){
      AttrNmToValM[CopyAttrNmToValI.first] = TAVal(CopyAttrNmToValI.second);
    }
  }
  ~TAValBs(){}

  bool IsAttr(const string& AttrNm) const{
    return AttrNmToValM.find(AttrNm) != AttrNmToValM.end();
  }
  TAVal& GenAttr(const string& AttrNm, const TAType& AttrType){
    AttrNmToValM[AttrNm].SetType(AttrType);
    return AttrNmToValM[AttrNm];
  }
  TAVal& GenCopyAttr(const string& AttrNm, const TAVal& PasteAVal){
    AttrNmToValM[AttrNm] = TAVal(PasteAVal);
    return AttrNmToValM[AttrNm];
  }
  void DelAttr(const string& AttrNm){
    IAssert(IsAttr(AttrNm));
    AttrNmToValM.erase(AttrNm);
  }
  const TAVal& GetAttrConst(const string& AttrNm) const{
    IAssert(IsAttr(AttrNm));
    return AttrNmToValM.at(AttrNm);
  }
  TAVal& GetAttr(const string& AttrNm){
    IAssert(IsAttr(AttrNm));
    return AttrNmToValM.at(AttrNm);
  }
  //void SetAttr(const string& AttrNm){}
  vector<pair<string, TAVal>> GetAttrNmValPrV() const;
};
typedef vector<TAValBs> TAValBsV;
typedef vector<TAValBsV> TAValBsVV;

///////////goals
class TAGoal{
private:
  //add new agent, have a nullptr to WorldMd
  NmToPAgentM GoalNmToAgentM;
  unordered_map<string, StrS> AgentNmToGoalAttrKeySM;
public:
  TAGoal(): GoalNmToAgentM(){}
  TAGoal(const TAGoal& CopyAGoal);
  ~TAGoal(){}

  NmToPAgentM& GetGoalNmToAgentM(){ return GoalNmToAgentM; }

  //Agent Admin
  bool IsGoalAttrKey(const string& AgentNm, const string& GoalAttrKey)const;
  void AddGoalAttrKey(const string& AgentNm, const string& GoalAttrKey);
  void DelGoalAttrKey(const string& AgentNm, const string& GoalAttrKey);

  bool IsAgent(const string& AgentNm) const{ return (GoalNmToAgentM.find(AgentNm) != GoalNmToAgentM.end()); }
  void AddAgent(const PAgent Agent);
  void DelAgent(const string& AgentNm);
  PAgent GetAgent(const string& AgentNm);

  static double GetAttrScore(const TAVal& ScoreAVal, const TAVal& GoalAVal);
  double GetStateScore(NmToPAgentM NmToAgentM)const;
  double GetStateImprovedScore(NmToPAgentM OrigNmToAgentM, NmToPAgentM NewNmToAgentM);
  bool IsGoalComplete(NmToPAgentM CurNmToAgentM)const;
};

class TAGoalBs{
private:
  unordered_map<string, TAGoal> NmToGoalM;
  StrS CompleteGoalNmS;
  unordered_map<string, StrS> GoalNmToPreGoalNmSM;
public:
  TAGoalBs(): NmToGoalM(){}
  TAGoalBs(const TAGoalBs& CopyAGoalBs){
    for (auto& CopyNmToGoalI : CopyAGoalBs.NmToGoalM){
      NmToGoalM[CopyNmToGoalI.first] = TAGoal(CopyNmToGoalI.second);
    }
    for (auto& CopyCompleteGoalNmI : CopyAGoalBs.CompleteGoalNmS){
      CompleteGoalNmS.insert(CopyCompleteGoalNmI);
    }
    for (auto& CopyGoalNmToPreGoalNmSI : CopyAGoalBs.GoalNmToPreGoalNmSM){
      GoalNmToPreGoalNmSM[CopyGoalNmToPreGoalNmSI.first] = CopyGoalNmToPreGoalNmSI.second;
    }
  }
  ~TAGoalBs(){}

  bool IsGoal(const string& GoalNm) const;
  void AddGoal(const string& GoalNm, const TAGoal& AGoal, const StrS& PreGoalNmS){
    NmToGoalM[GoalNm] = AGoal;
    GoalNmToPreGoalNmSM[GoalNm] = PreGoalNmS;
  }

  bool IsPreGoalsComplete(const string& GoalNm)const;
  bool IsAllPreGoalsComplete()const;
  bool IsAllDoableGoalsComplete()const;
  bool IsGoalComplete(const string& GoalNm)const;

  unordered_map<string, TAGoal>& GetNmToGoalMRef(){ return NmToGoalM; }
  TAGoal& GetGoal(const string& GoalNm){ return NmToGoalM[GoalNm]; }

  void UpdateCompleteGoals(NmToPAgentM& CurNmToAgentM);
  /*TAVal& GenAttr(const string& AttrNm, const TAType& AttrType){
    AttrNmToValM[AttrNm].SetType(AttrType);
    return AttrNmToValM[AttrNm];
  }
  void DelAttr(const string& AttrNm){
    IAssert(IsAttr(AttrNm));
    AttrNmToValM.erase(AttrNm);
  }
  const TAVal& GetAttr(const string& AttrNm) const{
    IAssert(IsAttr(AttrNm));
    return AttrNmToValM.at(AttrNm);
  }*/

  vector<pair<string, TAGoal>> GetNmGoalPrV();

  StrToDblM GetEvalGoalNmToActionScoreM(const NmToPAgentM& OrigNmToAgentM, const NmToPAgentM& NewNmToAgentM);
};

/////////////////run action
class TRunAction{
public:
  string ActionNm;
  string ActionDesc;
  string PrimaryGoalNm;
  double TotalActionScore;
  NmToPAgentM NmToAgentM;
public:
  TRunAction(): ActionNm(""), ActionDesc(""), PrimaryGoalNm(""), TotalActionScore(0.0){}
  TRunAction(const string& _ActionNm, const string& _ActionDesc, const string& _PrimaryGoalNm, const double& _TotalActionScore, const NmToPAgentM& _NmToAgentM):
    ActionNm(_ActionNm), ActionDesc(_ActionDesc), PrimaryGoalNm(_PrimaryGoalNm), TotalActionScore(_TotalActionScore), NmToAgentM(_NmToAgentM){};
  ~TRunAction(){};

};

//////////////////
// agents
//observe environment -- can access thru functions in TWorldMd
//react to environment  --  have functions
//has current state (x,y,mood etc) -- map with attributes of any type

//typedef void (TAgent::* AgentFunc)();
//typedef void (TLivingAgent::* AgentFunc)();
typedef shared_ptr<TWorldMd> PWorldMd;
class TAgent{
protected:
  PWorldMd WorldMd;
  TAValBs AValBs;
  TAGoalBs AGoalBs;
public:
  const string ANmKey;
  const string AMainTypeKey;
  const string ATypeSKey;
public:
  TAgent(TWorldMd& _WorldMd, const string& AgentNm): WorldMd(&_WorldMd), ANmKey("Name"), AMainTypeKey("MainType"), ATypeSKey("TypeS"){
    AValBs.GenAttr(ANmKey, TAType::Str).SetStr(AgentNm);
    AValBs.GenAttr(AMainTypeKey, TAType::Str).SetStr("TAgent");
    AValBs.GenAttr(ATypeSKey, TAType::Set).AddSetAVal(TAVal::GenStr("TAgent"));
  }
  TAgent(const TAgent& CopyAgent):
    WorldMd(CopyAgent.WorldMd),//note the agent isn't created inside the world model
    AValBs(CopyAgent.AValBs),
    AGoalBs(CopyAgent.AGoalBs),
    ANmKey(CopyAgent.ANmKey),
    AMainTypeKey(CopyAgent.AMainTypeKey),
    ATypeSKey(CopyAgent.ATypeSKey){}
  ~TAgent(){}

  virtual PAgent ClonePAgent(){ return PAgent(new TAgent(*this)); }

  //helper functions
  void SetWorldMd(PWorldMd NewWorldMd);

  bool IsAgentType(const string& TypeStr);
  bool IsAgentMainType(const string& TypeStr);
  string GetAgentMainTypeStr();

  void SetAValBs(TAValBs NewAValBs){ AValBs = NewAValBs; }
  TAValBs& GetAValBs(){ return AValBs; }

  void SetAGoalBs(TAGoalBs NewAGoalBs){ AGoalBs = NewAGoalBs; }
  TAGoalBs& GetAGoalBs(){ return AGoalBs; }

  bool IsAttr(const string& AttrNm)const{ return AValBs.IsAttr(AttrNm); }
  const TAVal& GetAttrConst(const string& AttrNm)const{ IAssert(IsAttr(AttrNm)); return AValBs.GetAttrConst(AttrNm); }
  TAVal& GetAttr(const string& AttrNm){ IAssert(IsAttr(AttrNm)); return AValBs.GetAttr(AttrNm); }

  bool IsAllDoableGoalsComplete();
  bool IsGoal(const string& GoalNm)const{ return AGoalBs.IsGoal(GoalNm); }

  bool IsAgentNm()const{ return AValBs.IsAttr("Name"); }

  string GetAgentNmStr() const{
    IAssert(IsAgentNm());
    string AgentNmStr = AValBs.GetAttrConst("Name").GetStr();
    return AgentNmStr;
  }

  void EvalAction(const NmToPAgentM& SimNmToAgentM, const string& ActionNm, const string& ActionDesc, 
    const string& PrimaryGoalNm, TRunAction& BestRunAction);
  double GetTotalActionScore(const NmToPAgentM& SimNmToAgentM);
  string GetActionPrimaryGoalNm(const NmToPAgentM& SimNmToAgentM);

  NmToPAgentM SimAction(void (TAgent::* AgentFunc)()){ return NmToPAgentM(); };

  bool IsStopAllActions();
  virtual TRunAction GetBestAction();

  bool Run();
};
//typedef shared_ptr<TAgent> PAgent;


class TTangibleAgent : public TAgent{
public:
  const string InInventorySKey;
  //const string InAgentStrKey;
  const string XKey;
  const string YKey;
public:
  TTangibleAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TAgent(_WorldMd, AgentNm),
    InInventorySKey("InInventoryS"),
    //InAgentStrKey("InAgentStr"),
    XKey("X"),
    YKey("Y"){
    AValBs.GetAttr(AMainTypeKey).SetStr("TTangibleAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TTangibleAgent"));
    AValBs.GenAttr(InInventorySKey, TAType::Set);
    //AValBs.GenAttr(InAgentStrKey, TAType::Str);
    AValBs.GenAttr(XKey, TAType::Int);
    AValBs.GenAttr(YKey, TAType::Int);
    AValBs.GetAttr(XKey).SetInt(XInt);
    AValBs.GetAttr(YKey).SetInt(YInt);
  }
  TTangibleAgent(const TTangibleAgent& CopyTangibleAgent):
    TAgent(CopyTangibleAgent),
    InInventorySKey(CopyTangibleAgent.InInventorySKey),
    //InAgentStrKey(CopyTangibleAgent.InAgentStrKey),
    XKey(CopyTangibleAgent.XKey),
    YKey(CopyTangibleAgent.YKey){}
  ~TTangibleAgent(){}

  PAgent ClonePAgent();

  //helper functions
  bool IsInInventory(const string& Item)const{ return GetAttrConst(InInventorySKey).IsSetAVal(TAVal::GenStr(Item)); }

  //bool IsInAgent()const{ return GetAttrConst(InAgentStrKey).GetStr() != ""; }

  bool IsAgentSameXY(const string& CompAgentNm);
  bool IsAgentXY(const int& X, const int& Y) const;
  void SetXY(const int& X, const int& Y){ SetX(X); SetY(Y); }

  int GetX()const{ return GetAttrConst(XKey).GetInt(); }
  void SetX(const int& NewX){ GetAttr(XKey).SetInt(NewX); }
  void IncX(){ GetAttr(XKey).SetInt(GetAttr(XKey).GetInt() + 1); }
  void DecX(){ GetAttr(XKey).SetInt(GetAttr(XKey).GetInt() - 1); }

  int GetY()const{ return GetAttrConst(YKey).GetInt(); }
  void SetY(const int& NewY){ GetAttr(YKey).SetInt(NewY); }
  void IncY(){ GetAttr(YKey).SetInt(GetAttr(YKey).GetInt() + 1); }
  void DecY(){ GetAttr(YKey).SetInt(GetAttr(YKey).GetInt() - 1); }

  bool CanGiveToAgentInventory(const string& GiveToAgentNm, const string& Item);
  void EffectGiveToAgentInventory(const string& GiveToAgentNm, const string& Item);

  bool CanTakeFromAgentInventory(const string& TakeFromAgentNm, const string& Item);
  void EffectTakeFromAgentInventory(const string& GiveToAgentNm, const string& Item);

  //helper actions
  bool TakeFromAgentInventory(const string& AgentNm, const string& Item);
  bool GiveToAgentInventory(const string& AgentNm, const string& Item);

  void AddInInventory(const string& Item);
  void DelInInventory(const string& Item);

  //void GoInAgent(const string& AgentNm);
  //void ExitAgent(){ GetAttr(InAgentStrKey).SetStr(""); }
  //run level action
  void RunGiveToAgentInventory(const string& GiveToAgentNm, const string& Item);

  void RunTakeFromAgentInventory(const string& TakeFromAgentNm, const string& Item);

  NmToPAgentM SimAction(const string& Str1, const string& Str2, void (TTangibleAgent::* AgentFunc)(const string&, const string&));

  TRunAction GetBestAction();
};
typedef shared_ptr<TTangibleAgent> PTangibleAgent;

class TGeoAgent : public TTangibleAgent{
public:
public:
  TGeoAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TTangibleAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TGeoAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TGeoAgent"));
  }
  TGeoAgent(const TGeoAgent& CopyGeoAgent): TTangibleAgent(CopyGeoAgent){}
  ~TGeoAgent(){}

  PAgent ClonePAgent();

  TRunAction GetBestAction();
};
typedef shared_ptr<TGeoAgent> PGeoAgent;

class THouseAgent : public TGeoAgent{
public:
public:
  THouseAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TGeoAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("THouseAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("THouseAgent"));
  }
  THouseAgent(const THouseAgent& CopyHouseAgent): TGeoAgent(CopyHouseAgent){}
  ~THouseAgent(){}

  PAgent ClonePAgent();

  TRunAction GetBestAction();
};
typedef shared_ptr<THouseAgent> PHouseAgent;

class TFlowerFieldAgent : public TGeoAgent{
public:
public:
  TFlowerFieldAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TGeoAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TFlowerFieldAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TFlowerFieldAgent"));
  }
  TFlowerFieldAgent(const TFlowerFieldAgent& CopyFlowerFieldAgent): TGeoAgent(CopyFlowerFieldAgent){}
  ~TFlowerFieldAgent(){}

  PAgent ClonePAgent();

  TRunAction GetBestAction();
};
typedef shared_ptr<TFlowerFieldAgent> PFlowerFieldAgent;

class TForestAgent : public TGeoAgent{
public:
public:
  TForestAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TGeoAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TForestAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TForestAgent"));
  }
  TForestAgent(const TForestAgent& CopyForestAgent): TGeoAgent(CopyForestAgent){}
  ~TForestAgent(){}

  PAgent ClonePAgent();

  TRunAction GetBestAction();
};
typedef shared_ptr<TForestAgent> PForestAgent;

class TLivingAgent : public TTangibleAgent{
public:
  const string WantSKey;
  const string EquipSKey;
  const string AwareOfSKey;
  const string GoalNmToAgentNmToRelationStrMMKey;
  const string EmotionalScoreKey;
  const string IsConsKey;
  //const string DesireSKey;
  //const string SatisfiedDesireSKey;
public:
  TLivingAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TTangibleAgent(_WorldMd, AgentNm, XInt, YInt),
    WantSKey("WantS"),
    EquipSKey("EquipS"),
    AwareOfSKey("AwareOfS"),
    //DesireSKey("DesireS"),
    //SatisfiedDesireSKey("SatisfiedDesireS"),
    GoalNmToAgentNmToRelationStrMMKey("GoalNmToAgentNmToRelationStrMM"),
    EmotionalScoreKey("EmotionalScore"),
    IsConsKey("IsCons"){
    AValBs.GetAttr(AMainTypeKey).SetStr("TLivingAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TLivingAgent"));
    AValBs.GenAttr(WantSKey, TAType::Set);
    AValBs.GenAttr(EquipSKey, TAType::Set);
    AValBs.GenAttr(AwareOfSKey, TAType::Set);
    AValBs.GenAttr(GoalNmToAgentNmToRelationStrMMKey, TAType::Map);
    AValBs.GenAttr(EmotionalScoreKey, TAType::Dbl);
    AValBs.GenAttr(IsConsKey, TAType::Bool); AValBs.GetAttr(IsConsKey).SetBool(true);
    //AValBs.GenAttr(DesireSKey, TAType::Set);
    //AValBs.GenAttr(SatisfiedDesireSKey, TAType::Set);
  }
  TLivingAgent(const TLivingAgent& CopyLivingAgent):
    TTangibleAgent(CopyLivingAgent),
    WantSKey(CopyLivingAgent.WantSKey),
    EquipSKey(CopyLivingAgent.EquipSKey),
    AwareOfSKey(CopyLivingAgent.AwareOfSKey),
    GoalNmToAgentNmToRelationStrMMKey(CopyLivingAgent.GoalNmToAgentNmToRelationStrMMKey),
    EmotionalScoreKey(CopyLivingAgent.EmotionalScoreKey),
    IsConsKey(CopyLivingAgent.IsConsKey){}

  //DesireSKey(CopyLivingAgent.DesireSKey),
  //SatisfiedDesireSKey(CopyLivingAgent.SatisfiedDesireSKey){}
  ~TLivingAgent(){}

  PAgent ClonePAgent();

  //helper functions
  bool IsEquipped(const string& Item)const{ return GetAttrConst(EquipSKey).IsSetAVal(TAVal::GenStr(Item)); }

  bool CanEquip(const string& Item)const;
  void EffectEquip(const string& Item);

  bool CanUnequip(const string& Item)const;
  void EffectUnequip(const string& Item);

  bool CanPickFlowers();
  void EffectPickFlowers();

  void AddWant(const string& AddWantStr){ GetAttr(WantSKey).AddSetAVal(TAVal::GenStr(AddWantStr)); }
  void DelWant(const string& DelWantStr){ GetAttr(WantSKey).DelSetAVal(TAVal::GenStr(DelWantStr)); }

  bool IsAwareOf(const string& AwareOfStr);
  void AddAwareOf(const string& AddAwareOfStr);
  void DelAwareOf(const string& DelAwareOfStr);

  void AddGoalAgentRelation(const string& GoalNm, const string& AgentNm, const string& RelationStr);
  void DelGoalAgentRelation(const string& GoalNm, const string& AgentNm);

  void SetEmotionalScore(const double AddDbl);
  void AddToEmotionalScore(const double AddDbl);

  bool CanGetAgentAwareOf(const string& AgentNm, const string& GetAwareOf);
  void EffectGetAgentAwareOf(const string& AgentNm, const string& GetAwareOf);

  bool IsCons()const;
  void SetIsCons(const bool& IsCons);

  bool CanEatAgent(const string& EatAgentNm);
  void EffectEatAgent(const string& EatAgentNm);

  //void AddDesire(const string& DesireStr);
  // void DelDesire(const string& DesireStr);

  //helper actions
  void MoveAgentsInInventoryToNewPos();
  void Wander(const int& Moves, int SetMoveDir = -1);

  void Equip(const string& Item){ if (CanEquip(Item)){ EffectEquip(Item); } }
  void Unequip(const string& Item){ if (CanUnequip(Item)){ EffectUnequip(Item); } }

  //run level actions
  void RunMove(const int& MoveDir = -1);
  void RunPickFlowers(){ if (CanPickFlowers()){ EffectPickFlowers(); } }
  void RunGetAgentAwareOf(const string& AgentNm, const string& GiveAwareOf);
  void RunEatAgent(const string& AgentNm);


  NmToPAgentM SimAction(void (TLivingAgent::* AgentFunc)());
  NmToPAgentM SimAction(const int& Int1, void (TLivingAgent::* AgentFunc)(const int&));
  NmToPAgentM SimAction(const string& Str1, void (TLivingAgent::* AgentFunc)(const string&));
  NmToPAgentM SimAction(const string& Str1, const string& Str2, void (TLivingAgent::* AgentFunc)(const string&, const string&));

  TRunAction GetBestAction();
};
typedef shared_ptr<TLivingAgent> PLivingAgent;

class TAnimalAgent : public TLivingAgent{
public:
public:
  TAnimalAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TLivingAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TAnimalAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TAnimalAgent"));
  }
  TAnimalAgent(const TAnimalAgent& CopyAnimalAgent): TLivingAgent(CopyAnimalAgent){}
  ~TAnimalAgent(){}

  PAgent ClonePAgent();
  //helper functions

  //helper actions

  //run-level actions

  TRunAction GetBestAction();
};
typedef shared_ptr<TAnimalAgent> PAnimalAgent;

class TWolfAgent : public TAnimalAgent{
public:
public:
  TWolfAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TAnimalAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TWolfAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TWolfAgent"));
  }
  TWolfAgent(const TWolfAgent& CopyWolfAgent): TAnimalAgent(CopyWolfAgent){}
  ~TWolfAgent(){}

  PAgent ClonePAgent();
  //helper functions

  //helper actions

  //run-level actions
  void RunMoveFast(const int& MoveDir = -1);

  NmToPAgentM SimAction(const int& Int1, void(TWolfAgent::* AgentFunc)(const int&));

  TRunAction GetBestAction();
};
typedef shared_ptr<TWolfAgent> PWolfAgent;

class TButterflyAgent : public TAnimalAgent{
public:
public:
  TButterflyAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TAnimalAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TButterflyAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TButterflyAgent"));
  }
  TButterflyAgent(const TButterflyAgent& CopyButterflyAgent): TAnimalAgent(CopyButterflyAgent){}
  ~TButterflyAgent(){}

  PAgent ClonePAgent();
  //helper functions

  //helper actions

  //run-level actions

  TRunAction GetBestAction();
};
typedef shared_ptr<TButterflyAgent> PButterflyAgent;

class TFrogAgent : public TAnimalAgent{
public:
public:
  TFrogAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TAnimalAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TFrogAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TFrogAgent"));
  }
  TFrogAgent(const TFrogAgent& CopyFrogAgent): TAnimalAgent(CopyFrogAgent){}
  ~TFrogAgent(){}

  PAgent ClonePAgent();
  //helper functions

  //helper actions

  //run-level actions

  TRunAction GetBestAction();
};
typedef shared_ptr<TFrogAgent> PFrogAgent;

class THumanAgent : public TLivingAgent{
public:
  string LivesInAgentStrKey;
public:
  THumanAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    TLivingAgent(_WorldMd, AgentNm, XInt, YInt),
    LivesInAgentStrKey("LivesInAgentStr"){
    AValBs.GetAttr(AMainTypeKey).SetStr("THumanAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("THumanAgent"));
    AValBs.GenAttr(LivesInAgentStrKey, TAType::Str);
  }
  THumanAgent(const THumanAgent& CopyHumanAgent):
    TLivingAgent(CopyHumanAgent),
    LivesInAgentStrKey(CopyHumanAgent.LivesInAgentStrKey){}
  ~THumanAgent(){}

  PAgent ClonePAgent();

  //helper actions
  void LivesInAgent(const string& AgentNm);

  bool CanAskApproval(const string& ApproveGoalNm, const string& RespondAgentNm);
  void EffectAskApproval(const string& ApproveGoalNm, const string& RespondAgentNm);

  bool CanRespondToAskApproval(const string& ApproveGoalNm, const string& AskAgentNm);
  void EffectRespondToAskApproval(const string& ApproveGoalNm, const string& AskAgentNm);
  void RespondToApproval(const string& ApproveGoal, const string& AskAgentNm);

  bool CanLookAtAgent(const string& AgentNm);
  void EffectLookAtAgent(const string& AgentNm);

  bool CanCryForHelp();
  void EffectCryForHelp(const double& MxCryDist);

  bool CanRespondToCryForHelp();
  void EffectRespondToCryForHelp(const int& X, const int& Y);
  void RespondToCryForHelp(const int& X, const int& Y);

  bool CanMakeAgentSpitOutAgent(const string& SpitAgentNm, const string& SpitOutAgentNm);
  void EffectMakeAgentSpitOutAgent(const string& SpitAgentNm, const string& SpitOutAgentNm);

  bool CanCarryAgent(const string& CarryAgentNm);
  void EffectCarryAgent(const string& CarryAgentNm);

  //run-level actions
  void RunAskApproval(const string& ApproveGoal, const string& RespondAgentNm);

  void RunLookAtAgent(const string& AgentNm);

  void RunCryForHelp();

  void RunMakeAgentSpitOutAgent(const string& SpitAgentNm, const string& SpitOutAgentNm);

  void RunCarryAgent(const string& CarryAgentNm);

  NmToPAgentM SimAction(const string& Str1, const string& Str2, void (THumanAgent::* AgentFunc)(const string&, const string&));
  NmToPAgentM SimAction(const string& Str1, void (THumanAgent::* AgentFunc)(const string&));
  NmToPAgentM SimAction(void (THumanAgent::* AgentFunc)());

  TRunAction GetBestAction();
};
typedef shared_ptr<THumanAgent> PHumanAgent;

class TRidingHoodAgent : public THumanAgent{
public:
public:
  TRidingHoodAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    THumanAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TRidingHoodAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TRidingHoodAgent"));
  }
  TRidingHoodAgent(const TRidingHoodAgent& CopyRidinHoodAgent):THumanAgent(CopyRidinHoodAgent){}
  ~TRidingHoodAgent(){}

  PAgent ClonePAgent();

  TRunAction GetBestAction();
};
typedef shared_ptr<TRidingHoodAgent> PRidingHoodAgent;

class TMotherAgent : public THumanAgent{
public:
public:
  TMotherAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    THumanAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TMotherAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TMotherAgent"));
  }
  TMotherAgent(const TMotherAgent& CopyMotherAgent): THumanAgent(CopyMotherAgent){}
  ~TMotherAgent(){}

  PAgent ClonePAgent();

  //helper actions

  //run-level actions

  TRunAction GetBestAction();
};
typedef shared_ptr<TMotherAgent> PMotherAgent;

class TGrandmaAgent : public THumanAgent{
public:
public:
  TGrandmaAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    THumanAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TGrandmaAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TGrandmaAgent"));
  }
  TGrandmaAgent(const TGrandmaAgent& CopyGrandmaAgent): THumanAgent(CopyGrandmaAgent){}
  ~TGrandmaAgent(){}

  PAgent ClonePAgent();

  //helper actions

  //run-level actions

  TRunAction GetBestAction();

};
typedef shared_ptr<TGrandmaAgent> PGrandmaAgent;

class TWoodsmanAgent : public THumanAgent{
public:
public:
  TWoodsmanAgent(TWorldMd& _WorldMd, const string& AgentNm, const int& XInt, const int& YInt):
    THumanAgent(_WorldMd, AgentNm, XInt, YInt){
    AValBs.GetAttr(AMainTypeKey).SetStr("TWoodsmanAgent");
    AValBs.GetAttr(ATypeSKey).AddSetAVal(TAVal::GenStr("TWoodsmanAgent"));
  }
  TWoodsmanAgent(const TWoodsmanAgent& CopyWoodsmanAgent): THumanAgent(CopyWoodsmanAgent){}
  ~TWoodsmanAgent(){}

  PAgent ClonePAgent();

  //helper actions

  //run-level actions

  TRunAction GetBestAction();

};
typedef shared_ptr<TWoodsmanAgent> PWoodsmanAgent;

////////////////
// geography
/*class TGeo{
private:
  TAValBsVV AValBsVV;
public:
  TGeo(const int& MxX, const int& MxY):
    AValBsVV(MxX, TAValBsV(MxY, TAValBs())){}
  ~TGeo(){}

  void Draw() const;

  bool Is(const int& X, const int& Y, const string& AttrNm){
    return AValBsVV[X][Y].IsAttr(AttrNm);
  }
  TAValBs& Get(const int& X, const int& Y){ return AValBsVV[X][Y]; }
  void Set(const int& X, const int& Y, const string& AttrNm, const string& AttrStr){
    AValBsVV[X][Y].GenAttr(AttrNm).SetStr(AttrStr);
  }
  void SetVecVal(const int& X, const int& Y, const string& AttrNm, const string& AttrStr){
    //TAVal AVal = TAVal::SetStr(AttrStr);
    //AValBsVV[X][Y].GenAttr(AttrNm).SetVecAVal();
  }
  void Del(const int& X, const int& Y, const string& AttrNm){
    IAssert(Is(X, Y, AttrNm));
    AValBsVV[X][Y].DelAttr(AttrNm);
  }

  void GetMxXY(int& MxX, int& MxY){
    MxX = int(AValBsVV.size() - 1);
    MxY = int(AValBsVV[0].size() - 1);
  }
};
*/

//////////////////
// world models
class TWorldMd{
private:
  NmToPAgentM NmToAgentM;
  int Time;
protected:
  //TGeo Geo;
public:
  TWorldMd(): Time(0){}
  /*TWorldMd(TWorldMd& CopyWorldMd){
    for (auto& CopyNmToAgentI : CopyWorldMd.NmToAgentM){
      NmToAgentM[CopyNmToAgentI.first] = PAgent(new TAgent(*CopyNmToAgentI.second));
      NmToAgentM[CopyNmToAgentI.first]->SetWorldMd(*this);
    }

    Time = CopyWorldMd.Time;
  }*/
  ~TWorldMd(){}

  void Run();
  void Draw()const;
  bool IsAllDoableGoalsComplete();

  const int& GetTime(){ return Time; }
  TWorldMd* GetWorldMd(){ return this; }


  //Agent Admin
  bool IsAgentType(const string& SearchAgentType)const;
  bool IsAgent(const string& AgentNm) const{ return (NmToAgentM.find(AgentNm) != NmToAgentM.end()); }

  void AddAgent(const PAgent Agent);
  void DelAgent(const string& AgentNm);
  PAgent GetAgent(const string& AgentNm);
  PTangibleAgent GetNearestAgentByType(const string& SrcAgentNm, const string& SearchAgentType);
  pair<double, double> GetNearestAgentXYPrByType(const string& SrcAgentNm, const string& SearchAgentType);
  double GetNearestAgentDistByType(const string& SrcAgentNm, const string& SearchAgentType);
  PAgent GetFirstAgentByType(const string& SearchAgentType);

  void AddGoal(const string& AgentNm, const string& GoalNm, const TAGoal& Goal, const StrS& PreGoalS);

  void SetAgentsValsGoalsBs(NmToPAgentM& NewNmToAgentM){
    for (auto& NmToAgentI : NmToAgentM){
      string AgentNm = NmToAgentI.first;
      if (NewNmToAgentM.find(AgentNm) == NewNmToAgentM.end()){ continue; }
      TAValBs NewAValBs = NewNmToAgentM[AgentNm]->GetAValBs();
      TAGoalBs NewAGoalBs = NewNmToAgentM[AgentNm]->GetAGoalBs();
      NmToAgentI.second->SetAValBs(NewAValBs);
      NmToAgentI.second->SetAGoalBs(NewAGoalBs);
    }
  }

  void SetNmToAgentM(NmToPAgentM& NewNmToAgentM){ NmToAgentM = NewNmToAgentM; }
  NmToPAgentM& GetNmToAgentMRef(){ return NmToAgentM; }
  NmToPAgentM GetNmToAgentMCopy()const;

};

class TRidingHoodWorldMd : public TWorldMd{
private:
public:
  TRidingHoodWorldMd(): TWorldMd(){
    //init agents
    PHouseAgent RHHouse(new THouseAgent(*this, "rhhouse", 1, 0)); AddAgent(RHHouse);
    PHouseAgent GrandmaHouse(new THouseAgent(*this, "grandmahouse", 1, 8)); AddAgent(GrandmaHouse);
    PForestAgent Forest1(new TForestAgent(*this, "forest1", 0, 7)); AddAgent(Forest1);
    PForestAgent Forest2(new TForestAgent(*this, "forest2", 0, 4)); AddAgent(Forest2);
    PForestAgent Forest3(new TForestAgent(*this, "forest3", 0, 5)); AddAgent(Forest3);
    PForestAgent Forest4(new TForestAgent(*this, "forest4", 0, 6)); AddAgent(Forest4);
    PForestAgent Forest5(new TForestAgent(*this, "forest5", 2, 7)); AddAgent(Forest5);
    PForestAgent Forest6(new TForestAgent(*this, "forest6", 2, 4)); AddAgent(Forest6);
    PForestAgent Forest7(new TForestAgent(*this, "forest7", 2, 5)); AddAgent(Forest7);
    PForestAgent Forest8(new TForestAgent(*this, "forest8", 2, 6)); AddAgent(Forest8);
    PForestAgent Forest9(new TForestAgent(*this, "forest9", 3, 4)); AddAgent(Forest9);
    PForestAgent Forest10(new TForestAgent(*this, "forest10", 3, 6)); AddAgent(Forest10);
    PForestAgent Forest11(new TForestAgent(*this, "forest11", 3, 5)); AddAgent(Forest11);
    PForestAgent Forest12(new TForestAgent(*this, "forest12", 3, 7)); AddAgent(Forest12);
    PForestAgent Forest13(new TForestAgent(*this, "forest13", 2, 8)); AddAgent(Forest13);
    PForestAgent Forest14(new TForestAgent(*this, "forest14", 3, 8)); AddAgent(Forest14);
    PFlowerFieldAgent FField1(new TFlowerFieldAgent(*this, "flower field 1", 0, 3)); AddAgent(FField1);
    PFlowerFieldAgent FField2(new TFlowerFieldAgent(*this, "flower field 2", 1, 3)); AddAgent(FField2);
    PFlowerFieldAgent FField3(new TFlowerFieldAgent(*this, "flower field 3", 2, 3)); AddAgent(FField3);
    PButterflyAgent Butterfly(new TButterflyAgent(*this, "butterfly", 2, 4)); AddAgent(Butterfly);
    //PFrogAgent Frog(new TFrogAgent(*this, "frog", 3, 7)); AddAgent(Frog);
    PWolfAgent Wolf(new TWolfAgent(*this, "wolf", 3, 8)); AddAgent(Wolf);
    PMotherAgent Mother(new TMotherAgent(*this, "mother", 1, 0)); AddAgent(Mother);
    PGrandmaAgent Grandma(new TGrandmaAgent(*this, "grandma", 1, 8)); AddAgent(Grandma);
    PWoodsmanAgent Woodsman(new TWoodsmanAgent(*this, "woodsman", 0, 6)); AddAgent(Woodsman);
    PRidingHoodAgent RidingHood(new TRidingHoodAgent(*this, "riding hood", 3, 0)); AddAgent(RidingHood);

    //set agent properties
    RidingHood->LivesInAgent("rhhouse");
    Mother->LivesInAgent("rhhouse");
    Grandma->LivesInAgent("grandmahouse");
    RidingHood->AddWant("see grandma");
    Mother->AddAwareOf("grandma's location");
    Mother->AddInInventory("basket");
    GrandmaHouse->AddInInventory("nightgown");
    GrandmaHouse->AddInInventory("sleeping cap");
    GrandmaHouse->AddInInventory("grandma perfume");

    //goal 1
    const string VisitGrandmaGoalNm = "visit grandma";//forward definition
    const string TalkMotherGoalNm = "discuss visiting grandma with mother";
    {

      TAGoal TalkMotherGoal;
      //goal agent
      PRidingHoodAgent GoalRidingHood(new TRidingHoodAgent(*RidingHood));
      const string& GoalRidingHoodNm = GoalRidingHood->GetAgentNmStr();
      ////goal attributes
      TalkMotherGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->XKey);
      TalkMotherGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->YKey);
      TalkMotherGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->AwareOfSKey);
      ////goal attribute values
      pair<double, double> NearestMotherXYPr = GetNearestAgentXYPrByType(GoalRidingHoodNm, "TMotherAgent");
      GoalRidingHood->SetXY(int(NearestMotherXYPr.first), int(NearestMotherXYPr.second));
      GoalRidingHood->AddAwareOf("grandma's location");

      //goal agent
      PMotherAgent GoalMother(new TMotherAgent(*Mother));
      const string& GoalMotherNm = GoalMother->GetAgentNmStr();
      ////goal attributes
      TalkMotherGoal.AddGoalAttrKey(GoalMotherNm, GoalMother->XKey);
      TalkMotherGoal.AddGoalAttrKey(GoalMotherNm, GoalMother->YKey);
      ////goal attribute values
      GoalMother->SetXY(GoalMother->GetX(), GoalMother->GetY());

      //goal attributes
      TalkMotherGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->GoalNmToAgentNmToRelationStrMMKey);
      //goal attribute values
      GoalRidingHood->AddGoalAgentRelation(VisitGrandmaGoalNm, GoalMotherNm, "approves");//mother relation to goal saved in ridinghood agent

      //add all goal agents
      TalkMotherGoal.AddAgent(GoalRidingHood);
      TalkMotherGoal.AddAgent(GoalMother);

      //precondition goals
      StrS PreGoalS;

      //add goal to agent
      AddGoal(RidingHood->GetAgentNmStr(), TalkMotherGoalNm, TalkMotherGoal, PreGoalS);
    }
    //goal 2
    const string PickGrandmaFlowersGoalNm = "get flowers for grandma";
    {
      TAGoal PickGrandmaFlowersGoal;
      //goal agent
      PRidingHoodAgent GoalRidingHood(new TRidingHoodAgent(*RidingHood));
      const string& GoalRidingHoodNm = GoalRidingHood->GetAgentNmStr();
      ////goal attributes
      PickGrandmaFlowersGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->InInventorySKey);
      PickGrandmaFlowersGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->XKey);
      PickGrandmaFlowersGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->YKey);
      ////goal attribute values
      GoalRidingHood->AddInInventory("flowers");
      pair<double, double> NearestFlowersXYPr = GetNearestAgentXYPrByType(GoalRidingHoodNm, "TFlowerFieldAgent");
      GoalRidingHood->SetXY(int(NearestFlowersXYPr.first), int(NearestFlowersXYPr.second));

      //add all goal agents
      PickGrandmaFlowersGoal.AddAgent(GoalRidingHood);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(TalkMotherGoalNm);

      //add goal to agent
      AddGoal(RidingHood->GetAgentNmStr(), PickGrandmaFlowersGoalNm, PickGrandmaFlowersGoal, PreGoalS);
    }

    //goal 3
    const string EnjoyNatureGoalNm = "enjoy nature";
    {
      TAGoal EnjoyNatureGoal;
      //goal agent
      PRidingHoodAgent GoalRidingHood(new TRidingHoodAgent(*RidingHood));
      const string& GoalRidingHoodNm = GoalRidingHood->GetAgentNmStr();
      ////goal attributes
      EnjoyNatureGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->XKey);
      EnjoyNatureGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->YKey);
      EnjoyNatureGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->EmotionalScoreKey);
      ////goal attribute values
      pair<double, double> NearestAnimalXY = GetNearestAgentXYPrByType(GoalRidingHoodNm, "TAnimalAgent");
      GoalRidingHood->SetXY(int(NearestAnimalXY.first), int(NearestAnimalXY.second));
      GoalRidingHood->SetEmotionalScore(10.0);

      //add all goal agents
      EnjoyNatureGoal.AddAgent(GoalRidingHood);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(TalkMotherGoalNm);
      PreGoalS.insert(PickGrandmaFlowersGoalNm);

      //add goal to agent
      AddGoal(RidingHood->GetAgentNmStr(), EnjoyNatureGoalNm, EnjoyNatureGoal, PreGoalS);
    }
    //goal 4
    //const string VisitGrandmaGoalNm = "visit grandma";
    {
      TAGoal VisitGrandmaGoal;
      //goal agent
      PGrandmaAgent GoalGrandma(new TGrandmaAgent(*dynamic_pointer_cast<TGrandmaAgent>(GetFirstAgentByType("TGrandmaAgent"))));
      const string& GoalGrandmaNm = GoalGrandma->GetAgentNmStr();

      //goal agent
      PRidingHoodAgent GoalRidingHood(new TRidingHoodAgent(*RidingHood));
      const string& GoalRidingHoodNm = GoalRidingHood->GetAgentNmStr();
      ////goal attribute values
      VisitGrandmaGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->XKey);
      VisitGrandmaGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->YKey);
      //VisitGrandmaGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->AwareOfSKey);
      ////goal attributes
      GoalRidingHood->SetXY(GoalGrandma->GetX(), GoalGrandma->GetY());
      //GoalRidingHood->AddAwareOf("grandma's location");

      //add all goal agents
      VisitGrandmaGoal.AddAgent(GoalRidingHood);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(TalkMotherGoalNm);
      PreGoalS.insert(PickGrandmaFlowersGoalNm);
      PreGoalS.insert(EnjoyNatureGoalNm);

      //add goal to agent
      AddGoal(RidingHood->GetAgentNmStr(), VisitGrandmaGoalNm, VisitGrandmaGoal, PreGoalS);
    }
    //goal 5
    const string GoToButterfliesGoalNm = "go to butterflies";
    {
      TAGoal GoToButterfliesGoal;
      //goal agent
      PWolfAgent GoalWolf(new TWolfAgent(*Wolf));
      const string& GoalWolfNm = GoalWolf->GetAgentNmStr();
      ////goal attributes
      GoToButterfliesGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->XKey);
      GoToButterfliesGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->YKey);
      ////goal attribute values
      pair<double, double> NearestButterfliesXYPr = GetNearestAgentXYPrByType(GoalWolfNm, "TButterflyAgent");
      GoalWolf->SetXY(int(NearestButterfliesXYPr.first), int(NearestButterfliesXYPr.second));

      //add all goal agents
      GoToButterfliesGoal.AddAgent(GoalWolf);

      //precondition goals
      StrS PreGoalS;

      //add goal to agent
      AddGoal(Wolf->GetAgentNmStr(), GoToButterfliesGoalNm, GoToButterfliesGoal, PreGoalS);
    }
    //goal 6
    //CAN ASK ALL AGENTS AT SAME LOCATION FOR AN ELEMENT IN THEIR AWARE SET
    //ASKS RH FOR GRANDMA LOCATION WHICH SHE HAS IN HER AWARES
    const string AskForGrandmaLocGoalNm = "find out grandma's location";
    {
      TAGoal AskForGrandmaLocGoal;
      //goal agent
      PWolfAgent GoalWolf(new TWolfAgent(*Wolf));
      const string& GoalWolfNm = GoalWolf->GetAgentNmStr();
      ////goal attributes
      AskForGrandmaLocGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->AwareOfSKey);
      ////goal attribute values
      GoalWolf->AddAwareOf("grandma's location");

      //add all goal agents
      AskForGrandmaLocGoal.AddAgent(GoalWolf);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(GoToButterfliesGoalNm);

      //add goal to agent
      AddGoal(Wolf->GetAgentNmStr(), AskForGrandmaLocGoalNm, AskForGrandmaLocGoal, PreGoalS);
    }
    //goal 7
    //MAKE WOLF A FASTER AGENT OVERALL, TRAVELS FURTHER TO GET TO RH
    //BUT ALSO TRAVELS FASTER TO GET TO GRANDMA
    const string GoToGrandmaGoalNm = "go to grandma";
    {
      TAGoal GoToGrandmaGoal;
      //goal agent
      PWolfAgent GoalWolf(new TWolfAgent(*Wolf));
      const string& GoalWolfNm = GoalWolf->GetAgentNmStr();
      ////goal attributes
      GoToGrandmaGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->XKey);
      GoToGrandmaGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->YKey);
      ////goal attribute values
      pair<double, double> NearestGrandmaXYPr = GetNearestAgentXYPrByType(GoalWolfNm, "TGrandmaAgent");
      GoalWolf->SetXY(int(NearestGrandmaXYPr.first), int(NearestGrandmaXYPr.second));

      //add all goal agents
      GoToGrandmaGoal.AddAgent(GoalWolf);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(AskForGrandmaLocGoalNm);

      //add goal to agent
      AddGoal(Wolf->GetAgentNmStr(), GoToGrandmaGoalNm, GoToGrandmaGoal, PreGoalS);
    }
    //goal 8
    //GRANDMA GOES IN WOLFS INTERNAL INVENTORY
    const string EatGrandmaGoalNm = "satisfy hunger";
    {
      TAGoal EatGrandmaGoal;
      //goal agent
      PWolfAgent GoalWolf(new TWolfAgent(*Wolf));
      const string& GoalWolfNm = GoalWolf->GetAgentNmStr();
      ////goal attributes
      EatGrandmaGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->InInventorySKey);
      ////goal attribute values
      PTangibleAgent GrandmaAgent = GetNearestAgentByType(GoalWolfNm, "TGrandmaAgent");
      GoalWolf->AddInInventory(GrandmaAgent->GetAgentNmStr());

      //add all goal agents
      EatGrandmaGoal.AddAgent(GoalWolf);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(GoToGrandmaGoalNm);

      //add goal to agent
      AddGoal(Wolf->GetAgentNmStr(), EatGrandmaGoalNm, EatGrandmaGoal, PreGoalS);
    }
    //goal 9
    //HOUSE INVENTORY HAS ALL GRANDMA ITEMS
    //WOLF EQUPIS ITEMS ONE BY ONE
    //NO IMPERSONATE ACTION AS IMPERSONATION ISN'T EQUIPPED
    const string TryImpersonateGrandmaGoalNm = "try impersonating grandma";
    {
      TAGoal TryImpersonateGrandmaGoal;
      //goal agent
      PWolfAgent GoalWolf(new TWolfAgent(*Wolf));
      const string& GoalWolfNm = GoalWolf->GetAgentNmStr();
      ////goal attributes
      TryImpersonateGrandmaGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->XKey);
      TryImpersonateGrandmaGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->YKey);
      TryImpersonateGrandmaGoal.AddGoalAttrKey(GoalWolfNm, GoalWolf->InInventorySKey);
      ////goal attribute values
      pair<double, double> NearestGrandmaXYPr = GetNearestAgentXYPrByType(GoalWolfNm, "TGrandmaAgent");
      GoalWolf->SetXY(int(NearestGrandmaXYPr.first), int(NearestGrandmaXYPr.second));
      GoalWolf->AddInInventory("nightgown");
      GoalWolf->AddInInventory("sleeping cap");
      GoalWolf->AddInInventory("grandma perfume");

      //add all goal agents
      TryImpersonateGrandmaGoal.AddAgent(GoalWolf);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(EatGrandmaGoalNm);

      //add goal to agent
      AddGoal(Wolf->GetAgentNmStr(), TryImpersonateGrandmaGoalNm, TryImpersonateGrandmaGoal, PreGoalS);
    }
    //goal 10
    // SHOUT FOR HELP PRECONDITION
    //SHOUT FOR HELP HAPPENS IF WOLF LOCATION IS SAME AS HOUSE LOCATION WHICH IS SAME AS RH LOCATION
    //SHOUT FOR HELP EFFECT
    //GO THRU ALL AGENTS AND RUN THEIR RESPONLD TO HELP ACTION
    ////ACTION HAS PRECONDITION THAT THEY'RE CLOSE ENOUGH
    ////ACTION EFFECT IS THEY GO TO LOCATION OF SHOUT FOR HELP
    ////GOAL TO RESPOND TO SHOUT TO HELP IS CREATED IN RESPONDING AGENTS REACTION ACTION
    const string CryForHelpGoalNm = "get help";///
    {
      TAGoal CryForHelpGoal;
      //goal agent
      PRidingHoodAgent GoalRidingHood(new TRidingHoodAgent(*RidingHood));
      const string& GoalRidingHoodNm = GoalRidingHood->GetAgentNmStr();
      ////goal attributes
      CryForHelpGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->XKey);
      CryForHelpGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->YKey);
      ////goal attribute values
      pair<double, double> NearestGrandmaXYPr = GetNearestAgentXYPrByType(GoalRidingHoodNm, "TGrandmaAgent");
      GoalRidingHood->SetXY(int(NearestGrandmaXYPr.first), int(NearestGrandmaXYPr.second));

      //goal agent
      PWoodsmanAgent GoalWoodsman(new TWoodsmanAgent(*Woodsman));
      const string& GoalWoodsmanNm = GoalWoodsman->GetAgentNmStr();
      ////goal attributes
      CryForHelpGoal.AddGoalAttrKey(GoalWoodsmanNm, GoalWoodsman->AwareOfSKey);
      ////goal attribute values
      GoalWoodsman->AddAwareOf("cry for help");

      //add all goal agents
      CryForHelpGoal.AddAgent(GoalRidingHood);
      CryForHelpGoal.AddAgent(GoalWoodsman);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(VisitGrandmaGoalNm);

      //add goal to agent
      AddGoal(RidingHood->GetAgentNmStr(), CryForHelpGoalNm, CryForHelpGoal, PreGoalS);
    }
    //goal 11
    //WOODSMAN SPECIFIC
    //SAME LOCATION AS WOLF WHICH HAS A HUMAN AGENT IN HIS INVENTORY
    //EFFECTS:
    //HUMAN AGENT GOES OUT OF WOLF INVENTORY
    const string MakeWolfSpitOutGrandmaGoalNm = "save grandma";/////
    {
      TAGoal MakeWolfSpitOutGrandmaGoal;

      //goal agent
      PWoodsmanAgent GoalWoodsman(new TWoodsmanAgent(*Woodsman));
      const string& GoalWoodsmanNm = GoalWoodsman->GetAgentNmStr();

      //goal agent
      PGrandmaAgent GoalGrandma(new TGrandmaAgent(*dynamic_pointer_cast<TGrandmaAgent>(GetFirstAgentByType("TGrandmaAgent"))));
      const string& GoalGrandmaNm = GoalGrandma->GetAgentNmStr();
      ////goal attributes
      MakeWolfSpitOutGrandmaGoal.AddGoalAttrKey(GoalGrandmaNm, GoalGrandma->IsConsKey);
      ////goal attribute values
      GoalGrandma->SetIsCons(true);

      //add all goal agents
      MakeWolfSpitOutGrandmaGoal.AddAgent(GoalGrandma);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert("respond to cry for help");

      //add goal to agent
      AddGoal(Woodsman->GetAgentNmStr(), MakeWolfSpitOutGrandmaGoalNm, MakeWolfSpitOutGrandmaGoal, PreGoalS);
    }
    //goal 12
    // WOLF SAME LOCATION AS WOODSMAN 
    //WOLF GOES INTO WOODSMAN INVENTORY
    //WOODSMAN GOES TO FIXED LOCATION
    //WHEN ARRIVED AT LOCATION WOODSMAN TAKES WOLF OUT OF INVENTORY
    const string CarryWolfInForestGoalNm = "get rid of wolf";
    {
      TAGoal CarryWolfInForestGoal;
      //goal agent
      PWoodsmanAgent GoalWoodsman(new TWoodsmanAgent(*Woodsman));
      const string& GoalWoodsmanNm = GoalWoodsman->GetAgentNmStr();
      ////goal attributes
      CarryWolfInForestGoal.AddGoalAttrKey(GoalWoodsmanNm, GoalWoodsman->XKey);
      CarryWolfInForestGoal.AddGoalAttrKey(GoalWoodsmanNm, GoalWoodsman->YKey);
      CarryWolfInForestGoal.AddGoalAttrKey(GoalWoodsmanNm, GoalWoodsman->InInventorySKey);
      ////goal attribute values
      GoalWoodsman->SetXY(3, 4);
      PTangibleAgent PNearestWolfTangibleAgent = GetNearestAgentByType(GoalWoodsmanNm, "TWolfAgent");
      GoalWoodsman->AddInInventory(PNearestWolfTangibleAgent->GetAgentNmStr());

      //add all goal agents
      CarryWolfInForestGoal.AddAgent(GoalWoodsman);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(MakeWolfSpitOutGrandmaGoalNm);

      //add goal to agent
      AddGoal(Woodsman->GetAgentNmStr(), CarryWolfInForestGoalNm, CarryWolfInForestGoal, PreGoalS);
    }
    //goal 13
    const string GiftGrandmaGoalNm = "give grandma gifts";
    {
      TAGoal GiftGrandmaGoal;
      //goal agent
      PGrandmaAgent GoalGrandma(new TGrandmaAgent(*dynamic_pointer_cast<TGrandmaAgent>(GetFirstAgentByType("TGrandmaAgent"))));
      const string& GoalGrandmaNm = GoalGrandma->GetAgentNmStr();
      ////goal attributes
      GiftGrandmaGoal.AddGoalAttrKey(GoalGrandmaNm, GoalGrandma->InInventorySKey);
      ////goal attribute values
      GoalGrandma->AddInInventory("flowers");
      GoalGrandma->AddInInventory("basket");

      //goal agent
      PRidingHoodAgent GoalRidingHood(new TRidingHoodAgent(*RidingHood));
      const string& GoalRidingHoodNm = GoalRidingHood->GetAgentNmStr();
      ////goal attribute values
      GiftGrandmaGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->XKey);
      GiftGrandmaGoal.AddGoalAttrKey(GoalRidingHoodNm, GoalRidingHood->YKey);
      ////goal attributes
      GoalRidingHood->SetXY(GoalGrandma->GetX(), GoalGrandma->GetY());

      //add all goal agents
      GiftGrandmaGoal.AddAgent(GoalGrandma);
      GiftGrandmaGoal.AddAgent(GoalRidingHood);

      //precondition goals
      StrS PreGoalS;
      PreGoalS.insert(CryForHelpGoalNm);

      //add goal to agent
      AddGoal(RidingHood->GetAgentNmStr(), GiftGrandmaGoalNm, GiftGrandmaGoal, PreGoalS);
    }

    //GOAL CREATION TEMPLATE
    const string GoalNm = "";
    {
      TAGoal Goal;
      //goal agent
      ////goal attributes
      ////goal attribute values

      //add all goal agents

      //precondition goals

      //add goal to agent
    }
  }
};
typedef shared_ptr<TRidingHoodWorldMd> PRidingHoodWorldMd;

#endif
