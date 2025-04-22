# Solution Document for Dashboard Configuration Issue

This document provides a detailed, step-by-step guide to resolve the issue where only the first dashboard (`dashForce`) correctly configures the data channel, while the other seven dashboards fail to do so. The solution ensures proper signal-slot connections, verifies object names, completes default mappings, debugs data updates, and dynamically updates channel lists in the settings dialog.

---

## Step 1: Ensure Signal-Slot Connections for All Dashboards

### Problem
The `dashboardSettingsChanged` signal from the dashboards might not be properly connected to the `handleDashboardSettingsChanged` slot in the `MainWindow` class for all dashboards, causing settings to apply only to `dashForce`.

### Solution
Explicitly connect the `dashboardSettingsChanged` signal from each dashboard to the `handleDashboardSettingsChanged` slot in the `MainWindow` constructor.

### Implementation
Add the following code in the `MainWindow` constructor to establish connections for all eight dashboards:

```cpp
connect(ui->dashForce, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashTorque, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashRPM, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashThrust, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashFuelConsumption, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashSparkPlugTemp, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashPower, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
connect(ui->dashPressure, &Dashboard::dashboardSettingsChanged, this, &MainWindow::handleDashboardSettingsChanged);
```

### Verification
- Ensure that the `objectName` properties of all dashboards in the UI file (e.g., `dashForce`, `dashTorque`, etc.) match the names used in the `ui->` references.
- Add a debug log in `handleDashboardSettingsChanged` to confirm that the slot is triggered for each dashboard when settings change:
  ```cpp
  qDebug() << "Settings changed for dashboard:" << sender()->objectName();
  ```

---

## Step 2: Verify and Complete Default Dashboard Mappings

### Problem
The `dashboardMappings` data structure might only include settings for `dashForce`, causing other dashboards to lack proper configuration.

### Solution
Update the `initDefaultDashboardMappings()` function to initialize mappings for all eight dashboards with appropriate default values.

### Implementation
Modify the `initDefaultDashboardMappings()` function in `MainWindow` as follows:

```cpp
void MainWindow::initDefaultDashboardMappings() {
    dashboardMappings["dashForce"] = { SourceModbus, 0, "Force", "N", 0, 5000, QColor(24, 189, 155), PointerStyle_Triangle, "A_0" };
    dashboardMappings["dashTorque"] = { SourceModbus, 1, "Torque", "N·m", 0, 500, QColor(217, 86, 86), PointerStyle_Indicator, "A_1" };
    dashboardMappings["dashRPM"] = { SourceModbus, 2, "RPM", "RPM", 0, 12000, QColor(255, 100, 0), PointerStyle_Indicator, "A_2" };
    dashboardMappings["dashThrust"] = { SourceModbus, 3, "Thrust", "N", 0, 8000, QColor(0, 100, 200), PointerStyle_Triangle, "A_3" };
    dashboardMappings["dashFuelConsumption"] = { SourceModbus, 4, "Fuel Consumption", "ml/min", 0, 1000, QColor(200, 180, 0), PointerStyle_Circle, "A_4" };
    dashboardMappings["dashSparkPlugTemp"] = { SourceModbus, 5, "Spark Plug Temp", "°C", 0, 900, QColor(250, 50, 50), PointerStyle_Indicator, "A_5" };
    dashboardMappings["dashPower"] = { SourceModbus, 6, "Power", "kW", 0, 100, QColor(100, 50, 200), PointerStyle_Triangle, "A_6" };
    dashboardMappings["dashPressure"] = { SourceModbus, 7, "Pressure", "MPa", 0, 10, QColor(50, 168, 82), PointerStyle_Indicator, "A_7" };
}
```

### Verification
- Check that the `objectName` of each dashboard in the UI file matches the keys in `dashboardMappings` (e.g., `dashForce`, `dashTorque`, etc.).
- After calling `initDefaultDashboardMappings()`, log the contents of `dashboardMappings` to confirm all dashboards are included:
  ```cpp
  for (const auto& key : dashboardMappings.keys()) {
      qDebug() << "Dashboard:" << key << "Channel:" << dashboardMappings[key].channelIndex;
  }
  ```

