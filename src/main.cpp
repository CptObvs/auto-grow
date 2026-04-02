/**
 * main.cpp — Balkon-Bewässerungssystem mit ESP32
 *
 * Funktionen:
 *  - Kapazitive Bodenfeuchtesensoren auslesen (ADC1)
 *  - Bewässerungslogik: Feuchte unter Schwellwert → Pumpe + Ventil öffnen
 *  - Timer-basiert: alle 30 Minuten Sensoren prüfen
 *  - Sicherheit: maximale Bewässerungszeit pro Zone, Pause zwischen Zonen
 *  - Serial-Logging für Debugging
 *  - WiFi-Verbindung + ESP-DASH Webinterface (https://github.com/ayushsharma82/ESP-DASH)
 *    → Fortschrittsbalken für Bodenfeuchte, Buttons für manuelle Bewässerung,
 *      Pumpenstatus, Echtzeit-Updates per WebSocket
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPDash.h>
#include "config.h"

// ============================================================
// Webserver und ESP-DASH Dashboard
// ============================================================

AsyncWebServer server(80);
ESPDash dashboard(&server);

// Fortschrittsbalken: Bodenfeuchte je Zone (0–100 %)
Card feuchteKarte1(&dashboard, PROGRESS_CARD, "Hochbeet 1",   "%");
Card feuchteKarte2(&dashboard, PROGRESS_CARD, "Hochbeet 2",   "%");
Card feuchteKarte3(&dashboard, PROGRESS_CARD, "Tomate 1",     "%");
Card feuchteKarte4(&dashboard, PROGRESS_CARD, "Tomate 2",     "%");

// Buttons: Manuelle Bewässerung pro Zone
Card btnZone1(&dashboard, BUTTON_CARD, "Zone 1 manuell");
Card btnZone2(&dashboard, BUTTON_CARD, "Zone 2 manuell");
Card btnZone3(&dashboard, BUTTON_CARD, "Zone 3 manuell");
Card btnZone4(&dashboard, BUTTON_CARD, "Zone 4 manuell");

// Status: Pumpe läuft / aus
Card pumpenStatus(&dashboard, STATUS_CARD, "Pumpe");

// Info: Minuten bis zur nächsten automatischen Prüfung
Card naechstePruefung(&dashboard, GENERIC_CARD, "Nächste Prüfung", "min");

// ============================================================
// Globale Zustände
// ============================================================

// Aktuelle Feuchtewerte in Prozent (0 = trocken, 100 = nass)
int feuchte[4] = {0, 0, 0, 0};

// Rohwerte der Sensoren (ADC 12-Bit, 0–4095)
int sensorRohwert[4] = {0, 0, 0, 0};

// Ventilstatus: true = offen, false = geschlossen
bool ventilOffen[4] = {false, false, false, false};

// Pumpe läuft
bool pumpeAktiv = false;

// Flags für manuelle Bewässerung — gesetzt von Button-Callbacks,
// ausgeführt im nächsten loop()-Durchlauf (thread-sicher)
volatile bool manuelleBewaesserung[4] = {false, false, false, false};

// Zeitstempel der letzten Prüfung
unsigned long letzterPruefZeitpunkt = 0;

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
 * Aktualisiert alle ESP-DASH Karten mit aktuellen Werten.
 */
void dashboardAktualisieren() {
    feuchteKarte1.update(feuchte[0]);
    feuchteKarte2.update(feuchte[1]);
    feuchteKarte3.update(feuchte[2]);
    feuchteKarte4.update(feuchte[3]);

    if (pumpeAktiv) {
        pumpenStatus.update("LÄUFT", "success");
    } else {
        pumpenStatus.update("AUS", "idle");
    }

    unsigned long vergangenMs = millis() - letzterPruefZeitpunkt;
    int restMinuten = (int)((PRUEF_INTERVALL_MS - min(vergangenMs, PRUEF_INTERVALL_MS)) / 60000);
    naechstePruefung.update(restMinuten);

    dashboard.sendUpdates();
}

/**
 * Schaltet Pumpe und ein Ventil ein, wartet die Bewässerungsdauer,
 * schaltet dann beide wieder aus.
 * Active-LOW Relais: LOW = an, HIGH = aus.
 */
