#include "RunMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"

RunMessenger::RunMessenger(RunAction* runAction)
  : G4UImessenger(),
    fRunAction(runAction),
    fXRayTubeDir(new G4UIdirectory("/xraytube/")),
    fSetEnergyCmd(new G4UIcmdWithADoubleAndUnit("/xraytube/setEnergy", this))
{
  fXRayTubeDir->SetGuidance("X-ray tube source controls.");

  fSetEnergyCmd->SetGuidance("Set the incident electron energy.");
  fSetEnergyCmd->SetParameterName("energy", false);
  fSetEnergyCmd->SetDefaultUnit("keV");
  fSetEnergyCmd->SetUnitCandidates("eV keV MeV");
  fSetEnergyCmd->SetRange("energy>0.");
  fSetEnergyCmd->AvailableForStates(G4State_Idle);
}

RunMessenger::~RunMessenger()
{
  delete fSetEnergyCmd;
  delete fXRayTubeDir;
}

void RunMessenger::SetNewValue(G4UIcommand* command, G4String value)
{
  if (command != fSetEnergyCmd) {
    return;
  }

  const G4double energy = fSetEnergyCmd->GetNewDoubleValue(value);
  fRunAction->SetElectronEnergy(energy);

  const auto* action = G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  auto* generator = const_cast<PrimaryGeneratorAction*>(
      dynamic_cast<const PrimaryGeneratorAction*>(action));
  if (generator) {
    generator->SetElectronEnergy(energy);
  }

  G4cout << "Incident electron energy set to " << energy / keV << " keV" << G4endl;
}
