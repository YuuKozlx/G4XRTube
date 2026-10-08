# G4XRTube

G4XRTube is a Geant4 application for Monte Carlo simulation of conventional
kilovoltage X-ray tubes. It transports incident electrons in an anode,
propagates bremsstrahlung and characteristic photons through filters, and
scores photon spectra on a downstream reference plane.

This version targets Geant4 11.3.x and writes spectra directly. ROOT is not
required.

## Build

Required dependencies are Geant4 11.3.x, CMake 3.16 or newer, and a C++17
compiler. Geant4 must include multithreading, UI, and visualization support.

```bash
source /path/to/geant4-install/bin/geant4.sh
cmake -S . -B build
cmake --build build -j
```

If needed, pass `-DGeant4_DIR=/path/to/Geant4/lib/cmake/Geant4` to CMake.

## Fixed plane and multiple offsets

`setScoringDistance` defines one physical XY reference plane. Every offset
added afterward is evaluated on that same plane in a single simulation:

```text
/XRtube/det/setScoringDistance 100 cm
/XRtube/det/clearScoringOffsets
/XRtube/det/addScoringOffset 0 0 cm
/XRtube/det/addScoringOffset 2 -1 cm
/XRtube/det/addScoringOffset -2 1 cm
```

Each offset is the center of a circular 12 cm radius sampling region. The
physical scoring disk expands to contain all regions. A photon in overlapping
regions contributes independently to each corresponding spectrum. The result
is measured at the configured plane, not inverse-square rescaled afterward.

The old `setScoringOffsetX` and `setScoringOffsetY` commands remain compatible
and modify the first offset. If the list is cleared and no offset is added,
the run produces no spectrum files.

## Output

The example above creates one `.spec` and one `.svg` per offset, for example:

```text
spectrum_x0cm_y0cm_z100cm.spec
spectrum_x0cm_y0cm_z100cm.svg
spectrum_x2cm_ym1cm_z100cm.spec
spectrum_x2cm_ym1cm_z100cm.svg
spectrum_xm2cm_y1cm_z100cm.spec
spectrum_xm2cm_y1cm_z100cm.svg
```

`.spec` uses 1 keV bins and the MCGPU-SPEC text layout. Values are weighted
photons per `(keV cm^2 mAs)`, normalized using `1 mAs = 6.241509074e15`
incident electrons and the 12 cm radius area. No SpekCalc or dose value is
used to force-align the result. SVG plots are generated directly in C++.

## Physics controls

Set atomic de-excitation controls before `/run/initialize`:

```text
/phys/setFluorescence true
/phys/setAuger true
/phys/setAugerCascade true
/phys/setPIXE false
```

Defaults are fluorescence on, Auger on, Auger cascade on, and PIXE off.
Supported electromagnetic selections are `standard`, `standard_option4`,
`livermore`, `penelope`, and `LowEP`.

## Run

From the build directory:

```bash
./G4XRTube W120_nofilter.mac
```

The supplied `W120_nofilter_distance_smoke.mac` demonstrates three offsets on
one 100 cm plane. The optional `geant4_gui_launcher.py` requires Python 3 and
PyQt6. Set fixed random seeds before `/run/beamOn` for repeatable runs.

## Citation

When using G4XRTube, cite the original G4XRTube publication:

> A. Idrissi et al., "G4XRTube: A Geant4-based Monte Carlo application for
> X-ray tube simulation," *Radiation Physics and Chemistry*, 2023, 110864.

Parts of the application are derived from Geant4 example code and remain
subject to the Geant4 Software License.
