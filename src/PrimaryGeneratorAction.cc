#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
  : fParticleGun(new G4ParticleGun(1))
{
  auto* electron = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(electron);
  fParticleGun->SetParticleEnergy(300.0 * keV);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., -1., 0.));
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  const G4double focalSpotSize = 1.0 * mm;
  const G4double x = focalSpotSize * (G4UniformRand() - 0.5);
  const G4double y = 5.5 * cm;
  const G4double z = focalSpotSize * (G4UniformRand() - 0.5);

  fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
  fParticleGun->GeneratePrimaryVertex(event);
}

void PrimaryGeneratorAction::SetElectronEnergy(G4double energy)
{
  fParticleGun->SetParticleEnergy(energy);
}
