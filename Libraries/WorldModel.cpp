#include "WorldModel.h"

bool TAVal::operator==(const TAVal& AVal) const{
  if (Type != AVal.Type) { return false; }
  switch (Type) {
  case TAType::Undef: { return true; }
  case TAType::Bool: { return Bool == AVal.Bool; }
  case TAType::Int: { return Int == AVal.Int; }
  case TAType::Dbl: { return Dbl == AVal.Dbl; }
  case TAType::Str: { return Str == AVal.Str; }
  case TAType::Vec: { return Vec == AVal.Vec; }
  case TAType::Set: { return Set == AVal.Set; }
  case TAType::Map: { return Map == AVal.Map; }
  default: { assert(false);  return false; }
  }
}

bool TAVal::operator<(const TAVal& AVal) const{
  if (Type != AVal.Type) { return Type < AVal.Type; }
  switch (Type) {
  case TAType::Undef: { return false; }
  case TAType::Bool: { return Bool < AVal.Bool; }
  case TAType::Int: { return Int < AVal.Int; }
  case TAType::Dbl: { return Dbl < AVal.Dbl; }
  case TAType::Str: { return Str < AVal.Str; }
  case TAType::Vec: { return Vec < AVal.Vec; }
  case TAType::Set: { return Set < AVal.Set; }
  case TAType::Map: { return false; }
  default: { assert(false);  return false; }
  }
}

void TAVal::AddSetAVal(const TAVal& AVal){
  IAssert(Type == TAType::Set); Set.insert(AVal);
}

void TAVal::DelSetAVal(const TAVal& AVal){
  IAssert(Type == TAType::Set);
  auto SetI = Set.find(AVal);
  //if value found, erase it (doesn't erase other duplicates)
  if (SetI != Set.end()){
    Set.erase(SetI);
  }
}

string TAVal::GetValStr() const{
  switch (GetType()) {
  case TAType::Undef: return "Undef";
  case TAType::Bool: if (Bool) { return "True"; } else { return "False"; }
  case TAType::Int: return to_string(Int);
  case TAType::Dbl: return to_string(Dbl);
  case TAType::Str: return Str;
  case TAType::Vec: return "[Vec]";
  case TAType::Set: return "[Set]";
  case TAType::Map: return "[Map]";
  default: assert(false); return "";
  }
}

TAVal& TAVal::GetOrSetMapAVal(const string& KeyStr, const TAVal& AVal){
  IAssert(Type == TAType::Map);
  if (IsMapKey(KeyStr)){
    return GetMapAVal(KeyStr);
  } else{
    return SetMapAVal(KeyStr, AVal);
  }
}

vector<pair<string, TAVal>> TAValBs::GetAttrNmValPrV() const{
  vector<pair<string, TAVal>> AttrNmValPrV;
  for (auto& AttrNmToValI : AttrNmToValM) {
    AttrNmValPrV.push_back(
      make_pair(AttrNmToValI.first, AttrNmToValI.second));
  }
  return AttrNmValPrV;
}

void TAgent::SetWorldMd(PWorldMd NewWorldMd){
  WorldMd = NewWorldMd;
}

bool TAgent::IsAgentType(const string& TypeStr){
  return AValBs.GetAttr(ATypeSKey).IsSetAVal(TAVal::GenStr(TypeStr));
}

bool TAgent::IsAgentMainType(const string& TypeStr){
  return AValBs.GetAttr(AMainTypeKey).GetStr() == TypeStr;
}

string TAgent::GetAgentMainTypeStr(){
  return AValBs.GetAttr(AMainTypeKey).GetStr();
}

bool TAgent::IsAllDoableGoalsComplete(){
  return AGoalBs.IsAllDoableGoalsComplete();
}

void TAgent::EvalAction(const NmToPAgentM& SimNmToAgentM, const string& ActionNm, const string& ActionDesc, const string& PrimaryGoalNm, TRunAction& BestRunAction){
  double TotalActionScore = GetTotalActionScore(SimNmToAgentM);
  if (TotalActionScore > BestRunAction.TotalActionScore){
    BestRunAction = TRunAction(ActionNm, ActionDesc, PrimaryGoalNm, TotalActionScore, SimNmToAgentM);
  }
}

double TAgent::GetTotalActionScore(const NmToPAgentM& SimNmToAgentM){
  StrToDblM GoalNmToActionScoreM = AGoalBs.GetEvalGoalNmToActionScoreM(WorldMd->GetNmToAgentMRef(), SimNmToAgentM);
  double TotalActionScore = 0.0;
  for (auto& GoalNmToActionScoreI : GoalNmToActionScoreM){
    const double& ActionScore = GoalNmToActionScoreI.second;
    TotalActionScore += ActionScore;
  }
  //if action score is negative, return highest score for any individual action
  if (TotalActionScore < 0.0){
    for (auto& GoalNmToActionScoreI : GoalNmToActionScoreM){
      const double& ActionScore = GoalNmToActionScoreI.second;
      TotalActionScore = max(TotalActionScore, ActionScore);
    }
  }
  return TotalActionScore;
}

string TAgent::GetActionPrimaryGoalNm(const NmToPAgentM& SimNmToAgentM){
  StrToDblM GoalNmToActionScoreM = AGoalBs.GetEvalGoalNmToActionScoreM(WorldMd->GetNmToAgentMRef(), SimNmToAgentM);
  double MxActionScore = 0;
  string PrimaryGoalNm = "";
  for (auto& GoalNmToActionScoreI : GoalNmToActionScoreM){
    const string& GoalNm = GoalNmToActionScoreI.first;
    const double& ActionScore = GoalNmToActionScoreI.second;
    if (ActionScore > MxActionScore){
      MxActionScore = ActionScore;
      PrimaryGoalNm = GoalNm;
    }
  }
  return PrimaryGoalNm;
}

PAgent TTangibleAgent::ClonePAgent(){
  return PTangibleAgent(new TTangibleAgent(*this));
}

bool TTangibleAgent::CanGiveToAgentInventory(const string& GiveToAgentNm, const string& Item){
  if (!IsAgentSameXY(GiveToAgentNm)){ return false; }
  if (!IsInInventory(Item)){ return false; }
  //can only give to a conscious agent
  PAgent PGiveToAgent = WorldMd->GetAgent(GiveToAgentNm);
  if (TLivingAgent* PGiveToLivingAgent = dynamic_cast<TLivingAgent*>(&*PGiveToAgent)){
    if (!PGiveToLivingAgent->IsCons()){ return false; }
  }
  return true;
}

void TTangibleAgent::EffectGiveToAgentInventory(const string& GiveToAgentNm, const string& Item){
  GiveToAgentInventory(GiveToAgentNm, Item);
}

bool TTangibleAgent::CanTakeFromAgentInventory(const string& TakeFromAgentNm, const string& Item){
  PTangibleAgent TakeFromAgent = dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(TakeFromAgentNm));
  if (!IsAgentSameXY(TakeFromAgentNm)){ return false; }
  if (!TakeFromAgent->IsInInventory(Item)){ return false; };
  return true;
}

void TTangibleAgent::EffectTakeFromAgentInventory(const string& GiveToAgentNm, const string& Item){
  TakeFromAgentInventory(GiveToAgentNm, Item);
}

bool TTangibleAgent::TakeFromAgentInventory(const string& AgentNm, const string& Item){
  PTangibleAgent TakeFromAgent = dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(AgentNm));
  if (!TakeFromAgent->IsInInventory(Item)){ return false; };
  TakeFromAgent->DelInInventory(Item);
  AddInInventory(Item);
  return true;
}

