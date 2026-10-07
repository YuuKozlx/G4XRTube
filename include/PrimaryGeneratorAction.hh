#ifndef G4XRTUBE_PRIMARY_GENERATOR_ACTION_HH
#define G4XRTUBE_PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction final : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction();
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;
  void SetElectronEnergy(G4double energy);

private:
  G4ParticleGun* fParticleGun = nullptr;
};

#endif
