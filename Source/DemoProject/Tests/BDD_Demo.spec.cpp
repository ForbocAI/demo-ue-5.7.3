#include "Misc/AutomationTest.h"

DEFINE_SPEC(FNewBDDSpec, "ForbocAI.Demo.NewBDD",
            EAutomationTestFlags::ProductFilter |
                EAutomationTestFlags::ApplicationContextMask)

void FNewBDDSpec::Define() {
  Describe("New Feature BDD", [this]() {
    It("Should prove BDD works in Demo Project", [this]() {
      bool bWorks = true;
      TestTrue("BDD Works", bWorks);
    });
  });
}
