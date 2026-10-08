#!/usr/bin/env python3
"""Generate an off-axis SpekCalc reference spectrum in MCGPU-SPEC format."""
from __future__ import annotations

import argparse
from pathlib import Path

import spekpy as sp


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("output", type=Path)
    parser.add_argument("--kvp", type=float, default=120.0)
    parser.add_argument("--angle-deg", type=float, default=15.0)
    parser.add_argument("--x-cm", type=float, default=0.0)
    parser.add_argument("--y-cm", type=float, default=0.0)
    parser.add_argument("--z-cm", type=float, default=100.0)
    parser.add_argument("--bin-width-kev", type=float, default=1.0)
    args = parser.parse_args()
    if args.kvp <= 0 or args.z_cm <= 0 or args.bin_width_kev <= 0:
        parser.error("kVp, z, and bin width must be positive")

    spectrum = sp.Spek(
        kvp=args.kvp,
        th=args.angle_deg,
        dk=args.bin_width_kev,
        mas=1.0,
        x=args.x_cm,
        y=args.y_cm,
        z=args.z_cm,
        physics="spekcalc",
    )
    centers, fluence = spectrum.get_spectrum(edges=False, flu=True, diff=True)
    half_bin = args.bin_width_kev / 2.0
    edges = [float(centers[0]) - half_bin]
    edges.extend(float(center) + half_bin for center in centers)

    lines = [
        "# Format: MCGPU-SPEC 1.0",
        "# SpekPy physics=spekcalc",
        f"# Position: x={args.x_cm:g} cm, y={args.y_cm:g} cm, "
        f"z={args.z_cm:g} cm; 1 mAs",
        f"# {args.kvp:g} kVp W target, {args.angle_deg:g} deg, no filtration",
        "# Energy[keV]  N[keV cm^2 mAs]^-1",
    ]
    lines.extend(
        f"{energy:.9g} {float(value):.9g}"
        for energy, value in zip(edges[:-1], fluence)
    )
    lines.append(f"{edges[-1]:.9g} 0")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text("\n".join(lines) + "\n", encoding="ascii")
    print(f"wrote {args.output}")
    print(f"mean_energy_keV={spectrum.get_emean():.9g}")
    print(f"air_kerma_uGy_per_mAs={spectrum.get_kerma(norm=True, to='air'):.9g}")


if __name__ == "__main__":
    main()
