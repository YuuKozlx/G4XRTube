#include "SpectrumRun.hh"

#include "G4SystemOfUnits.hh"

#include <algorithm>
#include <cmath>

namespace {
constexpr G4double scoringRadius = 120.0 * mm;
}

SpectrumRun::SpectrumRun(std::vector<ScoringPosition> positions,
                         G4double maximumEnergy)
  : fPositions(std::move(positions)),
    fBinWidth(1.0 * keV),
    fMaximumEnergy(maximumEnergy)
{
  const auto bins = std::max(1, static_cast<G4int>(std::ceil(maximumEnergy / fBinWidth)));
  fSpectra.assign(fPositions.size(), std::vector<G4double>(bins, 0.0));
}

void SpectrumRun::RecordPhoton(G4double energy, G4double x, G4double y,
                               G4double weight)
{
  if (fSpectra.empty()) {
    return;
  }
  const auto bin = static_cast<std::size_t>(energy / fBinWidth);
  if (bin >= fSpectra.front().size()) {
    return;
  }

  const G4double radiusSquared = scoringRadius * scoringRadius;
  for (std::size_t i = 0; i < fPositions.size(); ++i) {
    const G4double dx = x - fPositions[i].x;
    const G4double dy = y - fPositions[i].y;
    if (dx * dx + dy * dy <= radiusSquared) {
      fSpectra[i][bin] += weight;
    }
  }
}

void SpectrumRun::Merge(const G4Run* run)
{
  const auto* other = static_cast<const SpectrumRun*>(run);
  for (std::size_t position = 0; position < fSpectra.size(); ++position) {
    for (std::size_t bin = 0; bin < fSpectra[position].size(); ++bin) {
      fSpectra[position][bin] += other->fSpectra[position][bin];
    }
  }
  G4Run::Merge(run);
}
