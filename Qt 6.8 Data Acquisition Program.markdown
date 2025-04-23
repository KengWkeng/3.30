# Qt 6.8 Data Acquisition Program

This is my Qt 6.8 data acquisition program. The core data architecture is designed to collect data from sub-hardware into a data snapshot at each moment, regardless of how the acquisition is performed. Plotting is then based on these snapshots. I have designed a "Start All" button (`btnStartAll`) to initiate all acquisition tasks with a single click. Each snapshot includes the data and timestamp for each group and can be saved to a file.

## Modification Requirements

Now, I need to modify the program so that each time the acquisition stops and the "Start All" button is clicked again to restart, it performs the following:

- Checks all related timers, snapshot data, and timestamps to ensure they are completely cleared and initialized.
- Saved files should not be deleted; only the corresponding arrays should be cleared, timers reset to zero, and QCustomPlot initialized to a blank state.

## Image Format Requirements

### 1. QCustomPlot Plotting

- Remove small individual titles (similar to those in ECU plotting) to maximize the space occupied by the image within its group box.

### 2. Dynamic Y-axis Scaling

- The X-axis handling remains unchanged.
- For the speed channel of the ECU device, due to its long range (0 to 10,000), set its Y-axis to use a separate axis. In the same chart:
  - Other images share a common Y-axis.
  - The speed channel uses its own Y-axis.
- Initially, set the speed Y-axis display range to 0 to 6,000. When the data exceeds 6,000, dynamically adjust it to 0 to the current value.

### 3. Legend Requirements

- Each valid channel in each image must have a corresponding legend.
- Place the legend outside the coordinate axes, on the right side of the overall image.
- Each channel legend should include a checkbox in front to control visibility:
  - By default, all checkboxes are checked (visible).
  - Unchecking a checkbox dynamically hides the corresponding channel’s data display.