bool TTangibleAgent::GiveToAgentInventory(const string& AgentNm, const string& Item){
  if (!IsInInventory(Item)){ return false; }
  DelInInventory(Item);
  dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(AgentNm))->AddInInventory(Item);
  return true;
}

void TTangibleAgent::AddInInventory(const string& Item){
  GetAttr(InInventorySKey).AddSetAVal(TAVal::GenStr(Item));
}

void TTangibleAgent::DelInInventory(const string& Item){
  GetAttr(InInventorySKey).DelSetAVal(TAVal::GenStr(Item));
}

/*void TTangibleAgent::GoInAgent(const string& AgentNm){
  IAssert(WorldMd->IsAgent(AgentNm));
  GetAttr(InAgentStrKey).SetStr(AgentNm);
}*/

void TTangibleAgent::RunGiveToAgentInventory(const string& GiveToAgentNm, const string& Item){
  if (CanGiveToAgentInventory(GiveToAgentNm, Item)){ EffectGiveToAgentInventory(GiveToAgentNm, Item); }
}

void TTangibleAgent::RunTakeFromAgentInventory(const string& TakeFromAgentNm, const string& Item){
  if (CanTakeFromAgentInventory(TakeFromAgentNm, Item)){ EffectTakeFromAgentInventory(TakeFromAgentNm, Item); }
}

NmToPAgentM TTangibleAgent::SimAction(const string& Str1, const string& Str2, void(TTangibleAgent::* AgentFunc)(const string&, const string&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Str1, Str2);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

bool TAgent::IsStopAllActions(){
  if (TLivingAgent* PLivingAgent = dynamic_cast<THumanAgent*>(&*this)){
    if ((IsAgentType("TLivingAgent")) && (!PLivingAgent->IsCons())){ return true; }
  }
  return false;
}

TRunAction TAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return TRunAction();
}

bool TAgent::Run(){
  TRunAction NextAction = GetBestAction();
  if (NextAction.ActionNm.empty()){ return false; }
  WorldMd->SetAgentsValsGoalsBs(NextAction.NmToAgentM);
  //cout << "AGENT: " << GetAgentNmStr() << endl;
  //cout << "ACTION: " << NextAction.ActionNm << endl;
  cout << NextAction.ActionDesc << endl;
  AGoalBs.UpdateCompleteGoals(WorldMd->GetNmToAgentMRef());
  return true;
}

bool TTangibleAgent::IsAgentSameXY(const string& CompAgentNm){
  IAssert(WorldMd->IsAgent(CompAgentNm));
  PTangibleAgent CompTangibleAgent = dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(CompAgentNm));
  const int& CompAgentX = CompTangibleAgent->GetX();
  const int& CompAgentY = CompTangibleAgent->GetY();
  return ((GetX() == CompAgentX) && (GetY() == CompAgentY));
}

bool TTangibleAgent::IsAgentXY(const int& X, const int& Y) const{
  return (GetX() == X) && (GetY() == Y);
}

TRunAction TTangibleAgent::GetBestAction(){
  TRunAction BestRunAction = TAgent::GetBestAction();
  {
    //RunGiveToAgentInventory evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      string GiveToAgentNm = NmToAgentI.first;
      //only continue if the agent inherits from THumanAgent
      if (TTangibleAgent* PGiveToAgent = dynamic_cast<TTangibleAgent*>(&*NmToAgentI.second)){
        const multiset<TAVal>& InInventoryS = GetAttr(InInventorySKey).GetSet();
        for (auto& InInventoryI : InInventoryS){
          string InInventoryItem = InInventoryI.GetStr();
          NmToPAgentM SimNmToAgentM = SimAction(GiveToAgentNm, InInventoryItem, &THumanAgent::RunGiveToAgentInventory);
          const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
          const string ActionNm = "RunGiveToAgentInventory";
          const string ActionPerf = GetAgentNmStr() + " put " + InInventoryItem + " into " + GiveToAgentNm + "'s inventory";
          const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
          EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
        }
      }
    }
  }
  {
    //RunTakeFromAgentInventory evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      string TakeFromAgentNm = NmToAgentI.first;
      //only continue if the agent inherits from THumanAgent
      if (TTangibleAgent* PTakeFromAgent = dynamic_cast<TTangibleAgent*>(&*NmToAgentI.second)){
        const multiset<TAVal>& InInventoryS = PTakeFromAgent->GetAttr(InInventorySKey).GetSet();
        for (auto& InInventoryI : InInventoryS){
          string InInventoryItem = InInventoryI.GetStr();
          NmToPAgentM SimNmToAgentM = SimAction(TakeFromAgentNm, InInventoryItem, &THumanAgent::RunTakeFromAgentInventory);
          const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
          const string ActionNm = "RunTakeFromAgentInventory";
          const string ActionPerf = GetAgentNmStr() + " took " + InInventoryItem + " from " + TakeFromAgentNm + "'s inventory";
          const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm ;
          EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
        }
      }
    }
  }
  return BestRunAction;
}

void TLivingAgent::MoveAgentsInInventoryToNewPos(){
  const multiset<TAVal>& InInventoryS = GetAValBs().GetAttr(InInventorySKey).GetSet();
  for (auto& InInventoryI : InInventoryS){
    const string& Item = InInventoryI.GetStr();
    if (!WorldMd->IsAgent(Item)){ continue; }
    PAgent InventoryAgent = WorldMd->GetAgent(Item);
    if (TTangibleAgent* PTangibleInventoryAgent = dynamic_cast<TTangibleAgent*>(&*InventoryAgent)){
      PTangibleInventoryAgent->SetXY(GetX(), GetY());
    }
  }
}

void TLivingAgent::Wander(const int& Moves, int SetMoveDir){
  const string& AgentNm = AValBs.GetAttr("Name").GetStr();
  int MoveN = 0;
  while (MoveN < Moves){
    int CurX = GetX();
    int CurY = GetY();
    int MoveDir;
    if (SetMoveDir == -1){
      MoveDir = GenRandIntUniformDistr(1, 4);
    } else{ MoveDir = SetMoveDir; }
    switch (MoveDir) {
    case 1://up
      IncY(); MoveAgentsInInventoryToNewPos();
      MoveN++;
      break;
    case 2://down
      if (CurY > 0) {
        DecY(); MoveAgentsInInventoryToNewPos();
        MoveN++;
      } else if (SetMoveDir != -1){ return; }//if always move in this direction and y is already 0, can't move anymore
      break;
    case 3://left
      if (CurX > 0) {
        DecX(); MoveAgentsInInventoryToNewPos();
        MoveN++;
      } else if (SetMoveDir != -1){ return; }//if always move in this direction and x is already 0, can't move anymore
      break;
    case 4://right
      IncX(); MoveAgentsInInventoryToNewPos();
      MoveN++;
      break;
    }
  }
}

void TLivingAgent::RunMove(const int& MoveDir){
  if (MoveDir == -1){
    TLivingAgent::Wander(1);
  } else{
    TLivingAgent::Wander(1, MoveDir);
  }
}

void TLivingAgent::RunGetAgentAwareOf(const string& AgentNm, const string& GiveAwareOf){
  if (CanGetAgentAwareOf(AgentNm, GiveAwareOf)){ EffectGetAgentAwareOf(AgentNm, GiveAwareOf); }
}

void TLivingAgent::RunEatAgent(const string& AgentNm){
  if (CanEatAgent(AgentNm)){ EffectEatAgent(AgentNm); }
}

NmToPAgentM TLivingAgent::SimAction(void (TLivingAgent::* AgentFunc)()){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)();
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

