#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det)
  : G4UImessenger(),
    fDetector(Det),
    fXRayTubeDir(nullptr),
    fDetDir(nullptr),
    fTargMatCmd(nullptr),
    fFilterMatCmd(nullptr),
    fAnodeAngleCmd(nullptr),
    fFilterThicknessCmd(nullptr),
    fInherentFilterMatCmd(nullptr),
    fInherentFilterThicknessCmd(nullptr),
    fScoringDistanceCmd(nullptr),
    fScoringOffsetXCmd(nullptr),
    fScoringOffsetYCmd(nullptr)
{
  fXRayTubeDir = new G4UIdirectory("/XRtube/");
  fXRayTubeDir->SetGuidance("G4XRTube geometry controls.");

  G4bool broadcast = false;
  fDetDir = new G4UIdirectory("/XRtube/det/", broadcast);
  fDetDir->SetGuidance("Target and filtration controls.");

  fTargMatCmd = new G4UIcmdWithAString("/XRtube/det/setTargetMaterial", this);
  fTargMatCmd->SetGuidance("Select material of the target");
  fTargMatCmd->SetParameterName("G4_Material", false);
  fTargMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fFilterMatCmd = new G4UIcmdWithAString("/XRtube/det/setFilterMaterial", this);
  fFilterMatCmd->SetGuidance("Select material of the filter");
  fFilterMatCmd->SetParameterName("G4_Material", false);
  fFilterMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fAnodeAngleCmd = new G4UIcmdWithADoubleAndUnit("/XRtube/det/setAnodeAngle", this);
  fAnodeAngleCmd->SetGuidance("Set the x-ray tube anode angle.");
  fAnodeAngleCmd->SetUnitCategory("Angle");
  fAnodeAngleCmd->SetDefaultUnit("deg");
  fAnodeAngleCmd->SetParameterName("AnodeAngle", false);
  fAnodeAngleCmd->SetRange("AnodeAngle>0. && AnodeAngle<90.");
  fAnodeAngleCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fFilterThicknessCmd = new G4UIcmdWithADoubleAndUnit("/XRtube/det/setFilterThickness", this);
  fFilterThicknessCmd->SetGuidance("Set the filter thickness.");
  fFilterThicknessCmd->SetUnitCategory("Length");
  fFilterThicknessCmd->SetDefaultUnit("mm");
  fFilterThicknessCmd->SetParameterName("FilterThickness", false);
  fFilterThicknessCmd->SetRange("FilterThickness>0.");
  fFilterThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fInherentFilterMatCmd = new G4UIcmdWithAString("/XRtube/det/setInherentFilterMaterial", this);
  fInherentFilterMatCmd->SetGuidance("Select material of the inherent filter.");
  fInherentFilterMatCmd->SetParameterName("G4_Material", false);
  fInherentFilterMatCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fInherentFilterThicknessCmd =
      new G4UIcmdWithADoubleAndUnit("/XRtube/det/setInherentFilterThickness", this);
  fInherentFilterThicknessCmd->SetGuidance("Set the inherent filter thickness.");
  fInherentFilterThicknessCmd->SetUnitCategory("Length");
  fInherentFilterThicknessCmd->SetDefaultUnit("mm");
  fInherentFilterThicknessCmd->SetParameterName("InherentFilterThickness", false);
  fInherentFilterThicknessCmd->SetRange("InherentFilterThickness>0.");
  fInherentFilterThicknessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fScoringDistanceCmd =
      new G4UIcmdWithADoubleAndUnit("/XRtube/det/setScoringDistance", this);
  fScoringDistanceCmd->SetGuidance("Set target-to-scoring-plane distance.");
  fScoringDistanceCmd->SetUnitCategory("Length");
  fScoringDistanceCmd->SetDefaultUnit("cm");
  fScoringDistanceCmd->SetParameterName("ScoringDistance", false);
  fScoringDistanceCmd->SetRange("ScoringDistance>0.");
  fScoringDistanceCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fScoringOffsetXCmd =
      new G4UIcmdWithADoubleAndUnit("/XRtube/det/setScoringOffsetX", this);
  fScoringOffsetXCmd->SetGuidance("Set scoring-plane center X offset.");
  fScoringOffsetXCmd->SetUnitCategory("Length");
  fScoringOffsetXCmd->SetDefaultUnit("cm");
  fScoringOffsetXCmd->SetParameterName("ScoringOffsetX", false);
  fScoringOffsetXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fScoringOffsetYCmd =
      new G4UIcmdWithADoubleAndUnit("/XRtube/det/setScoringOffsetY", this);
  fScoringOffsetYCmd->SetGuidance("Set scoring-plane center Y offset.");
  fScoringOffsetYCmd->SetUnitCategory("Length");
  fScoringOffsetYCmd->SetDefaultUnit("cm");
  fScoringOffsetYCmd->SetParameterName("ScoringOffsetY", false);
  fScoringOffsetYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

DetectorMessenger::~DetectorMessenger()
{
  delete fTargMatCmd;
  delete fFilterMatCmd;
  delete fAnodeAngleCmd;
  delete fFilterThicknessCmd;
  delete fInherentFilterMatCmd;
  delete fInherentFilterThicknessCmd;
  delete fScoringDistanceCmd;
  delete fScoringOffsetXCmd;
  delete fScoringOffsetYCmd;
  delete fDetDir;
  delete fXRayTubeDir;
}

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fTargMatCmd) {
    fDetector->SetAnodeMaterial(newValue);
  }
  else if (command == fFilterMatCmd) {
    fDetector->SetFilterMaterial(newValue);
  }
  else if (command == fAnodeAngleCmd) {
    fDetector->SetAnodeAngle(fAnodeAngleCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fFilterThicknessCmd) {
    fDetector->SetFilterThickness(fFilterThicknessCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fInherentFilterMatCmd) {
    fDetector->SetInherentFilterMaterial(newValue);
  }
  else if (command == fInherentFilterThicknessCmd) {
    fDetector->SetInherentFilterThickness(
        fInherentFilterThicknessCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fScoringDistanceCmd) {
    fDetector->SetScoringDistance(fScoringDistanceCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fScoringOffsetXCmd) {
    fDetector->SetScoringOffsetX(fScoringOffsetXCmd->GetNewDoubleValue(newValue));
  }
  else if (command == fScoringOffsetYCmd) {
    fDetector->SetScoringOffsetY(fScoringOffsetYCmd->GetNewDoubleValue(newValue));
  }
}
