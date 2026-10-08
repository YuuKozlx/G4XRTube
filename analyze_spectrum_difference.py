#!/usr/bin/env python3
"""Quantify G4XRTube and SpekCalc spectrum differences by energy and area."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
import uproot

ELECTRONS_PER_MAS = 6.241509074e15


def read_spec(path: Path) -> tuple[np.ndarray, np.ndarray]:
    rows = []
    for line in path.read_text(encoding="ascii").splitlines():
        if line.strip() and not line.lstrip().startswith("#"):
            rows.append(tuple(map(float, line.split()[:2])))
    values = np.asarray(rows)
    return values[:, 0], values[:, 1]


def integral(values: np.ndarray, edges: np.ndarray, lo: float, hi: float) -> float:
    centers = (edges[:-1] + edges[1:]) / 2.0
    mask = (centers >= lo) & (centers < hi)
    return float(np.sum(values[mask] * np.diff(edges)[mask]))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("root_file", type=Path)
    parser.add_argument("reference", type=Path)
    parser.add_argument("--electrons", type=float, default=5_000_000)
    parser.add_argument("--center-x-mm", type=float, default=20.0)
    parser.add_argument("--center-y-mm", type=float, default=-10.0)
    args = parser.parse_args()

    with uproot.open(args.root_file) as root_file:
        counts, edges = root_file["h0"].to_numpy()
        photons = root_file["spectrum"].arrays(
            ["energy_keV", "x_mm", "y_mm", "weight"], library="np"
        )
    g4 = counts / args.electrons * ELECTRONS_PER_MAS / (np.pi * 12.0**2)
    ref_edges, ref_values = read_spec(args.reference)

    ranges = [(0, 7), (7, 12), (12, 55), (55, 70), (70, 121)]
    print("energy_range_keV,g4,spekcalc,g4_over_spekcalc")
    for lo, hi in ranges:
        g4_value = integral(g4, edges, lo, hi)
        ref_value = integral(ref_values[:-1], ref_edges, lo, hi)
        print(f"{lo}-{hi},{g4_value:.9g},{ref_value:.9g},{g4_value/ref_value:.6f}")

    energy = np.asarray(photons["energy_keV"], dtype=float)
    x = np.asarray(photons["x_mm"], dtype=float)
    y = np.asarray(photons["y_mm"], dtype=float)
    weight = np.asarray(photons["weight"], dtype=float)
    radius = np.hypot(x - args.center_x_mm, y - args.center_y_mm)
    print("radius_cm,records,fluence_per_cm2_mas,mean_energy_keV,l_band_7_12")
    for radius_cm in (3.0, 6.0, 12.0):
        selected = radius <= radius_cm * 10.0
        l_selected = selected & (energy >= 7.0) & (energy < 12.0)
        scale = ELECTRONS_PER_MAS / args.electrons / (np.pi * radius_cm**2)
        fluence = (
            float(np.sum(weight[selected]))
            * scale
        )
        l_fluence = float(np.sum(weight[l_selected])) * scale
        mean = float(np.average(energy[selected], weights=weight[selected]))
        print(
            f"{radius_cm:g},{np.count_nonzero(selected)},{fluence:.9g},"
            f"{mean:.9g},{l_fluence:.9g}"
        )


if __name__ == "__main__":
    main()
