# Aufbauanleitung

Schritt-für-Schritt Aufbau des Balkon-Bewässerungssystems.

---

## Voraussetzungen

- Alle Teile aus der [Einkaufsliste](../README.md#einkaufsliste) vorhanden
- Lötkolben + Lötzinn
- Multimeter (DC-Volt Messung)
- Seitenschneider, Abisolierzange
- Schraubenzieher (Schlitz, für Schraubklemmen)
- PC mit installiertem ESPHome (`pip install esphome`)

---

## Schritt 1 — Buck Converter einstellen

> ⚠️ Zuerst einstellen, bevor ESP32 oder Relais angeschlossen werden!

1. Buck Converter an **12V Eingang** anschließen (noch nichts anderes verbunden)
2. Multimeter an OUT+ und OUT- halten
3. Trimmpoti drehen bis Anzeige **5,0 V** zeigt (±0,1 V)
4. Netzteil abstecken

```
Buck Converter:

  IN+  ──► 12V (Netzteil +)
  IN-  ──► GND (Netzteil -)
  OUT+ ──► 5V  → ESP32 VIN + Relaismodul VCC
  OUT- ──► GND (gemeinsame Masse)
```

---

## Schritt 2 — Relaismodul konfigurieren

> Vor der Verkabelung die Jumper am Modul umstecken!

Jeder der 8 Kanäle hat einen 3-Pin-Jumper mit den Positionen **Low / Com / High**.

**Jumper auf HIGH stellen** (Com–High verbinden) für alle verwendeten Kanäle (IN1–IN5):

```
Low  ○
Com  ○─┐
High ○─┘  ← Jumper hier
```

Damit aktiviert das Relais wenn der ESP32-Ausgang HIGH (3,3V) ist — sicher gegen ungewolltes Anziehen beim Boot oder Stromverlust.

---

## Schritt 3 — Hauptverkabelung (12V)

```
12V Netzteil (DC-Buchse):

  (+) ──┬──► Buck Converter IN+
        └──► Relaismodul DC+  (12V für Spulen)
        └──► Relaismodul COM 1–5  (12V, geschaltet zu Lasten)

  (-) ──┬──► Buck Converter IN-
        └──► Relaismodul DC-  (gemeinsame Masse)
```

---

## Schritt 4 — ESP32 anschließen

```
Buck OUT+ (5V) ──► ESP32 VIN
Buck OUT+ (5V) ──► Relaismodul VCC  (Logik / Optokoppler)
Buck OUT- (GND) ──► ESP32 GND
Buck OUT- (GND) ──► Relaismodul GND
```

> ESP32 VIN verträgt 5V. Nicht 5V direkt an 3V3-Pin!  
> Der ESP32 erzeugt intern 3,3V für sich selbst und die Sensoren.

---

## Schritt 5 — Freilaufdioden löten

> Wichtig! Induktionsspannungen beim Abschalten der Magnetventile/Pumpe können ohne Diode den Relais-Kontakt und den ESP32 beschädigen.

Pro Last (Pumpe + 4 Ventile) = 5 Dioden:

1. Diode **antiparallel** zur Last löten:
   - **Kathode** (Ring-Markierung) → an den **Pluspol** der Last
   - **Anode** → an den **Minuspol** der Last
2. Lötstellen mit Schrumpfschlauch isolieren

```
     Last
  (+)──┬──────────────────► zum Relais NO
       │
   [K──►──A]   ← Diode (Kathode oben = Richtungspfeil zeigt nach unten)
       │
  (-)──┴──────────────────► GND
```

---

## Schritt 6 — Relais mit ESP32 verdrahten

| Relais-Anschluss | Verbindung |
|-----------------|------------|
| VCC | Buck OUT+ (5V) |
| GND | GND |
| DC+ | 12V (Netzteil +) |
| DC- | GND |
| IN1 | ESP32 GPIO25 (Pumpe) |
| IN2 | ESP32 GPIO26 (Ventil Hochbeet 1) |
| IN3 | ESP32 GPIO27 (Ventil Hochbeet 2) |
| IN4 | ESP32 GPIO14 (Ventil Tomate 1) |
| IN5 | ESP32 GPIO12 (Ventil Tomate 2) |
| COM 1–5 | 12V (Netzteil +) |
| NO 1 | Pumpe + (Diode bereits gelötet) |
| NO 2 | Ventil 1 + |
| NO 3 | Ventil 2 + |
| NO 4 | Ventil 3 + |
| NO 5 | Ventil 4 + |

---

## Schritt 7 — Bodensensoren anschließen

| Sensor-Pin | Verbindung |
|------------|------------|
| VCC | ESP32 3V3 |
| GND | ESP32 GND |
| AOUT | ESP32 GPIO34 / 35 / 32 / 33 |

> Sensoren auf **3,3V** betreiben — nicht 5V!

---

## Schritt 8 — 4-adriges Kabel pro Standort vorbereiten

```
Pro Standort ein Kabelsatz aus Steuerleitung 4×0,3mm²:

  🔴 Ader 1 (12V):  Relais NO → Ventil (+)
  ⚫ Ader 2 (GND):  GND → Ventil (-) + Sensor GND
  🟡 Ader 3 (3V3):  ESP32 3V3 → Sensor VCC
  🔵 Ader 4 (Sig):  ESP32 GPIO → Sensor AOUT
```

---

## Schritt 9 — Wassersystem aufbauen

Detaillierter Schlauchplan: [wassersystem.md](wassersystem.md)

1. Schlauch 6×9mm auf Länge schneiden
2. T-Stücke in Schlauch einsetzen, Schlauchschellen festziehen
3. Pumpe: Ansaugschlauch vom Tank, Druckschlauch zum ersten T-Stück
4. Ventile: je einen kurzen Schlauch vom T-Stück zum Ventileingang, Ausgang zum Topf

```
Schlauchschelle korrekt:
  Schlauch mindestens 15 mm über Stutzen schieben
  Schlauchschelle über dem Stutzen positionieren
  Festziehen bis Schlauch nicht mehr abziehbar
```

---

## Schritt 10 — Kabel am Schlauch fixieren

```
════════════════════════  Schlauch 6mm
│         │         │    Kabelbinder (alle ~30 cm)
──────────────────────── Steuerleitung 4×0,3mm²
```

---

## Schritt 11 — Löten am Standort

Am Topf/Beet Ventil und Sensor an das 4-adrige Kabel anschließen:

1. Kabelende ~5 cm abisolieren
2. **Ader 2 (GND)** dreifach aufteilen → Ventil (-), Sensor GND, Dioden-Anode
3. Ventil: Ader 1 (12V) → (+), Ader 2 (GND) → (-)
4. Sensor: Ader 3 (3V3) → VCC, Ader 2 (GND) → GND, Ader 4 → AOUT
5. Alle Lötstellen mit Schrumpfschlauch isolieren

---

## Schritt 12 — Firmware flashen

```bash
# Zugangsdaten eintragen
cp secrets.yaml.example secrets.yaml
# secrets.yaml öffnen: WiFi, OTA-Passwort, API-Key eintragen

# ESP32 per USB-C anschließen, erstmalig flashen
esphome run auto-grow.yaml

# IP-Adresse aus dem Log ablesen
# Browser: http://192.168.x.x/

# Ab jetzt OTA (kein USB mehr nötig)
esphome run auto-grow.yaml
```

---

## Schritt 13 — Sensoren kalibrieren

```bash
esphome logs auto-grow.yaml   # Rohwerte beobachten
```

1. Sensor trocken halten → Spannung ablesen (typisch ~2,7 V)
2. Sensor in Wasser tauchen → Spannung ablesen (typisch ~1,2 V)
3. Werte in `auto-grow.yaml` eintragen:

```yaml
- calibrate_linear:
    - 2.7 -> 0
    - 1.2 -> 100
```

4. `esphome run auto-grow.yaml` → neu flashen

---

## Schritt 14 — Funktionstest

1. **Relais-Test:** Im Web-Interface „Zone 1 manuell" → Relais muss hörbar klicken, LED leuchtet
2. **Wassertest:** Pumpe in Tank, Schläuche verlegt → Zone starten → Wasser fließt
3. **Sensor-Test:** Sensor anfeuchten → Wert steigt; trocknen → Wert sinkt

---

## Häufige Fehler

| Problem | Ursache | Lösung |
|---------|---------|--------|
| Relais zieht nicht an | Jumper auf LOW statt HIGH | Jumper am Modul auf HIGH umstecken |
| Relais zieht beim Boot an | Jumper auf LOW | Jumper auf HIGH |
| Ventil öffnet nicht | Diode falsch gepolt | Diode umdrehen |
| ESP32 startet nicht | Falsche Spannung an VIN | Buck auf 5,0 V trimmen |
| Sensor zeigt immer 0 % | VCC falsch | 3V3-Pin prüfen (nicht 5V!) |
| Sensor zeigt immer 100 % | Signal-Ader vertauscht | GPIO-Ader prüfen |
| Pumpe läuft, kein Wasser | Ventil bleibt zu | Relais-Anschluss prüfen |
| WiFi verbindet nicht | Falsche Zugangsdaten | secrets.yaml prüfen |
