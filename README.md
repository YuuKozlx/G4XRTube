# G4XRTube

G4XRTube is a Geant4 application for Monte Carlo simulation of conventional
kilovoltage X-ray tubes. It transports incident electrons in an anode,
propagates the generated bremsstrahlung and characteristic photons through
inherent and additional filters, and scores the photon energy and position at
a downstream plane. The output supports studies of X-ray spectra, beam
quality, and spatial beam distributions, including the anode heel effect.

This repository contains the updated application used for the accompanying
beam-quality and spatial-distribution study. The present release was prepared
for Geant4 11.3.x.

## Main features

- configurable target material, anode angle, and filtration through Geant4
  macro commands;
- selectable Geant4 electromagnetic physics constructors;
- configurable incident-electron energy;
- bremsstrahlung splitting through Geant4 secondary biasing;
- multithreaded execution when Geant4 is built with multithreading;
- weighted photon spectra, two-dimensional spatial maps, and axial profiles;
- ROOT histogram and ntuple output;
- optional PyQt6 launcher for generating macros and starting simulations.

Atomic de-excitation controls are exposed before `/run/initialize`:

```text
/phys/setFluorescence true
/phys/setAuger true
/phys/setAugerCascade true
/phys/setPIXE false
```

The defaults are fluorescence on, Auger on, Auger cascade on, and PIXE off.
Set `/phys/setPIXE true` explicitly when modeling particle-induced X-ray
emission from charged-particle shell ionization.

## Configurable spectrum scoring distance

The circular spectrum scoring plane is 50 cm from the target by default. Set
its target-to-plane distance before `/run/initialize` with:

```text
/XRtube/det/setScoringDistance 100 cm
/XRtube/det/setScoringOffsetX 2 cm
/XRtube/det/setScoringOffsetY -1 cm
```

Any positive Geant4 length and unit are accepted (for example `500 mm`,
`100 cm`, or `1 m`). The world volume expands automatically when a distance
or transverse offset exceeds its default extent. Offsets may be positive or
negative and default to zero. The scoring plane remains parallel to XY, with
its center at `(offsetX, offsetY, -distance)`. Each run scores photons at the
configured physical plane; the result is not produced by post-processing an
inverse-square scaling of a spectrum recorded at another distance.

## Requirements

Required:

- Geant4 11.3.x with ROOT analysis support, multithreading, UI, and
  visualization enabled;
- CMake 3.16 or newer;
- a C++17 compiler.

Optional:

- ROOT, to run `plot.C` and create the example plots;
- Python 3 and PyQt6, to use `geant4_gui_launcher.py`.

Before configuring the project, load the Geant4 environment supplied by your
installation. For example:

```bash
source /path/to/geant4-install/bin/geant4.sh
```

## Build

From the repository root:

```bash
cmake -S . -B build
cmake --build build -j
```

If CMake does not locate Geant4 automatically, provide its package directory:

```bash
cmake -S . -B build -DGeant4_DIR=/path/to/Geant4/lib/cmake/Geant4
cmake --build build -j
```

CMake copies the example macros, visualization files, ROOT analysis macro, and
Python launcher into the build directory.

## Run the application

### Batch mode

Run one of the supplied macro files from the build directory:

```bash
cd build
./G4XRTube H100.mac
```

The other examples are run in the same way:

```bash
./G4XRTube L55.mac
./G4XRTube N100.mac
./G4XRTube W150.mac
```

### Interactive mode

Start the executable without a macro argument:

```bash
cd build
./G4XRTube
```

This opens the Geant4 UI and loads `init_vis.mac`, `vis.mac`, and `gui.mac`.

### Optional PyQt6 launcher

Install PyQt6 in the active Python environment and start:

```bash
cd build
python3 geant4_gui_launcher.py
```

The launcher can create a macro from a radiation-quality preset or from custom
geometry, physics, filtration, biasing, and run settings. The generated macro
is passed directly to the `G4XRTube` executable.

## Supplied configurations

The event counts below are retained from the supplied study macros.

| Macro | Electron energy | Anode angle | Inherent filter | Additional filter | Primary electrons |
|---|---:|---:|---|---|---:|
| `L55.mac` | 55 keV | 20° | 4 mm Al | 1.2 mm Cu | 100,000,000 |
| `H100.mac` | 100 keV | 15° | 4 mm Al | 0.15 mm Cu | 5,000,000 |
| `N100.mac` | 100 keV | 20° | 4 mm Al | 5 mm Cu | 10,000,000 |
| `W150.mac` | 150 keV | 15° | 4 mm Al | 1.0 mm Sn | 100,000,000 |

All four examples use a tungsten target, `G4EmStandardPhysics_option4`, a
5 µm production range cut, and an electron-bremsstrahlung splitting factor of
200. To select the number of worker threads, uncomment and edit
`/run/numberOfThreads` near the beginning of a macro.

## Macro commands

The application adds the following commands:

