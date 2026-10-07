// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#ifndef G4XRTUBE_SENSITIVE_DETECTOR_HH
#define G4XRTUBE_SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"

class G4Step;

class SensitiveDetector final : public G4VSensitiveDetector
{
public:
  explicit SensitiveDetector(const G4String& name);
  ~SensitiveDetector() override = default;

  G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
};

#endif
