import numpy as np
import os
from math import sqrt, pi, cos, sin
import random

# ----------------------------------
# Utility: Write dataset in your format
# ----------------------------------
def save_dataset(folder, name, points):
    os.makedirs(folder, exist_ok=True)
    path = os.path.join(folder, f"{name}.txt")
    with open(path, "w") as f:
        f.write(f"{len(points)}\n")
        for x, y in points:
            f.write(f"{x:.6f} {y:.6f}\n")
    print(f"✔ Saved: {path}")


# ----------------------------------
# Dataset Generators
# ----------------------------------

def uniform(n):
    return np.random.rand(n, 2) * 100


def clusters(n, k=5):
    centers = np.random.rand(k, 2) * 100
    pts = []
    for _ in range(n):
        cx, cy = centers[np.random.randint(0, k)]
        pts.append([np.random.normal(cx, 7), np.random.normal(cy, 7)])
    return np.array(pts)


def grid(n):
    side = int(sqrt(n))
    xs, ys = np.meshgrid(np.linspace(0, 100, side), np.linspace(0, 100, side))
    pts = np.column_stack((xs.ravel(), ys.ravel()))
    pts = pts[:n]
    pts += np.random.randn(*pts.shape) * 0.2  # tiny noise avoid degeneracy
    return pts


def circle(n):
    theta = np.random.rand(n) * 2 * pi
    r = 40 + np.random.randn(n) * 2
    return np.column_stack((50 + r * np.cos(theta), 50 + r * np.sin(theta)))


def annulus(n):
    theta = np.random.rand(n) * 2 * pi
    r = np.random.uniform(30, 50, n)
    return np.column_stack((50 + r*np.cos(theta), 50+r*np.sin(theta)))


def adversarial(n):
    x = np.linspace(0, 100, n)
    y = 10 * np.sin(x / 8) + np.random.randn(n) * 0.5
    return np.column_stack((x, y))


def adversarial_with_noise(n):
    base_n = int(n * 0.7)
    noise_n = n - base_n
    base = adversarial(base_n)
    noise = np.random.rand(noise_n, 2) * 100
    return np.vstack((base, noise))


def poisson_disk(n, radius=5):
    pts = []
    attempts = 0
    while len(pts) < n and attempts < n * 50:
        p = np.random.rand(2) * 100
        if all(np.linalg.norm(p - q) > radius for q in pts):
            pts.append(p)
        attempts += 1
    while len(pts) < n:  # fallback: fill randomly if too dense
        pts.append(np.random.rand(2) * 100)
    return np.array(pts)


# ----------------------------------
# Main generation loop
# ----------------------------------

sizes = [50, 200, 500, 1000, 3000]

generators = {
    "uniform": uniform,
    "clusters": clusters,
    "grid": grid,
    "circle": circle,
    "annulus": annulus,
    "poisson": poisson_disk,
    "adversarial": adversarial,
    "adv_noise": adversarial_with_noise,
}

print("\n Generating dataset suite for Minimum Weight Triangulation benchmarking...\n")

for size in sizes:
    for name, fn in generators.items():
        pts = fn(size)
        save_dataset(f"datasets/{name}", f"{name}_{size}", pts)

print("\n All datasets generated successfully!\n")
