// Copy this file to src/secrets.h and set your own values.
//
//     cp src/secrets.example.h src/secrets.h
//
// src/secrets.h is gitignored, so your real values never reach the repository.
// If it is missing the firmware still builds, using the placeholder below and
// printing a warning at boot.
#pragma once

// Default WiFi password for the node's own access point.
// WPA2 requires at least 8 characters.
#define AP_PASS_DEFAULT "changeme123"