NmToPAgentM TLivingAgent::SimAction(const int& Int1, void(TLivingAgent::* AgentFunc)(const int&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Int1);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

NmToPAgentM TLivingAgent::SimAction(const string& Str1, void(TLivingAgent::* AgentFunc)(const string&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Str1);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

NmToPAgentM TLivingAgent::SimAction(const string& Str1, const string& Str2, void(TLivingAgent::* AgentFunc)(const string&, const string&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Str1, Str2);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

TRunAction TLivingAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TTangibleAgent::GetBestAction();
  //RunMove evaluation
  {
    for (int MoveDir = 1; MoveDir <= 4; MoveDir++){
      NmToPAgentM SimNmToAgentM = SimAction(MoveDir, &TLivingAgent::RunMove);
      const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
      const string ActionNm = "RunMove";
      string ActionPerf = GetAgentNmStr() + " moved 1 unit ";
      switch (MoveDir){
      case(1): {ActionPerf += "up"; break; }
      case(2): {ActionPerf += "down"; break; }
      case(3): {ActionPerf += "left"; break; }
      case(4): {ActionPerf += "right"; break; }
      }
      const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
      EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
      //const string ActionNm = "RunMove MoveDir: " + to_string(MoveDir);
    }
  }
  //RunPickFlowers evaluation
  {
    NmToPAgentM SimNmToAgentM = SimAction(&TLivingAgent::RunPickFlowers);
    const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
    const string ActionNm = "RunPickFlowers";
    const string ActionPerf = GetAgentNmStr() + " picked some flowers";
    const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
    EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
  }
  //RunGetAgentAwareOf evaluation
  {
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      //only continue if the agent inherits from TLivingAgent
      if (TLivingAgent* PTangibleAgent = dynamic_cast<TLivingAgent*>(&*NmToAgentI.second)){
        const multiset<TAVal>& AwareOfS = PTangibleAgent->AValBs.GetAttr(AwareOfSKey).GetSet();
        for (auto& AwareOfI : AwareOfS){
          NmToPAgentM SimNmToAgentM = SimAction(NmToAgentI.first, AwareOfI.GetStr(), &TLivingAgent::RunGetAgentAwareOf);
          const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
          const string ActionNm = "RunGetAgentAwareOf";
          const string ActionPerf = GetAgentNmStr() + " became aware of " + AwareOfI.GetStr() + " by asking " + NmToAgentI.first;
          const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
          EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
        }
      }
    }
  }
  //RunEatAgent evaluation
  {
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      NmToPAgentM SimNmToAgentM = SimAction(NmToAgentI.first, &TLivingAgent::RunEatAgent);
      const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
      const string ActionNm = "RunEatAgent";
      const string ActionPerf = GetAgentNmStr() + " ate " + NmToAgentI.first;
      const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
      EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
    }
  }
  return BestRunAction;
}

void THumanAgent::RunAskApproval(const string& ApproveGoal, const string& RespondAgentNm){
  if (CanAskApproval(ApproveGoal, RespondAgentNm)){ EffectAskApproval(ApproveGoal, RespondAgentNm); }
}

void THumanAgent::RunLookAtAgent(const string& AgentNm){
  if (CanLookAtAgent(AgentNm)){ EffectLookAtAgent(AgentNm); }
}

void THumanAgent::RunCryForHelp(){
  if (CanCryForHelp()){ EffectCryForHelp(4.0); }
}

void THumanAgent::RunMakeAgentSpitOutAgent(const string& SpitAgentNm, const string& SpitOutAgentNm){
  if (CanMakeAgentSpitOutAgent(SpitAgentNm, SpitOutAgentNm)){
    EffectMakeAgentSpitOutAgent(SpitAgentNm, SpitOutAgentNm);
  }
}

void THumanAgent::RunCarryAgent(const string& CarryAgentNm){
  if (CanCarryAgent(CarryAgentNm)){ EffectCarryAgent(CarryAgentNm); }
}

