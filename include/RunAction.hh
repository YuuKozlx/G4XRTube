#ifndef G4XRTUBE_RUN_ACTION_HH
#define G4XRTUBE_RUN_ACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;
class RunMessenger;
class DetectorConstruction;

class RunAction final : public G4UserRunAction
{
public:
  explicit RunAction(const DetectorConstruction* detector);
  ~RunAction() override;

  void BeginOfRunAction(const G4Run* run) override;
  void EndOfRunAction(const G4Run* run) override;
  G4Run* GenerateRun() override;
  void SetElectronEnergy(G4double energy) { fElectronEnergy = energy; }

private:
  G4double fElectronEnergy;
  const DetectorConstruction* fDetector;
  RunMessenger* fRunMessenger = nullptr;
};

#endif
