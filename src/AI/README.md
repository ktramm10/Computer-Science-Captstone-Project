# AI Python Tools

This folder contains a small scikit-learn workflow for training a game prompt classifier from `training.csv`.

## Setup

From the repository root:

```powershell
py -m venv .venv
.\.venv\Scripts\python -m pip install --upgrade pip
.\.venv\Scripts\python -m pip install -r requirements.txt
```

## Train

```powershell
.\.venv\Scripts\python src\AI\train_model.py
```

This creates `src/AI/game_prompt_model.joblib`.

## Export Cleaned Dataset

```powershell
.\.venv\Scripts\python src\AI\train_model.py --export-cleaned
```

This creates `src/AI/cleaned_training.csv`, which is the documented cleaned dataset artifact for submission.

## Predict

```powershell
.\.venv\Scripts\python src\AI\train_model.py --predict "make a platformer with coins"
```

The current dataset is very small, so the first model is only a starter. Add more rows to `training.csv` for better predictions.

## Documentation

Submission documentation is in `docs/`:

- `docs/BUSINESS_REQUIREMENTS.md`
- `docs/DATA_PRODUCT_DOCUMENTATION.md`
- `docs/SUBMISSION_CHECKLIST.md`
- `docs/screenshots/README.md`