NmToPAgentM THumanAgent::SimAction(const string& Str1, const string& Str2, void (THumanAgent::* AgentFunc)(const string&, const string&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Str1, Str2);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

NmToPAgentM THumanAgent::SimAction(const string& Str1, void(THumanAgent::* AgentFunc)(const string&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Str1);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

NmToPAgentM THumanAgent::SimAction(void(THumanAgent::* AgentFunc)()){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)();
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

PAgent TRidingHoodAgent::ClonePAgent(){
  return PRidingHoodAgent(new TRidingHoodAgent(*this));
}

TRunAction TRidingHoodAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return THumanAgent::GetBestAction();
}

void TWorldMd::Run(){
  Draw();
  Time++;
  while (!IsAllDoableGoalsComplete()) {
    //for (auto& NmToAgentI : NmToAgentM) {
    auto NmToAgentI = NmToAgentM.begin();
    advance(NmToAgentI, GenRandIntUniformDistr(0, int(NmToAgentM.size()) - 1));
    //if ((Time==8)&&(NmToAgentI->first=="rh")){
      //continue;
      //cout << "ADRIAN DEBUG TEST WORKED" << endl;
    //}
    if (NmToAgentI->second->IsAllDoableGoalsComplete()){ continue; }//skip agents with no goals left
    bool IsRun = NmToAgentI->second->Run();
    if (!IsRun){ continue; }
    Time++;
    //cout << "Time " << to_string(Time) << ": " << endl;
    Draw();
  }
}

void TWorldMd::AddGoal(const string& AgentNm, const string& GoalNm, const TAGoal& Goal, const StrS& PreGoalS){
  GetAgent(AgentNm)->GetAGoalBs().AddGoal(GoalNm, Goal, PreGoalS);
}

NmToPAgentM TWorldMd::GetNmToAgentMCopy() const{
  NmToPAgentM CopyNmToPAgentM;
  for (auto& NmToPAgentI : NmToAgentM){
    CopyNmToPAgentM[NmToPAgentI.first] = NmToPAgentI.second->ClonePAgent();
  }
  return CopyNmToPAgentM;
}

bool TWorldMd::IsAgentType(const string& SearchAgentType) const{
  for (auto& NmToAgentI : NmToAgentM){
    if (NmToAgentI.second->IsAgentType(SearchAgentType)){ return true; }
  }
  return false;
}

void TWorldMd::AddAgent(const PAgent Agent){
  IAssert(Agent->IsAgentNm());
  const string& AgentNm = Agent->GetAgentNmStr();
  NmToAgentM[AgentNm] = Agent;
}

void TWorldMd::DelAgent(const string& AgentNm){
  IAssert(IsAgent(AgentNm));
  NmToAgentM.erase(AgentNm);
}

PAgent TWorldMd::GetAgent(const string& AgentNm){
  IAssert(IsAgent(AgentNm));
  return NmToAgentM[AgentNm];
}

PTangibleAgent TWorldMd::GetNearestAgentByType(const string& SrcAgentNm, const string& SearchAgentType){
  PTangibleAgent SrcAgent = dynamic_pointer_cast<TTangibleAgent>(GetAgent(SrcAgentNm));
  const double& SrcX = SrcAgent->GetX();
  const double& SrcY = SrcAgent->GetY();
  double MnDist = 9999999;
  PTangibleAgent MnDistAgent;
  for (auto& NmToAgentI : NmToAgentM){
    PTangibleAgent DstAgent = dynamic_pointer_cast<TTangibleAgent>(NmToAgentI.second);
    if (DstAgent->IsAgentType(SearchAgentType)){
      const double& DstX = DstAgent->GetX();
      const double& DstY = DstAgent->GetY();
      double Dist = GetXYCoordDist(SrcX, SrcY, DstX, DstY);
      if (Dist < MnDist){
        MnDistAgent = DstAgent;
        MnDist = Dist;
      }
    }
  }
  if (MnDist == 9999999){
    return NULL;
  } else{
    return MnDistAgent;
  }
}

pair<double, double> TWorldMd::GetNearestAgentXYPrByType(const string& SrcAgentNm, const string& SearchAgentType){
  PTangibleAgent SrcAgent = dynamic_pointer_cast<TTangibleAgent>(GetAgent(SrcAgentNm));
  const double& SrcX = SrcAgent->GetX();
  const double& SrcY = SrcAgent->GetY();
  double NearestX = 9999999;
  double NearestY = 9999999;
  double MnDist = 9999999;
  for (auto& NmToAgentI : NmToAgentM){
    PTangibleAgent DstAgent = dynamic_pointer_cast<TTangibleAgent>(NmToAgentI.second);
    if (DstAgent->IsAgentType(SearchAgentType)){
      const double& DstX = DstAgent->GetX();
      const double& DstY = DstAgent->GetY();
      double Dist = GetXYCoordDist(SrcX, SrcY, DstX, DstY);
      if (Dist < MnDist){
        NearestX = DstX;
        NearestY = DstY;
        MnDist = Dist;
      }
    }
  }
  if (MnDist == 9999999){
    return make_pair(NULL, NULL);
  } else{
    return make_pair(NearestX, NearestY);
  }
}

double TWorldMd::GetNearestAgentDistByType(const string& SrcAgentNm, const string& SearchAgentType){
  PTangibleAgent SrcAgent = dynamic_pointer_cast<TTangibleAgent>(GetAgent(SrcAgentNm));
  const double& SrcX = SrcAgent->GetX();
  const double& SrcY = SrcAgent->GetY();
  double MnDist = 9999999;
  for (auto& NmToAgentI : NmToAgentM){
    PTangibleAgent DstAgent = dynamic_pointer_cast<TTangibleAgent>(NmToAgentI.second);
    if (DstAgent->IsAgentType(SearchAgentType)){
      const double& DstX = DstAgent->GetX();
      const double& DstY = DstAgent->GetY();
      double Dist = GetXYCoordDist(SrcX, SrcY, DstX, DstY);
      if (Dist < MnDist){
        MnDist = Dist;
      }
    }
  }
  if (MnDist == 9999999){
    return NULL;
  } else{
    return MnDist;
  }
}

PAgent TWorldMd::GetFirstAgentByType(const string& SearchAgentType){
  for (auto& NmToAgentI : NmToAgentM){
    if (NmToAgentI.second->IsAgentType(SearchAgentType)){ return NmToAgentI.second; }
  }
  return NULL;
}

void TWorldMd::Draw()const{
  int MxX = 0;
  int MxY = 0;
  //put all agents on map
  StrVV AgentStrMapVV(99, StrV(99));//init 2D vector with empty strings
  for (auto& NmToAgentI : NmToAgentM) {
    const string& AgentNm = NmToAgentI.first;
    PAgent Agent = NmToAgentI.second;
    //check agent has XY coordinate attribtues
    if ((Agent->IsAttr("X")) && (Agent->IsAttr("Y"))){
      PTangibleAgent TangibleAgent = dynamic_pointer_cast<TTangibleAgent>(Agent);
      int AgentX = TangibleAgent->GetX();
      int AgentY = TangibleAgent->GetY();
      if (AgentX > MxX){ MxX = AgentX; }
      if (AgentY > MxY){ MxY = AgentY; }
      string& MapXYStr = AgentStrMapVV[AgentX][AgentY];
      if (!MapXYStr.empty()){ MapXYStr += ","; }
      MapXYStr += AgentNm;
    }
  }
  //draw map
  for (int Y = MxY; Y >=0; Y--){
    for (int X = 0; X <= MxX; X++){
      cout << setw(32) << setfill('*') << AgentStrMapVV[X][Y] << '|';
    }
    cout << endl;
  }
  cout << endl;
  return;
}

bool TWorldMd::IsAllDoableGoalsComplete(){
  for (auto& NmToAgentI : NmToAgentM){
    PAgent Agent = NmToAgentI.second;
    if (!Agent->IsAllDoableGoalsComplete()){ return false; }
  }
  return true;
}

PAgent TLivingAgent::ClonePAgent(){
  return PLivingAgent(new TLivingAgent(*this));
}

bool TLivingAgent::CanEquip(const string& Item)const{
  if (!IsCons()){ return false; }
  if (!IsInInventory(Item)){ return false; }
  return true;
}

void TLivingAgent::EffectEquip(const string& Item){
  DelInInventory(Item);
  GetAttr(EquipSKey).AddSetAVal(TAVal::GenStr(Item));
}

bool TLivingAgent::CanUnequip(const string& Item) const{
  if (!IsCons()){ return false; }
  return IsEquipped(Item);
}

void TLivingAgent::EffectUnequip(const string& Item){
  GetAttr(EquipSKey).DelSetAVal(TAVal::GenStr(Item));
  AddInInventory(Item);
}

bool TLivingAgent::CanPickFlowers(){
  if (!IsCons()){ return false; }
  if (WorldMd->GetNearestAgentDistByType(GetAgentNmStr(), "TFlowerFieldAgent") == 0){
    return true;
  } else{
    return false;
  }
}

void TLivingAgent::EffectPickFlowers(){
  AddInInventory("flowers");
}

bool TLivingAgent::IsAwareOf(const string& AwareOf){
  return AValBs.GetAttr(AwareOfSKey).IsSetAVal(TAVal::GenStr(AwareOf));
}

void TLivingAgent::AddAwareOf(const string& AddAwareOf){
  GetAttr(AwareOfSKey).AddSetAVal(TAVal::GenStr(AddAwareOf));
}

void TLivingAgent::DelAwareOf(const string& DelAwareOf){
  GetAttr(AwareOfSKey).DelSetAVal(TAVal::GenStr(DelAwareOf));
}

void TLivingAgent::AddGoalAgentRelation(const string& GoalNm, const string& AgentNm, const string& RelationStr){
  TAVal& GoalNmToAgentNmToRelationStrMM = GetAttr(GoalNmToAgentNmToRelationStrMMKey);
  TAVal& AgentNmToRelationStrMM = GoalNmToAgentNmToRelationStrMM.GetOrSetMapAVal(GoalNm, TAVal(TAType::Map));
  AgentNmToRelationStrMM.SetMapAVal(AgentNm, TAVal::GenStr(RelationStr));
  //TAVal& AgentNmToRelationStrMM
  /*
  //is goal already in map
  if (GoalNmToAgentNmToRelationStrMM.IsMapKey(GoalNm)){
    TAVal& AgentNmToRelationStrM = GoalNmToAgentNmToRelationStrMM.GetMapAVal(GoalNm);
    //is agent already in map
    if (AgentNmToRelationStrM.IsMapKey(AgentNm)){
      AgentNmToRelationStrM.GetMapAVal(AgentNm)
    } else{}
    TAVal AgentNmToRelationStrM(TAType::Map);


    //GoalNmToAgentNmToRelationStrMM.SetMapAVal()
  } else{
    //GoalNmToAgentNmToRelationStrMM.SetMapAVal()
  }*/
}

void TLivingAgent::DelGoalAgentRelation(const string& GoalNm, const string& AgentNm){
  TAVal& GoalNmToAgentNmToRelationStrMM = GetAttr(GoalNmToAgentNmToRelationStrMMKey);
  TAVal& AgentNmToRelationStrMM = GoalNmToAgentNmToRelationStrMM.GetOrSetMapAVal(GoalNm, TAVal(TAType::Map));
  AgentNmToRelationStrMM.DelMapAVal(AgentNm);
}

void TLivingAgent::SetEmotionalScore(const double AddDbl){
  AValBs.GetAttr(EmotionalScoreKey).SetDbl(AddDbl);
}

void TLivingAgent::AddToEmotionalScore(const double AddDbl){
  const double& CurEmotionalScore = AValBs.GetAttr(EmotionalScoreKey).GetDbl();
  AValBs.GetAttr(EmotionalScoreKey).SetDbl(CurEmotionalScore + AddDbl);
}

bool TLivingAgent::CanGetAgentAwareOf(const string& AgentNm, const string& GetAwareOf){
  if (!IsCons()){ return false; }
  PLivingAgent GetFromLivingAgent = dynamic_pointer_cast<TLivingAgent>(WorldMd->GetAgent(AgentNm));
  if (!IsAgentSameXY(AgentNm)){ return false; }
  if (!GetFromLivingAgent->IsAwareOf(GetAwareOf)){ return false; }
  return true;
}

void TLivingAgent::EffectGetAgentAwareOf(const string& AgentNm, const string& GetAwareOf){
  AddAwareOf(GetAwareOf);
}

bool TLivingAgent::IsCons()const{
  return AValBs.GetAttrConst(IsConsKey).GetBool();
}

void TLivingAgent::SetIsCons(const bool& IsCons){
  AValBs.GetAttr(IsConsKey).SetBool(IsCons);
}

bool TLivingAgent::CanEatAgent(const string& EatAgentNm){
  if (!IsCons()){ return false; }
  if (!IsAgentSameXY(EatAgentNm)){ return false; }
  if (IsInInventory(EatAgentNm)){ return false; }
  return true;
}

void TLivingAgent::EffectEatAgent(const string& EatAgentNm){
  if ((IsAgentType("TWolfAgent")) && (WorldMd->GetAgent(EatAgentNm)->IsAgentType("TGrandmaAgent"))){
    AddInInventory(EatAgentNm);
    PLivingAgent PEatLivingAgent = dynamic_pointer_cast<TLivingAgent>(WorldMd->GetAgent(EatAgentNm));
    PEatLivingAgent->SetIsCons(false);
  }
}

PAgent THumanAgent::ClonePAgent(){
  return PHumanAgent(new THumanAgent(*this));
}

void THumanAgent::LivesInAgent(const string& AgentNm){
  IAssert(WorldMd->IsAgent(AgentNm));
  GetAttr(LivesInAgentStrKey).SetStr(AgentNm);
}

bool THumanAgent::CanAskApproval(const string& ApproveGoalNm, const string& RespondAgentNm){
  if (!IsCons()){ return false; }
  if (!IsAgentSameXY(RespondAgentNm)){ return false; }
  return true;
}

void THumanAgent::EffectAskApproval(const string& ApproveGoalNm, const string& RespondAgentNm){
  PAgent RespondPAgent = WorldMd->GetAgent(RespondAgentNm);
  PHumanAgent RespondPHumanAgent = dynamic_pointer_cast<THumanAgent>(RespondPAgent);
  RespondPHumanAgent->RespondToApproval(ApproveGoalNm, GetAgentNmStr());
}

bool THumanAgent::CanRespondToAskApproval(const string& ApproveGoalNm, const string& RespondAgentNm){
  if (!IsCons()){ return false; }
  if (!IsAgentSameXY(RespondAgentNm)){ return false; }
  return true;
}

void THumanAgent::EffectRespondToAskApproval(const string& ApproveGoalNm, const string& AskAgentNm){
  //const string AgentTypeStr = GetAgentTypeStr();
  //const string AskAgentTypeStr = WorldMd->GetAgent(AskAgentNm)->GetAgentTypeStr();
  if ((IsAgentType("TMotherAgent")) && (WorldMd->GetAgent(AskAgentNm)->IsAgentType("TRidingHoodAgent"))){
    if ((ApproveGoalNm == "visit grandma") && (WorldMd->IsAgentType("TGrandmaAgent"))){
      GiveToAgentInventory(AskAgentNm, "basket");
      PRidingHoodAgent AskRidingHoodAgent = dynamic_pointer_cast<TRidingHoodAgent>(WorldMd->GetAgent(AskAgentNm));
      AskRidingHoodAgent->AddGoalAgentRelation(ApproveGoalNm, GetAgentNmStr(), "approves");
    }
  } else{
    AddGoalAgentRelation(ApproveGoalNm, AskAgentNm, "approves");
  }
}

void THumanAgent::RespondToApproval(const string& ApproveGoal, const string& AskAgentNm){
  if (CanRespondToAskApproval(ApproveGoal, AskAgentNm)){ EffectRespondToAskApproval(ApproveGoal, AskAgentNm); }
}

bool THumanAgent::CanLookAtAgent(const string& AgentNm){
  if (!IsCons()){ return false; }
  //same location
  if (!IsAgentSameXY(AgentNm)){ return false; }
  return true;
}

void THumanAgent::EffectLookAtAgent(const string& AgentNm){
  PAgent PLookAtAgent = WorldMd->GetAgent(AgentNm);
  //if agent is an animal
  if (TAnimalAgent* PLookAtAnimalAgent = dynamic_cast<TAnimalAgent*>(&*PLookAtAgent)){
    AddToEmotionalScore(1.0);
  }
}

bool THumanAgent::CanCryForHelp(){
  if (!IsCons()){ return false; }
  return true;
}

void THumanAgent::EffectCryForHelp(const double& MxCryDist){
  const double& X = double(GetX());
  const double& Y = double(GetY());
  for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
    if (THumanAgent* PHumanAgent = dynamic_cast<THumanAgent*>(&*NmToAgentI.second)){
      const double& CurX = PHumanAgent->GetX();
      const double& CurY = PHumanAgent->GetY();
      //agent close enough to respond to cry
      if (MxCryDist >= GetXYCoordDist(X, Y, CurX, CurY)){
        PHumanAgent->RespondToCryForHelp(int(X), int(Y));
      }
    }
  }
}