| Command | Purpose | Example |
|---|---|---|
| `/XRtube/det/setTargetMaterial` | Set the anode material | `G4_W` |
| `/XRtube/det/setAnodeAngle` | Set the anode angle | `15 deg` |
| `/XRtube/det/setInherentFilterMaterial` | Set inherent-filter material | `G4_Al` |
| `/XRtube/det/setInherentFilterThickness` | Set inherent-filter thickness | `4 mm` |
| `/XRtube/det/setFilterMaterial` | Set added-filter material | `G4_Cu` |
| `/XRtube/det/setFilterThickness` | Set added-filter thickness | `0.15 mm` |
| `/phys/SelectPhysicsList` | Select the electromagnetic constructor | `standard_option4` |
| `/phys/setCuts` | Set the common production range cut | `5 um` |
| `/phys/setFluorescence` | Enable or disable fluorescence photons | `true` |
| `/phys/setAuger` | Enable or disable Auger electrons | `true` |
| `/phys/setAugerCascade` | Enable or disable the Auger cascade | `true` |
| `/phys/setPIXE` | Enable or disable PIXE atomic de-excitation | `false` |
| `/XRtube/det/setScoringDistance` | Set target-to-scoring-plane distance | `100 cm` |
| `/XRtube/det/setScoringOffsetX` | Set scoring-plane X offset | `2 cm` |
| `/XRtube/det/setScoringOffsetY` | Set scoring-plane Y offset | `-1 cm` |
| `/xraytube/setEnergy` | Set the incident-electron energy | `100 keV` |

Supported electromagnetic selections are `standard`, `standard_option4`,
`livermore`, `penelope`, and `LowEP`.

Geometry and physics commands are placed before the first `/run/initialize`.
The source-energy command is placed after initialization, when the
primary-generator action exists. The supplied macros then call
`/run/initialize` again before `/run/beamOn`, following the run sequence used
for the study configurations.

## Geometry and scoring

The primary source emits electrons toward the tungsten anode from a square
1 mm × 1 mm focal-spot region. The inherent and additional filters are placed
between the anode and a circular scoring plane of 120 mm radius located 50 cm
from the coordinate origin. The anode--cathode direction is the Y axis.

Photons entering the scoring plane are recorded with their statistical weight.
The application writes `spectra.root` with these objects:

| Object | Type | Content |
|---|---|---|
| `h0` | TH1 | weighted photon-energy spectrum in keV |
| `hXY` | TH2 | weighted photon-count map in the X–Y plane |
| `hX` | TH1 | weighted X profile |
| `hY` | TH1 | weighted Y profile |
| `hXY_Energy` | TH2 | energy-weighted spatial map in keV |
| `spectrum` | ntuple | photon energy, X, Y, and statistical weight |

The histogram weights account for secondary biasing. The ntuple stores the
weight explicitly so that alternative post-processing can reproduce the
weighted distributions.

## ROOT plots

After a simulation has produced `spectra.root`, run:

```bash
root -l -q plot.C
```

The macro creates:

- `spectrum.pdf`;
- `spatial_maps.pdf`;
- `spatial_profiles.pdf`.

## Reproducible random seeds

By default, the application initializes the random engine from the system
time. For an exactly repeatable run, add fixed seeds to the macro before
`/run/beamOn`, for example:

```text
/random/setSeeds 1234567 7654321
```

Record the Geant4 version, macro file, seed pair, thread count, and compiler
configuration together with each reported result.

## Repository structure

```text
G4XRTube/
├── CMakeLists.txt
├── main.cc
├── include/                 C++ headers
├── src/                     C++ implementation
├── H100.mac                 example simulation macros
├── L55.mac
├── N100.mac
├── W150.mac
├── init_vis.mac             interactive initialization
├── vis.mac                  OpenGL visualization
├── gui.mac                  Geant4 Qt menu entries
├── geant4_gui_launcher.py   optional PyQt6 launcher
└── plot.C                   optional ROOT plotting macro
```

## Citation

When using G4XRTube, cite the accompanying article and the original application
paper:

> A. Idrissi, I. Dourki, T. El Bardouni, O. El Hajjaji,
> M. Drissi El-Bouzaidi, and M. Mira, “G4XRTube: A Geant4-based Monte Carlo
> application for X-ray tube simulation,” *Radiation Physics and Chemistry*,
> 2023, 110864. <https://doi.org/10.1016/j.radphyschem.2023.110864>

Geant4 must also be cited in accordance with the Geant4 collaboration's
publication and license guidance.

## License and attribution

Parts of the application are derived from Geant4 example code and remain
subject to the Geant4 Software License:
<https://geant4.web.cern.ch/download/license.html>.

This product includes software developed by Members of the Geant4
Collaboration (<http://cern.ch/geant4>).

Before publishing this repository, add the license selected by the copyright
holder for the original G4XRTube components and retain all applicable upstream
notices.
