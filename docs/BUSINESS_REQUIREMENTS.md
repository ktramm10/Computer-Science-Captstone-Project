# Business Vision and Requirements

## Business Vision

The project is a 2D game engine with an AI prompt assistant that helps a designer create starter scenes from plain language game ideas. The business goal is to reduce the time required to move from a concept prompt to a playable prototype by recommending an appropriate game genre, scene template, entities, and required components.

## Target Users

- Student developers building prototypes for a capstone or portfolio project.
- Game designers who need a fast first draft of a scene layout.
- Instructors or reviewers who need visible evidence that the data product can explain, predict, and evaluate its output.

## Business Requirements

- The product must accept a user prompt from the in application AI Prompt panel.
- The product must classify the prompt into a supported game genre.
- The product must recommend a scene template, entities, components, and confidence score.
- The product must expose diagnostics and requirement coverage in the console and in an in app dashboard.
- The product must support data inspection through command line interactive queries.
- The product must document the raw dataset, cleaned dataset, data preparation code, model code, accuracy results, and executable outputs.
- The engine must resolve runtime scenes, scripts, assets, and AI files from the project root so reviewers can run the executable consistently.

## Success Criteria

- A designer can generate a scene from a prompt without editing JSON manually.
- The AI output includes decision support information: genre, template, entities, components, and confidence.
- The dashboard includes at least three visualization types: metric gauges, a histogram, and requirement coverage tiles.
- The model reports an accuracy assessment after training.
- Source code and executable paths are documented for reviewers.
- The executable can be started from the project root while still loading saved scenes and assets correctly.
