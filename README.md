# SQ Decoder VST3

Minimal VST3 plugin scaffold that decodes stereo SQ-encoded material to 4-channel output.

## Setup

1. Clone the VST3 SDK into `external/vst3sdk`:
   - https://github.com/steinbergmedia/vst3sdk
2. Configure and build:

```sh
cmake -S . -B build
cmake --build build
```

The generated VST3 bundle will be placed in the SDK's standard output directory (usually under `build/VST3/`).

## Notes

- Inputs: stereo (L/R)
- Outputs: quadraphonic (LF/RF/LR/RR)
- Parameter: Separation (0..1) blends between stereo duplication and SQ matrix decode.
