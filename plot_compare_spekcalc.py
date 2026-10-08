#!/usr/bin/env python3
"""Convert a G4XRTube ROOT spectrum to SpekCalc units and compare it."""
from __future__ import annotations

import argparse
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt
import uproot

ELECTRONS_PER_MAS = 6.241509074e15
SCORING_RADIUS_CM = 12.0
SCORING_AREA_CM2 = np.pi * SCORING_RADIUS_CM**2


def read_spec(path: Path) -> tuple[np.ndarray, np.ndarray]:
    rows = []
    for line in path.read_text(encoding="ascii").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        e, v = line.split()[:2]
        rows.append((float(e), float(v)))
    data = np.asarray(rows, dtype=float)
    return data[:, 0], data[:, 1]


def main() -> None:
    p = argparse.ArgumentParser()
    p.add_argument("root_file", type=Path)
    p.add_argument("output", type=Path)
    p.add_argument("--reference", type=Path)
    p.add_argument("--electrons", type=float, default=5_000_000)
    p.add_argument("--scoring-distance-cm", type=float, required=True)
    p.add_argument("--scoring-offset-x-cm", type=float, default=0.0)
    p.add_argument("--scoring-offset-y-cm", type=float, default=0.0)
    args = p.parse_args()
    if args.electrons <= 0 or args.scoring_distance_cm <= 0:
        p.error("--electrons and --scoring-distance-cm must be positive")

    with uproot.open(args.root_file) as f:
        h = f["h0"]
        counts, edges = h.to_numpy()
    widths = np.diff(edges)
    centers = edges[:-1] + widths / 2.0
    # Weighted photon count / simulated electron, converted to 1 mAs and
    # from the 50 cm scoring plane to SpekCalc's 100 cm reference distance.
    g4 = counts / args.electrons * ELECTRONS_PER_MAS
    g4 /= SCORING_AREA_CM2 * widths
    g4_integral = float(np.sum(g4 * widths))
    g4_mean = float(np.sum(g4 * widths * centers) / g4_integral)

    ref_e = ref_v = None
    ref_integral = ref_mean = None
    if args.reference:
        ref_e, ref_v = read_spec(args.reference)
        ref_widths = np.diff(ref_e)
        ref_centers = ref_e[:-1] + ref_widths / 2.0
        ref_integral = float(np.sum(ref_v[:-1] * ref_widths))
        ref_mean = float(
            np.sum(ref_v[:-1] * ref_widths * ref_centers) / ref_integral
        )

    plt.style.use("seaborn-v0_8-whitegrid")
    fig, ax = plt.subplots(figsize=(10, 5.8), dpi=160)
    ax.step(centers, g4, where="mid", color="#146c94", lw=1.7,
            label="G4XRTube (Geant4, 120 kVp, no filtration)")
    if ref_e is not None:
        ax.step(ref_e, ref_v, where="post", color="#b42318", lw=1.5,
                alpha=0.85, label="SpekCalc model reference")
    ax.set(title="120 kVp W-target spectrum: G4XRTube vs SpekCalc",
           xlabel="Photon energy (keV)",
           ylabel=r"Photons / (keV cm$^2$ mAs) at scoring position",
           xlim=(0, 120))
    ax.set_ylim(bottom=0)
    ax.legend(frameon=True)
    ax.text(0.99, 0.97,
            f"G4 normalization: {args.electrons:,.0f} electrons\n"
            f"Scoring disk: radius={SCORING_RADIUS_CM:g} cm\n"
            f"Center: x={args.scoring_offset_x_cm:g}, "
            f"y={args.scoring_offset_y_cm:g}, "
            f"z={args.scoring_distance_cm:g} cm\n"
            "1 mAs = 6.241509e15 electrons",
            transform=ax.transAxes, ha="right", va="top", fontsize=8.5,
            bbox={"boxstyle": "round,pad=0.35", "facecolor": "white", "alpha": 0.85})
    fig.tight_layout()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(args.output, bbox_inches="tight")
    print(f"wrote {args.output}")
    print(f"integrated_g4_photons_per_cm2_mas={g4_integral:.9g}")
    print(f"mean_g4_energy_keV={g4_mean:.9g}")
    print(f"peak_g4_photons_per_kev_cm2_mas={np.max(g4):.9g}")
    if ref_integral is not None and ref_mean is not None:
        print(f"integrated_spekcalc_photons_per_cm2_mas={ref_integral:.9g}")
        print(f"mean_spekcalc_energy_keV={ref_mean:.9g}")


if __name__ == "__main__":
    main()
