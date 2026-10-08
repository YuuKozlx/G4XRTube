#ifndef G4XRTUBE_SPECTRUM_RUN_HH
#define G4XRTUBE_SPECTRUM_RUN_HH

#include "G4Run.hh"
#include "ScoringPosition.hh"

#include <vector>

class SpectrumRun final : public G4Run
{
public:
  SpectrumRun(std::vector<ScoringPosition> positions, G4double maximumEnergy);

  void RecordPhoton(G4double energy, G4double x, G4double y, G4double weight);
  void Merge(const G4Run* run) override;

  const std::vector<ScoringPosition>& GetPositions() const { return fPositions; }
  const std::vector<std::vector<G4double>>& GetSpectra() const { return fSpectra; }
  G4double GetBinWidth() const { return fBinWidth; }
  G4double GetMaximumEnergy() const { return fMaximumEnergy; }

private:
  std::vector<ScoringPosition> fPositions;
  std::vector<std::vector<G4double>> fSpectra;
  G4double fBinWidth;
  G4double fMaximumEnergy;
};

#endif
