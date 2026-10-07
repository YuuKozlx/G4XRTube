#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "SensitiveDetector.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Para.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
    worldPV(nullptr),
    fAnodeAngle(12.0 * deg),
    fFilterThickness(0.8 * mm),
    fInherentFilterThickness(1.0 * mm),
    fInherentFilterMaterial(nullptr),
    fAnodeMaterial(nullptr),
    fFilterMaterial(nullptr),
    vacuum(nullptr),
    beryllium(nullptr),
    detectorLV(nullptr),
    anodeLV(nullptr),
    filterLV(nullptr),
    worldLV(nullptr),
    vacuumBoxLV(nullptr),
    inhfilterLV(nullptr),
    fDetectorMessenger(nullptr),
    fCheckOverlaps(true)
{
  DefineMaterials();

  fInherentFilterMaterial = beryllium;
  fDetectorMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
  delete fDetectorMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructVolumes();
}

void DetectorConstruction::DefineMaterials()
{
  auto* nist = G4NistManager::Instance();

  fAnodeMaterial = nist->FindOrBuildMaterial("G4_W");
  beryllium      = nist->FindOrBuildMaterial("G4_Be");
  vacuum         = nist->FindOrBuildMaterial("G4_Galactic");

  fFilterMaterial = vacuum;
}

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  const G4double worldSizeXYZ = 500.0 * cm;

  auto* solidWorld = new G4Box("World",
                               worldSizeXYZ / 2.0,
                               worldSizeXYZ / 2.0,
                               worldSizeXYZ / 2.0);

  worldLV = new G4LogicalVolume(solidWorld, vacuum, "World");

  worldPV = new G4PVPlacement(nullptr, G4ThreeVector(), worldLV, "World",
                              nullptr, false, 0, fCheckOverlaps);

  // Inherent filter
  auto* inhfilterSolid = new G4Box("inhfilter",
                                   5.0 * cm,
                                   5.0 * cm,
                                   fInherentFilterThickness / 2.0);

  inhfilterLV = new G4LogicalVolume(inhfilterSolid,
                                    fInherentFilterMaterial,
                                    "inhfilter");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(0., 0., -30.0 * mm),
                    inhfilterLV,
                    "inhfilter",
                    worldLV,
                    false,
                    0,
                    fCheckOverlaps);

  // External filter
  auto* solidFilter = new G4Box("filter",
                                30.0 * cm,
                                30.0 * cm,
                                fFilterThickness / 2.0);

  filterLV = new G4LogicalVolume(solidFilter, fFilterMaterial, "filter");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(0., 0., -40.0 * mm),
                    filterLV,
                    "filter",
                    worldLV,
                    false,
                    0,
                    fCheckOverlaps);

  // Vacuum box
  const G4double vacuumX = 1.25 * cm;
  const G4double vacuumY = 6.00 * cm;
  const G4double vacuumZ = 1.25 * cm;

  auto* vacuumBoxSolid = new G4Box("vacuumBox", vacuumX, vacuumY, vacuumZ);
  vacuumBoxLV = new G4LogicalVolume(vacuumBoxSolid, vacuum, "vacuumBox");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(0., 0., 0.),
                    vacuumBoxLV,
                    "vacuumBox",
                    worldLV,
                    false,
                    0,
                    fCheckOverlaps);

  // Anode
  const G4double theta = 0.0 * deg;
  const G4double phi   = 0.0 * deg;

  auto* solidAnode = new G4Para("Anode",
                                10.0 * mm,
                                5.0 * mm,
                                5.0 * mm,
                                fAnodeAngle,
                                theta,
                                phi);

  anodeLV = new G4LogicalVolume(solidAnode, fAnodeMaterial, "Anode");

  auto* rotAnode = new G4RotationMatrix();
  rotAnode->rotateY(-90.0 * deg);
  rotAnode->rotateZ(-90.0 * deg);

  new G4PVPlacement(rotAnode,
                    G4ThreeVector(0., -10.0 * mm, 0.),
                    anodeLV,
                    "Anode",
                    vacuumBoxLV,
                    false,
                    0,
                    fCheckOverlaps);

  // Detector scoring plane
  auto* solidDetector = new G4Tubs("detector",
                                   0.0 * mm,
                                   120.0 * mm,
                                   0.5 * mm,
                                   0.0 * deg,
                                   360.0 * deg);

  detectorLV = new G4LogicalVolume(solidDetector, vacuum, "detector");

  new G4PVPlacement(nullptr,
                    G4ThreeVector(0., 0., -50.0 * cm),
                    detectorLV,
                    "detector",
                    worldLV,
                    false,
                    0,
                    fCheckOverlaps);

  auto* blue = new G4VisAttributes(G4Colour(0., 0., 1., 0.1));
  blue->SetVisibility(true);
  blue->SetForceSolid(true);

  auto* black = new G4VisAttributes(G4Colour(0., 0., 0., 1.));
  black->SetVisibility(true);
  black->SetForceSolid(true);

  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());
  vacuumBoxLV->SetVisAttributes(blue);
  anodeLV->SetVisAttributes(black);
  detectorLV->SetVisAttributes(black);
  filterLV->SetVisAttributes(black);
  inhfilterLV->SetVisAttributes(blue);

  return worldPV;
}

