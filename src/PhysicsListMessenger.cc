//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file PhysicsListMessenger.cc
/// \brief Definition of the PhysicsListMessenger class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsListMessenger.hh"
#include "PhysicsList.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::PhysicsListMessenger(PhysicsList *pPhys) : G4UImessenger(),
                                                                 fPhysicsList(pPhys),
                                                                 fPhysDir(nullptr),
                                                                 fCutsCmd(nullptr),
                                                                 fListCmd(nullptr),
                                                                 fPixeCmd(nullptr),
                                                                 fFluorescenceCmd(nullptr),
                                                                 fAugerCmd(nullptr),
                                                                 fAugerCascadeCmd(nullptr)
{
  fPhysDir = new G4UIdirectory("/phys/");
  fPhysDir->SetGuidance("PhysicsList control");

  fCutsCmd = new G4UIcmdWithADoubleAndUnit("/phys/setCuts", this);
  fCutsCmd->SetGuidance("set cuts");
  fCutsCmd->SetParameterName("cuts", false);
  fCutsCmd->SetUnitCategory("Length");
  fCutsCmd->SetRange("cuts>0.");
  fCutsCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fListCmd = new G4UIcmdWithAString("/phys/SelectPhysicsList", this);
  fListCmd->SetGuidance("Select the EM physics list");
  fListCmd->SetParameterName("PList", false);
  fListCmd->AvailableForStates(G4State_PreInit);

  fPixeCmd = new G4UIcmdWithABool("/phys/setPIXE", this);
  fPixeCmd->SetGuidance("Enable or disable PIXE atomic de-excitation.");
  fPixeCmd->SetParameterName("PIXE", false);
  fPixeCmd->AvailableForStates(G4State_PreInit);

  fFluorescenceCmd = new G4UIcmdWithABool("/phys/setFluorescence", this);
  fFluorescenceCmd->SetGuidance("Enable or disable fluorescence photons.");
  fFluorescenceCmd->SetParameterName("Fluorescence", false);
  fFluorescenceCmd->AvailableForStates(G4State_PreInit);

  fAugerCmd = new G4UIcmdWithABool("/phys/setAuger", this);
  fAugerCmd->SetGuidance("Enable or disable Auger electrons.");
  fAugerCmd->SetParameterName("Auger", false);
  fAugerCmd->AvailableForStates(G4State_PreInit);

  fAugerCascadeCmd = new G4UIcmdWithABool("/phys/setAugerCascade", this);
  fAugerCascadeCmd->SetGuidance("Enable or disable Auger cascade.");
  fAugerCascadeCmd->SetParameterName("AugerCascade", false);
  fAugerCascadeCmd->AvailableForStates(G4State_PreInit);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete fPhysDir;
  delete fCutsCmd;
  delete fListCmd;
  delete fPixeCmd;
  delete fFluorescenceCmd;
  delete fAugerCmd;
  delete fAugerCascadeCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand *command,
                                       G4String newValue)
{
  if (command == fCutsCmd) {
    fPhysicsList->SetDefaultCutsValue(fCutsCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fListCmd) {
    fPhysicsList->SelectPhysicsList(newValue);
  }
  else if (command == fPixeCmd) {
    fPhysicsList->SetPIXE(fPixeCmd->GetNewBoolValue(newValue));
  }
  else if (command == fFluorescenceCmd) {
    fPhysicsList->SetFluorescence(fFluorescenceCmd->GetNewBoolValue(newValue));
  }
  else if (command == fAugerCmd) {
    fPhysicsList->SetAuger(fAugerCmd->GetNewBoolValue(newValue));
  }
  else if (command == fAugerCascadeCmd) {
    fPhysicsList->SetAugerCascade(fAugerCascadeCmd->GetNewBoolValue(newValue));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
