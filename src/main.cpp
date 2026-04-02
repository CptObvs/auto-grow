/**
 * main.cpp — Balkon-Bewässerungssystem mit ESP32
 *
 * Funktionen:
 *  - Kapazitive Bodenfeuchtesensoren auslesen (ADC1)
 *  - Bewässerungslogik: Feuchte unter Schwellwert → Pumpe + Ventil öffnen
 *  - Timer-basiert: alle 30 Minuten Sensoren prüfen
 *  - Sicherheit: maximale Bewässerungszeit pro Zone, Pause zwischen Zonen
 *  - Serial-Logging für Debugging
 *  - WiFi-Verbindung + einfacher Webserver (Sensorwerte + Ventilstatus)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "config.h"

// ============================================================
// Globale Zustände
// ============================================================

// Aktuelle Feuchtewerte in Prozent (0 = trocken, 100 = nass)
int feuchte[4] = {0, 0, 0, 0};

// Rohwerte der Sensoren (ADC 12-Bit)
int sensorRohwert[4] = {0, 0, 0, 0};

// Ventilstatus: true = offen, false = geschlossen
bool ventilOffen[4] = {false, false, false, false};

// Pumpe läuft
bool pumpeAktiv = false;

// Zeitstempel der letzten Prüfung
unsigned long letzterPruefZeitpunkt = 0;

// Webserver auf Port 80
AsyncWebServer webserver(80);

// ============================================================
// Hilfsfunktionen
// ============================================================

/**
 * Liest einen ADC-Pin mehrfach und gibt den Mittelwert zurück.
 * Reduziert Messrauschen des ESP32-ADC.
 */
int adcMittelwert(int pin) {
    long summe = 0;
    for (int i = 0; i < ADC_MITTELWERT_ANZAHL; i++) {
        summe += analogRead(pin);
        delay(5);
    }
    return (int)(summe / ADC_MITTELWERT_ANZAHL);
}

/**
 * Berechnet Feuchtigkeitsprozent aus Rohwert und Kalibrierungswerten.
 * Rohwert hoch = trocken, Rohwert niedrig = nass.
 */
int rohwertZuProzent(int rohwert, int trocken, int nass) {
    int prozent = map(rohwert, trocken, nass, 0, 100);
    return constrain(prozent, 0, 100);
}

/**
 * Schaltet Pumpe und ein Ventil ein, wartet die Bewässerungsdauer,
 * schaltet dann beide wieder aus.
 * Active-LOW Relais: LOW = an, HIGH = aus.
 */
void zoneBewaessern(int ventilIndex) {
    const int ventilPins[4] = {PIN_VENTIL_1, PIN_VENTIL_2, PIN_VENTIL_3, PIN_VENTIL_4};
    const char* zoneNamen[4] = {"Hochbeet 1", "Hochbeet 2", "Tomate 1", "Tomate 2"};

    Serial.printf("[Bewässerung] Zone %d (%s) wird bewässert...\n",
                  ventilIndex + 1, zoneNamen[ventilIndex]);

    // Ventil und Pumpe einschalten (Active-LOW)
    digitalWrite(ventilPins[ventilIndex], LOW);
    digitalWrite(PIN_PUMPE, LOW);
    ventilOffen[ventilIndex] = true;
    pumpeAktiv = true;

    delay(BEWAESSERUNGS_DAUER_MS);

    // Ventil und Pumpe ausschalten
    digitalWrite(PIN_PUMPE, HIGH);
    digitalWrite(ventilPins[ventilIndex], HIGH);
    pumpeAktiv = false;
    ventilOffen[ventilIndex] = false;

    Serial.printf("[Bewässerung] Zone %d fertig. Pause %lu s...\n",
                  ventilIndex + 1, PAUSE_ZWISCHEN_ZONEN_MS / 1000);
    delay(PAUSE_ZWISCHEN_ZONEN_MS);
}

/**
 * Liest alle 4 Sensoren aus und speichert Roh- und Prozentwerte.
 */
void sensorenLesen() {
    const int pins[4]     = {PIN_SENSOR_1, PIN_SENSOR_2, PIN_SENSOR_3, PIN_SENSOR_4};
    const int trocken[4]  = {SENSOR_1_TROCKEN, SENSOR_2_TROCKEN,
                              SENSOR_3_TROCKEN, SENSOR_4_TROCKEN};
    const int nass[4]     = {SENSOR_1_NASS, SENSOR_2_NASS,
                              SENSOR_3_NASS, SENSOR_4_NASS};

    Serial.println("[Sensoren] Messung läuft...");
    for (int i = 0; i < 4; i++) {
        sensorRohwert[i] = adcMittelwert(pins[i]);
        feuchte[i] = rohwertZuProzent(sensorRohwert[i], trocken[i], nass[i]);
        Serial.printf("  Sensor %d: Rohwert=%d, Feuchte=%d%%\n",
                      i + 1, sensorRohwert[i], feuchte[i]);
    }
}

/**
 * Prüft alle Zonen und startet Bewässerung wenn nötig.
 */
void bewaesserungPruefen() {
    Serial.println("[Prüfung] Bewässerungsbedarf wird geprüft...");
    for (int i = 0; i < 4; i++) {
        if (feuchte[i] < FEUCHTE_SCHWELLWERT) {
            Serial.printf("[Prüfung] Zone %d: Feuchte %d%% < Schwellwert %d%% → Bewässerung!\n",
                          i + 1, feuchte[i], FEUCHTE_SCHWELLWERT);
            zoneBewaessern(i);
        } else {
            Serial.printf("[Prüfung] Zone %d: Feuchte %d%% — OK, kein Bedarf.\n",
                          i + 1, feuchte[i]);
        }
    }
    Serial.println("[Prüfung] Abgeschlossen.");
}

