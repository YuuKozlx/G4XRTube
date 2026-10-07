// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"

#include "G4RunManagerFactory.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "Randomize.hh"

#include <cstdlib>
#include <ctime>
#include <iostream>

int main(int argc, char** argv)
{
  if (argc > 2) {
    std::cerr << "Usage: " << argv[0] << " [macro.mac]\n";
    return EXIT_FAILURE;
  }

  auto* ui = (argc == 1) ? new G4UIExecutive(argc, argv) : nullptr;

  G4Random::setTheEngine(new CLHEP::MTwistEngine);
  G4Random::setTheSeed(static_cast<long>(std::time(nullptr)));

  auto* runManager = G4RunManagerFactory::CreateRunManager();
  runManager->SetUserInitialization(new DetectorConstruction());
  runManager->SetUserInitialization(new PhysicsList());
  runManager->SetUserInitialization(new ActionInitialization());

  auto* uiManager = G4UImanager::GetUIpointer();
  G4VisManager* visManager = nullptr;
  G4int commandStatus = 0;

  if (ui) {
    visManager = new G4VisExecutive;
    visManager->Initialize();
    commandStatus = uiManager->ApplyCommand("/control/execute init_vis.mac");
    if (commandStatus == 0) {
      ui->SessionStart();
    }
    delete ui;
  }
  else {
    commandStatus = uiManager->ApplyCommand(G4String("/control/execute ") + argv[1]);
  }

  delete visManager;
  delete runManager;

  if (commandStatus != 0) {
    std::cerr << "Geant4 command processing failed with status "
              << commandStatus << ".\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
