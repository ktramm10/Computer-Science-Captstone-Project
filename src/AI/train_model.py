from __future__ import annotations

"""Train and use the AI game prompt classifier.

Inputs:
- training.csv provides labeled prompt examples with genre, entity, component, and scene template columns.

Outputs:
- game_prompt_model.joblib stores the trained scikit-learn model.
- cleaned_training.csv is created only when running with --export-cleaned.
"""

import argparse
import csv
import json
import re
from collections import Counter, defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

import joblib
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score
from sklearn.model_selection import train_test_split
from sklearn.pipeline import Pipeline


DATA_PATH = Path(__file__).with_name("training.csv")
CLEANED_DATA_PATH = Path(__file__).with_name("cleaned_training.csv")
MODEL_PATH = Path(__file__).with_name("game_prompt_model.joblib")
COLUMN_ORDER = ["prompt_id", "prompt", "expected_genre", "expected_entities", "expected_components", "scene_template"]
REQUIRED_COLUMNS = set(COLUMN_ORDER)
MAX_PROMPT_CHARS = 2000


# Trim text and collapse repeated whitespace into single spaces.
def normalize_text(value: str) -> str:
    return re.sub(r"\s+", " ", value.strip())


# Split a semicolon-separated training cell into clean item names.
def split_items(value: str) -> list[str]:
    return [item.strip() for item in value.split(";") if item.strip()]


# Normalize one CSV row and enforce the expected list formatting.
def clean_training_row(row: dict[str, str]) -> dict[str, str]:
    cleaned = {column: normalize_text(row.get(column, "")) for column in REQUIRED_COLUMNS}
    cleaned["prompt"] = cleaned["prompt"][:MAX_PROMPT_CHARS]
    cleaned["expected_entities"] = ";".join(split_items(cleaned["expected_entities"]))
    cleaned["expected_components"] = ";".join(split_items(cleaned["expected_components"]))
    return cleaned


