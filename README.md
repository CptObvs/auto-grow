# auto-grow 🌱

ESP32-gesteuertes Bewässerungssystem für Balkon-Hochbeete — automatisiert die Bewässerung anhand von Echtzeit-Bodenfeuchtewerten.

---

## Projektübersicht

4 Zonen (Hochbeet 1, Hochbeet 2, Tomate 1, Tomate 2) werden unabhängig voneinander bewässert. Ein ESP32 liest kapazitive Bodenfeuchtesensoren aus und öffnet bei Bedarf das zugehörige Magnetventil sowie die Pumpe.

Firmware-Basis: **[ESPHome](https://github.com/esphome/esphome)** — Konfiguration per YAML, kein manuelles Programmieren nötig. Das eingebaute Web-Interface ist unter `http://<IP>/` erreichbar und bietet:

- Echtzeit-Feuchtigkeitswerte aller 4 Zonen
- Schwellwerte und Bewässerungsdauer direkt im Browser einstellen (persistent im Flash)
- Bewässerungsintervall wählbar (5 min Test / 1h / 6h / 12h / 24h)
- Manuelle Bewässerung einzelner Zonen per Knopfdruck
- OTA-Firmware-Updates — kein USB nach dem ersten Flash nötig

---

## Projektstruktur

```
auto-grow/
├── README.md
├── auto-grow.yaml         ← ESPHome Konfiguration
├── secrets.yaml           ← WiFi/OTA/API Zugangsdaten (nicht committen)
├── secrets.yaml.example   ← Vorlage
├── flash.sh               ← Flash-Skript (Linux/macOS)
├── compile.bat            ← Compile-Skript (Windows)
└── docs/
    ├── schaltplan.md      ← Stromlaufplan
    ├── wassersystem.md    ← Schlauch- und Wasserflussplan
    └── aufbau.md          ← Schritt-für-Schritt Aufbauanleitung
```

---

## Einkaufsliste

### Amazon

| # | Teil | Link |
|---|------|------|
| 1 | ESP32 DevKit V1 TYPE-C | [amazon.de](https://amzn.eu/d/05KvfIrH) |
| 2 | 12V 5A Netzteil | [amazon.de](https://amzn.eu/d/09q50eiW) |
| 3 | Pumpe 12V Membranpumpe | [amazon.de](https://amzn.eu/d/0dt5nRb8) |
| 4 | Kapazitive Bodenfeuchtesensoren v1.2 | [amazon.de](https://amzn.eu/d/06stYvHB) |
| 5 | Silikonschlauch 6×9mm | [amazon.de](https://amzn.eu/d/0gMqrNm9) |
| 6 | Steuerleitung 4×0,3mm² | [amazon.de](https://amzn.eu/d/0e25FQ1N) |
| 7 | 8-Kanal Relaismodul 12V (High/Low Trigger) | [amazon.de](https://amzn.eu/d/0eBUVhZA) |

### AliExpress

| # | Teil | Link |
|---|------|------|
| 8 | Mini-Magnetventil 12V NC 6mm | [aliexpress.com](https://de.aliexpress.com/item/1005009365155941.html) |
| 9 | Buck Converter (einstellbar, mit Display) | [aliexpress.com](https://de.aliexpress.com/item/1005006982300493.html) |
| 10 | Gleichrichterdioden (Freilaufdioden, 10er+) | [aliexpress.com](https://de.aliexpress.com/item/1005009923993443.html) |
| 11 | T-Stücke 6mm Schlauch | [aliexpress.com](https://de.aliexpress.com/item/1005001610001402.html) |

---

## Hardware-Übersicht

| Komponente | Beschreibung |
|-----------|-------------|
| ESP32 DevKit TYPE-C | Mikrocontroller, 3,3V Logik |
| 12V 5A Netzteil | Hauptstromversorgung für Pumpe, Ventile, Relaisplatine |
| Buck Converter | 12V → 5V für ESP32 VIN (intern weiter auf 3,3V) |
| 8-Kanal Relaismodul | 12V Spulen (SRD-12VDC-SL-C), HIGH-Level Trigger, Jumper pro Kanal |
| Pumpe 12V | Membranpumpe, selbstansaugend, ~0,3A |
| 4× Magnetventil 12V NC | Normalerweise geschlossen — bei Stromverlust kein Wasserfluss |
| 4× Bodenfeuchte-Sensor | Kapazitiv v1.2, 3,3V, analoger Spannungsausgang 1,2–2,7V |
| Gleichrichterdioden | Freilaufdioden antiparallel zu jeder Last (Pumpe + 4 Ventile) |

---

## GPIO-Pinbelegung

| GPIO | Funktion | Richtung |
|------|----------|----------|
| GPIO25 | Relais CH1 → Pumpe | Ausgang |
| GPIO26 | Relais CH2 → Ventil Hochbeet 1 | Ausgang |
| GPIO27 | Relais CH3 → Ventil Hochbeet 2 | Ausgang |
| GPIO14 | Relais CH4 → Ventil Tomate 1 | Ausgang |
| GPIO12 | Relais CH5 → Ventil Tomate 2 | Ausgang |
| GPIO34 | ADC ← Sensor Hochbeet 1 | Eingang analog |
| GPIO35 | ADC ← Sensor Hochbeet 2 | Eingang analog |
| GPIO32 | ADC ← Sensor Tomate 1 | Eingang analog |
| GPIO33 | ADC ← Sensor Tomate 2 | Eingang analog |

> **Relais HIGH-Level Trigger:** Jumper am Modul auf HIGH stellen. GPIO HIGH (3,3V) = Relais zieht an. Interner Pulldown hält alle Pins LOW bei Stromverlust/Boot → kein ungewolltes Schalten.

---

## Stromversorgung

```
12V Netzteil
  ├── → Buck Converter (12V→5V) → ESP32 VIN → intern 3,3V → Sensoren VCC
  ├── → Relaismodul DC+ (12V Spulenversorgung)
  └── → Relaismodul COM 1–5 (geschaltete 12V an Pumpe/Ventile)

5V Buck-Ausgang → Relaismodul VCC (Logik/Optokoppler)
GND durchgehend gemeinsam
```

---

## Web-Interface

| Bereich | Inhalt |
|---------|--------|
| 🌱 Bodenfeuchte | Feuchtigkeitswerte 0–100 % aller 4 Zonen |
| ⚙️ Schwellwerte & Zeiten | Schwellwert pro Zone (10–80 %), Bewässerungsdauer (5–120 s), Intervall (5 min–24 h) |
| 💧 Ventile & Pumpe | Status, manuell schalten (Interlock: max. 1 Ventil gleichzeitig) |
| 🔘 Manuelle Steuerung | Jetzt prüfen, Zone 1–4 manuell, Safe Mode, Neustart |
| 📡 System | WiFi-Signal, Uptime, ESP32-Temperatur, IP-Adresse |

---

## Sicherheits-Features

- **Ventil-Interlock** — Maximal 1 Ventil gleichzeitig offen
- **Pumpen-Schutz** — Pumpe startet nur wenn mindestens ein Ventil offen ist
- **Pumpen-Timeout** — Abschaltung nach max. 3 Minuten (nicht-blockierend im Hintergrund)
- **Relay-Failsafe** — HIGH-Level Trigger + Pulldown: Relais fällt bei Stromverlust/Reset garantiert ab
- **ALWAYS_OFF** — Nach jedem Neustart sind alle Ausgänge explizit aus
- **NC-Ventile** — Magnetventile schließen stromlos → kein unkontrollierter Wasserfluss

---

## Sensoren kalibrieren

Die Sensoren geben eine analoge Spannung aus: trocken = hohe Spannung, nass = niedrige Spannung.

```bash
esphome logs auto-grow.yaml   # Rohwerte im Log beobachten
```

1. Sensor in trockene Erde / Luft halten → Spannung ablesen (~2,7 V)
2. Sensor in Wasser tauchen → Spannung ablesen (~1,2 V)
3. Werte in `auto-grow.yaml` unter `calibrate_linear` eintragen:

```yaml
- calibrate_linear:
    - 2.7 -> 0      # V_trocken → 0 %
    - 1.2 -> 100    # V_nass   → 100 %
```

> Jeden Sensor einzeln kalibrieren — Werte weichen je nach Exemplar leicht ab.

---

## Schnellstart

```bash
# 1. ESPHome installieren
pip install esphome

# 2. Zugangsdaten konfigurieren
cp secrets.yaml.example secrets.yaml
# secrets.yaml öffnen und WiFi, OTA-Passwort, API-Key eintragen

# 3. Erstmalig per USB flashen
esphome run auto-grow.yaml

# 4. IP aus dem Log ablesen, Browser öffnen
# http://192.168.x.x/

# 5. Ab jetzt OTA (kein USB mehr nötig)
esphome run auto-grow.yaml
```

---

## Weitere Dokumentation

- 📐 [Schaltplan / Stromlaufplan](docs/schaltplan.md)
- 🚿 [Wassersystem-Diagramm](docs/wassersystem.md)
- 🔧 [Aufbauanleitung](docs/aufbau.md)
