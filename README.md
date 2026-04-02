# auto-grow 🌱

ESP32-powered smart irrigation system for balcony raised beds — automates watering based on real-time soil moisture readings

---

## Projektübersicht

Automatisches Bewässerungssystem für 4 Hochbeete/Pflanztöpfe auf dem Balkon. Ein ESP32 liest kapazitive Bodenfeuchtesensoren aus und öffnet bei Bedarf jeweils ein Magnetventil plus die Pumpe, um die Pflanze zu bewässern.

Die Firmware basiert auf **[ESPHome](https://github.com/esphome/esphome)** — einem weit verbreiteten Open-Source-Projekt für ESP32/ESP8266, das Firmware aus einer YAML-Konfigurationsdatei generiert. Das eingebaute Web-Interface erlaubt:

- 📊 Echtzeit-Anzeige aller Bodenfeuchtewerte
- ⚙️ Schwellwerte und Bewässerungsdauer direkt im Browser einstellen (werden im Flash gespeichert — kein Neu-Flashen nötig)
- 💧 Manuelle Bewässerung einzelner Zonen per Knopfdruck
- 🔄 Over-the-Air (OTA) Firmware-Updates — kein USB nach dem ersten Flash nötig

## Projektstruktur

```
auto-grow/
├── README.md
├── auto-grow.yaml        ← ESPHome Konfiguration (Haupt-Firmware)
├── secrets.yaml          ← WiFi-Zugangsdaten (nicht committen! in .gitignore)
├── secrets.yaml.example  ← Vorlage für secrets.yaml
├── .gitignore
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

> **Hinweis:** Das Relaismodul ist Active-LOW — GPIO HIGH = Relais offen (Ventil zu), GPIO LOW = Relais geschlossen (Ventil auf). ESPHome übernimmt das automatisch via `inverted: true`.

---

## Web-Interface (ESPHome)

Die Firmware basiert auf **[ESPHome](https://github.com/esphome/esphome)** von [@esphome](https://github.com/esphome). Das Dashboard ist unter `http://<IP-Adresse>/` erreichbar und in vier Bereiche gegliedert:

| Bereich | Inhalt |
|---------|--------|
| 🌱 Bodenfeuchte | Aktuelle Feuchtigkeitswerte aller 4 Zonen in % |
| ⚙️ Schwellwerte & Zeiten | Schwellwert pro Zone (10–80 %) + Bewässerungsdauer (5–120 s) als Schieberegler — werden im Flash gespeichert |
| 💧 Ventile & Pumpe | Aktueller Status, manuell ein-/ausschalten |
| 🔘 Manuelle Steuerung | „Jetzt prüfen", Zone 1–4 manuell bewässern, ESP32 neu starten |

Seite aktualisiert sich automatisch. Alle Einstellungen bleiben nach einem Neustart erhalten.

---

## Sensoren kalibrieren

Die kapazitiven Sensoren v1.2 geben eine analoge Spannung aus (0–3,3V bei `attenuation: 11dB`).

### Kalibrierungsverfahren

1. **ESPHome Logs öffnen:**
   ```bash
   esphome logs auto-grow.yaml
   ```
   oder seriellen Monitor (115200 Baud)

2. **Trockenwert ermitteln:**
   - Sensor in Luft oder trockene Erde halten
   - Gemessene Spannung ablesen (typisch ~3,0 V)

3. **Nasswert ermitteln:**
   - Sensor ins Wasser tauchen
   - Gemessene Spannung ablesen (typisch ~1,2 V)

4. **In `auto-grow.yaml` eintragen** (Abschnitt `calibrate_linear`):
   ```yaml
   - calibrate_linear:
       - 3.0 -> 0      # Dein V_trocken → 0 %
       - 1.2 -> 100    # Dein V_nass    → 100 %
   ```

5. Firmware erneut flashen:
   ```bash
   esphome run auto-grow.yaml
   ```

### Typische Spannungswerte (Sensor v1.2, 3,3V)

| Zustand | Spannung |
|---------|----------|
| Trocken (Luft) | ~2,8–3,2 V |
| Nass (Wasser) | ~1,0–1,4 V |

> Werte können je nach Sensor leicht abweichen — immer selbst kalibrieren!

---

## Schnellstart

### 1. ESPHome installieren

```bash
pip install esphome
```

Oder als VSCode-Erweiterung: [ESPHome Dashboard](https://esphome.io/guides/getting_started_hassio.html)

### 2. Zugangsdaten konfigurieren

```bash
cp secrets.yaml.example secrets.yaml
```

`secrets.yaml` öffnen und ausfüllen:
```yaml
wifi_ssid: "DeinNetzwerk"
wifi_password: "DeinPasswort"
ota_password: "sicheres_passwort"
api_key: "..."   # generieren: python3 -c "import base64,os; print(base64.b64encode(os.urandom(32)).decode())"
```

### 3. Erstmalig flashen (USB)

ESP32 per USB-C anschließen:
```bash
esphome run auto-grow.yaml
```

### 4. IP-Adresse herausfinden & Web-Interface öffnen

```bash
esphome logs auto-grow.yaml
# → "[WiFi]: Connected! IP: 192.168.x.x"
```

Browser: `http://192.168.x.x/`

### 5. Ab jetzt: OTA-Updates (kein USB mehr nötig)

```bash
esphome run auto-grow.yaml
# ESPHome erkennt den ESP32 im Netzwerk und flasht per WiFi
```

---

## Weitere Dokumentation

- 📐 [Schaltplan / Stromlaufplan](docs/schaltplan.md)
- 🚿 [Wassersystem-Diagramm](docs/wassersystem.md)
- 🔧 [Schritt-für-Schritt Aufbauanleitung](docs/aufbau.md)
