// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#ifndef G4XRTUBE_ACTION_INITIALIZATION_HH
#define G4XRTUBE_ACTION_INITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;

class ActionInitialization final : public G4VUserActionInitialization
{
public:
  explicit ActionInitialization(const DetectorConstruction* detector)
    : fDetector(detector) {}
  ~ActionInitialization() override = default;

  void BuildForMaster() const override;
  void Build() const override;

private:
  const DetectorConstruction* fDetector;
};

#endif
