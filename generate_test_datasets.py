import numpy as np
import os
from math import sqrt, pi, cos, sin
import random

def save_dataset(folder, name, points):
    os.makedirs(folder, exist_ok=True)
    path = os.path.join(folder, f"{name}.pnt")
    with open(path, "w") as f:
        f.write(f"{len(points)}\n")
        for x, y in points:
            f.write(f"{x:.6f} {y:.6f}\n")
    print(f"Saved: {path}")

#  Dataset Generators (10 Types for Testing all 3 Methods and Comparing)
def convex_polygon(n):
    theta = np.linspace(0, 2*pi, n, endpoint=False)
    r = 50
    return np.column_stack((50 + r*np.cos(theta), 50 + r*np.sin(theta)))

def near_convex_polygon(n, noise=0.5):
    pts = convex_polygon(n)
    return pts + np.random.randn(*pts.shape) * noise

def fractal_boundary(n):
    # Rough Koch-like perturbation on circle
    pts = convex_polygon(n)
    wiggle = np.random.randn(n, 2) * (np.sin(np.linspace(0, 10*pi, n))[:, None] * 2)
    return pts + wiggle

def gaussian_cluster(n):
    mean = np.array([50, 50])
    cov = np.array([[150, 0], [0, 150]])
    return np.random.multivariate_normal(mean, cov/cov.max(), n) * 100

def hierarchical_clusters(n):
    cluster_sizes = np.random.multinomial(n, [0.6, 0.3, 0.1])
    centers = [(30, 30), (70, 70), (50, 90)]
    pts = []
    for size, (cx, cy) in zip(cluster_sizes, centers):
        pts.append(np.column_stack([np.random.normal(cx, 5, size), np.random.normal(cy, 5, size)]))
    return np.vstack(pts)

def line_with_noise(n):
    x = np.linspace(0, 100, n)
    y = 50 + np.random.randn(n) * 0.5
    return np.column_stack((x, y))

def latin_hypercube(n):
    pts = np.column_stack((np.random.permutation(n), np.random.permutation(n)))
    pts = pts.astype(float)
    pts /= n
    return pts * 100

def concentric_rings(n):
    k = max(2, n // 3)
    theta = np.random.rand(n) * 2*pi
    r = np.where(np.arange(n) < k, 30 + np.random.randn(n)*2, 50 + np.random.randn(n)*2)
    return np.column_stack((50 + r*np.cos(theta), 50 + r*np.sin(theta)))

def spiral(n):
    theta = np.linspace(0, 6*pi, n)
    r = np.linspace(5, 50, n) + np.random.randn(n)
    return np.column_stack((50 + r * np.cos(theta), 50 + r * np.sin(theta)))

def zigzag_adversarial(n):
    x = np.linspace(0, 100, n)
    y = 50 + 20 * ((np.arange(n) % 2) * 2 - 1) + np.random.randn(n)*0.5
    return np.column_stack((x, y))

# Registry
generators = {
    "convex": convex_polygon,
    "near_convex": near_convex_polygon,
    "fractal": fractal_boundary,
    "gaussian": gaussian_cluster,
    "hierarchical": hierarchical_clusters,
    "line_noise": line_with_noise,
    "latin_hypercube": latin_hypercube,
    "rings": concentric_rings,
    "spiral": spiral,
    "zigzag": zigzag_adversarial,
}

# Main Generation Loop
sizes = [25, 50, 200, 500, 1000, 2000, 3000]

print("\nGenerating MWT Benchmark Datasets...\n")

for size in sizes:
    for name, fn in generators.items():
        pts = fn(size)
        save_dataset(f"final_test_datasets/{name}", f"{name}_{size}", pts)

print("\n All datasets generated successfully!\n")
