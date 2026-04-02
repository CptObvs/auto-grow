# auto-grow 🌱

ESP32-powered smart irrigation system for balcony raised beds — automates watering based on real-time soil moisture readings

---

## Projektübersicht

Automatisches Bewässerungssystem für 4 Hochbeete/Pflanztöpfe auf dem Balkon. Ein ESP32 liest kapazitive Bodenfeuchtesensoren aus und öffnet bei Bedarf jeweils ein Magnetventil + die Pumpe, um die Pflanze zu bewässern. Alle 30 Minuten wird geprüft, ob Bewässerungsbedarf besteht.

Das Webinterface basiert auf **[ESP-DASH](https://github.com/ayushsharma82/ESP-DASH)** — einem quelloffenen ESP32/ESP8266 Dashboard mit Echtzeit-WebSocket-Updates, Fortschrittsbalken für Feuchtigkeitswerte und Buttons für manuelle Bewässerung.

## Projektstruktur

```
auto-grow/
├── README.md
├── platformio.ini
├── src/
│   └── main.cpp
├── include/
│   └── config.h
└── docs/
    ├── schaltplan.md
    ├── wassersystem.md
    └── aufbau.md
```

---

## Einkaufsliste

| # | Teil | Menge | Woher | Preis |
|---|------|-------|-------|-------|
| 1 | ESP32 DevKit TYPE-C | 1 | Amazon | ~8€ |
| 2 | 12V 5A Netzteil | 1 | Amazon | ~12€ |
| 3 | DC-Buchse 5,5×2,1mm Schraubklemme | 1 | Amazon | ~5€ |
| 4 | Pumpe RUNCCI-YUN R385 2er Pack | 1 | Amazon | ~12€ |
| 5 | Buck Converter LM2596 mit Display | 1 | AliExpress | ~2€ |
| 6 | 6-Kanal Relaismodul 5V | 1 | AliExpress | ~3€ |
| 7 | Mini-Ventile 12V NC 6mm | 4 | AliExpress | ~2€/Stk |
| 8 | Dioden 1N4007 50er Pack | 1 | AliExpress | ~1€ |
| 9 | Bodensensoren kapazitiv v1.2 5er Pack | 1 | AliExpress | ~4€ |
| 10 | Silikonschlauch 6×9mm 5m | 2 | AliExpress | ~4€/Stk |
| 11 | T-Stücke 6mm | 3 | AliExpress | ~0,50€/Stk |
| 12 | Schlauchschellen 8-10mm 10er | 1 | AliExpress | ~1,50€ |
| 13 | Schrumpfschlauch-Set 127 Stk | 1 | AliExpress | ~1,50€ |
| 14 | Steuerleitung 4×0,3mm² schwarz 10m | 1 | Amazon | ~12€ |
| 15 | Kabelbinder schwarz mini 100er | 1 | Amazon | ~2€ |
| **Gesamt** | | | | **~75€** |

---

## Hardware-Übersicht

### Komponenten

1. **ESP32 DevKit TYPE-C** — Mikrocontroller
2. **12V 5A Netzteil** — Hauptstromversorgung
3. **DC-Buchse 5,5×2,1mm** mit Schraubklemme — Netzteil-Anschluss
4. **Buck Converter LM2596 mit Display** — 12V → 5V Wandler für ESP32 und Relaismodul
5. **6-Kanal Relaismodul 5V** — Schaltet Ventile und Pumpe
6. **Pumpe RUNCCI-YUN R385** — 12V Mini-Membranpumpe (~0,3A, 1,5–2 L/min, selbstansaugend, 5mm Stutzen)
7. **4× Mini-Magnetventile 12V NC** — 6mm Schlauchanschluss direkt (kein Gewinde)
8. **5× Kapazitive Bodenfeuchtesensoren v1.2** — 3,3V, analog (4 aktiv + 1 Reserve)
9. **Dioden 1N4007** — Freilaufdioden an jedem Ventil und der Pumpe (5 Stück)
10. **Silikonschlauch 6×9mm** — 2×5m
11. **T-Stücke 6mm** — 3 Stück für Wasserverteilung
12. **Schlauchschellen 8-10mm** — 10er Pack
13. **Schrumpfschlauch-Set** — 127 Stück verschiedene Größen
14. **Steuerleitung 4×0,3mm²** — 10m, führt pro Standort 12V, GND, 3,3V und Signal
15. **Kabelbinder schwarz mini** — 100er Pack

---

## GPIO-Pinbelegung

| GPIO | Funktion | Richtung |
|------|----------|----------|
| GPIO25 | Relais CH1 → Pumpe | Ausgang |
| GPIO26 | Relais CH2 → Ventil 1 (Hochbeet 1) | Ausgang |
| GPIO27 | Relais CH3 → Ventil 2 (Hochbeet 2) | Ausgang |
| GPIO14 | Relais CH4 → Ventil 3 (Tomate 1) | Ausgang |
| GPIO12 | Relais CH5 → Ventil 4 (Tomate 2) | Ausgang |
| GPIO34 | ADC ← Sensor 1 (Hochbeet 1) | Eingang analog |
| GPIO35 | ADC ← Sensor 2 (Hochbeet 2) | Eingang analog |
| GPIO32 | ADC ← Sensor 3 (Tomate 1) | Eingang analog |
| GPIO33 | ADC ← Sensor 4 (Tomate 2) | Eingang analog |

> **Hinweis:** Das Relaismodul ist Active-LOW — GPIO HIGH = Relais offen (Ventil zu), GPIO LOW = Relais geschlossen (Ventil auf).

---

## Sensoren kalibrieren

Die kapazitiven Sensoren v1.2 geben einen analogen Wert zwischen 0 und 4095 (12-Bit ADC) zurück.

### Kalibrierungsverfahren

1. **Trockenwert ermitteln:**
   - Sensor in trockene Erde stecken (oder in Luft halten)
   - Seriellen Monitor öffnen (115200 Baud)
   - Angezeigten Rohwert notieren → `SENSOR_x_TROCKEN` in `config.h` eintragen

2. **Nasswert ermitteln:**
   - Sensor in nasse/frisch gegossene Erde stecken
   - Rohwert notieren → `SENSOR_x_NASS` in `config.h` eintragen

3. **Schwellwert festlegen:**
   - `FEUCHTE_SCHWELLWERT` in `config.h` auf ~40% setzen
   - Wenn berechnete Feuchte unter 40% → Bewässerung startet

### Typische Rohwerte (Sensor v1.2, 3,3V)

| Zustand | ADC-Rohwert |
|---------|-------------|
| Trocken (Luft) | ~3200–3500 |
| Nass (Wasser) | ~1200–1500 |

> Werte können je nach Sensor leicht abweichen — immer selbst kalibrieren!

---

## Webinterface (ESP-DASH)

Das Dashboard läuft direkt auf dem ESP32 und ist im Browser unter der IP-Adresse des ESP32 erreichbar. Es basiert auf **[ESP-DASH](https://github.com/ayushsharma82/ESP-DASH)** von [@ayushsharma82](https://github.com/ayushsharma82).

| Karte | Typ | Inhalt |
|-------|-----|--------|
| Hochbeet 1–4 | Fortschrittsbalken | Bodenfeuchte in % (0 = trocken, 100 = nass) |
| Zone 1–4 manuell | Button | Manuelle Bewässerung der jeweiligen Zone auslösen |
| Pumpe | Status | Zeigt ob die Pumpe gerade läuft |
| Nächste Prüfung | Info | Minuten bis zur nächsten automatischen Prüfung |

Seite aktualisiert sich automatisch per WebSocket — kein manuelles Neuladen nötig.

---



1. Repository klonen
2. [PlatformIO](https://platformio.org/) installieren (VSCode-Erweiterung oder CLI)
3. `include/config.h` öffnen und WiFi-Zugangsdaten eintragen:
   ```cpp
   const char* WIFI_SSID = "DeinNetzwerk";
   const char* WIFI_PASSWORD = "DeinPasswort";
   ```
4. Sensoren kalibrieren und Werte in `config.h` eintragen
5. Firmware auf den ESP32 flashen:
   ```bash
   pio run --target upload
   ```
6. Seriellen Monitor öffnen um Logs zu sehen:
   ```bash
   pio device monitor
   ```
7. Im Browser die IP-Adresse des ESP32 aufrufen → Webinterface

---

## Weitere Dokumentation

- 📐 [Schaltplan / Stromlaufplan](docs/schaltplan.md)
- 🚿 [Wassersystem-Diagramm](docs/wassersystem.md)
- 🔧 [Schritt-für-Schritt Aufbauanleitung](docs/aufbau.md)
