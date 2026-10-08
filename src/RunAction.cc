#include "RunAction.hh"

#include "DetectorConstruction.hh"
#include "RunMessenger.hh"
#include "SpectrumRun.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4Threading.hh"
#include "G4UnitsTable.hh"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
constexpr G4double scoringRadius = 120.0 * mm;
constexpr G4double electronPerMilliampereSecond = 6.241509074e15;

std::string positionTag(G4double value, char axis)
{
  std::ostringstream tag;
  tag << axis;
  if (value < 0.0) {
    tag << 'm';
  }
  tag << std::fixed << std::setprecision(3) << std::abs(value / cm);
  std::string result = tag.str();
  while (!result.empty() && result.back() == '0') result.pop_back();
  if (!result.empty() && result.back() == '.') result.pop_back();
  for (auto& character : result) {
    if (character == '.') character = 'p';
  }
  return result;
}

std::string outputStem(const ScoringPosition& position, G4double distance)
{
  std::ostringstream stem;
  stem << "spectrum_" << positionTag(position.x, 'x') << "cm_"
       << positionTag(position.y, 'y') << "cm_"
       << positionTag(distance, 'z') << "cm";
  return stem.str();
}

void writeSpectrum(const std::string& stem, const SpectrumRun& spectrum,
                   std::size_t positionIndex, G4double distance,
                   G4double electronEnergy)
{
  const auto& position = spectrum.GetPositions()[positionIndex];
  const auto& counts = spectrum.GetSpectra()[positionIndex];
  const G4double areaCm2 = CLHEP::pi * std::pow(scoringRadius / cm, 2);
  const G4double events = spectrum.GetNumberOfEvent();
  const G4double scale = events > 0.0
      ? electronPerMilliampereSecond / (events * areaCm2 * (spectrum.GetBinWidth() / keV))
      : 0.0;

  std::ofstream spec(stem + ".spec");
  spec << "# Format: MCGPU-SPEC 1.0\n"
       << "# Source: G4XRTube Geant4 simulation\n"
       << "# Position: x=" << position.x / cm << " cm, y=" << position.y / cm
       << " cm, z=" << distance / cm << " cm; 1 mAs\n"
       << "# Scoring radius: " << scoringRadius / cm << " cm\n"
       << "# Primary electrons: " << spectrum.GetNumberOfEvent() << "\n"
       << "# Electron energy: " << electronEnergy / keV << " keV\n"
       << "# Energy[keV]  N[keV cm^2 mAs]^-1\n"
       << std::setprecision(10);

  std::ofstream svg(stem + ".svg");
  const G4double plotWidth = 900.0;
  const G4double plotHeight = 560.0;
  const G4double left = 86.0;
  const G4double right = 24.0;
  const G4double top = 38.0;
  const G4double bottom = 72.0;
  G4double maximum = 0.0;
  for (const auto count : counts) maximum = std::max(maximum, count * scale);
  maximum = maximum > 0.0 ? maximum * 1.08 : 1.0;

  svg << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"900\" height=\"560\" viewBox=\"0 0 900 560\">\n"
      << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n"
      << "<g font-family=\"sans-serif\" fill=\"#222\">\n"
      << "<text x=\"450\" y=\"24\" text-anchor=\"middle\" font-size=\"17\">G4XRTube spectrum at ("
      << position.x / cm << ", " << position.y / cm << ") cm, z=" << distance / cm << " cm</text>\n"
      << "<path d=\"M " << left << ' ' << top << " V " << plotHeight - bottom
      << " H " << plotWidth - right << "\" fill=\"none\" stroke=\"#222\"/>\n";
  for (int tick = 0; tick <= 5; ++tick) {
    const G4double y = top + (plotHeight - top - bottom) * tick / 5.0;
    const G4double value = maximum * (5 - tick) / 5.0;
    svg << "<path d=\"M " << left << ' ' << y << " H " << plotWidth - right
        << "\" stroke=\"#ddd\"/>\n"
        << "<text x=\"" << left - 10 << "\" y=\"" << y + 4
        << "\" text-anchor=\"end\" font-size=\"11\">" << std::setprecision(3)
        << value << "</text>\n";
  }
  const G4double plotUsableWidth = plotWidth - left - right;
  const G4double plotUsableHeight = plotHeight - top - bottom;
  svg << "<path fill=\"none\" stroke=\"#1769aa\" stroke-width=\"1.8\" d=\"";
  for (std::size_t bin = 0; bin < counts.size(); ++bin) {
    const G4double energyKeV = (bin + 1.0) * spectrum.GetBinWidth() / keV;
    const G4double fluence = counts[bin] * scale;
    spec << energyKeV << ' ' << fluence << '\n';
    const G4double xLeft = left + plotUsableWidth * bin *
        (spectrum.GetBinWidth() / keV) / (spectrum.GetMaximumEnergy() / keV);
    const G4double xRight = left + plotUsableWidth * energyKeV /
        (spectrum.GetMaximumEnergy() / keV);
    const G4double y = top + plotUsableHeight * (1.0 - fluence / maximum);
    if (bin == 0) {
      svg << "M " << xLeft << ' ' << y << ' ';
    }
    else {
      svg << "V " << y << ' ';
    }
    svg << "H " << xRight << ' ';
  }
  svg << "\"/>\n"
      << "<text x=\"" << (left + plotWidth - right) / 2 << "\" y=\"" << plotHeight - 20
      << "\" text-anchor=\"middle\" font-size=\"13\">Photon energy (keV)</text>\n"
      << "<text transform=\"translate(20 " << (top + plotHeight - bottom) / 2
      << ") rotate(-90)\" text-anchor=\"middle\" font-size=\"13\">Photons / (keV cm&#178; mAs)</text>\n"
      << "</g></svg>\n";
}
}

RunAction::RunAction(const DetectorConstruction* detector)
  : fElectronEnergy(300.0 * keV),
    fDetector(detector),
    fRunMessenger(new RunMessenger(this))
{}

RunAction::~RunAction()
{
  delete fRunMessenger;
}

G4Run* RunAction::GenerateRun()
{
  return new SpectrumRun(fDetector->GetScoringPositions(), fElectronEnergy);
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4cout << "Electron energy: " << G4BestUnit(fElectronEnergy, "Energy") << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  if (!IsMaster()) return;
  const auto* spectrum = static_cast<const SpectrumRun*>(run);
  for (std::size_t index = 0; index < spectrum->GetPositions().size(); ++index) {
    const auto stem = outputStem(spectrum->GetPositions()[index],
                                 fDetector->GetScoringDistance());
    writeSpectrum(stem, *spectrum, index, fDetector->GetScoringDistance(),
                  fElectronEnergy);
    G4cout << "Wrote " << stem << ".spec and " << stem << ".svg" << G4endl;
  }
}
