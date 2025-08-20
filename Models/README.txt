AI Models Directory
===================

This directory contains AI models for advanced pitch correction:

CREPE/
  - Place CREPE pitch detection models here (.onnx files)
  - Download from: https://github.com/marl/crepe

DDSP/
  - Place DDSP synthesis models here (.onnx files)
  - Download from: https://github.com/magenta/ddsp

The plugin will automatically detect and load these models when available.
If no models are found, the plugin will use fallback algorithms.