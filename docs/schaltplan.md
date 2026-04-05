# Schaltplan / Stromlaufplan

---

## Gesamtübersicht

```
╔══════════════════════════════════════════════════════════╗
║               12V 5A NETZTEIL                            ║
╚══════════════════════════╤═══════════════════════════════╝
                           │ 12V / GND
          ┌────────────────┼──────────────────────────┐
          │                │                          │
          ▼                ▼                          ▼
┌──────────────────┐  ┌────────────────────┐  ┌─────────────┐
│  BUCK CONVERTER  │  │  RELAISMODUL 8-CH  │  │  COM 1–5    │
│  12V → 5V        │  │  SRD-12VDC-SL-C    │  │  (12V an    │
│                  │  │                    │  │   Lasten)   │
│  OUT+ 5V ────────┼─►│  VCC  (Logik)      │  └──────┬──────┘
│  OUT- GND ───────┼─►│  GND               │         │ 12V
└────────┬─────────┘  │                    │    ┌────┴──────────────────┐
         │ 5V         │  IN1 ◄── GPIO25    │    │ NO1 → Pumpe (+)       │
         ▼            │  IN2 ◄── GPIO26    │    │ NO2 → Ventil 1 (+)    │
┌──────────────────┐  │  IN3 ◄── GPIO27    │    │ NO3 → Ventil 2 (+)    │
│     ESP32        │  │  IN4 ◄── GPIO14    │    │ NO4 → Ventil 3 (+)    │
│  DevKit TYPE-C   │  │  IN5 ◄── GPIO12    │    │ NO5 → Ventil 4 (+)    │
│                  │  │                    │    └───────────────────────┘
│  VIN  ◄── 5V     │  │  DC+  ◄── 12V      │    (je mit Freilaufdiode)
│  GND  ◄── GND    │  │  DC-  ◄── GND      │
│                  │  └────────────────────┘
│  GPIO25 ─────────┼──────────────────────► Relais IN1
│  GPIO26 ─────────┼──────────────────────► Relais IN2
│  GPIO27 ─────────┼──────────────────────► Relais IN3
│  GPIO14 ─────────┼──────────────────────► Relais IN4
│  GPIO12 ─────────┼──────────────────────► Relais IN5
│                  │
│  GPIO34 ◄────────┼──── Sensor 1 AOUT (Hochbeet 1)
│  GPIO35 ◄────────┼──── Sensor 2 AOUT (Hochbeet 2)
│  GPIO32 ◄────────┼──── Sensor 3 AOUT (Tomate 1)
│  GPIO33 ◄────────┼──── Sensor 4 AOUT (Tomate 2)
│                  │
│  3V3 ────────────┼──► Sensor 1–4 VCC
│  GND ────────────┼──► Sensor 1–4 GND
└──────────────────┘
```

---

## Spannungsverteilung

```
12V Netzteil
  │
  ├── 12V ──► Buck Converter IN+
  │              └── OUT+ 5V ──┬──► ESP32 VIN  (intern → 3,3V)
  │                             └──► Relaismodul VCC (Logik/Optokoppler)
  │
  ├── 12V ──► Relaismodul DC+ (Spulenversorgung SRD-12VDC)
  ├── 12V ──► Relaismodul COM 1–5 (geschaltet zu Lasten über NO)
  │
  └── GND ──► gemeinsame Masse (Buck, Relais, ESP32, Lasten)

3,3V (vom ESP32 3V3-Pin):
  └──► alle 4 Bodensensoren VCC
       (4 × ~5 mA = 20 mA — weit unter ESP32 3V3 Limit von 50 mA)
```

---

## Relais → Lasten (mit Freilaufdioden)

```
12V ──── Relais COM ──┬── NO1 ──► Pumpe (+) ────────┬──► GND
                      │          [◄ Diode]───────────┘
                      │
                      ├── NO2 ──► Ventil 1 (+) ──────┬──► GND
                      │          [◄ Diode]────────────┘
                      │
                      ├── NO3 ──► Ventil 2 (+) ──────┬──► GND
                      │          [◄ Diode]────────────┘
                      │
                      ├── NO4 ──► Ventil 3 (+) ──────┬──► GND
                      │          [◄ Diode]────────────┘
                      │
                      └── NO5 ──► Ventil 4 (+) ──────┬──► GND
                                 [◄ Diode]────────────┘

Freilaufdiode ANTIPARALLEL zur Last:
  Kathode (Ring) → Last (+)
  Anode          → Last (-)
  Leitet Induktionsspannung beim Abschalten ab → schützt Relais-Kontakte und ESP32.
```

---

## Relais-Logik (HIGH-Level Trigger)

```
Jumper am Modul: HIGH-Level Trigger

  ESP32 GPIO HIGH (3,3V) ──► Optokoppler leitet ──► Relais zieht an ──► NO schließt
  ESP32 GPIO LOW  (0V)   ──► Optokoppler sperrt ──► Relais fällt ab ──► NO offen

Interner Pulldown in ESPHome:
  Bei Stromverlust / Boot / Reset → GPIO floatet auf LOW → Relais bleibt garantiert offen
```

---

## Verkabelung pro Standort (4-adriges Kabel)

```
ZENTRALE                                    STANDORT (Topf / Beet)
┌──────────────────┐                        ┌──────────────────────┐
│                  │                        │                      │
│  Relais NO ──────┼── 🔴 Ader 1 (12V) ─────┼──► Ventil (+)        │
│                  │                        │                      │
│  GND ────────────┼── ⚫ Ader 2 (GND) ──────┼──► Ventil (-)        │
│                  │                        │    + Sensor GND      │
│                  │                        │    (aufteilen,       │
│                  │                        │     anlöten)         │
│  3V3 ────────────┼── 🟡 Ader 3 (3V3) ──────┼──► Sensor VCC        │
│                  │                        │                      │
│  GPIO (ADC) ◄────┼── 🔵 Ader 4 (Sig) ──────┼◄── Sensor AOUT       │
│                  │                        │                      │
└──────────────────┘                        └──────────────────────┘

Kabel am Schlauch mit Kabelbindern (alle ~30 cm) befestigt.
```

---

## Technische Daten

| Eigenschaft | Wert |
|-------------|------|
| Versorgungsspannung | 12V DC |
| Strom Pumpe | ~0,3 A |
| Strom pro Ventil | ~0,2–0,4 A |
| Strom ESP32 (WiFi aktiv) | ~0,25 A |
| Strom Relaismodul (Logik) | ~0,05 A |
| **Gesamtstrom (max., alle 5 Lasten)** | **~1,5 A** (Netzteil: 5 A ✅) |
| Relaistyp | SRD-12VDC-SL-C, HIGH-Level Trigger |
| Relais-Spulenspannung | 12V DC |
| Relais-Logikspannung (VCC) | 5V |
| Steuerpegel ESP32 | 3,3V (reicht für Optokoppler) |
| ADC-Auflösung ESP32 | 12 Bit (0–4095) |
| Sensor-Versorgung | 3,3V |
| Sensor-Ausgang (trocken) | ~2,7 V |
| Sensor-Ausgang (nass) | ~1,2 V |
