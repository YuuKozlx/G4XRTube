#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TROOT.h>
#include <TStyle.h>

#include <iostream>

namespace {

TH1D* normalizedProfile(const TH1D* source, const char* name)
{
  auto* result = static_cast<TH1D*>(source->Clone(name));
  result->SetDirectory(nullptr);
  const double maximum = result->GetMaximum();
  if (maximum > 0.0) {
    result->Scale(1.0 / maximum);
  }
  return result;
}

TH2D* normalizedMap(const TH2D* source, const char* name)
{
  auto* result = static_cast<TH2D*>(source->Clone(name));
  result->SetDirectory(nullptr);
  const double maximum = result->GetMaximum();
  if (maximum > 0.0) {
    result->Scale(1.0 / maximum);
    result->SetMinimum(1.0e-4);
    result->SetMaximum(1.0);
  }
  return result;
}

void configureProfile(TH1D* histogram, Color_t color, Style_t style)
{
  histogram->SetLineColor(color);
  histogram->SetLineStyle(style);
  histogram->SetLineWidth(2);
  histogram->GetYaxis()->SetTitle("Normalized value");
  histogram->GetYaxis()->SetRangeUser(0.0, 1.08);
}

}  // namespace

void plot(const char* inputFile = "spectra.root")
{
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);

  TFile file(inputFile, "READ");
  if (file.IsZombie()) {
    std::cerr << "Cannot open " << inputFile << std::endl;
    return;
  }

  auto* spectrum = dynamic_cast<TH1D*>(file.Get("h0"));
  auto* countMap = dynamic_cast<TH2D*>(file.Get("hXY"));
  auto* energyMap = dynamic_cast<TH2D*>(file.Get("hXY_Energy"));

  if (!spectrum || !countMap || !energyMap) {
    std::cerr << "The input file does not contain h0, hXY, and hXY_Energy."
              << std::endl;
    return;
  }

  // Spectrum
  auto* spectrumNormalized = static_cast<TH1D*>(spectrum->Clone("spectrumNormalized"));
  spectrumNormalized->SetDirectory(nullptr);
  const double spectrumIntegral = spectrumNormalized->Integral();
  if (spectrumIntegral > 0.0) {
    spectrumNormalized->Scale(1.0 / spectrumIntegral);
  }

  auto* spectrumCanvas = new TCanvas("spectrumCanvas", "Spectrum", 900, 650);
  spectrumCanvas->SetGrid();
  spectrumNormalized->SetTitle("Normalized X-ray photon spectrum");
  spectrumNormalized->GetXaxis()->SetTitle("Photon energy (keV)");
  spectrumNormalized->GetYaxis()->SetTitle("Normalized photon fluence per bin");
  spectrumNormalized->SetLineColor(kBlue + 1);
  spectrumNormalized->SetLineWidth(2);
  spectrumNormalized->Draw("HIST");

  auto* spectrumText = new TPaveText(0.66, 0.76, 0.89, 0.89, "NDC");
  spectrumText->SetFillColor(0);
  spectrumText->SetTextAlign(12);
  spectrumText->AddText(Form("Mean energy: %.2f keV", spectrum->GetMean()));
  spectrumText->AddText(Form("RMS: %.2f keV", spectrum->GetRMS()));
  spectrumText->Draw();
  spectrumCanvas->SaveAs("spectrum.pdf");

  // Two-dimensional photon-count and energy-weighted maps
  auto* countMapNormalized = normalizedMap(countMap, "countMapNormalized");
  auto* energyMapNormalized = normalizedMap(energyMap, "energyMapNormalized");

  auto* mapsCanvas = new TCanvas("mapsCanvas", "Spatial maps", 1300, 560);
  mapsCanvas->Divide(2, 1);

  mapsCanvas->cd(1);
  gPad->SetRightMargin(0.15);
  gPad->SetLogz();
  countMapNormalized->SetTitle("Normalized photon-count distribution");
  countMapNormalized->GetXaxis()->SetTitle("X (mm)");
  countMapNormalized->GetYaxis()->SetTitle("Y (mm)");
  countMapNormalized->GetZaxis()->SetTitle("Normalized photon count");
  countMapNormalized->Draw("COLZ");

  mapsCanvas->cd(2);
  gPad->SetRightMargin(0.15);
  gPad->SetLogz();
  energyMapNormalized->SetTitle("Normalized energy-weighted distribution");
  energyMapNormalized->GetXaxis()->SetTitle("X (mm)");
  energyMapNormalized->GetYaxis()->SetTitle("Y (mm)");
  energyMapNormalized->GetZaxis()->SetTitle("Normalized energy-weighted fluence");
  energyMapNormalized->Draw("COLZ");

  mapsCanvas->SaveAs("spatial_maps.pdf");

  // X and Y profiles for both scored quantities
  auto* countX = countMap->ProjectionX("countX");
  auto* countY = countMap->ProjectionY("countY");
  auto* energyX = energyMap->ProjectionX("energyX");
  auto* energyY = energyMap->ProjectionY("energyY");

  auto* countXNormalized = normalizedProfile(countX, "countXNormalized");
  auto* countYNormalized = normalizedProfile(countY, "countYNormalized");
  auto* energyXNormalized = normalizedProfile(energyX, "energyXNormalized");
  auto* energyYNormalized = normalizedProfile(energyY, "energyYNormalized");

  configureProfile(countXNormalized, kBlue + 1, 1);
  configureProfile(energyXNormalized, kRed + 1, 2);
  configureProfile(countYNormalized, kBlue + 1, 1);
  configureProfile(energyYNormalized, kRed + 1, 2);

  auto* profilesCanvas = new TCanvas("profilesCanvas", "Spatial profiles", 1300, 560);
  profilesCanvas->Divide(2, 1);

  profilesCanvas->cd(1);
  gPad->SetGrid();
  countXNormalized->SetTitle("X profile");
  countXNormalized->GetXaxis()->SetTitle("X (mm)");
  countXNormalized->Draw("HIST");
  energyXNormalized->Draw("HIST SAME");
  auto* legendX = new TLegend(0.65, 0.78, 0.89, 0.89);
  legendX->AddEntry(countXNormalized, "Photon count", "l");
  legendX->AddEntry(energyXNormalized, "Energy weighted", "l");
  legendX->Draw();

  profilesCanvas->cd(2);
  gPad->SetGrid();
  countYNormalized->SetTitle("Y profile (anode--cathode axis)");
  countYNormalized->GetXaxis()->SetTitle("Y (mm)");
  countYNormalized->Draw("HIST");
  energyYNormalized->Draw("HIST SAME");
  auto* legendY = new TLegend(0.65, 0.78, 0.89, 0.89);
  legendY->AddEntry(countYNormalized, "Photon count", "l");
  legendY->AddEntry(energyYNormalized, "Energy weighted", "l");
  legendY->Draw();

  profilesCanvas->SaveAs("spatial_profiles.pdf");

  std::cout << "Mean photon energy: " << spectrum->GetMean() << " keV\n"
            << "Photon-count centroid: (" << countMap->GetMean(1) << ", "
            << countMap->GetMean(2) << ") mm\n"
            << "Created spectrum.pdf, spatial_maps.pdf, and spatial_profiles.pdf"
            << std::endl;
}