// ============================================================
// Webserver — HTML-Seite
// ============================================================

/**
 * Erstellt die HTML-Antwort mit aktuellem Status aller Zonen.
 */
String htmlSeiteErstellen() {
    const char* zoneNamen[4] = {"Hochbeet 1", "Hochbeet 2", "Tomate 1", "Tomate 2"};

    String html = R"rawliteral(<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="30">
  <title>auto-grow — Bewässerungssystem</title>
  <style>
    body { font-family: sans-serif; max-width: 600px; margin: 2rem auto; padding: 0 1rem; }
    h1 { color: #2d6a2d; }
    table { width: 100%; border-collapse: collapse; margin: 1rem 0; }
    th, td { padding: 0.5rem 1rem; border: 1px solid #ccc; text-align: left; }
    th { background: #e8f5e9; }
    .nass { color: #1565c0; }
    .trocken { color: #b71c1c; }
    .aktiv { color: #2e7d32; font-weight: bold; }
    .inaktiv { color: #757575; }
    .status { margin: 1rem 0; padding: 0.5rem 1rem; border-radius: 4px; }
    .pumpe-an { background: #c8e6c9; }
    .pumpe-aus { background: #f5f5f5; }
    small { color: #888; }
  </style>
</head>
<body>
  <h1>🌱 auto-grow</h1>
  <p>Balkon-Bewässerungssystem | <small>Seite lädt automatisch alle 30 s</small></p>
)rawliteral";

    // Pumpenstatus
    html += "<div class=\"status ";
    html += pumpeAktiv ? "pumpe-an\">🔵 Pumpe: <strong>LÄUFT</strong>" : "pumpe-aus\">⚪ Pumpe: <strong>AUS</strong>";
    html += "</div>\n";

    // Zonentabelle
    html += "<table>\n";
    html += "<tr><th>Zone</th><th>Feuchte</th><th>Rohwert</th><th>Ventil</th></tr>\n";

    for (int i = 0; i < 4; i++) {
        bool trocken = feuchte[i] < FEUCHTE_SCHWELLWERT;
        html += "<tr>";
        html += "<td>" + String(zoneNamen[i]) + "</td>";
        html += "<td class=\"" + String(trocken ? "trocken" : "nass") + "\">";
        html += String(feuchte[i]) + "%" + String(trocken ? " ⚠️" : " ✅");
        html += "</td>";
        html += "<td>" + String(sensorRohwert[i]) + "</td>";
        html += "<td class=\"" + String(ventilOffen[i] ? "aktiv" : "inaktiv") + "\">";
        html += ventilOffen[i] ? "OFFEN 💧" : "ZU";
        html += "</td></tr>\n";
    }

    html += "</table>\n";
    html += "<p><small>Schwellwert: " + String(FEUCHTE_SCHWELLWERT) + "% | ";
    html += "Nächste Prüfung in: ~" +
            String((PRUEF_INTERVALL_MS - (millis() - letzterPruefZeitpunkt)) / 60000) +
            " min</small></p>\n";
    html += "</body></html>";
    return html;
}

// ============================================================
// Setup
// ============================================================

void setup() {
    Serial.begin(115200);
    Serial.println("\n[System] Balkon-Bewässerungssystem startet...");

    // Relais-Pins als Ausgang — HIGH = Relais aus (Active-LOW)
    const int relaisPins[5] = {PIN_PUMPE, PIN_VENTIL_1, PIN_VENTIL_2,
                                PIN_VENTIL_3, PIN_VENTIL_4};
    for (int pin : relaisPins) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);  // Alle Relais initial ausgeschaltet
    }
    Serial.println("[System] Relais initialisiert (alle aus).");

    // ADC-Auflösung auf 12 Bit setzen
    analogReadResolution(12);
    Serial.println("[System] ADC: 12-Bit Auflösung.");

    // WiFi verbinden
    Serial.printf("[WiFi] Verbinde mit \"%s\"...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int versuche = 0;
    while (WiFi.status() != WL_CONNECTED && versuche < 30) {
        delay(1000);
        Serial.print(".");
        versuche++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("\n[WiFi] Verbunden! IP-Adresse: %s\n",
                      WiFi.localIP().toString().c_str());

        // Webserver-Route registrieren
        webserver.on("/", HTTP_GET, [](AsyncWebServerRequest* anfrage) {
            anfrage->send(200, "text/html", htmlSeiteErstellen());
        });
        webserver.begin();
        Serial.println("[Webserver] Gestartet auf Port 80.");
    } else {
        Serial.println("\n[WiFi] Verbindung fehlgeschlagen — Offline-Betrieb.");
    }

    // Erste Messung sofort durchführen
    sensorenLesen();
    bewaesserungPruefen();
    letzterPruefZeitpunkt = millis();

    Serial.println("[System] Bereit. Warte auf nächsten Prüfzyklus...");
}

// ============================================================
// Hauptschleife
// ============================================================

void loop() {
    unsigned long jetztMs = millis();

    // Alle 30 Minuten Sensoren prüfen und ggf. bewässern
    if (jetztMs - letzterPruefZeitpunkt >= PRUEF_INTERVALL_MS) {
        Serial.printf("\n[Timer] %lu ms seit letzter Prüfung — neuer Zyklus.\n",
                      jetztMs - letzterPruefZeitpunkt);
        sensorenLesen();
        bewaesserungPruefen();
        letzterPruefZeitpunkt = millis();
    }

    delay(1000);
}