void DetectorConstruction::SetAnodeMaterial(G4String materialChoice)
{
  auto* material = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (!material) {
    G4cout << "\n--> warning from DetectorConstruction::SetAnodeMaterial: "
           << materialChoice << " not found" << G4endl;
    return;
  }

  fAnodeMaterial = material;
  if (anodeLV) {
    anodeLV->SetMaterial(fAnodeMaterial);
  }

  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

void DetectorConstruction::SetFilterMaterial(G4String materialChoice)
{
  auto* material = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (!material) {
    G4cout << "\n--> warning from DetectorConstruction::SetFilterMaterial: "
           << materialChoice << " not found" << G4endl;
    return;
  }

  fFilterMaterial = material;
  if (filterLV) {
    filterLV->SetMaterial(fFilterMaterial);
  }

  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

void DetectorConstruction::SetAnodeAngle(G4double value)
{
  fAnodeAngle = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetFilterThickness(G4double value)
{
  fFilterThickness = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

void DetectorConstruction::SetInherentFilterMaterial(G4String materialChoice)
{
  auto* material = G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);

  if (!material) {
    G4cout << "\n--> warning from DetectorConstruction::SetInherentFilterMaterial: "
           << materialChoice << " not found" << G4endl;
    return;
  }

  fInherentFilterMaterial = material;
  if (inhfilterLV) {
    inhfilterLV->SetMaterial(fInherentFilterMaterial);
  }

  G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}

void DetectorConstruction::SetInherentFilterThickness(G4double value)
{
  fInherentFilterThickness = value;
  G4RunManager::GetRunManager()->ReinitializeGeometry();
}

G4Material* DetectorConstruction::GetFilterMaterial() const
{
  return fFilterMaterial;
}

G4Material* DetectorConstruction::GetTargetMaterial() const
{
  return fAnodeMaterial;
}

G4double DetectorConstruction::GetAnodeAngle() const
{
  return fAnodeAngle;
}

G4double DetectorConstruction::GetFilterThickness() const
{
  return fFilterThickness;
}

G4Material* DetectorConstruction::GetInherentFilterMaterial() const
{
  return fInherentFilterMaterial;
}

G4double DetectorConstruction::GetInherentFilterThickness() const
{
  return fInherentFilterThickness;
}

void DetectorConstruction::ConstructSDandField()
{
  auto* sd = new SensitiveDetector("DetectorSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);

  if (detectorLV) {
    detectorLV->SetSensitiveDetector(sd);
  }
}