# Read training rows from CSV, validate columns, and skip unusable rows.
def load_training_rows(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as file:
        reader = csv.DictReader(file)
        missing_columns = REQUIRED_COLUMNS.difference(reader.fieldnames or [])
        if missing_columns:
            missing = ", ".join(sorted(missing_columns))
            raise ValueError(f"{path} is missing required columns: {missing}")
        rows = [clean_training_row(row) for row in reader]
        return [row for row in rows if row.get("prompt") and row.get("expected_genre")]


# Write normalized training rows to a CSV file for documentation or review.
def export_cleaned_rows(rows: list[dict[str, str]], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="", encoding="utf-8") as file:
        writer = csv.DictWriter(file, fieldnames=COLUMN_ORDER)
        writer.writeheader()
        writer.writerows(rows)


# Build row counts, label distributions, and prompt length statistics.
def summarize_dataset(rows: list[dict[str, str]]) -> dict[str, Any]:
    genre_counts = Counter(row["expected_genre"] for row in rows)
    entity_counts = Counter(entity for row in rows for entity in split_items(row["expected_entities"]))
    component_counts = Counter(component for row in rows for component in split_items(row["expected_components"]))
    prompt_lengths = [len(row["prompt"].split()) for row in rows]
    missing_cells = sum(1 for row in rows for column in REQUIRED_COLUMNS if not row.get(column))

    return {
        "dataset_path": str(DATA_PATH),
        "row_count": len(rows),
        "required_columns": sorted(REQUIRED_COLUMNS),
        "missing_cells_after_cleaning": missing_cells,
        "genres": dict(sorted(genre_counts.items())),
        "entities": dict(sorted(entity_counts.items())),
        "components": dict(sorted(component_counts.items())),
        "prompt_word_count": {
            "min": min(prompt_lengths) if prompt_lengths else 0,
            "max": max(prompt_lengths) if prompt_lengths else 0,
            "average": round(sum(prompt_lengths) / len(prompt_lengths), 2) if prompt_lengths else 0,
        },
    }


# Render count dictionaries as simple console bar chart lines.
def make_ascii_bars(values: dict[str, int], width: int = 32) -> list[str]:
    if not values:
        return ["(no values)"]
    max_value = max(values.values())
    lines = []
    for label, value in sorted(values.items()):
        bar_width = max(1, round((value / max_value) * width)) if max_value else 0
        lines.append(f"{label:20} {'#' * bar_width} {value}")
    return lines


# Group training data by genre to recommend entities, components, and templates.
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


# Create the TF-IDF and LogisticRegression classifier pipeline.
def make_pipeline() -> Pipeline:
    return Pipeline(
        steps=[
            ("vectorizer", TfidfVectorizer(ngram_range=(1, 2), lowercase=True)),
            ("classifier", LogisticRegression(max_iter=1000)),
        ]
    )


# Create the structured report used for project requirement documentation.
def build_requirement_report(package: dict[str, Any], data_path: Path, model_path: Path) -> dict[str, Any]:
    dataset_summary = package["dataset_summary"]
    return {
        "descriptive_method": "Dataset profiling with frequency counts, prompt word-count summary, and console bar charts.",
        "nondescriptive_method": "Predictive LogisticRegression text classifier using TF-IDF n-gram features.",
        "collected_or_available_datasets": {
            "training_dataset": str(data_path),
            "rows_available": dataset_summary["row_count"],
            "labels_available": list(dataset_summary["genres"].keys()),
        },
        "decision_support_functionality": "Classifies a designer prompt into a game genre and recommends scene templates, entities, components, and confidence.",
        "dataset_featurizing_parsing_cleaning_wrangling": [
            "CSV parsing with required-column validation",
            "Whitespace normalization and empty-row filtering",
            "Semicolon-list parsing for entities and components",
            "TF-IDF unigram/bigram feature extraction",
        ],
        "data_exploration_and_preparation_methods": [
            "Genre, entity, and component frequency distributions",
            "Prompt word-count profiling",
            "Stratified train/test split when class counts support it",
            "Fallback training accuracy report for very small datasets",
        ],
        "data_visualization_functionalities": "Console bar charts for genre, entity, and component distribution inspection.",
        "interactive_queries": [
            "--query genres",
            "--query genre:<label>",
            "--query component:<name>",
            "--predict or --predict-file for prompt classification",
        ],
        "machine_learning_methods_and_algorithms": [
            "TfidfVectorizer(ngram_range=(1, 2))",
            "LogisticRegression(max_iter=1000)",
            "scikit-learn Pipeline",
        ],
        "accuracy_evaluation": {
            "metric": "accuracy_score",
            "score": round(float(package["accuracy"]), 4),
            "kind": package["accuracy_kind"],
        },
        "industry_appropriate_security_features": [
            "Local-only CSV/model workflow with no network calls",
            "Required schema validation before training",
            f"Prompt length capped at {MAX_PROMPT_CHARS} characters",
            "UTF-8 file handling and temp-file prompt handoff from the C++ bridge",
        ],
        "monitor_and_maintain_tools": {
            "model_artifact": str(model_path),
            "trained_at_utc": package["trained_at_utc"],
            "dataset_row_count": dataset_summary["row_count"],
            "label_distribution": dataset_summary["genres"],
            "missing_cells_after_cleaning": dataset_summary["missing_cells_after_cleaning"],
        },
    }


# Train the classifier, evaluate accuracy, and save the model package.
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
        "dataset_summary": summarize_dataset(rows),
        "trained_at_utc": datetime.now(timezone.utc).isoformat(),
    }
    package["requirement_report"] = build_requirement_report(package, data_path, model_path)
    model_path.parent.mkdir(parents=True, exist_ok=True)
    joblib.dump(package, model_path)
    return package


# Load a trained model and classify one prompt into genre recommendations.
def predict(model_path: Path, prompt: str) -> dict[str, Any]:
    package = joblib.load(model_path)
    pipeline = package["pipeline"]
    prompt = normalize_text(prompt)[:MAX_PROMPT_CHARS]
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
        "requirement_report": package.get("requirement_report", {}),
    }
    if hasattr(pipeline, "predict_proba"):
        probabilities = pipeline.predict_proba([prompt])[0]
        best_probability = max(probabilities)
        result["confidence"] = round(float(best_probability), 3)
    return result


# Run a small dataset query for genres, genre rows, or component rows.
def query_rows(rows: list[dict[str, str]], query: str) -> dict[str, Any]:
    normalized_query = normalize_text(query)
    if normalized_query == "genres":
        return {"query": query, "genres": dict(sorted(Counter(row["expected_genre"] for row in rows).items()))}
    if normalized_query.startswith("genre:"):
        label = normalize_text(normalized_query.split(":", 1)[1])
        matches = [row for row in rows if row["expected_genre"].lower() == label.lower()]
        return {"query": query, "match_count": len(matches), "rows": matches}
    if normalized_query.startswith("component:"):
        component = normalize_text(normalized_query.split(":", 1)[1])
        matches = [row for row in rows if component.lower() in [item.lower() for item in split_items(row["expected_components"])] ]
        return {"query": query, "match_count": len(matches), "rows": matches}
    return {
        "query": query,
        "error": "Supported queries are: genres, genre:<label>, component:<name>.",
    }