---

## Step 3: Debug Data Updates in `updateDashboardByMapping`

### Problem
Even with correct mappings, dashboards might not display data correctly if `updateDashboardByMapping` fails to process updates for all dashboards.

### Solution
Add debug logging in `updateDashboardByMapping` to verify that each dashboard receives the correct data values.

### Implementation
Modify `updateDashboardByMapping` to include a debug statement:

```cpp
void MainWindow::updateDashboardByMapping(const QString& name, double value) {
    if (dashboardMappings.contains(name)) {
        auto mapping = dashboardMappings[name];
        qDebug() << "Updating dashboard:" << name << "Source:" << mapping.sourceType << "Channel:" << mapping.channelIndex << "Value:" << value;
        // Existing code to update the dashboard UI
    } else {
        qDebug() << "No mapping found for dashboard:" << name;
    }
}
```

### Verification
- Run the application and monitor the debug output to ensure that all dashboards (e.g., `dashForce`, `dashTorque`, etc.) are being updated with the correct channel index and value.
- If any dashboard logs "No mapping found," revisit Step 2 to ensure its mapping is correctly initialized.

---

## Step 4: Dynamically Update Channel Lists in Settings Dialog

### Problem
The channel list in the settings dialog might not reflect the actual number of available channels, leading to incorrect or missing channel selections for dashboards.

### Solution
Modify the `updateChannelList` lambda in `createDataSourceGroup` to dynamically generate the channel list based on the current configuration.

### Implementation
Update the `updateChannelList` lambda as follows:

```cpp
auto updateChannelList = [=](int index) {
    channelCombo->clear();
    switch (index) {
        case 0: // Modbus
            int modbusChannels = parentWidget()->findChild<QLineEdit*>("lineSegNum")->text().toInt();
            for (int i = 0; i < modbusChannels; i++)
                channelCombo->addItem(tr("Channel %1 (A_%2)").arg(i).arg(i));
            break;
        case 1: // DAQ
            QStringList daqChannels = parentWidget()->findChild<QLineEdit*>("channelsEdit")->text().split('/');
            for (int i = 0; i < daqChannels.size(); i++)
                channelCombo->addItem(tr("Channel %1 (B_%2)").arg(i).arg(i));
            break;
        case 2: // ECU
            channelCombo->addItem(tr("Throttle (C_0)"));
            // Add additional ECU channels as needed
            break;
        case 3: // CustomData
            for (int i = 0; i < 5; ++i)
                channelCombo->addItem(tr("Channel %1 (D_%2)").arg(i).arg(i));
            break;
    }
};
```

### Verification
- Test the settings dialog by changing the data source type (e.g., Modbus, DAQ) and verify that the `channelCombo` dropdown updates with the correct number of channels.
- For Modbus, ensure the number of channels matches the value in `lineSegNum`. For DAQ, confirm it matches the parsed channel list from `channelsEdit`.

---

## Final Verification

After implementing the above steps, perform the following checks to ensure the issue is fully resolved:

1. **Signal-Slot Connections**:
   - Confirm that changing settings for any dashboard triggers `handleDashboardSettingsChanged` and logs the correct dashboard name.
2. **Default Mappings**:
   - Verify that `dashboardMappings` contains entries for all eight dashboards with correct channel indices and settings.
3. **Data Updates**:
   - Check the debug logs from `updateDashboardByMapping` to ensure all dashboards receive real-time data updates from their assigned channels.
4. **Channel Lists**:
   - Validate that the settings dialog’s channel list dynamically updates and allows correct channel assignments for each dashboard.

### Additional Debugging
If any dashboards still fail to configure correctly:
- Add more detailed logs in `handleDashboardSettingsChanged` and `updateDashboardByMapping` to trace the data flow.
- Double-check the UI file to ensure no typos exist in the `objectName` properties.

By completing these steps, all eight dashboards should now correctly configure their data channels and display real-time data as expected.