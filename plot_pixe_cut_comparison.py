#!/usr/bin/env python3
"""Plot the W L-line sensitivity to PIXE and production cuts."""
from __future__ import annotations

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import uproot

ROOT = Path(__file__).resolve().parent / "build-container"
ELECTRONS_PER_MAS = 6.241509074e15
AREA_CM2 = np.pi * 12.0**2


def g4_spectrum(name: str, electrons: float) -> tuple[np.ndarray, np.ndarray]:
    with uproot.open(ROOT / name) as root_file:
        values, edges = root_file["h0"].to_numpy()
    centers = (edges[:-1] + edges[1:]) / 2.0
    fluence = values / electrons * ELECTRONS_PER_MAS / AREA_CM2
    return centers, fluence


def reference() -> tuple[np.ndarray, np.ndarray]:
    rows = []
    path = ROOT / "spekcalc_120kVp_x2cm_y-1cm_z100cm.spec"
    for line in path.read_text(encoding="ascii").splitlines():
        if line.strip() and not line.lstrip().startswith("#"):
            rows.append(tuple(map(float, line.split()[:2])))
    data = np.asarray(rows)
    return data[:-1, 0] + np.diff(data[:, 0]) / 2.0, data[:-1, 1]


def main() -> None:
    series = [
        ("SpekCalc", *reference(), "#b42318", 2.0),
        ("G4 PIXE on, cut 5 um", *g4_spectrum(
            "spectra_W120_x2cm_y-1cm_z100cm.root", 5_000_000
        ), "#146c94", 1.8),
        ("G4 PIXE on, cut 1 um", *g4_spectrum(
            "spectra_W120_pixe_on_cut1um_1M.root", 1_000_000
        ), "#d97706", 1.4),
        ("G4 PIXE on, cut 0.1 um", *g4_spectrum(
            "spectra_W120_pixe_on_cut0p1um_1M.root", 1_000_000
        ), "#7656a0", 1.4),
        ("G4 PIXE off, cut 5 um", *g4_spectrum(
            "spectra_W120_pixe_off_cut5um_1M.root", 1_000_000
        ), "#27864b", 1.8),
    ]
    ratios = [1.0, 2.530922, 2.514811, 2.482413, 1.006975]

    plt.style.use("seaborn-v0_8-whitegrid")
    fig, (ax, bx) = plt.subplots(
        1, 2, figsize=(12, 5.4), dpi=160,
        gridspec_kw={"width_ratios": [2.2, 1.0]},
    )
    for label, energy, fluence, color, width in series:
        ax.step(energy, fluence, where="mid", label=label, color=color, lw=width)
    ax.set_xlim(5, 15)
    ax.set_ylim(bottom=0)
    ax.set_xlabel("Photon energy (keV)")
    ax.set_ylabel(r"Photons / (keV cm$^2$ mAs)")
    ax.set_title("Tungsten L-line region")
    ax.legend(frameon=True, fontsize=8)

    labels = ["SpekCalc", "PIXE on\n5 um", "PIXE on\n1 um",
              "PIXE on\n0.1 um", "PIXE off\n5 um"]
    colors = [item[3] for item in series]
    bars = bx.bar(labels, ratios, color=colors, width=0.72)
    bx.axhline(1.0, color="#555555", lw=1.0)
    bx.set_ylabel("7-12 keV fluence / SpekCalc")
    bx.set_title("Integrated L-band ratio")
    bx.set_ylim(0, 2.9)
    bx.tick_params(axis="x", labelsize=8)
    for bar, ratio in zip(bars, ratios):
        bx.text(bar.get_x() + bar.get_width() / 2.0, ratio + 0.06,
                f"{ratio:.2f}x", ha="center", va="bottom", fontsize=8)

    fig.suptitle("120 kVp W target: PIXE and production-cut sensitivity")
    fig.tight_layout()
    output = ROOT / "W120_L_line_pixe_cut_comparison.png"
    fig.savefig(output, bbox_inches="tight")
    print(f"wrote {output}")


if __name__ == "__main__":
    main()
