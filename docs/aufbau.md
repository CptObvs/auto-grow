# Schritt-für-Schritt Aufbauanleitung

Balkon-Bewässerungssystem mit ESP32 — Aufbau auf Deutsch

---

## Voraussetzungen

- Alle Teile aus der [Einkaufsliste](../README.md#einkaufsliste) vorhanden
- Lötkolben + Lötzinn
- Multimeter
- Seitenschneider, Abisolierzange
- Schraubenzieher (Flach, für Schraubklemmen)
- PC mit [VSCode + PlatformIO](https://platformio.org/install/ide?install=vscode)

---

## Schritt 1 — Buck Converter einstellen

> ⚠️ **Zuerst Buck einstellen, bevor ESP32 oder Relais angeschlossen werden!**

1. Buck Converter (LM2596) an **12V Eingang** anschließen (noch nichts anderes verbunden)
2. Multimeter auf DC-Volt stellen, Messspitzen an OUT+ und OUT- halten
3. Trimmpoti (kleines blaues Poti auf dem Buck) mit Schraubenzieher drehen:
   - Uhrzeigersinn → Spannung sinkt
   - Gegenuhrzeigersinn → Spannung steigt
4. Auf **5,0 V** einstellen (±0,1 V)
5. Netzteil abstecken

```
    Buck LM2596 mit Display:
    
    IN+  ──► 12V (Netzteil +)
    IN-  ──► GND (Netzteil -)
    OUT+ ──► 5V (zu ESP32 VIN + Relais VCC)
    OUT- ──► GND (gemeinsame Masse)
    
    Display zeigt Ausgangsspannung an → auf 5,0 V trimmen
```

---

## Schritt 2 — DC-Buchse und Hauptverkabelung

1. **DC-Buchse 5,5×2,1mm** in Gehäuse/Verteilerbox einbauen
2. An Schraubklemme anlöten oder anschrauben:
   - Mittelpin (+) → 12V Sammelschiene
   - Außenring (-) → GND Sammelschiene
3. Von 12V Sammelschiene:
   - Kabel zu Buck IN+
   - Kabel zu Relaismodul COM 1–5 (alle 5 zusammen an 12V)
4. Von GND Sammelschiene:
   - Kabel zu Buck IN-
   - Kabel zu Relaismodul GND

```
    DC-Buchse (Schraubklemme):
    
    Mittelpin (+) ──┬──► Buck IN+
                    └──► Relais COM 1–5 (12V)
    
    Außenring (-) ──┬──► Buck IN-
                    └──► Relais GND
                    └──► gemeinsame Masse
```

---

## Schritt 3 — ESP32 an Buck anschließen

1. Buck OUT+ → ESP32 **VIN**
2. Buck OUT- → ESP32 **GND**
3. Buck OUT+ → Relaismodul **VCC**
4. Buck OUT- → Relaismodul **GND** (falls noch nicht verbunden)

> ESP32 VIN verträgt 5V. **Nicht** 5V an 3V3-Pin!

---

## Schritt 4 — Freilaufdioden löten (an Pumpe und Ventile)

> 🔑 Wichtig! Ohne Dioden können Spannungsspitzen beim Abschalten den ESP32 beschädigen.

**Pro Last (Pumpe + 4 Ventile) = 5 Dioden 1N4007:**

1. Diode **antiparallel** zur Last anlöten:
   - Kathode (Ring-Seite) → Pluspol der Last
   - Anode → Minuspol der Last
2. Lötstellen mit passendem **Schrumpfschlauch** isolieren

```
    Ventil/Pumpe:
    
    (+) ──┬────────────────────► zum Relais NO
          │
          └──[Anode──◄──Kathode]── (-)
                1N4007
                                  │
    (-) ──────────────────────────┴──► GND
    
    Schrumpfschlauch über die gelötete Verbindung schrumpfen.
```

---

## Schritt 5 — Relais mit ESP32 verdrahten

Relaismodul hat pro Kanal: **IN**, **COM**, **NO**, **NC**

Nur **IN** und **NO** + **COM** werden genutzt:

| Relais-Pin | Verbindung |
|------------|------------|
| VCC | Buck OUT+ (5V) |
| GND | GND |
| IN1 | ESP32 GPIO25 (Pumpe) |
| IN2 | ESP32 GPIO26 (Ventil 1) |
| IN3 | ESP32 GPIO27 (Ventil 2) |
| IN4 | ESP32 GPIO14 (Ventil 3) |
| IN5 | ESP32 GPIO12 (Ventil 4) |
| COM 1–5 | 12V (Netzteil +) |
| NO 1 | Pumpe + (Diode bereits gelötet) |
| NO 2 | Ventil 1 + (Diode bereits gelötet) |
| NO 3 | Ventil 2 + (Diode bereits gelötet) |
| NO 4 | Ventil 3 + (Diode bereits gelötet) |
| NO 5 | Ventil 4 + (Diode bereits gelötet) |

> **Active-LOW:** Relais zieht an wenn GPIO auf LOW — im Code `digitalWrite(pin, LOW)` = an.

---

## Schritt 6 — Bodensensoren anschließen

| Sensor-Pin | Verbindung |
|------------|------------|
| VCC | ESP32 3V3 |
| GND | ESP32 GND |
| AOUT | ESP32 GPIO (34, 35, 32, 33) |

> Sensoren auf **3,3V** betreiben — nicht 5V (beschädigt ADC-Eingang des ESP32)!

---

## Schritt 7 — 4-adriges Kabel für Standorte vorbereiten

Pro Standort ein Kabelsatz:

```
    Kabel 4×0,3mm² konfektionieren:
    
    🔴 Ader 1 (12V):  Relais NO → Ventil (+)
    ⚫ Ader 2 (GND):   GND → Ventil (-) + Sensor GND
    🟡 Ader 3 (3V3):  ESP32 3V3 → Sensor VCC
    🔵 Ader 4 (Sig):  ESP32 GPIO → Sensor AOUT
    
    Längen (ca.):
    Standort 1: ~2,0 m
    Standort 2: ~2,5 m
    Standort 3: ~3,0 m
    Standort 4: ~2,5 m
```

---

## Schritt 8 — Wassersystem aufbauen

1. **Schläuche zuschneiden** (Silikonschlauch 6×9mm, Maßangaben → `docs/wassersystem.md`)
2. **T-Stücke einsetzen:**
   - Schlauch auf T-Stück-Stutzen aufschieben
   - Schlauchschelle positionieren und festziehen
3. **Pumpe anschließen:**
   - Ansaugschlauch auf Pumpen-Eingang + Schlauchschelle
   - Druckschlauch auf Pumpen-Ausgang + Schlauchschelle
4. **Ventile anschließen:**
   - Schlauch von T-Stück auf Ventil-Eingang + Schlauchschelle
   - Kurzer Ausgangsschlauch vom Ventil zur Tropfstelle im Topf

```
    Schlauchschelle richtig montieren:
    
    Schlauch über Stutzen schieben (mindestens 15 mm)
    Schlauchschelle über den Schlauch schieben
    Schlauchschelle ÜBER dem Stutzen positionieren
    Mit Schraubenzieher festziehen bis Schlauch nicht mehr abziehbar
```

---

## Schritt 9 — Kabel am Schlauch fixieren

1. Kabel parallel zum Schlauch legen
2. Alle ~30 cm einen **Kabelbinder** um Schlauch + Kabel
3. Überstehende Enden der Kabelbinder abzwicken

```
    ════════════════════════  Schlauch
    │         │         │    Kabelbinder
    ──────────────────────── 4-adriges Kabel
```

---

## Schritt 10 — Löten am Standort

Am Standort (Topf/Beet) müssen Ventil und Sensor an das 4-adrige Kabel angeschlossen werden:

1. Kabelende ~5 cm abisolieren
2. **Ader 2 (GND)** aufteilen:
   - Einen ~10 cm langen GND-Abzweig anlöten
   - Schrumpfschlauch drüber, schrumpfen
3. **Ventil anschließen:**
   - Ader 1 (12V) → Ventil (+)
   - Ader 2 Abzweig → Ventil (-)
   - Diode bereits dran (aus Schritt 4)
4. **Sensor anschließen:**
   - Ader 3 (3V3) → Sensor VCC
   - Ader 2 (GND) → Sensor GND
   - Ader 4 (Signal) → Sensor AOUT
5. Alle Lötstellen mit Schrumpfschlauch isolieren

---

## Schritt 11 — Sensoren kalibrieren

1. Firmware flashen (Schritt 12)
2. Seriellen Monitor öffnen (`pio device monitor`, 115200 Baud)
3. **Trockenwert:**
   - Sensor in Luft halten
   - Rohwert ablesen (typisch ~3200–3500)
   - In `include/config.h` als `SENSOR_x_TROCKEN` eintragen
4. **Nasswert:**
   - Sensor in ein Glas Wasser tauchen
   - Rohwert ablesen (typisch ~1200–1500)
   - In `include/config.h` als `SENSOR_x_NASS` eintragen
5. Firmware erneut flashen mit neuen Kalibrierungswerten

---

## Schritt 12 — Firmware flashen

1. Repository klonen:
   ```bash
   git clone https://github.com/CptObvs/auto-grow.git
   cd auto-grow
   ```
2. `include/config.h` öffnen, WiFi-Zugangsdaten eintragen:
   ```cpp
   const char* WIFI_SSID     = "DeinNetzwerk";
   const char* WIFI_PASSWORD = "DeinPasswort";
   ```
3. ESP32 per USB-C anschließen
4. Flashen:
   ```bash
   pio run --target upload
   ```
5. Seriellen Monitor starten:
   ```bash
   pio device monitor
   ```
6. IP-Adresse aus dem Serial Monitor ablesen
7. IP im Browser aufrufen → **ESP-DASH Dashboard** öffnet sich

---

## Schritt 13 — Erster Test

1. **Trockentest** (ohne Wasser):
   - Im Dashboard auf „Zone 1 manuell" klicken
   - Relais muss hörbar klicken
   - LED am Relaismodul leuchtet
   - Pumpe läuft kurz (Geräusch)

2. **Wassertest:**
   - Pumpe in Tank, Schläuche zu Töpfen verlegen
   - Alle Schlauchverbindungen auf Dichtigkeit prüfen
   - Manuell eine Zone starten → Wasser soll aus dem Auslass kommen

3. **Sensortest:**
   - Dashboard zeigt Feuchtigkeitswerte für alle 4 Zonen
   - Sensor mit Finger anfeuchten → Wert steigt
   - Sensor trockenwischen → Wert sinkt

---

## Häufige Fehler

| Problem | Ursache | Lösung |
|---------|---------|--------|
| Ventil öffnet nicht | Diode falsch gepolt | Diode umdrehen |
| ESP32 startet nicht | Falsche Spannung an VIN | Buck auf 5V trimmen |
| Sensor zeigt immer 0% | 3V3 falsch angeschlossen | VCC-Pin prüfen |
| Sensor zeigt immer 100% | Signal-Ader vertauscht | Ader 4 prüfen |
| Pumpe läuft, kein Wasser | Ventil bleibt zu | Relais-Anschluss prüfen |
| WiFi verbindet nicht | Falsche Zugangsdaten | config.h prüfen |
| Dashboard leer | IP-Adresse falsch | Serial Monitor öffnen |
