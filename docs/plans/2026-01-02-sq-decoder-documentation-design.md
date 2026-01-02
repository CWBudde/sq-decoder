# SQ Decoder Documentation Design

**Date**: 2026-01-02
**Purpose**: Create comprehensive technical documentation explaining the SQ quadrophonic decoding algorithm
**Approach**: Code-to-Theory (analyze implementations, reverse-engineer theory, validate with research)

## Project Goals

- Document the SQ quadrophonic decoding algorithm from existing Delphi/VST implementations
- Create technical reference for audio engineers/developers familiar with DSP
- Explain algorithm mechanics, implementation differences, historical context, and mathematical foundations
- Analyze what has been implemented in the existing plugins

## Target Audience

Audio engineers and developers with DSP knowledge, familiar with:
- Digital signal processing concepts
- Phase shifting and filtering
- Audio plugin development
- Matrix operations

## Documentation Scope

1. **Algorithm mechanics**: How phase shifting and matrix operations work step-by-step
2. **Implementation comparison**: Differences between basic (recursive) and SQ² (FFT) approaches
3. **Historical context**: Background on SQ quadrophonic system and design rationale
4. **Mathematical detail**: Formulas, equations, signal flow diagrams
5. **Plugin analysis**: What has been implemented in the existing Delphi/VST code

## Source Material

Located in `/mnt/c/Users/Chris/Code/Plugins/Quadrophonic/`:

- **SQ/Decoder/**: Basic time-domain implementation with recursive phase shifters
- **SQ²/Decoder/**: FFT-based implementation with Hilbert transform
- **QS/Decoder/**: Related QS matrix system (for comparison)

## Documentation Structure

### 1. Executive Summary
Brief overview of SQ quadrophonic decoding: what it does (2-channel to 4-channel conversion), why it matters, purpose of this document.

### 2. Implementation Analysis
Deep code analysis of existing implementations:
- **Basic SQ Decoder**: Recursive filter implementation, time-domain processing
  - TPhaseShift class: recursive 90° phase shifter
  - Matrix coefficients: sqrt(2)/2 ≈ 0.707
  - V2M_Process: sample-by-sample processing
- **SQ² Decoder**: FFT-based Hilbert transform
  - Frequency-domain phase shifting
  - Block-based processing with overlap
  - Transfer function construction
- Annotated code snippets with explanations
- Performance characteristics (latency, CPU, accuracy)

### 3. Core Algorithm Extraction
Distilled common elements from both implementations:
- SQ matrix equations for 4-channel decode
- 90° phase shift requirement and why it's needed
- Channel routing: LT, RT → LF, RF, LB, RB
- The sqrt(2)/2 scaling factor
- Input/output relationships

### 4. Mathematical Foundation
Rigorous technical detail:
- Phase shift mathematics (Hilbert transform theory)
- Why 90° phase shift is central to SQ
- Matrix algebra for the SQ decode
- Frequency domain analysis
- Transfer functions for both implementations
- Signal flow diagrams
- Comparison of recursive vs FFT phase shift accuracy

### 5. Theoretical Context
Research-backed explanation:
- SQ system history (CBS, 1970s quadrophonic)
- Design goals and constraints
- How SQ compares to QS and other matrix systems
- Why this particular matrix was chosen
- Validation against published SQ specifications

### 6. Implementation Tradeoffs
Practical comparison for developers:
- **Basic approach**: Low CPU, low latency, phase shift accuracy limits
- **SQ² approach**: Higher quality phase shift, higher CPU cost, block processing latency
- When to choose each approach
- Accuracy measurements

### 7. Appendices
- Complete code listings from both implementations
- Frequency response plots (if generated)
- Phase response comparisons
- Test results and validation
- References to SQ literature and patents

## Deliverables

- Main documentation file: `docs/sq-decoder-explained.md`
- Supporting diagrams/images (if created): `docs/images/`
- Code references to source implementations

## Approach: Code-to-Theory

1. **Analyze existing code deeply**: Extract algorithms, equations, data flow from Delphi source
2. **Reverse-engineer theory**: Derive the mathematical principles from implementation
3. **Research and validate**: Cross-reference with SQ system literature, patents, specifications
4. **Synthesize**: Create comprehensive explanation that bridges code and theory

## Success Criteria

- Clear explanation of how SQ decoding works at both implementation and theoretical levels
- Detailed comparison between basic and SQ² approaches with tradeoffs
- Historical and theoretical context that explains WHY the algorithm works this way
- Mathematical rigor appropriate for audio engineering audience
- Actionable for developers who want to implement their own SQ decoder
