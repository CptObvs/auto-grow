# Schaltplan / Stromlaufplan

Balkon-Bewässerungssystem mit ESP32 — vollständiger Stromlaufplan

---

## Gesamtübersicht

```
╔══════════════════════════════════════════════════════════════════════╗
║                        12V 5A NETZTEIL                               ║
║                     (DC-Buchse 5,5×2,1mm)                            ║
╚══════════════════════════════╤═══════════════════════════════════════╝
                               │ 12V / GND
               ┌───────────────┴───────────────────────────┐
               │                                           │
               ▼                                           ▼
   ┌───────────────────────┐              ┌────────────────────────────┐
   │   BUCK CONVERTER      │              │    RELAISMODUL 6-Kanal     │
   │     LM2596            │              │         5V                 │
   │  12V → 5V (Display)   │              │  VCC ←── 5V (vom Buck)    │
   │                       │              │  GND ←── GND               │
   │  IN+: 12V             │              │                            │
   │  IN-: GND             │              │  IN1 ←── GPIO25 (Pumpe)   │
   │  OUT+: 5V ────────────┼──────────────►  IN2 ←── GPIO26 (Ventil1) │
   │  OUT-: GND ───────────┼──────────────►  IN3 ←── GPIO27 (Ventil2) │
   └───────────┬───────────┘              │  IN4 ←── GPIO14 (Ventil3) │
               │ 5V                       │  IN5 ←── GPIO12 (Ventil4) │
               ▼                          │                            │
   ┌───────────────────────┐              │  NO1 ──► Pumpe (+)        │
   │       ESP32           │              │  NO2 ──► Ventil 1 (+)     │
   │    DevKit TYPE-C      │              │  NO3 ──► Ventil 2 (+)     │
   │                       │              │  NO4 ──► Ventil 3 (+)     │
   │  VIN ←── 5V           │              │  NO5 ──► Ventil 4 (+)     │
   │  GND ←── GND          │              │  COM1–5 ←── 12V           │
   │                       │              └────────────────────────────┘
   │  GPIO25 ──────────────┼──────────────► Relais IN1
   │  GPIO26 ──────────────┼──────────────► Relais IN2
   │  GPIO27 ──────────────┼──────────────► Relais IN3
   │  GPIO14 ──────────────┼──────────────► Relais IN4
   │  GPIO12 ──────────────┼──────────────► Relais IN5
   │                       │
   │  GPIO34 ◄─────────────┼──── Sensor 1 Signal
   │  GPIO35 ◄─────────────┼──── Sensor 2 Signal
   │  GPIO32 ◄─────────────┼──── Sensor 3 Signal
   │  GPIO33 ◄─────────────┼──── Sensor 4 Signal
   │                       │
   │  3V3 ──────────────────┼──► Sensor 1–4 VCC
   │  GND ──────────────────┼──► Sensor 1–4 GND
   └───────────────────────┘
```

---

## Relais → Lasten (mit Freilaufdioden)

```
12V ──── COM (Relaismodul) ──┬── NO1 ──► Pumpe (+) ──┬──► GND
                              │    1N4007 ◄────────────┘
                              │    (Kathode an +, Anode an -)
                              │
                              ├── NO2 ──► Ventil 1 (+) ──┬──► GND
                              │    1N4007 ◄───────────────┘
                              │
                              ├── NO3 ──► Ventil 2 (+) ──┬──► GND
                              │    1N4007 ◄───────────────┘
                              │
                              ├── NO4 ──► Ventil 3 (+) ──┬──► GND
                              │    1N4007 ◄───────────────┘
                              │
                              └── NO5 ──► Ventil 4 (+) ──┬──► GND
                                   1N4007 ◄───────────────┘

Diode 1N4007 immer ANTIPARALLEL zur Last:
   Last (+) ──── [Anode──Diode──Kathode] ──── Last (+)
   (Diode leitet Spannungsspitzen ab, wenn Relais öffnet)
```

---

## Freilaufdiode im Detail

```
Relais NO ──────┬──────► Ventil/Pumpe (+)
                │              │
           [1N4007]        (Spule)
           Kathode            │
           oben               │
                         Ventil/Pumpe (-)
                │              │
                └──────────────┘
                       │
                      GND

Schaltbild vereinfacht:
    (+) ──►[K|◄|A]──► (-)
                Diode sperrt im Normalbetrieb,
                leitet beim Abschalten (Induktionsspannung)
```

---

## Verkabelung pro Standort (4-adriges Kabel)

```
    ZENTRALE                              STANDORT (Topf/Beet)
    ┌────────────────┐                    ┌─────────────────────────┐
    │                │                    │                         │
    │  Relais NO ────┼── 🔴 Ader 1 (12V) ─┼──► Ventil (+)          │
    │                │                    │         │               │
    │                │   ⚫ Ader 2 (GND) ──┼──►──────┤               │
    │            GND ┼───────────────────►┼── Ventil (-)           │
    │                │                    │    + Sensor GND         │
    │                │                    │    (anlöten +           │
    │                │                    │     Schrumpfschlauch)   │
    │            3V3 ┼── 🟡 Ader 3 (3V3) ──┼──► Sensor VCC          │
    │                │                    │                         │
    │  GPIO (ADC) ◄──┼── 🔵 Ader 4 (Sig) ──┼◄── Sensor AOUT         │
    │                │                    │                         │
    └────────────────┘                    └─────────────────────────┘

    Kabel am Schlauch mit Kabelbindern befestigt → ein "Strang"
    Ventil und Sensor sind ~20–30 cm voneinander entfernt
```

### Aufteilung am Standort (GND-Ader wird geteilt)

```
    ⚫ Ader 2 (GND) kommt an
            │
            ├──► Ventil (-)
            ├──► Diode 1N4007 (Anode)
            └──► Sensor GND

    → 3 Verbindungen auf einer Ader (anlöten + Schrumpfschlauch)
```

---

## Spannungsverteilung

```
12V 5A Netzteil
      │
      ├── 12V ──► Buck Converter (LM2596) ──► 5V ──┬──► ESP32 VIN
      │                                             └──► Relaismodul VCC
      │
      ├── 12V ──► Relaismodul COM 1–5 (geschaltet zu Lasten)
      │
      └── GND ──► gemeinsame Masse (Buck, Relais, Lasten)

3,3V:
      ESP32 3V3 Pin ──► alle 4 Bodensensoren VCC
      (max. ~100 mA, 4 Sensoren à ~5 mA = 20 mA → unbedenklich)
```

---

## Technische Hinweise

| Punkt | Wert |
|-------|------|
| Versorgungsspannung | 12V DC |
| Strom Pumpe | ~0,3 A |
| Strom pro Ventil | ~0,2–0,4 A |
| Strom ESP32 | ~0,25 A (mit WiFi) |
| Strom Relaismodul | ~0,1 A |
| **Gesamtstrom (max.)** | **~1,5 A** (weit unter 5 A ✅) |
| Relaistyp | Active-LOW (LOW = Relais zieht an) |
| ADC-Auflösung | 12 Bit (0–4095) |
| Sensor-Versorgung | 3,3 V |