bool THumanAgent::CanRespondToCryForHelp(){
  if (!IsCons()){ return false; }
  return true;
}

void THumanAgent::EffectRespondToCryForHelp(const int& X, const int& Y){
  AddAwareOf("cry for help");
  const string GoToCryHelpGoalNm = "respond to cry for help";
  {
    TAGoal GoToCryHelpGoal;
    //goal agent
    PHumanAgent ThisGoalAgent(new THumanAgent(*this));
    const string& ThisGoalAgentNm = ThisGoalAgent->GetAgentNmStr();
    ////goal attributes
    GoToCryHelpGoal.AddGoalAttrKey(ThisGoalAgentNm, ThisGoalAgent->XKey);
    GoToCryHelpGoal.AddGoalAttrKey(ThisGoalAgentNm, ThisGoalAgent->YKey);
    ////goal attribute values
    ThisGoalAgent->SetXY(int(X), int(Y));

    //add all goal agents
    GoToCryHelpGoal.AddAgent(ThisGoalAgent);

    //precondition goals
    StrS PreGoalS;

    //add goal to agent
    WorldMd->AddGoal(ThisGoalAgent->GetAgentNmStr(), GoToCryHelpGoalNm, GoToCryHelpGoal, PreGoalS);
  }
}

void THumanAgent::RespondToCryForHelp(const int& X, const int& Y){
  if (CanRespondToCryForHelp()){ EffectRespondToCryForHelp(X, Y); }
}

