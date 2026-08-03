from __future__ import annotations

import argparse
import csv
import json
from collections import Counter, defaultdict
from pathlib import Path
from typing import Any

import joblib
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split
from sklearn.pipeline import Pipeline


DATA_PATH = Path(__file__).with_name("training.csv")
MODEL_PATH = Path(__file__).with_name("game_prompt_model.joblib")
REQUIRED_COLUMNS = {"prompt_id", "prompt", "expected_genre", "expected_entities", "expected_components", "scene_template"}


def split_items(value: str) -> list[str]:
    return [item.strip() for item in value.split(";") if item.strip()]


def load_training_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)
        missing_columns = REQUIRED_COLUMNS.difference(reader.fieldnames or [])
        if missing_columns:
            missing = ", ".join(sorted(missing_columns))
            raise ValueError(f"{path} is missing required columns: {missing}")
        return [row for row in reader if row.get("prompt") and row.get("expected_genre")]


def build_genre_profiles(rows: list[dict[str, str]]) -> dict[str, dict[str, list[str]]]:
    grouped: dict[str, dict[str, list[str]]] = defaultdict(
        lambda: {
            "entities": [],
            "components": [],
            "scene_templates": [],
        }
    )

    for row in rows:
        genre = row["expected_genre"]
        grouped[genre]["entities"].extend(split_items(row["expected_entities"]))
        grouped[genre]["components"].extend(split_items(row["expected_components"]))
        grouped[genre]["scene_templates"].append(row["scene_template"])

    profiles: dict[str, dict[str, list[str]]] = {}
    for genre, values in grouped.items():
        template_counts = Counter(values["scene_templates"])

        profiles[genre] = {
            "entities": sorted(set(values["entities"])),
            "components": sorted(set(values["components"])),
            "scene_templates": sorted(set(values["scene_templates"])),
            "default_scene_template": template_counts.most_common(1)[0][0],
        }
    return profiles


def make_pipeline() -> Pipeline:
    return Pipeline(
        steps=[
            ("vectorizer", TfidfVectorizer(ngram_range=(1, 2), lowercase=True)),
            ("classifier", LogisticRegression(max_iter=1000)),
        ]
    )


def train(data_path: Path, model_path: Path) -> dict[str, Any]:
    rows = load_training_rows(data_path)
    if len(rows) < 2:
        raise ValueError("At least two training rows are required.")

    prompts = [row["prompt"] for row in rows]
    genres = [row["expected_genre"] for row in rows]
    genre_counts = Counter(genres)
    pipeline = make_pipeline()

    can_split = len(rows) >= 8 and min(genre_counts.values()) >= 2
    if can_split:
        train_prompts, test_prompts, train_genres, test_genres = train_test_split(
            prompts,
            genres,
            test_size=0.25,
            random_state=42,
            stratify=genres,
        )
        pipeline.fit(train_prompts, train_genres)
        accuracy = accuracy_score(test_genres, pipeline.predict(test_prompts))
        accuracy_kind = "validation"
    else:
        pipeline.fit(prompts, genres)
        accuracy = accuracy_score(genres, pipeline.predict(prompts))
        accuracy_kind = "training"

    package = {
        "pipeline": pipeline,
        "genre_profiles": build_genre_profiles(rows),
        "labels": sorted(genre_counts),
        "accuracy": accuracy,
        "accuracy_kind": accuracy_kind,
    }
    model_path.parent.mkdir(parents=True, exist_ok=True)
    joblib.dump(package, model_path)
    return package


def predict(model_path: Path, prompt: str) -> dict[str, Any]:
    package = joblib.load(model_path)
    pipeline = package["pipeline"]
    genre = pipeline.predict([prompt])[0]
    profile = package["genre_profiles"].get(genre, {"entities": [], "components": [],
        "scene_templates": [],
        "default_scene_template": None,
     },
    )
    result: dict[str, Any] = {
        "prompt": prompt,
        "genre": genre,
        "entities": profile["entities"],
        "components": profile["components"],
        "scene_template": profile["default_scene_template"],
        "available_scene_templates": profile["scene_templates"],
    }
    if hasattr(pipeline, "predict_proba"):
        probabilities = pipeline.predict_proba([prompt])[0]
        best_probability = max(probabilities)
        result["confidence"] = round(float(best_probability), 3)
    return result


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Train or use the game prompt classifier.")
    parser.add_argument("--data", type=Path, default=DATA_PATH, help="Path to training.csv")
    parser.add_argument("--model", type=Path, default=MODEL_PATH, help="Path to save/load the model")
    parser.add_argument("--predict", help="Prompt to classify after a model has been trained")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    if args.predict:
        result = predict(args.model, args.predict)
        print(json.dumps(result, indent=2))
        return
    package = train(args.data, args.model)
    print(f"Saved model to {args.model}")
    print(f"Labels: {', '.join(package['labels'])}")
    print(f"{package['accuracy_kind'].title()} accuracy: {package['accuracy']:.2f}")


if __name__ == "__main__":
    main()
