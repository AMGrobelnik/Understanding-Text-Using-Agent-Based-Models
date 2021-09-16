#include "WorldModel.h"

using namespace amg;

int main(){
  PRidingHoodWorldMd RHWM(new TRidingHoodWorldMd());
  RHWM->Run();
}