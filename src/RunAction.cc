#include "RunAction.hh"
#include "RunMessenger.hh"

#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <algorithm>

RunAction::RunAction()
  : fElectronEnergy(300.0 * keV),
    fRunMessenger(new RunMessenger(this))
{
  auto* analysis = G4AnalysisManager::Instance();
  analysis->SetDefaultFileType("root");
  analysis->SetNtupleMerging(true);

  analysis->CreateH1("h0", "X-ray photon spectrum",
                     300, 0., 300. * keV, "keV");
  analysis->CreateH2("hXY", "Photon-count spatial distribution",
                     50, -120., 120., 50, -120., 120., "mm", "mm");
  analysis->CreateH1("hX", "Photon-count X profile", 50, -120., 120., "mm");
  analysis->CreateH1("hY", "Photon-count Y profile", 50, -120., 120., "mm");
  analysis->CreateH2("hXY_Energy", "Energy-weighted spatial distribution",
                     50, -120., 120., 50, -120., 120., "mm", "mm");

  analysis->CreateNtuple("spectrum", "Scored photons");
  analysis->CreateNtupleDColumn("energy_keV");
  analysis->CreateNtupleDColumn("x_mm");
  analysis->CreateNtupleDColumn("y_mm");
  analysis->CreateNtupleDColumn("weight");
  analysis->FinishNtuple();
}

RunAction::~RunAction()
{
  delete fRunMessenger;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4cout << "Electron energy: " << G4BestUnit(fElectronEnergy, "Energy") << G4endl;

  auto* analysis = G4AnalysisManager::Instance();
  const auto energyKeV = fElectronEnergy / keV;
  const auto nBins = std::max(1, static_cast<G4int>(energyKeV));
  analysis->SetH1(0, nBins, 0., fElectronEnergy, "keV");
  analysis->OpenFile("spectra.root");
}

void RunAction::EndOfRunAction(const G4Run*)
{
  auto* analysis = G4AnalysisManager::Instance();
  analysis->Write();
  analysis->CloseFile();
}
