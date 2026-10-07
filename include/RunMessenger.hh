#ifndef G4XRTUBE_RUN_MESSENGER_HH
#define G4XRTUBE_RUN_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"

class RunAction;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

class RunMessenger final : public G4UImessenger
{
public:
  explicit RunMessenger(RunAction* runAction);
  ~RunMessenger() override;

  void SetNewValue(G4UIcommand* command, G4String value) override;

private:
  RunAction* fRunAction;
  G4UIdirectory* fXRayTubeDir = nullptr;
  G4UIcmdWithADoubleAndUnit* fSetEnergyCmd = nullptr;
};

#endif
