import os
import json
import cbor2
import pandas as pd

base_dir = "./gesture-classifier-export"  # folder containing 'training/' and 'testing/'
labels_file = os.path.join(base_dir, "info.labels")
datasets = []

# load mapping from filename base (e.g. 'circle.69md0drd') -> label
label_map = {}
if os.path.exists(labels_file):
    try:
        with open(labels_file, "r", encoding="utf-8") as lf:
            info = json.load(lf)
            for ent in info.get("files", []):
                name = ent.get("name")
                lab = None
                if isinstance(ent.get("label"), dict):
                    # label may be {"type":"label","label":"circle"}
                    lab = ent["label"].get("label")
                elif isinstance(ent.get("label"), str):
                    lab = ent.get("label")
                if name:
                    label_map[name] = lab or "unknown"
    except Exception:
        # fall back to empty mapping
        label_map = {}
else:
    print(f"⚠️  Warning: labels file not found at {labels_file}. Labels will be 'unknown'.")

for split in ["training", "testing"]:
    split_dir = os.path.join(base_dir, split)
    if not os.path.isdir(split_dir):
        print(f"⚠️  Skip missing directory: {split_dir}")
        continue

    for f in os.listdir(split_dir):
        if not f.endswith(".cbor"):
            continue

        base = f[:-5]  # remove .cbor
        # normalize base name to match info.labels 'name' entries which omit the '.ingestion...' suffix
        base_key = base.split('.ingestion')[0]
        cbor_path = os.path.join(split_dir, f)

        # determine label from info.labels mapping if available
        label = label_map.get(base_key, "unknown")

        # load raw sample (6-axis IMU)
        try:
            with open(cbor_path, "rb") as cf:
                decoded = cbor2.load(cf)
        except Exception as e:
            print(f"⚠️  Failed to read {cbor_path}: {e}")
            continue

        # decoded can be a dict that contains the actual sensor data under a key
        def find_sensor_matrix(obj):
            # If it's already a list of lists of numbers, return it
            if isinstance(obj, list) and obj:
                # check first element
                first = obj[0]
                if isinstance(first, (list, tuple)) and all(isinstance(x, (int, float)) for x in first):
                    return obj

            # If it's a dict, search values for a candidate
            if isinstance(obj, dict):
                for v in obj.values():
                    # direct candidate
                    if isinstance(v, list) and v:
                        if isinstance(v[0], (list, tuple)) and all(isinstance(x, (int, float)) for x in v[0]):
                            return v
                    # recurse into nested dicts
                    if isinstance(v, dict):
                        candidate = find_sensor_matrix(v)
                        if candidate is not None:
                            return candidate
                    # sometimes payload is bytes containing nested CBOR
                    if isinstance(v, (bytes, bytearray)):
                        try:
                            nested = cbor2.loads(v)
                            candidate = find_sensor_matrix(nested)
                            if candidate is not None:
                                return candidate
                        except Exception:
                            pass

            # nothing found
            return None

        matrix = find_sensor_matrix(decoded)
        if matrix is None:
            print(f"⚠️  No numeric sensor matrix found in {cbor_path}; skipping")
            continue

        # flatten each timestep into a row
        for i, row in enumerate(matrix):
            # if row is not a sequence of numbers, skip
            if not isinstance(row, (list, tuple)):
                continue
            row_dict = {f"axis{j}": (float(val) if isinstance(val, (int, float)) else None) for j, val in enumerate(row)}
            row_dict["label"] = label
            row_dict["filename"] = base
            row_dict["timestep"] = i
            row_dict["split"] = split
            datasets.append(row_dict)

# combine all
if not datasets:
    print("No data found — dataset.csv will not be created.")
else:
    full_df = pd.DataFrame(datasets)
    out_path = "dataset.csv"
    full_df.to_csv(out_path, index=False)
    print("✅ Saved", out_path, "with shape:", full_df.shape)