# Print a consistently formatted console section heading.
def print_section(title: str) -> None:
    print(f"\n=== {title} ===")


# Print dataset, model, visualization, and maintenance details to the console.
def print_requirement_report(package: dict[str, Any], data_path: Path, model_path: Path) -> None:
    report = package["requirement_report"]
    dataset_summary = package["dataset_summary"]

    print_section("Algorithm Requirement Report")
    print(f"Descriptive method: {report['descriptive_method']}")
    print(f"Nondescriptive method: {report['nondescriptive_method']}")
    print(f"Dataset: {data_path}")
    print(f"Rows available: {dataset_summary['row_count']}")
    print(f"Required columns: {', '.join(dataset_summary['required_columns'])}")
    print(f"Prompt word count: min {dataset_summary['prompt_word_count']['min']}, avg {dataset_summary['prompt_word_count']['average']}, max {dataset_summary['prompt_word_count']['max']}")
    print(f"Decision support: {report['decision_support_functionality']}")

    print_section("Data Preparation")
    for item in report["dataset_featurizing_parsing_cleaning_wrangling"]:
        print(f"- {item}")
    for item in report["data_exploration_and_preparation_methods"]:
        print(f"- {item}")

    print_section("Console Data Visualizations")
    print("Genre distribution:")
    print("\n".join(make_ascii_bars(dataset_summary["genres"])))
    print("Entity distribution:")
    print("\n".join(make_ascii_bars(dataset_summary["entities"])))
    print("Component distribution:")
    print("\n".join(make_ascii_bars(dataset_summary["components"])))

    print_section("Interactive Queries")
    for item in report["interactive_queries"]:
        print(f"- {item}")

    print_section("Machine Learning and Accuracy")
    for item in report["machine_learning_methods_and_algorithms"]:
        print(f"- {item}")
    print(f"{package['accuracy_kind'].title()} accuracy: {package['accuracy']:.2f}")

    print_section("Security, Monitoring, and Maintenance")
    for item in report["industry_appropriate_security_features"]:
        print(f"- {item}")
    print(f"Model artifact: {model_path}")
    print(f"Trained at UTC: {package['trained_at_utc']}")
    print(f"Missing cells after cleaning: {dataset_summary['missing_cells_after_cleaning']}")


# Define and parse command-line options for training, prediction, and queries.
def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Train or use the game prompt classifier.")
    parser.add_argument("--data", type=Path, default=DATA_PATH, help="Path to training.csv")
    parser.add_argument("--model", type=Path, default=MODEL_PATH, help="Path to save/load the model")
    parser.add_argument("--predict", help="Prompt to classify after a model has been trained")
    parser.add_argument("--predict-file", type=Path, help="File containing a prompt to classify")
    parser.add_argument("--query", help="Interactive dataset query: genres, genre:<label>, or component:<name>")
    parser.add_argument("--export-cleaned", type=Path, nargs="?", const=CLEANED_DATA_PATH, help="Write the cleaned training dataset CSV and exit")
    return parser.parse_args()


# Run the requested CLI workflow: export, query, predict, or train.
def main() -> None:
    args = parse_args()
    if args.export_cleaned:
        rows = load_training_rows(args.data)
        export_cleaned_rows(rows, args.export_cleaned)
        print(f"Exported cleaned dataset to {args.export_cleaned}")
        print(f"Rows exported: {len(rows)}")
        return

    if args.query:
        rows = load_training_rows(args.data)
        print(json.dumps(query_rows(rows, args.query), indent=2))
        return

    prompt = args.predict
    if args.predict_file:
        prompt = args.predict_file.read_text(encoding="utf-8")
    if prompt is not None:
        if not args.model.exists():
            train(args.data, args.model)
        result = predict(args.model, prompt)
        print(json.dumps(result, indent=2))
        return
    package = train(args.data, args.model)
    print(f"Saved model to {args.model}")
    print(f"Labels: {', '.join(package['labels'])}")
    print_requirement_report(package, args.data, args.model)


if __name__ == "__main__":
    main()
