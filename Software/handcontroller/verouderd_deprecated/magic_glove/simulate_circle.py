import math
import random

def analyze_stroke_for_circle(points, min_stroke_length=20, min_angular=5.2, max_radius_ratio=0.45, min_radius=2.0):
    n = len(points)
    if n < min_stroke_length:
        return False, {}

    # centroid
    cx = sum(p[0] for p in points) / n
    cy = sum(p[1] for p in points) / n

    radii = []
    angles = []
    for (x, y) in points:
        dx = x - cx
        dy = y - cy
        r = math.hypot(dx, dy)
        radii.append(r)
        angles.append(math.atan2(dy, dx))

    r_mean = sum(radii) / n
    r_var = sum((r - r_mean) ** 2 for r in radii) / n
    r_std = math.sqrt(max(0.0, r_var))
    radius_ratio = (r_std / r_mean) if r_mean > 0 else float('inf')

    # unwrap angles and compute total angular traversal
    total_angular = 0.0
    prev = angles[0]
    for i in range(1, n):
        a = angles[i]
        # unwrap
        while (a - prev) > math.pi:
            a -= 2.0 * math.pi
        while (a - prev) < -math.pi:
            a += 2.0 * math.pi
        total_angular += abs(a - prev)
        prev = a

    metrics = {
        'stroke_length': n,
        'mean_radius': r_mean,
        'std_radius': r_std,
        'radius_ratio': radius_ratio,
        'total_angular_rad': total_angular
    }

    angular_ok = (total_angular >= min_angular)
    radius_ok = (r_mean >= min_radius) and (radius_ratio <= max_radius_ratio)

    return (angular_ok and radius_ok), metrics


# Generators for synthetic strokes

def generate_circle(radius=20.0, revolutions=1.0, points=200, noise=0.5, radius_jitter=0.5, partial=0.0):
    # radius in same units (degrees-like). partial is fraction of a full revolution to remove (0..1)
    pts = []
    angle_start = 0.0
    angle_end = 2.0 * math.pi * revolutions * (1.0 - partial)
    for i in range(points):
        t = i / float(points - 1)
        a = angle_start + t * (angle_end - angle_start)
        r = radius + random.uniform(-radius_jitter, radius_jitter)
        x = r * math.cos(a) + random.uniform(-noise, noise)
        y = r * math.sin(a) + random.uniform(-noise, noise)
        pts.append((x, y))
    return pts


def generate_line(length=40.0, points=200, noise=0.5):
    pts = []
    for i in range(points):
        t = i / float(points - 1)
        x = (t - 0.5) * length + random.uniform(-noise, noise)
        y = random.uniform(-noise, noise)
        pts.append((x, y))
    return pts


def generate_arc(radius=20.0, arc_fraction=0.5, points=200, noise=0.5):
    return generate_circle(radius=radius, revolutions=arc_fraction, points=points, noise=noise)


def generate_noisy(points=200, spread=50.0):
    return [(random.uniform(-spread, spread), random.uniform(-spread, spread)) for _ in range(points)]


if __name__ == '__main__':
    random.seed(12345)

    cases = [
        ('Full circle clean', generate_circle(radius=25.0, points=300, noise=0.2)),
        ('Full circle noisy', generate_circle(radius=25.0, points=300, noise=1.0, radius_jitter=1.5)),
        ('Partial arc', generate_arc(radius=25.0, arc_fraction=0.6, points=300, noise=0.5)),
        ('Straight line', generate_line(length=80.0, points=300, noise=0.5)),
        ('Random noise', generate_noisy(points=300, spread=30.0)),
        ('Small circle (too small)', generate_circle(radius=1.0, points=100, noise=0.1)),
    ]

    for name, pts in cases:
        detected, metrics = analyze_stroke_for_circle(pts)
        print(f"Case: {name}")
        print(f"  Detected: {detected}")
        print(f"  stroke_length: {metrics.get('stroke_length')}")
        print(f"  mean_radius: {metrics.get('mean_radius'):.3f}")
        print(f"  std_radius: {metrics.get('std_radius'):.3f}")
        print(f"  radius_ratio: {metrics.get('radius_ratio'):.3f}")
        print(f"  total_angular_rad: {metrics.get('total_angular_rad'):.3f}")
        print('')
