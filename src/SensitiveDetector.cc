// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#include "SensitiveDetector.hh"

#include "SpectrumRun.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
  : G4VSensitiveDetector(name)
{}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto* track = step->GetTrack();
  const auto particleName = track->GetParticleDefinition()->GetParticleName();
  track->SetTrackStatus(fStopAndKill);

  if (particleName != "gamma") {
    return false;
  }

  const auto* preStep = step->GetPreStepPoint();
  const G4double energy = preStep->GetKineticEnergy();
  const G4double weight = preStep->GetWeight();
  const auto& position = preStep->GetPosition();
  const G4double x = position.x();
  const G4double y = position.y();

  auto* run = static_cast<SpectrumRun*>(
      G4RunManager::GetRunManager()->GetNonConstCurrentRun());
  if (run) {
    run->RecordPhoton(energy, x, y, weight);
  }

  return true;
}
