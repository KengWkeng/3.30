# Development Documentation: Clearing Historical Data in Qt Charts

This document provides a step-by-step guide to modify a Qt application to clear historical data in charts when starting a new data collection, ensuring that old curves do not overlay new data.

## Objective
Start each new data collection with fresh charts by clearing both graph objects and historical data stored in vectors.

## Prerequisites
- Existing Qt application with QCustomPlot-based charts.
- Familiarity with Qt's C++ framework and signal-slot mechanism.

## Steps to Implement

### Step 1: Declare Member Variables
Convert static vectors to member variables in the `MainWindow` class for explicit state management.

**File:** `mainwindow.h`

**Action:** Add the following to the `private` section:
```cpp
private:
    // Modbus plot data
    QVector<double> modbusTimeData;
    QVector<QVector<double>> modbusValues;

    // DAQ plot data
    QVector<double> daqTimeData;
    QVector<QVector<double>> daqValues;

    // ECU plot data
    QVector<double> ecuTimeData;
    QVector<QVector<double>> ecuValues;

    // Custom variable plot data
    QVector<double> customTimeData;
    QVector<QVector<double>> customValues;
```

### Step 2: Modify `updateAllPlots` Function
Update the `updateAllPlots` function to use member variables instead of static ones.

**File:** `mainwindow.cpp`

**Action:** Replace static vector logic with member variables. Example for Modbus plot:
```cpp
void MainWindow::updateAllPlots(const DataSnapshot &snapshot, int snapshotCount)
{
    // Update Modbus plot
    if (snapshot.modbusValid && ui->modbusCustomPlot) {
        if (ui->modbusCustomPlot->graphCount() == 0) {
            myplotInit(ui->modbusCustomPlot);
        }

        int currentModbusRegs = snapshot.modbusData.size();
        if (modbusValues.size() != currentModbusRegs) {
            modbusValues.resize(currentModbusRegs);
        }

        modbusTimeData.append(snapshot.timestamp);
        for (int ch = 0; ch < currentModbusRegs; ++ch) {
            modbusValues[ch].append(snapshot.modbusData[ch]);
            while (modbusValues[ch].size() > maxDataPoints) {
                modbusValues[ch].removeFirst();
            }
        }
        while (modbusTimeData.size() > maxDataPoints) {
            modbusTimeData.removeFirst();
        }

        if (!modbusTimeData.isEmpty()) {
            while (ui->modbusCustomPlot->graphCount() < currentModbusRegs) {
                ui->modbusCustomPlot->addGraph();
            }
            while (ui->modbusCustomPlot->graphCount() > currentModbusRegs) {
                ui->modbusCustomPlot->removeGraph(ui->modbusCustomPlot->graphCount() - 1);
            }

            for (int ch = 0; ch < currentModbusRegs; ++ch) {
                ui->modbusCustomPlot->graph(ch)->setData(modbusTimeData, modbusValues[ch], true);
            }

            double latestTimestamp = snapshot.timestamp;
            double displayWindowSeconds = 60.0;
            ui->modbusCustomPlot->xAxis->setRange(
                latestTimestamp <= displayWindowSeconds ? 0 : latestTimestamp - displayWindowSeconds,
                latestTimestamp
            );
            ui->modbusCustomPlot->yAxis->rescale();
            ui->modbusCustomPlot->replot(QCustomPlot::rpQueuedReplot);
        }
    }

    // Repeat similar updates for DAQ, ECU, and customVar plots
}
```

**Note:** Apply analogous changes to `daqCustomPlot`, `ECUCustomPlot`, and `customVarCustomPlot` sections.

### Step 3: Clear Data on Start
Clear member variables and reset graphs when starting a new collection.

**File:** `mainwindow.cpp`

**Action:** Modify `on_btnStartAll_clicked`:
```cpp
void MainWindow::on_btnStartAll_clicked()
{
    if (currentRunMode == RunMode::Idle) {
        currentRunMode = RunMode::Normal;

        // Clear Modbus plot
        if (ui->modbusCustomPlot) {
            ui->modbusCustomPlot->clearGraphs();
            ui->modbusCustomPlot->clearItems();
            ui->modbusCustomPlot->replot();
            modbusTimeData.clear();
            modbusValues.clear();
        }

        // Clear DAQ plot
        if (ui->daqCustomPlot) {
            ui->daqCustomPlot->clearGraphs();
            ui->daqCustomPlot->clearItems();
            ui->daqCustomPlot->replot();
            daqTimeData.clear();
            daqValues.clear();
        }

        // Clear ECU plot
        if (ui->ECUCustomPlot) {
            ui->ECUCustomPlot->clearGraphs();
            ui->ECUCustomPlot->clearItems();
            ui->ECUCustomPlot->replot();
            ecuTimeData.clear();
            ecuValues.clear();
        }

        // Clear customVar plot
        if (ui->customVarCustomPlot) {
            ui->customVarCustomPlot->clearGraphs();
            ui->customVarCustomPlot->clearItems();
            ui->customVarCustomPlot->replot();
            customTimeData.clear();
            customValues.clear();
        }

        // Reinitialize plots
        myplotInit(ui->modbusCustomPlot);
        setupDAQPlot();
        ECUPlotInit();
        setupDash1Plot();

        // Continue with existing start logic...
    }
    // Existing stop logic...
}
```

### Step 4: Verify Additional Plots
Check and update other plots like `dash1plot` if they store historical data.

**Action:** If `setupDash1Plot` uses its own data vectors, add clearing logic similar to above.

## Verification
- Start a new data collection and confirm that only new data appears.
- Check that old curves do not persist across collections.
- Test all plot types (Modbus, DAQ, ECU, customVar, etc.) for consistency.

## Benefits
- **Clean Slate:** Each collection begins with empty data vectors and fresh graphs.
- **Consistency:** Uniform handling across all plot types prevents data overlap.

This guide ensures your Qt charts display only current data, enhancing usability and clarity.