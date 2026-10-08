#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "G4SystemOfUnits.hh"
#include "G4LossTableManager.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4EmParameters.hh"

// EM physics lists
#include "G4EmStandardPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4DecayPhysics.hh"

PhysicsList::PhysicsList()
  : G4VUserPhysicsList(),
    fParticleList(nullptr),
    fEmPhysicsList(nullptr),
    fPIXE(false),
    fFluorescence(true),
    fAuger(true),
    fAugerCascade(true),
    fPMessenger(nullptr)
{
  defaultCutValue = 1.0 * um;

  SetVerboseLevel(1);

  fPMessenger   = new PhysicsListMessenger(this);
  fParticleList = new G4DecayPhysics(verboseLevel);
  fEmPhysicsList = new G4EmPenelopePhysics(verboseLevel);
}

PhysicsList::~PhysicsList()
{
  delete fParticleList;
  delete fEmPhysicsList;
  delete fPMessenger;
}

void PhysicsList::ConstructParticle()
{
  fParticleList->ConstructParticle();
}

void PhysicsList::ConstructProcess()
{
  auto* emParameters = G4EmParameters::Instance();
  emParameters->SetFluo(fFluorescence);
  emParameters->SetAuger(fAuger || fAugerCascade);
  emParameters->SetPixe(fPIXE);

  AddTransportation();

  if (fParticleList) {
    fParticleList->ConstructProcess();
  }

  if (fEmPhysicsList) {
    fEmPhysicsList->ConstructProcess();
  }

  auto* de = new G4UAtomicDeexcitation();
  de->SetFluo(fFluorescence);
  de->SetAuger(fAuger);
  de->SetAugerCascade(fAugerCascade);
  de->SetPIXE(fPIXE);
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);
}

void PhysicsList::SelectPhysicsList(const G4String& name)
{
  if (verboseLevel > 1) {
    G4cout << "### PhysicsList::SelectPhysicsList: <" << name << "> ###" << G4endl;
  }

  delete fEmPhysicsList;
  fEmPhysicsList = nullptr;

  if (name == "standard") {
    fEmPhysicsList = new G4EmStandardPhysics(verboseLevel);
    G4cout << "Selected G4EmStandardPhysics" << G4endl;
  }
  else if (name == "standard_option4") {
    fEmPhysicsList = new G4EmStandardPhysics_option4(verboseLevel);
    G4cout << "Selected G4EmStandardPhysics_option4" << G4endl;
  }
  else if (name == "livermore") {
    fEmPhysicsList = new G4EmLivermorePhysics(verboseLevel);
    G4cout << "Selected G4EmLivermorePhysics" << G4endl;
  }
  else if (name == "penelope") {
    fEmPhysicsList = new G4EmPenelopePhysics(verboseLevel);
    G4cout << "Selected G4EmPenelopePhysics" << G4endl;
  }
  else if (name == "LowEP") {
    fEmPhysicsList = new G4EmLowEPPhysics(verboseLevel);
    G4cout << "Selected G4EmLowEPPhysics" << G4endl;
  }
  else {
    G4cout << "Unknown EM physics list '" << name
           << "'; using G4EmPenelopePhysics." << G4endl;
    fEmPhysicsList = new G4EmPenelopePhysics(verboseLevel);
  }
}

void PhysicsList::SetCuts()
{
  SetCutsWithDefault();

  if (verboseLevel > 0) {
    DumpCutValuesTable();
  }
}

void PhysicsList::SetDefaultCutsValue(G4double value)
{
  defaultCutValue = value;
}

void PhysicsList::SetPIXE(G4bool value)
{
  fPIXE = value;
}

void PhysicsList::SetFluorescence(G4bool value)
{
  fFluorescence = value;
}

void PhysicsList::SetAuger(G4bool value)
{
  fAuger = value;
}

void PhysicsList::SetAugerCascade(G4bool value)
{
  fAugerCascade = value;
}