bool THumanAgent::CanMakeAgentSpitOutAgent(const string& SpitAgentNm, const string& SpitOutAgentNm){
  if (!IsCons()){ return false; }
  if (!IsAgentSameXY(SpitAgentNm)){ return false; }
  PAgent PSpitAgent = WorldMd->GetAgent(SpitAgentNm);
  PAgent PSpitOutAgent = WorldMd->GetAgent(SpitOutAgentNm);
  if (TTangibleAgent* PSpitTangibleAgent = dynamic_cast<TTangibleAgent*>(&*PSpitAgent)){
    if (TTangibleAgent* PSpitOutTangibleAgent = dynamic_cast<TTangibleAgent*>(&*PSpitOutAgent)){
      if (!PSpitTangibleAgent->IsInInventory(SpitOutAgentNm)){ return false; }
    } else { return false; }//must be tangible agent
  } else { return false; }//must be tangible agent
  return true;
}

void THumanAgent::EffectMakeAgentSpitOutAgent(const string& SpitAgentNm, const string& SpitOutAgentNm){
  PTangibleAgent PSpitTangibleAgent = dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(SpitAgentNm));
  PTangibleAgent PSpitOutTangibleAgent = dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(SpitOutAgentNm));
  if ((IsAgentType("TWoodsmanAgent")) && (PSpitTangibleAgent->IsAgentType("TWolfAgent"))){
    if (THumanAgent* PSpitOutHumanAgent = dynamic_cast<THumanAgent*>(&*PSpitOutTangibleAgent)){
      PSpitTangibleAgent->DelInInventory(SpitOutAgentNm);
      PSpitOutHumanAgent->SetIsCons(true);
    }
  }
}

bool THumanAgent::CanCarryAgent(const string& CarryAgentNm){
  if (!IsCons()){ return false; }
  if (!IsAgentSameXY(CarryAgentNm)){ return false; }
  PAgent PCarryAgent = WorldMd->GetAgent(CarryAgentNm);
  if (TTangibleAgent* PCarryTangibleAgent = dynamic_cast<TTangibleAgent*>(&*PCarryAgent)){
    if (IsInInventory(CarryAgentNm)){ return false; }
  } else { return false; }//must be tangible agent
  return true;
}

void THumanAgent::EffectCarryAgent(const string& CarryAgentNm){
  PTangibleAgent PCarryTangibleAgent = dynamic_pointer_cast<TTangibleAgent>(WorldMd->GetAgent(CarryAgentNm));
  if ((IsAgentType("TWoodsmanAgent")) && (PCarryTangibleAgent->IsAgentType("TWolfAgent"))){
    AddInInventory(CarryAgentNm);
  }
}

TRunAction THumanAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TLivingAgent::GetBestAction();
  {
    //RunAskApproval evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      //only continue if the agent inherits from THumanAgent
      if (THumanAgent* PHumanAgent = dynamic_cast<THumanAgent*>(&*NmToAgentI.second)){
        unordered_map<string, TAGoal> NmToGoalM = GetAGoalBs().GetNmToGoalMRef();
        for (auto NmToGoalI = NmToGoalM.cbegin(); NmToGoalI != NmToGoalM.cend();){
          NmToPAgentM SimNmToAgentM = SimAction(NmToGoalI->first, NmToAgentI.first, &THumanAgent::RunAskApproval);
          const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
          const string ActionNm = "RunAskApproval";
          const string ActionPerf = GetAgentNmStr() + " asked " + NmToAgentI.first + " for approval to " + NmToGoalI->first;
          const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
          EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
          ++NmToGoalI;
        }
      }
    }
  }
  {
    //RunLookAtAgent evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      //only continue if the agent inherits from TTangibleAgent
      if (TTangibleAgent* PTangibleAgent = dynamic_cast<TTangibleAgent*>(&*NmToAgentI.second)){
        NmToPAgentM SimNmToAgentM = SimAction(NmToAgentI.first, &THumanAgent::RunLookAtAgent);
        const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
        const string ActionNm = "RunLookAtAgent";
        const string ActionPerf = GetAgentNmStr() + " looked at " + NmToAgentI.first;
        const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
        EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
      }
    }
  }
  {
    //RunCryForHelp evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      //only continue if the agent inherits from THumanAgent
      if (THumanAgent* PHumanAgent = dynamic_cast<THumanAgent*>(&*NmToAgentI.second)){
        NmToPAgentM SimNmToAgentM = SimAction(&THumanAgent::RunCryForHelp);
        const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
        const string ActionNm = "RunCryForHelp";
        const string ActionPerf = GetAgentNmStr() + " cried for help";
        const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
        EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
      }
    }
  }
  {
    //RunMakeAgentSpitOutAgent evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      //only continue if the agent inherits from TTangibleAgent
      if (TTangibleAgent* PSpitTangibleAgent = dynamic_cast<TTangibleAgent*>(&*NmToAgentI.second)){
        const string& SpitTangibleAgentNm = PSpitTangibleAgent->GetAgentNmStr();
        const multiset<TAVal>& SpitAgentInInventoryS = PSpitTangibleAgent->GetAValBs().GetAttr(InInventorySKey).GetSet();
        for (auto& SpitAgentInInventoryI : SpitAgentInInventoryS){
          const string SpitOutAgentNm = SpitAgentInInventoryI.GetStr();
          if (!WorldMd->IsAgent(SpitOutAgentNm)){ continue; }
          NmToPAgentM SimNmToAgentM = SimAction(SpitTangibleAgentNm, SpitOutAgentNm, &THumanAgent::RunMakeAgentSpitOutAgent);
          const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
          const string ActionNm = "RunMakeAgentSpitOutAgent";
          const string ActionPerf = GetAgentNmStr() + " made " + SpitTangibleAgentNm + " spit out " + SpitOutAgentNm;
          const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
          EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
        }
      }
    }
  }
  {
    //RunCarryAgent evaluation
    for (auto& NmToAgentI : WorldMd->GetNmToAgentMRef()){
      //only continue if the agent inherits from TTangibleAgent
      if (TTangibleAgent* PCarryTangibleAgent = dynamic_cast<TTangibleAgent*>(&*NmToAgentI.second)){
        const string& CarryTangibleAgentNm = PCarryTangibleAgent->GetAgentNmStr();
        NmToPAgentM SimNmToAgentM = SimAction(CarryTangibleAgentNm, &THumanAgent::RunCarryAgent);
        const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
        const string ActionNm = "RunCarryAgent";
        const string ActionPerf = GetAgentNmStr() + " picked up " + CarryTangibleAgentNm;
        const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
        EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
      }
    }
  }
  return BestRunAction;
}

PAgent TMotherAgent::ClonePAgent(){
  return PMotherAgent(new TMotherAgent(*this));
}

TRunAction TMotherAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return THumanAgent::GetBestAction();
}

PAgent TGrandmaAgent::ClonePAgent(){
  return PGrandmaAgent(new TGrandmaAgent(*this));
}

TRunAction TGrandmaAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return THumanAgent::GetBestAction();
}

PAgent THouseAgent::ClonePAgent(){
  return PHouseAgent(new THouseAgent(*this));
}

TRunAction THouseAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return TGeoAgent::GetBestAction();
}

PAgent TGeoAgent::ClonePAgent(){
  return PGeoAgent(new TGeoAgent(*this));
}

TRunAction TGeoAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return TTangibleAgent::GetBestAction();
}

