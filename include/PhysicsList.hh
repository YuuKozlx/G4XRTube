#ifndef G4XRTUBE_PHYSICS_LIST_HH
#define G4XRTUBE_PHYSICS_LIST_HH

#include "globals.hh"
#include "G4VUserPhysicsList.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;

class PhysicsList final : public G4VUserPhysicsList
{
public:
  PhysicsList();
  ~PhysicsList() override;

  void ConstructParticle() override;
  void ConstructProcess() override;
  void SetCuts() override;

  void SetDefaultCutsValue(G4double);
  void SetPIXE(G4bool value);
  void SetFluorescence(G4bool value);
  void SetAuger(G4bool value);
  void SetAugerCascade(G4bool value);
  void SelectPhysicsList(const G4String& name);

private:
  PhysicsList& operator=(const PhysicsList&) = delete;
  PhysicsList(const PhysicsList&) = delete;

  G4VPhysicsConstructor* fParticleList;
  G4VPhysicsConstructor* fEmPhysicsList;
  G4bool fPIXE;
  G4bool fFluorescence;
  G4bool fAuger;
  G4bool fAugerCascade;

  PhysicsListMessenger* fPMessenger;
};

#endif
