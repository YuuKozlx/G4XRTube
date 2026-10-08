#ifndef G4XRTUBE_DETECTOR_CONSTRUCTION_HH
#define G4XRTUBE_DETECTOR_CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4Material.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class DetectorMessenger;

class DetectorConstruction final : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  ~DetectorConstruction() override;

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

  void SetAnodeMaterial(G4String);
  void SetFilterMaterial(G4String);
  void SetAnodeAngle(G4double value);
  void SetFilterThickness(G4double value);
  void SetInherentFilterMaterial(G4String);
  void SetInherentFilterThickness(G4double value);
  void SetScoringDistance(G4double value);
  void SetScoringOffsetX(G4double value);
  void SetScoringOffsetY(G4double value);

  G4Material* GetFilterMaterial() const;
  G4Material* GetTargetMaterial() const;
  G4double GetAnodeAngle() const;
  G4double GetFilterThickness() const;
  G4Material* GetInherentFilterMaterial() const;
  G4double GetInherentFilterThickness() const;
  G4double GetScoringDistance() const;
  G4double GetScoringOffsetX() const;
  G4double GetScoringOffsetY() const;

private:
  void DefineMaterials();
  G4VPhysicalVolume* ConstructVolumes();

private:
  G4VPhysicalVolume* worldPV;

  G4double fAnodeAngle;
  G4double fFilterThickness;
  G4double fInherentFilterThickness;
  G4double fScoringDistance;
  G4double fScoringOffsetX;
  G4double fScoringOffsetY;

  G4Material* fInherentFilterMaterial;
  G4Material* fAnodeMaterial;
  G4Material* fFilterMaterial;

  G4Material* vacuum;
  G4Material* beryllium;

  G4LogicalVolume* detectorLV;
  G4LogicalVolume* anodeLV;
  G4LogicalVolume* filterLV;
  G4LogicalVolume* worldLV;
  G4LogicalVolume* vacuumBoxLV;
  G4LogicalVolume* inhfilterLV;

  DetectorMessenger* fDetectorMessenger;
  G4bool fCheckOverlaps;
};

#endif
