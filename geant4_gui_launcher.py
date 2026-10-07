import os
import sys
from pathlib import Path

from PyQt6.QtCore import QProcess, Qt
from PyQt6.QtGui import QFont, QTextCursor
from PyQt6.QtWidgets import (
    QApplication,
    QCheckBox,
    QComboBox,
    QFileDialog,
    QFormLayout,
    QGridLayout,
    QGroupBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QPlainTextEdit,
    QSizePolicy,
    QSpinBox,
    QSplitter,
    QVBoxLayout,
    QWidget,
)


class Geant4Gui(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Geant4 X-ray Tube GUI (PyQt6)")
        self.resize(1400, 900)

        self.process = QProcess(self)
        self.process.readyReadStandardOutput.connect(self.handle_stdout)
        self.process.readyReadStandardError.connect(self.handle_stderr)
        self.process.finished.connect(self.process_finished)

        self._build_ui()
        self.apply_preset("N-100")
        self.refresh_macro_preview()

    def _build_ui(self):
        central = QWidget()
        self.setCentralWidget(central)

        main_layout = QVBoxLayout(central)

        splitter = QSplitter(Qt.Orientation.Horizontal)
        main_layout.addWidget(splitter)

        left_panel = QWidget()
        left_layout = QVBoxLayout(left_panel)
        left_layout.setSpacing(10)

        left_layout.addWidget(self._build_paths_group())
        left_layout.addWidget(self._build_presets_group())
        left_layout.addWidget(self._build_geometry_group())
        left_layout.addWidget(self._build_physics_group())
        left_layout.addWidget(self._build_biasing_group())
        left_layout.addWidget(self._build_buttons_group())
        left_layout.addStretch(1)

        right_panel = QWidget()
        right_layout = QVBoxLayout(right_panel)

        macro_label = QLabel("Generated macro")
        macro_label.setFont(QFont("Sans Serif", 11, QFont.Weight.Bold))
        right_layout.addWidget(macro_label)

        self.macro_preview = QPlainTextEdit()
        self.macro_preview.setReadOnly(True)
        self.macro_preview.setLineWrapMode(QPlainTextEdit.LineWrapMode.NoWrap)
        right_layout.addWidget(self.macro_preview, 1)

        log_label = QLabel("Execution log")
        log_label.setFont(QFont("Sans Serif", 11, QFont.Weight.Bold))
        right_layout.addWidget(log_label)

        self.log_output = QPlainTextEdit()
        self.log_output.setReadOnly(True)
        self.log_output.setLineWrapMode(QPlainTextEdit.LineWrapMode.NoWrap)
        right_layout.addWidget(self.log_output, 1)
        self.log("Ready.")

        splitter.addWidget(left_panel)
        splitter.addWidget(right_panel)
        splitter.setSizes([420, 980])

    def _build_paths_group(self):
        group = QGroupBox("Paths")
        layout = QGridLayout(group)

        self.exe_edit = QLineEdit("./G4XRTube")
        self.workdir_edit = QLineEdit(os.getcwd())
        self.macro_edit = QLineEdit(str(Path(os.getcwd()) / "generated_run.mac"))

        layout.addWidget(QLabel("Executable"), 0, 0)
        layout.addWidget(self.exe_edit, 0, 1)
        exe_btn = QPushButton("Browse")
        exe_btn.clicked.connect(self.browse_executable)
        layout.addWidget(exe_btn, 0, 2)

        layout.addWidget(QLabel("Working dir"), 1, 0)
        layout.addWidget(self.workdir_edit, 1, 1)
        workdir_btn = QPushButton("Browse")
        workdir_btn.clicked.connect(self.browse_workdir)
        layout.addWidget(workdir_btn, 1, 2)

        layout.addWidget(QLabel("Macro file"), 2, 0)
        layout.addWidget(self.macro_edit, 2, 1)
        macro_btn = QPushButton("Browse")
        macro_btn.clicked.connect(self.browse_macro)
        layout.addWidget(macro_btn, 2, 2)

        self._connect_refresh([self.exe_edit, self.workdir_edit, self.macro_edit])
        return group

    def _build_presets_group(self):
        group = QGroupBox("Presets")
        layout = QHBoxLayout(group)

        self.preset_combo = QComboBox()
        self.preset_combo.addItems(
            ["L-55", "H-100", "N-60", "N-80", "N-100", "N-120", "W-150", "Custom"]
        )
        self.preset_combo.currentTextChanged.connect(self.apply_preset)

        layout.addWidget(QLabel("Spectrum preset"))
        layout.addWidget(self.preset_combo, 1)
        return group

    def _build_geometry_group(self):
        group = QGroupBox("Geometry")
        layout = QFormLayout(group)

        self.target_edit = QLineEdit("G4_W")
        self.anode_angle_edit = QLineEdit("20")
        self.inherent_material_edit = QLineEdit("G4_Al")
        self.inherent_thickness_edit = QLineEdit("4")
        self.filter_material_edit = QLineEdit("G4_Cu")
        self.filter_thickness_edit = QLineEdit("5")

        layout.addRow("Target material", self.target_edit)
        layout.addRow("Anode angle [deg]", self.anode_angle_edit)
        layout.addRow("Inherent filter material", self.inherent_material_edit)
        layout.addRow("Inherent thickness [mm]", self.inherent_thickness_edit)
        layout.addRow("Additional filter material", self.filter_material_edit)
        layout.addRow("Additional thickness [mm]", self.filter_thickness_edit)

        self._connect_refresh([
            self.target_edit,
            self.anode_angle_edit,
            self.inherent_material_edit,
            self.inherent_thickness_edit,
            self.filter_material_edit,
            self.filter_thickness_edit,
        ])
        return group

    def _build_physics_group(self):
        group = QGroupBox("Physics and Run")
        layout = QFormLayout(group)

        self.physics_combo = QComboBox()
        self.physics_combo.addItems(["standard", "standard_option4", "livermore", "penelope", "LowEP"])

        self.cut_edit = QLineEdit("5")
        self.energy_edit = QLineEdit("100")
        self.events_edit = QLineEdit("1000000")
        self.workers_spin = QSpinBox()
        self.workers_spin.setRange(1, 256)
        self.workers_spin.setValue(4)
        self.control_verbose_spin = QSpinBox()
        self.control_verbose_spin.setRange(0, 5)
        self.control_verbose_spin.setValue(1)
        self.run_verbose_spin = QSpinBox()
        self.run_verbose_spin.setRange(0, 5)
        self.run_verbose_spin.setValue(1)

        layout.addRow("Physics list", self.physics_combo)
        layout.addRow("Cuts [um]", self.cut_edit)
        layout.addRow("Tube energy [keV]", self.energy_edit)
        layout.addRow("Events", self.events_edit)
        layout.addRow("Workers", self.workers_spin)
        layout.addRow("Control verbose", self.control_verbose_spin)
        layout.addRow("Run verbose", self.run_verbose_spin)

        self.physics_combo.currentTextChanged.connect(self.refresh_macro_preview)
        self._connect_refresh([self.cut_edit, self.energy_edit, self.events_edit])
        self.workers_spin.valueChanged.connect(self.refresh_macro_preview)
        self.control_verbose_spin.valueChanged.connect(self.refresh_macro_preview)
        self.run_verbose_spin.valueChanged.connect(self.refresh_macro_preview)
        return group

    def _build_biasing_group(self):
        group = QGroupBox("Bremsstrahlung Biasing")
        layout = QFormLayout(group)

        self.biasing_check = QCheckBox("Enable /process/em/setSecBiasing eBrem world")
        self.biasing_check.setChecked(True)
        self.bias_factor_edit = QLineEdit("200")
        self.bias_limit_edit = QLineEdit("1000")

        layout.addRow(self.biasing_check)
        layout.addRow("Bias factor", self.bias_factor_edit)
        layout.addRow("Bias max energy [keV]", self.bias_limit_edit)

        self.biasing_check.toggled.connect(self.refresh_macro_preview)
        self._connect_refresh([self.bias_factor_edit, self.bias_limit_edit])
        return group

    def _build_buttons_group(self):
        group = QGroupBox("Actions")
        layout = QVBoxLayout(group)

        preview_btn = QPushButton("Preview macro")
        preview_btn.clicked.connect(self.refresh_macro_preview)

        save_btn = QPushButton("Save macro")
        save_btn.clicked.connect(self.save_macro)

        run_btn = QPushButton("Run simulation")
        run_btn.clicked.connect(self.run_simulation)

        stop_btn = QPushButton("Stop run")
        stop_btn.clicked.connect(self.stop_simulation)

        for btn in [preview_btn, save_btn, run_btn, stop_btn]:
            btn.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
            layout.addWidget(btn)

        return group

    def _connect_refresh(self, widgets):
        for widget in widgets:
            if isinstance(widget, QLineEdit):
                widget.textChanged.connect(self.refresh_macro_preview)

    def apply_preset(self, preset_name):
        presets = {
            "L-55": (55, 20, "G4_Al", 4, "G4_Cu", 1.2),
            "H-100": (100, 15, "G4_Al", 4, "G4_Cu", 0.15),
            "N-60": (60, 20, "G4_Al", 4, "G4_Cu", 0.6),
            "N-80": (80, 20, "G4_Al", 4, "G4_Cu", 2),
            "N-100": (100, 20, "G4_Al", 4, "G4_Cu", 5),
            "N-120": (120, 20, "G4_Al", 4, "G4_Cu", 7),
            "W-150": (150, 15, "G4_Al", 4, "G4_Sn", 1),
        }

        if preset_name in presets:
            energy, angle, inherent_mat, inherent_mm, filter_mat, filter_mm = presets[preset_name]
            self.target_edit.setText("G4_W")
            self.energy_edit.setText(str(energy))
            self.anode_angle_edit.setText(str(angle))
            self.inherent_material_edit.setText(inherent_mat)
            self.inherent_thickness_edit.setText(str(inherent_mm))
            self.filter_material_edit.setText(filter_mat)
            self.filter_thickness_edit.setText(str(filter_mm))
        self.refresh_macro_preview()

    def build_macro(self):
        lines = [
            "# Auto-generated Geant4 macro",
            "# PyQt6 GUI launcher",
            "",
            f"/run/numberOfThreads {self.workers_spin.value()}",
            "",
            "# GEOMETRY SETTINGS",
            f"/XRtube/det/setTargetMaterial {self.target_edit.text().strip()}",
            f"/XRtube/det/setAnodeAngle {self.anode_angle_edit.text().strip()} deg",
            f"/XRtube/det/setInherentFilterMaterial {self.inherent_material_edit.text().strip()}",
            f"/XRtube/det/setInherentFilterThickness {self.inherent_thickness_edit.text().strip()} mm",
            f"/XRtube/det/setFilterMaterial {self.filter_material_edit.text().strip()}",
            f"/XRtube/det/setFilterThickness {self.filter_thickness_edit.text().strip()} mm",
            "",
            "# PHYSICS SETTINGS",
            f"/phys/SelectPhysicsList {self.physics_combo.currentText()}",
            f"/phys/setCuts {self.cut_edit.text().strip()} um",
            "",
            "# INITIALIZE",
            "/run/initialize",
            "",
        ]

        if self.biasing_check.isChecked():
            lines.extend([
                "# BIASING",
                (
                    "/process/em/setSecBiasing eBrem world "
                    f"{self.bias_factor_edit.text().strip()} {self.bias_limit_edit.text().strip()} keV"
                ),
                "",
            ])

        lines.extend([
            "# RUN CONTROL",
            f"/control/verbose {self.control_verbose_spin.value()}",
            f"/run/verbose {self.run_verbose_spin.value()}",
            "",
            "# TUBE VOLTAGE",
            f"/xraytube/setEnergy {self.energy_edit.text().strip()} keV",
            "",
            "# APPLY UPDATED RUN CONFIGURATION",
            "/run/initialize",
            "",
            "# RUN SIMULATION",
            f"/run/beamOn {self.events_edit.text().strip()}",
            "",
        ])
        return "\n".join(lines)

    def refresh_macro_preview(self):
        self.macro_preview.setPlainText(self.build_macro())

    def browse_executable(self):
        path, _ = QFileDialog.getOpenFileName(self, "Select Geant4 executable")
        if path:
            self.exe_edit.setText(path)

    def browse_workdir(self):
        path = QFileDialog.getExistingDirectory(self, "Select working directory")
        if path:
            self.workdir_edit.setText(path)

    def browse_macro(self):
        path, _ = QFileDialog.getSaveFileName(
            self,
            "Save macro file",
            self.macro_edit.text(),
            "Geant4 macro (*.mac);;All files (*)",
        )
        if path:
            self.macro_edit.setText(path)

    def save_macro(self):
        macro_path = self.macro_edit.text().strip()
        if not macro_path:
            QMessageBox.critical(self, "Missing path", "Please choose a macro output path.")
            return
        try:
            with open(macro_path, "w", encoding="utf-8") as handle:
                handle.write(self.build_macro())
            self.log(f"Saved macro to: {macro_path}")
        except OSError as exc:
            QMessageBox.critical(self, "Save failed", str(exc))

    def run_simulation(self):
        if self.process.state() != QProcess.ProcessState.NotRunning:
            QMessageBox.information(self, "Simulation running", "A simulation is already running.")
            return

        exe_path = self.exe_edit.text().strip()
        workdir = self.workdir_edit.text().strip()
        macro_path = self.macro_edit.text().strip()

        if not exe_path or not os.path.isfile(exe_path):
            QMessageBox.critical(self, "Invalid executable", f"Executable not found:\n{exe_path}")
            return

        if not workdir or not os.path.isdir(workdir):
            QMessageBox.critical(self, "Invalid working directory", f"Directory not found:\n{workdir}")
            return

        try:
            with open(macro_path, "w", encoding="utf-8") as handle:
                handle.write(self.build_macro())
        except OSError as exc:
            QMessageBox.critical(self, "Cannot write macro", str(exc))
            return

        self.log(f"Saved macro to: {macro_path}")
        self.log(f"Running executable: {exe_path}")
        self.log(f"Working directory: {workdir}")

        self.process.setWorkingDirectory(workdir)
        self.process.setProgram(exe_path)
        self.process.setArguments([macro_path])
        self.process.start()

        if not self.process.waitForStarted(3000):
            QMessageBox.critical(self, "Run failed", "The process could not be started.")
            self.log("Failed to start process.")

    def stop_simulation(self):
        if self.process.state() == QProcess.ProcessState.NotRunning:
            self.log("No running simulation to stop.")
            return
        self.process.kill()
        self.log("Stop requested.")

    def handle_stdout(self):
        data = bytes(self.process.readAllStandardOutput()).decode(errors="replace")
        if data:
            self.log_output.moveCursor(QTextCursor.MoveOperation.End)
            self.log_output.insertPlainText(data)
            self.log_output.ensureCursorVisible()

    def handle_stderr(self):
        data = bytes(self.process.readAllStandardError()).decode(errors="replace")
        if data:
            self.log_output.moveCursor(QTextCursor.MoveOperation.End)
            self.log_output.insertPlainText(data)
            self.log_output.ensureCursorVisible()

    def process_finished(self):
        exit_code = self.process.exitCode()
        self.log(f"Simulation finished with exit code {exit_code}.")

    def log(self, text):
        self.log_output.appendPlainText(text)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Geant4Gui()
    window.show()
    sys.exit(app.exec())