void zoneBewaessern(int ventilIndex) {
    const int ventilPins[4]    = {PIN_VENTIL_1, PIN_VENTIL_2, PIN_VENTIL_3, PIN_VENTIL_4};
    const char* zoneNamen[4]   = {"Hochbeet 1", "Hochbeet 2", "Tomate 1", "Tomate 2"};

    Serial.printf("[Bewässerung] Zone %d (%s) wird bewässert...\n",
                  ventilIndex + 1, zoneNamen[ventilIndex]);

    // Ventil und Pumpe einschalten (Active-LOW)
    digitalWrite(ventilPins[ventilIndex], LOW);
    digitalWrite(PIN_PUMPE, LOW);
    ventilOffen[ventilIndex] = true;
    pumpeAktiv = true;
    dashboardAktualisieren();

    delay(BEWAESSERUNGS_DAUER_MS);

    // Ventil und Pumpe ausschalten
    digitalWrite(PIN_PUMPE, HIGH);
    digitalWrite(ventilPins[ventilIndex], HIGH);
    pumpeAktiv = false;
    ventilOffen[ventilIndex] = false;
    dashboardAktualisieren();

    Serial.printf("[Bewässerung] Zone %d fertig. Pause %lu s...\n",
                  ventilIndex + 1, PAUSE_ZWISCHEN_ZONEN_MS / 1000);
    delay(PAUSE_ZWISCHEN_ZONEN_MS);
}

/**
 * Liest alle 4 Sensoren aus und speichert Roh- und Prozentwerte.
 */
void sensorenLesen() {
    const int pins[4]    = {PIN_SENSOR_1, PIN_SENSOR_2, PIN_SENSOR_3, PIN_SENSOR_4};
    const int trocken[4] = {SENSOR_1_TROCKEN, SENSOR_2_TROCKEN,
                             SENSOR_3_TROCKEN, SENSOR_4_TROCKEN};
    const int nass[4]    = {SENSOR_1_NASS, SENSOR_2_NASS,
                             SENSOR_3_NASS, SENSOR_4_NASS};

    Serial.println("[Sensoren] Messung läuft...");
    for (int i = 0; i < 4; i++) {
        sensorRohwert[i] = adcMittelwert(pins[i]);
        feuchte[i] = rohwertZuProzent(sensorRohwert[i], trocken[i], nass[i]);
        Serial.printf("  Sensor %d: Rohwert=%d, Feuchte=%d%%\n",
                      i + 1, sensorRohwert[i], feuchte[i]);
    }
    dashboardAktualisieren();
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

    // ESP-DASH Button-Callbacks registrieren
    // Callback läuft im WebServer-Task → nur Flag setzen, Ausführung in loop()
    btnZone1.attachCallback([](int val) { if (val) manuelleBewaesserung[0] = true; });
    btnZone2.attachCallback([](int val) { if (val) manuelleBewaesserung[1] = true; });
    btnZone3.attachCallback([](int val) { if (val) manuelleBewaesserung[2] = true; });
    btnZone4.attachCallback([](int val) { if (val) manuelleBewaesserung[3] = true; });

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
        server.begin();
        Serial.println("[Webserver] ESP-DASH gestartet auf Port 80.");
        Serial.println("[Webserver] https://github.com/ayushsharma82/ESP-DASH");
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

    // Manuelle Bewässerung über Dashboard-Button
    for (int i = 0; i < 4; i++) {
        if (manuelleBewaesserung[i]) {
            manuelleBewaesserung[i] = false;
            Serial.printf("[Manuell] Zone %d ausgelöst über Dashboard.\n", i + 1);
            zoneBewaessern(i);
            sensorenLesen();
        }
    }

    // Alle 30 Minuten Sensoren prüfen und ggf. bewässern
    if (jetztMs - letzterPruefZeitpunkt >= PRUEF_INTERVALL_MS) {
        Serial.printf("\n[Timer] %lu ms seit letzter Prüfung — neuer Zyklus.\n",
                      jetztMs - letzterPruefZeitpunkt);
        sensorenLesen();
        bewaesserungPruefen();
        letzterPruefZeitpunkt = millis();
    }

    // Dashboard-Countdown aktualisieren (jede Minute)
    static unsigned long letztesDashboardUpdate = 0;
    if (jetztMs - letztesDashboardUpdate >= 60000) {
        dashboardAktualisieren();
        letztesDashboardUpdate = jetztMs;
    }

    delay(1000);
}

