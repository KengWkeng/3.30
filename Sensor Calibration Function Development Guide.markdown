# Sensor Calibration Function Development Guide

This development guide provides a comprehensive roadmap for implementing the "Sensor Calibration" functionality in a Qt project. It consolidates all customer requirements, including main page integration, calibration logic, UI layout, multi-channel support, and specific needs for batch calibration of temperature sensors. The document aims to guide developers in creating modular, debuggable code that seamlessly integrates with existing logic (e.g., `snapshotthread.cpp` and the main page).

---

## 1. Main Page Menu Functionality

- **Menu Item Addition**:
  - Add a "Calibrate Sensor" option to the main page menu.
  - Initially set to disabled (grayed out), dependent on the completion of "Setup Initialization" and "Read Initialization."
- **State Control**:
  - Enable the menu item only after "Setup Initialization" and "Read Initialization" are successfully completed.
  - If initialization is incomplete, keep it disabled and prompt the user: "Please complete initialization first."
- **Trigger Behavior**:
  - Upon clicking "Calibrate Sensor," open a standalone calibration window (using the `CalibrationDialog` class).
  - The window does not start data collection immediately; it waits for the user to click the "Start Calibration" button.

---

## 2. Calibration Logic

### 2.1 Device Selection and Channel Display

- **Device Selection Dropdown**:
  - Modify to include the following options:
    - "Temperature Sensor Calibration" (corresponding to modbus device)
    - "Force Sensor Calibration" (corresponding to daq device)
    - "ECU Data Correction" (corresponding to ECU device)
    - "CustomData Correction" (corresponding to CustomData device)
- **Channel Selection Dropdown**:
  - **For "Temperature Sensor Calibration"**:
    - Instead of listing specific channels, display "All Channels: a~b," where `a` and `b` are the start and end indices of valid channels (assuming continuous channels).
    - For example, if valid channels are 0 to 3, display "All Channels: 0~3."
  - **For Other Devices**:
    - Display only currently valid channels; empty or invalid channels are excluded.
    - Users can select a single channel for calibration.

### 2.2 Temperature Sensor Batch Calibration

- **Channel Validity Check**:
  - Retrieve valid channel information for the modbus device from `snapshot` to determine the continuous range of valid channels (a to b).
- **Real-Time Display**:
  - **Plot**: Simultaneously display curves for all valid modbus channels, with each curve representing one channel.
  - **Data Table**: Show real-time data for all valid channels, with each row corresponding to a channel.
- **Calibration Value Input and Collection**:
  - Users input a single calibration value applicable to all valid channels.
  - Collect the subsequent 5 seconds of data and calculate the average for each valid channel.
  - Pair the calibration value with each channel’s average value and store them.
- **Fitting and Saving**:
  - Upon clicking "Confirm," perform least squares fitting for each valid channel (`y = ax³ + bx² + cx + d`).
  - Save each channel’s fitted coefficients to the corresponding `calibration.ini` file.
  - Append a system timestamp to each channel’s entry in the ini file.

### 2.3 Other Device Calibration

- **Channel Selection**:
  - Display only valid channels, allowing users to select a single channel for calibration.
- **Real-Time Display**:
  - The plot and data table show data only for the selected channel.
- **Calibration Logic**:
  - Similar to temperature sensors but applied to a single channel at a time.
  - When saving, append a system timestamp to the channel’s entry in the ini file.

---

## 3. UI Layout

- **Overall Layout**:
  - The window is divided into two parts: the left side occupies 3/4 of the width, and the right side occupies 1/4.
- **Left Upper Section**:
  - `QCustomPlot` Plot:
    - For temperature sensors, display multiple curves (one per valid channel).
    - For other devices, display a single curve.
- **Left Lower Section**:
  - Data Table:
    - **Temperature Sensors**: Header includes "Calibration Value," Channel a, Channel a+1, ..., Channel b.
    - **Other Devices**: Header includes "Calibration Value" and the selected channel.
    - The first row shows the user-input calibration value, with subsequent rows showing the collected average values for each channel.
- **Right Upper Section**:
  - Device and Channel Selection Dropdowns:
    - Device Selection: Temperature Sensor Calibration, Force Sensor Calibration, ECU Data Correction, CustomData Correction.
    - Channel Selection: Dynamically adjusted based on the device.
- **Right Lower Section**:
  - Display real-time collected values for the current channel (or all channels).
  - Provide a calibration value input field.
  - Buttons: "Start Calibration," "Initialize Current Channel," "Confirm," "Exit."

---

## 4. `calibration.ini` File Format Requirements

### File Structure
- The file contains the following sections to store calibration coefficients for different devices:
  - `[Modbus]`: Stores calibration coefficients for modbus devices (temperature sensors).
  - `[DAQ]`: Stores calibration coefficients for daq devices (force sensors).
  - `[ECU]`: Stores calibration coefficients for ECU devices, with channels possibly including comments (e.g., `Throttle`, `EngineSpeed`).
  - `[Custom]`: Stores calibration coefficients for Custom devices, with channels possibly including comments indicating data sources (e.g., `DAQ[0] * ECU[0] * 10.0`).

### Channel Calibration Coefficient Format
- Each channel’s calibration coefficients are stored in the following format:
  - `Channel_n = a, b, c, d`, where:
    - `n` is the channel number (starting from 0).
    - `a, b, c, d` are the fitted coefficients corresponding to the polynomial `y = ax³ + bx² + cx + d`.
  - A system timestamp is appended after the coefficients in the format `; Timestamp: yyyy-MM-dd HH:mm:ss`.
  - Optional: Some channels may include additional comments (e.g., `Throttle` in `[ECU]` or data expressions in `[Custom]`), which are placed after the timestamp or independently at the end of the line.

### Example
```ini
[Modbus]
Channel_0 = 1.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_1 = 2.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_2 = 0.5, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_3 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_4 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_5 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_6 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_7 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_8 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_9 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_10 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_11 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_12 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-01 12:00:00
Channel_13 = 0.0, 0.0, 1.0, 0.0 ; Timestamp: 2023-10-