TAGoal::TAGoal(const TAGoal& CopyAGoal){
  const NmToPAgentM& CopyGoalNmToAgentM = CopyAGoal.GoalNmToAgentM;
  for (auto& CopyGoalNmToAgentI : CopyGoalNmToAgentM){
    GoalNmToAgentM[CopyGoalNmToAgentI.first] = PAgent(new TAgent(*CopyGoalNmToAgentI.second));
  }
  const unordered_map<string, StrS>& CopyAgentNmToGoalAttrKeySM = CopyAGoal.AgentNmToGoalAttrKeySM;
  for (auto& CopyAgentNmToGoalAttrKeySI : CopyAgentNmToGoalAttrKeySM){
    AgentNmToGoalAttrKeySM[CopyAgentNmToGoalAttrKeySI.first] = CopyAgentNmToGoalAttrKeySI.second;
  }
}

bool TAGoal::IsGoalAttrKey(const string& AgentNm, const string& GoalAttrKey)const{
  bool IsFind;
  try{
    const StrS& GoalAttrKeyS = AgentNmToGoalAttrKeySM.at(AgentNm);
    IsFind = GoalAttrKeyS.find(GoalAttrKey) != GoalAttrKeyS.end();
  } catch (const std::exception& Except){//exception if GoalAttrKey not found
    return false;
  }
  return IsFind;
}

void TAGoal::AddGoalAttrKey(const string& AgentNm, const string& GoalAttrKey){
  AgentNmToGoalAttrKeySM[AgentNm].insert(GoalAttrKey);
}

void TAGoal::DelGoalAttrKey(const string& AgentNm, const string& GoalAttrKey){
  AgentNmToGoalAttrKeySM[AgentNm].erase(GoalAttrKey);
}

void TAGoal::AddAgent(const PAgent Agent){
  IAssert(Agent->IsAgentNm());
  const string& AgentNm = Agent->GetAgentNmStr();
  auto DeepCopyAgent(Agent);
  //GoalNmToAgentM[AgentNm] = Agent;
  GoalNmToAgentM[AgentNm] = DeepCopyAgent;
}

void TAGoal::DelAgent(const string& AgentNm){
  IAssert(IsAgent(AgentNm));
  GoalNmToAgentM.erase(AgentNm);
}

PAgent TAGoal::GetAgent(const string& AgentNm){
  IAssert(IsAgent(AgentNm));
  return GoalNmToAgentM[AgentNm];
}

double TAGoal::GetAttrScore(const TAVal& ScoreAVal, const TAVal& GoalAVal){
  TAType ScoreAType = ScoreAVal.GetType();
  TAType GoalAType = GoalAVal.GetType();
  IAssert(GoalAType == ScoreAType);
  switch (GoalAType){
  case (TAType::Bool):
    {
      if (ScoreAVal.GetBool() == GoalAVal.GetBool()){ return 1.0; } else{ return 0.0; }
    }
  case (TAType::Int):
    {
      const double ScoreDbl = ScoreAVal.GetInt();
      const double GoalDbl = GoalAVal.GetInt();
      return 1.0 / (max(ScoreDbl, GoalDbl) - min(ScoreDbl, GoalDbl) + 1.0);
    }
  case (TAType::Dbl):
    {
      const double ScoreDbl = ScoreAVal.GetDbl();
      const double GoalDbl = GoalAVal.GetDbl();
      return 1.0 / (max(ScoreDbl, GoalDbl) - min(ScoreDbl, GoalDbl) + 1.0);
    }
  case (TAType::Str):
    {
      if (ScoreAVal.GetStr() == GoalAVal.GetStr()){ return 1.0; } else{ return 0.0; }
    }
  case (TAType::Vec):
    {
      vector<TAVal> ScoreAValV = ScoreAVal.GetVec();
      vector<TAVal> GoalAValV = GoalAVal.GetVec();
      if ((ScoreAValV.size() == 0) || (GoalAValV.size() == 0)){ return 0.0; }
      double AttrScoresSum = 0.0;
      for (auto& GoalAValI : GoalAValV){
        double MxAttrScore = 0.0;
        auto MxScoreAValI = ScoreAValV.begin();
        auto ScoreAValI = ScoreAValV.begin();
        while (ScoreAValI != ScoreAValV.end()){
          double AttrScore = GetAttrScore(*ScoreAValI, GoalAValI);
          if (AttrScore > MxAttrScore){
            MxAttrScore = AttrScore;
            MxScoreAValI = ScoreAValI;
          }
        }
        if (MxAttrScore > 0.0){
          AttrScoresSum += MxAttrScore;
          ScoreAValV.erase(MxScoreAValI);
        }
      }
      double AttrScore = AttrScoresSum / double(GoalAValV.size());
      return AttrScore;
    }
  case (TAType::Set)://Improve later
    {
      multiset<TAVal> ScoreAValS = ScoreAVal.GetSet();
      multiset<TAVal> GoalAValS = GoalAVal.GetSet();
      if ((ScoreAValS.size() == 0) || (GoalAValS.size() == 0)){ return 0.0; }
      double AttrScoresSum = 0.0;
      for (auto& GoalAValI : GoalAValS){
        double MxAttrScore = 0.0;
        auto MxScoreAValI = ScoreAValS.begin();
        auto ScoreAValI = ScoreAValS.begin();
        while (ScoreAValI != ScoreAValS.end()){
          double AttrScore = GetAttrScore(*ScoreAValI, GoalAValI);
          if (AttrScore > MxAttrScore){
            MxAttrScore = AttrScore;
            MxScoreAValI = ScoreAValI;
          }
          ++ScoreAValI;
        }
        if (MxAttrScore > 0.0){
          AttrScoresSum += MxAttrScore;
          ScoreAValS.erase(MxScoreAValI);
        }
      }
      double AttrScore = AttrScoresSum / double(GoalAValS.size());
      return AttrScore;
    }
  case(TAType::Map):
    {
      unordered_map<string, TAVal> ScoreStrToAValM = ScoreAVal.GetMap();
      unordered_map<string, TAVal> GoalStrToAValM = GoalAVal.GetMap();
      double AttrScore = 0.0;
      for (auto& GoalStrToAValI : GoalStrToAValM){
        const string& KeyStr = GoalStrToAValI.first;
        if (ScoreStrToAValM.find(KeyStr) != ScoreStrToAValM.end()){
          AttrScore += GetAttrScore(ScoreStrToAValM[KeyStr], GoalStrToAValM[KeyStr]);
        }
      }
      return AttrScore;
    }
  default:
    {
      break;
    }
  }
  return 0.0;
}

double TAGoal::GetStateScore(NmToPAgentM NmToAgentM)const{
  double StateScore = 0.0;
  for (auto& GoalNmToAgentI : GoalNmToAgentM){
    string AgentNm = GoalNmToAgentI.first;
    PAgent GoalPAgent = GoalNmToAgentI.second;
    IAssert(NmToAgentM.find(AgentNm) != NmToAgentM.end());
    TAValBs GoalAValBs = GoalPAgent->GetAValBs();
    vector<pair<string, TAVal>> GoalANmValPrV = GoalAValBs.GetAttrNmValPrV();
    for (auto& GoalANmValPrI : GoalANmValPrV){
      string AttrNm = GoalANmValPrI.first;
      TAVal GoalAVal = GoalANmValPrI.second;
      if (!IsGoalAttrKey(AgentNm, AttrNm)){ continue; }//skip non-goal attributes
      IAssert(NmToAgentM[AgentNm]->IsAttr(AttrNm));
      TAVal StateAVal = NmToAgentM[AgentNm]->GetAttr(AttrNm);
      StateScore += GetAttrScore(StateAVal, GoalAVal);
    }
  }
  return StateScore;
}

