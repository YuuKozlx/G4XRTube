// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#include "SensitiveDetector.hh"

#include "G4AnalysisManager.hh"
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
  const G4double energyKeV = energy / keV;
  const G4double weight = preStep->GetWeight();
  const auto& position = preStep->GetPosition();
  const G4double x = position.x();
  const G4double y = position.y();

  auto* analysis = G4AnalysisManager::Instance();

  // The spectrum histogram declares keV as its unit, so it receives the
  // Geant4 internal energy value. The ntuple stores an explicit keV value.
  analysis->FillH1(0, energy, weight);
  analysis->FillH2(0, x, y, weight);
  analysis->FillH1(1, x, weight);
  analysis->FillH1(2, y, weight);
  analysis->FillH2(1, x, y, energyKeV * weight);

  analysis->FillNtupleDColumn(0, 0, energyKeV);
  analysis->FillNtupleDColumn(0, 1, x / mm);
  analysis->FillNtupleDColumn(0, 2, y / mm);
  analysis->FillNtupleDColumn(0, 3, weight);
  analysis->AddNtupleRow(0);

  return true;
}
