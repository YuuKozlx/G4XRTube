#!/usr/bin/env python3
"""Plot a weighted G4XRTube photon spectrum from a ROOT file."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import uproot


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("root_file", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    with uproot.open(args.root_file) as root_file:
        histogram = root_file["h0"]
        values, edges = histogram.to_numpy()
        spectrum = root_file["spectrum"].arrays(
            ["energy_keV", "weight"], library="np"
        )

    energies = np.asarray(spectrum["energy_keV"], dtype=float)
    weights = np.asarray(spectrum["weight"], dtype=float)
    weight_sum = float(weights.sum())
    mean_energy = float(np.dot(energies, weights) / weight_sum)

    centers = 0.5 * (edges[:-1] + edges[1:])
    peak_index = int(np.argmax(values))
    peak_energy = float(centers[peak_index])

    plt.style.use("seaborn-v0_8-whitegrid")
    fig, ax = plt.subplots(figsize=(10, 5.8), dpi=160)
    ax.step(centers, values, where="mid", color="#146c94", linewidth=1.8)
    ax.fill_between(centers, values, step="mid", color="#72b7d5", alpha=0.24)
    ax.axvline(mean_energy, color="#d97706", linestyle="--", linewidth=1.2,
               label=f"Weighted mean: {mean_energy:.2f} keV")
    ax.scatter([peak_energy], [values[peak_index]], color="#b42318", s=28,
               zorder=3, label=f"Peak bin: {peak_energy:.1f} keV")

    ax.set_title("G4XRTube H-100 X-ray Spectrum")
    ax.set_xlabel("Photon energy (keV)")
    ax.set_ylabel("Weighted photon count")
    ax.set_xlim(float(edges[0]), float(edges[-1]))
    ax.set_ylim(bottom=0)
    ax.legend(frameon=True)
    ax.text(
        0.99,
        0.97,
        f"Weighted photons: {weight_sum:.3f}\n"
        f"Recorded photons: {len(energies):,}",
        transform=ax.transAxes,
        ha="right",
        va="top",
        fontsize=9,
        bbox={"boxstyle": "round,pad=0.35", "facecolor": "white", "alpha": 0.85},
    )
    fig.tight_layout()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(args.output, bbox_inches="tight")
    print(f"wrote {args.output}")
    print(f"mean_energy_keV={mean_energy:.6f}")
    print(f"peak_bin_keV={peak_energy:.6f}")
    print(f"weighted_photons={weight_sum:.6f}")


if __name__ == "__main__":
    main()