double TAGoal::GetStateImprovedScore(NmToPAgentM OrigNmToAgentM, NmToPAgentM NewNmToAgentM){
  double OrigStateScore = GetStateScore(OrigNmToAgentM);
  double NewStateScore = GetStateScore(NewNmToAgentM);
  return NewStateScore - OrigStateScore;
}

bool TAGoal::IsGoalComplete(NmToPAgentM CurNmToAgentM)const{
  return GetStateScore(CurNmToAgentM) == GetStateScore(GoalNmToAgentM);
}

bool TAGoalBs::IsGoal(const string& GoalNm) const{
  return (NmToGoalM.find(GoalNm) != NmToGoalM.end()) || (CompleteGoalNmS.find(GoalNm) != CompleteGoalNmS.end());
}

bool TAGoalBs::IsPreGoalsComplete(const string& GoalNm) const{
  IAssert(IsGoal(GoalNm));
  const StrS& PreGoalNmS = GoalNmToPreGoalNmSM.at(GoalNm);
  for (auto& PreGoalNmI : PreGoalNmS){
    if (!IsGoalComplete(PreGoalNmI)){ return false; }
  }
  return true;
}

bool TAGoalBs::IsAllPreGoalsComplete() const{
  for (auto& GoalNmToPreGoalNmSI : GoalNmToPreGoalNmSM){
    for (auto& PreGoalNmI : GoalNmToPreGoalNmSI.second){
      if (!IsGoalComplete(PreGoalNmI)){ return false; }
    }
  }
  return true;
}

bool TAGoalBs::IsAllDoableGoalsComplete() const{
  //some pre goals aren't yet defined as goals at time of checking
  const bool& IsNmToGoalMEmptyBool = NmToGoalM.empty();
  const bool& IsAllPreGoalsCompleteBool = IsAllPreGoalsComplete();
  //for all uncomplete goals
  for (auto& NmToGoalI : NmToGoalM){
    const string& GoalNm = NmToGoalI.first;
    if (IsPreGoalsComplete(GoalNm)){ return false; }
  }
  return true;
}

bool TAGoalBs::IsGoalComplete(const string& GoalNm) const{
  //IAssert(IsGoal(GoalNm));
  return CompleteGoalNmS.find(GoalNm) != CompleteGoalNmS.end();
}

void TAGoalBs::UpdateCompleteGoals(NmToPAgentM& CurNmToAgentM){
  for (auto NmToGoalI = NmToGoalM.cbegin(); NmToGoalI != NmToGoalM.cend();){
    if (!IsPreGoalsComplete(NmToGoalI->first)){//goal incomplete if PreGoals incomplete
      ++NmToGoalI;
    } else if (NmToGoalI->second.IsGoalComplete(CurNmToAgentM)){//goal complete 
      CompleteGoalNmS.insert(NmToGoalI->first);
      NmToGoalI = NmToGoalM.erase(NmToGoalI);
    } else {//goal incomplete
      ++NmToGoalI;
    }
  }
}

vector<pair<string, TAGoal>> TAGoalBs::GetNmGoalPrV(){
  vector<pair<string, TAGoal>> NmGoalPrV;
  for (auto& NmToGoalI : NmToGoalM) {
    NmGoalPrV.push_back(make_pair(NmToGoalI.first, NmToGoalI.second));
  }
  return NmGoalPrV;
}

StrToDblM TAGoalBs::GetEvalGoalNmToActionScoreM(const NmToPAgentM& OrigNmToAgentM, const NmToPAgentM& NewNmToAgentM){
  unordered_map<string, double> GoalNmToActionScoreM;
  for (auto& NmToGoalI : NmToGoalM){
    if (!IsPreGoalsComplete(NmToGoalI.first)){ continue; }//skip consideration of goals that don't have all pregoals complete
    const string GoalNm = NmToGoalI.first;
    TAGoal Goal = NmToGoalI.second;
    double ActionScore = Goal.GetStateImprovedScore(OrigNmToAgentM, NewNmToAgentM);
    GoalNmToActionScoreM[GoalNm] = ActionScore;
  }
  return GoalNmToActionScoreM;
}

PAgent TFlowerFieldAgent::ClonePAgent(){
  return PFlowerFieldAgent(new TFlowerFieldAgent(*this));
}

TRunAction TFlowerFieldAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  return TGeoAgent::GetBestAction();
}

PAgent TAnimalAgent::ClonePAgent(){
  return PAnimalAgent(new TAnimalAgent(*this));
}

TRunAction TAnimalAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TLivingAgent::GetBestAction();
  return BestRunAction;
}

PAgent TWolfAgent::ClonePAgent(){
  return PWolfAgent(new TWolfAgent(*this));
}

void TWolfAgent::RunMoveFast(const int& MoveDir){
  if (MoveDir == -1){
    TLivingAgent::Wander(4);
  } else{
    TLivingAgent::Wander(4, MoveDir);
  }
}

NmToPAgentM TWolfAgent::SimAction(const int& Int1, void(TWolfAgent::* AgentFunc)(const int&)){
  NmToPAgentM NmToAgentMapBefore = WorldMd->GetNmToAgentMCopy();
  (this->*AgentFunc)(Int1);
  NmToPAgentM NmToAgentMapAfter = WorldMd->GetNmToAgentMCopy();
  WorldMd->SetAgentsValsGoalsBs(NmToAgentMapBefore);
  return NmToAgentMapAfter;
}

TRunAction TWolfAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TAnimalAgent::GetBestAction();
  //RunMoveFast evaluation
  {
    for (int MoveDir = 1; MoveDir <= 4; MoveDir++){
      NmToPAgentM SimNmToAgentM = SimAction(MoveDir, &TWolfAgent::RunMoveFast);
      const string PrimaryGoalNm = GetActionPrimaryGoalNm(SimNmToAgentM);
      const string ActionNm = "RunMoveFast";
      string ActionPerf = GetAgentNmStr() + " quickly moved 4 units ";
      switch (MoveDir){
      case(1): {ActionPerf += "up"; break; }
      case(2): {ActionPerf += "down"; break; }
      case(3): {ActionPerf += "left"; break; }
      case(4): {ActionPerf += "right"; break; }
      }
      const string ActionDesc = "At " + to_string(WorldMd->GetTime()) + " minutes, " + ActionPerf + ", in order to " + PrimaryGoalNm;
      EvalAction(SimNmToAgentM, ActionNm, ActionDesc, PrimaryGoalNm, BestRunAction);
    }
  }
  return BestRunAction;
}

PAgent TButterflyAgent::ClonePAgent(){
  return PButterflyAgent(new TButterflyAgent(*this));

}

TRunAction TButterflyAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TAnimalAgent::GetBestAction();
  return BestRunAction;
}

PAgent TFrogAgent::ClonePAgent(){
  return PFrogAgent(new TFrogAgent(*this));
}

TRunAction TFrogAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TAnimalAgent::GetBestAction();
  return BestRunAction;
}

PAgent TForestAgent::ClonePAgent(){
  return PForestAgent(new TForestAgent(*this));
}

TRunAction TForestAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = TGeoAgent::GetBestAction();
  return BestRunAction;
}

PAgent TWoodsmanAgent::ClonePAgent(){
  return PWoodsmanAgent(new TWoodsmanAgent(*this));
}

TRunAction TWoodsmanAgent::GetBestAction(){
  if (IsStopAllActions()){ return TRunAction(); }
  TRunAction BestRunAction = THumanAgent::GetBestAction();
  return BestRunAction;
}
