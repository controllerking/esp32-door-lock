Door Lock Controller - Arduino Project
AI Generated Overview

This project is an Arduino-based door lock controller that uses Wi-Fi to provide remote control via a web interface. The controller operates three relays to manage the locking and unlocking mechanism, with options to configure timings for the lock and unlock actions. The web interface allows you to lock, unlock, and adjust settings for the door lock mechanism.
Components

    Arduino board with Wi-Fi capability (e.g., ESP32)
    SPDT Relays:
        Relay 1 (GPIO 5) - Controls direction for locking.
        Relay 2 (GPIO 18) - Controls direction for unlocking.
        Relay 3 (GPIO 19) - Controls power to the motor.
    Built-in LED (GPIO 2) - Indicates status.

Features

    Wi-Fi Connectivity: Connects to a Wi-Fi network to enable remote control.
    Web Interface: Simple, mobile-friendly interface for controlling the door lock.
    Persistent Storage: Stores timing configurations in non-volatile memory (Preferences library).
    Relay Control: Manages relay operations for locking/unlocking and reversing the motor direction.

Pin Configuration

    Relay 1 Pin (GPIO 5): Controls direction for locking.
    Relay 2 Pin (GPIO 18): Controls direction for unlocking.
    Relay 3 Pin (GPIO 19): Powers the motor.
    LED Pin (GPIO 2): Status indication.

Default Timing Configuration

    Lock Time: 2100 milliseconds
    Unlock Time: 5300 milliseconds
    Lock Reverse Time: 1900 milliseconds
    Unlock Reverse Time: 4900 milliseconds

Installation and Setup

    Connect the Arduino to your relays and other components as per the pin configuration.
    Upload the code to your Arduino board using the Arduino IDE.
    Power up the Arduino and wait for it to connect to Wi-Fi.
    Access the web interface:
        Open a web browser and navigate to the IP address displayed on the serial monitor.

Web Interface

    Main Page: Provides buttons to lock or unlock the door.
    Settings Page: Allows you to adjust the timings for the lock and unlock actions.

Endpoints

    /: Main control page for locking and unlocking.
    /lock: Triggers the door locking sequence.
    /unlock: Triggers the door unlocking sequence.
    /stop: Stops any ongoing door movement.
    /settings: Access the settings page to modify timing parameters.
    /setTimes (POST): Updates the timing configuration. Expects parameters lockTime, unlockTime, lockReverseTime, unlockReverseTime.

Usage

    Locking the Door: Click the "Lock Door" button on the main page, or access the /lock endpoint.
    Unlocking the Door: Click the "Unlock Door" button on the main page, or access the /unlock endpoint.
    Stop Movement: Click the "Stop" button or access the /stop endpoint to stop the door's movement.
    Adjusting Timings: Use the settings page to modify and save the lock and unlock timings.

Troubleshooting

    No Wi-Fi Connection: Ensure the SSID and password are correct and that the network is within range.
    Relays Not Responding: Check wiring and ensure the relays are connected to the correct GPIO pins.
    Incorrect Timing: Adjust the timing parameters through the settings page and save the configuration.
