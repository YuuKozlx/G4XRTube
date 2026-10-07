// Portions of this file are derived from Geant4 example code and are subject
// to the Geant4 Software License: https://geant4.web.cern.ch/download/license.html

#include "EventAction.hh"
#include "G4RunManager.hh"

EventAction::EventAction()
{
  G4RunManager::GetRunManager()->SetPrintProgress(1000000);
}
