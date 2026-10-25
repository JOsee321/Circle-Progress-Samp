# Analysis Images

Reference frames, simulations, and comparison assets generated during the development and testing of `shapesProgress.inc`.

## File List

### In-Game Reference Frames (preview.gif)
- `frame_0.png`, `frame0.png`: Initial camera view.
- `frame_50.png`: Circle textdraw rendered over the bridge.
- `frame_150.png`, `frame_250.png`, `frame_350.png`, `frame_450.png`, `frame_550.png`: Animation sequence frames.
- `thumb_50.png`: 640x360 scaled overview of frame 50.
- `diff.png`: Difference map highlighting UI elements.
- `circle_zoom_50.png`, `02_circle_zoom_asli.png`: 1:1 scale crop showing widescreen distortion and uneven stroke thickness in the original script.

### Original Code Issue Simulations
- `sim_current_15.png`: Default polygon step (15.0), showing sparse dots.
- `sim_current_3.png`: Polygon step 3.0 in the original code, showing thick side strokes but thin top and bottom strokes.
- `04_simulasi_asli_polygon3_bolong.png`: The 60-degree top gap caused by the loop boundary bug.

### Fixed Simulations
- `sim_fixed_square.png`: Corrected circle with 16:9 aspect ratio and 1:1 square dot scaling.
- `sim_fixed_smooth.png`: Higher polygon density test (step 2.0).
- `sim_solid_3.png`, `sim_solid_2.png`: Solid stroke tests.
- `sim_round_3.png`, `sim_round_thick.png`: Round dot sprite simulations.

### Comparisons
- `before_after_comparison.png`: Side-by-side render comparison between original and fixed code.
- `thick_comparison.png`: Heavy stroke comparison.
- `06_perbandingan_sebelum_sesudah.png`: Side-by-side comparison summary.
