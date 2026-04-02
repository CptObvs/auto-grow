#pragma once

// ============================================================
// config.h — Konfigurationsparameter Balkon-Bewässerungssystem
// ============================================================
// Alle anpassbaren Werte sind hier zentral definiert.
// Vor dem ersten Start WiFi-Zugangsdaten und Sensor-
// Kalibrierungswerte eintragen!

// ------------------------------------------------------------
// WiFi-Zugangsdaten
// ------------------------------------------------------------
const char* WIFI_SSID     = "DeinNetzwerkName";
const char* WIFI_PASSWORD = "DeinWiFiPasswort";

// ------------------------------------------------------------
// GPIO-Pins — Relais (Active-LOW: LOW = Relais an)
// ------------------------------------------------------------
constexpr int PIN_PUMPE    = 25;  // Relais CH1 → Pumpe
constexpr int PIN_VENTIL_1 = 26;  // Relais CH2 → Ventil 1 (Hochbeet 1)
constexpr int PIN_VENTIL_2 = 27;  // Relais CH3 → Ventil 2 (Hochbeet 2)
constexpr int PIN_VENTIL_3 = 14;  // Relais CH4 → Ventil 3 (Tomate 1)
constexpr int PIN_VENTIL_4 = 12;  // Relais CH5 → Ventil 4 (Tomate 2)

// ------------------------------------------------------------
// GPIO-Pins — Analogsensoren (ADC1, 12-Bit: 0–4095)
// ------------------------------------------------------------
constexpr int PIN_SENSOR_1 = 34;  // ADC ← Sensor 1 (Hochbeet 1)
constexpr int PIN_SENSOR_2 = 35;  // ADC ← Sensor 2 (Hochbeet 2)
constexpr int PIN_SENSOR_3 = 32;  // ADC ← Sensor 3 (Tomate 1)
constexpr int PIN_SENSOR_4 = 33;  // ADC ← Sensor 4 (Tomate 2)

// ------------------------------------------------------------
// Sensor-Kalibrierung (Rohwerte, 12-Bit ADC)
// Trockenwert: Sensor in Luft / trockener Erde
// Nasswert:    Sensor in sehr nasser Erde / Wasser
// Diese Werte variieren je nach Sensor — selbst kalibrieren!
// ------------------------------------------------------------
constexpr int SENSOR_1_TROCKEN = 3300;
constexpr int SENSOR_1_NASS    = 1300;

constexpr int SENSOR_2_TROCKEN = 3300;
constexpr int SENSOR_2_NASS    = 1300;

constexpr int SENSOR_3_TROCKEN = 3300;
constexpr int SENSOR_3_NASS    = 1300;

constexpr int SENSOR_4_TROCKEN = 3300;
constexpr int SENSOR_4_NASS    = 1300;

// ------------------------------------------------------------
// Bewässerungsparameter
// ------------------------------------------------------------

// Prozentualer Feuchtigkeitsschwellwert — unter diesem Wert
// wird eine Zone bewässert (0 = trocken, 100 = nass)
constexpr int FEUCHTE_SCHWELLWERT = 40;

// Maximale Bewässerungszeit pro Zone in Millisekunden (30 s)
constexpr unsigned long BEWAESSERUNGS_DAUER_MS = 30000UL;

// Pause zwischen zwei aufeinanderfolgenden Bewässerungen (ms)
// Verhindert, dass mehrere Zonen gleichzeitig laufen (10 s)
constexpr unsigned long PAUSE_ZWISCHEN_ZONEN_MS = 10000UL;

// Prüfintervall: Alle 30 Minuten Sensoren lesen
constexpr unsigned long PRUEF_INTERVALL_MS = 30UL * 60UL * 1000UL;

// Anzahl der ADC-Messwerte für Mittelwertbildung
constexpr int ADC_MITTELWERT_ANZAHL = 10;
