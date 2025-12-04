import json
import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
import tensorflow as tf
from tensorflow import keras
from tensorflow.keras import layers

# Load dataset
df = pd.read_csv("dataset.csv")

# Get axis columns
feature_cols = [c for c in df.columns if c.startswith("axis")]
if not feature_cols:
    raise SystemExit("No axis columns found")

df = df.dropna(subset=feature_cols + ["label"]).reset_index(drop=True)
df = df.sort_values(["filename", "timestep"]).reset_index(drop=True)

# Aggregate per recording with fixed window approach
# We'll use a simpler approach: extract statistical features per recording
def extract_features(g):
    arr = g[feature_cols].astype(float).values
    feats = []
    for i in range(len(feature_cols)):
        a = arr[:, i]
        feats.extend([a.mean(), a.std(), a.min(), a.max()])
    return feats

records = []
labels = []
for filename, g in df.groupby("filename"):
    feats = extract_features(g)
    records.append(feats)
    labels.append(g["label"].mode().iloc[0])

X = np.array(records, dtype=np.float32)
y = np.array(labels)

# Encode labels
le = LabelEncoder()
y_enc = le.fit_transform(y)
num_classes = len(le.classes_)

with open("labels.json", "w", encoding="utf-8") as f:
    json.dump(list(le.classes_), f, ensure_ascii=False, indent=2)

# Normalize
mean = X.mean(axis=0)
std = X.std(axis=0)
std[std == 0] = 1.0
X_norm = (X - mean) / std

# Split
X_train, X_test, y_train, y_test = train_test_split(
    X_norm, y_enc, test_size=0.2, random_state=42, stratify=y_enc
)

# Build simple dense model for TFLite - flatten input to avoid EXPAND_DIMS
model = keras.Sequential([
    layers.Dense(32, activation='relu', input_shape=(X_train.shape[1],)),
    layers.Dense(16, activation='relu'),
    layers.Dense(num_classes, activation='softmax')
])

model.compile(
    optimizer='adam',
    loss='sparse_categorical_crossentropy',
    metrics=['accuracy']
)

# Train
model.fit(X_train, y_train, epochs=50, batch_size=8, validation_split=0.2, verbose=1)

# Evaluate
loss, acc = model.evaluate(X_test, y_test)
print(f"\nTest Accuracy: {acc:.4f}")

# Save preprocessing params
np.savez("model_params.npz", mean=mean, std=std, num_features=X.shape[1])

# Convert to TFLite with settings for microcontroller compatibility
converter = tf.lite.TFLiteConverter.from_keras_model(model)
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS]
tflite_model = converter.convert()

with open("gesture_model.tflite", "wb") as f:
    f.write(tflite_model)
print("✅ Saved gesture_model.tflite")

# Convert to C array
def convert_to_c_array(model_bytes, var_name="model_data"):
    lines = [f"const unsigned char {var_name}[] = {{"]
    for i in range(0, len(model_bytes), 12):
        chunk = model_bytes[i:i+12]
        hex_vals = ", ".join(f"0x{b:02x}" for b in chunk)
        lines.append(f"  {hex_vals},")
    lines[-1] = lines[-1].rstrip(",")
    lines.append("};")
    lines.append(f"const unsigned int {var_name}_len = {len(model_bytes)};")
    return "\n".join(lines)

c_array = convert_to_c_array(tflite_model, "gesture_model")

with open("gesture_model.h", "w") as f:
    f.write("#ifndef GESTURE_MODEL_H\n")
    f.write("#define GESTURE_MODEL_H\n\n")
    f.write(c_array)
    f.write("\n\n#endif\n")

print("✅ Saved gesture_model.h")
