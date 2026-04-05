# Wassersystem

Schlauchplan und Wasserfluss des Bewässerungssystems.

---

## Wasserfluss-Übersicht

```
┌──────────────┐
│  Wassertank  │
└──────┬───────┘
       │ Ansaugschlauch 6mm
       ▼
┌──────────────┐
│    PUMPE     │  12V Membranpumpe, selbstansaugend, ~1,5–2 L/min
└──────┬───────┘
       │ Druckschlauch 6mm
       ▼
┌──────────────┐
│  T-Stück 1   │
└──┬───────────┘
   │                    │
   ▼                    ▼
┌──────────┐        ┌──────────┐
│ T-Stück 2│        │ T-Stück 3│
└──┬───┬───┘        └──┬───┬───┘
   │   │               │   │
   ▼   ▼               ▼   ▼
 V1   V2             V3   V4      ← 12V NC Magnetventile
  │    │               │    │
  ▼    ▼               ▼    ▼
HB1  HB2            Tom1  Tom2    ← Hochbeet 1/2, Tomate 1/2
[S1] [S2]           [S3]  [S4]   ← Bodenfeuchte-Sensoren
```

---

## Schlauch-Dimensionen

| Abschnitt | Typ | Länge (ca.) |
|-----------|-----|-------------|
| Tank → Pumpe (Ansaugung) | 6×9mm Silikon | 0,3 m |
| Pumpe → T-Stück 1 | 6×9mm Silikon | 0,3 m |
| T-Stück 1 → T-Stück 2 | 6×9mm Silikon | 0,5 m |
| T-Stück 1 → T-Stück 3 | 6×9mm Silikon | 0,5 m |
| T-Stück 2 → Ventil 1 | 6×9mm Silikon | 0,3 m |
| T-Stück 2 → Ventil 2 | 6×9mm Silikon | 0,3 m |
| T-Stück 3 → Ventil 3 | 6×9mm Silikon | 0,3 m |
| T-Stück 3 → Ventil 4 | 6×9mm Silikon | 0,3 m |
| Ventil → Standort (je) | 6×9mm Silikon | 0,3–1,0 m |
| **Gesamt** | | **~4–6 m** (10 m Rolle reicht) |

---

## Schlauchschellen-Positionen

Schlauchschelle an jeder Verbindung (⊕):

```
Tank ──⊕── Pumpe-Eingang ──⊕── T-Stück 1 ──⊕──┬──⊕── T-Stück 2 ──⊕──┬──⊕── Ventil 1 ──⊕── Standort 1
                                                │                      └──⊕── Ventil 2 ──⊕── Standort 2
                                                └──⊕── T-Stück 3 ──⊕──┬──⊕── Ventil 3 ──⊕── Standort 3
                                                                       └──⊕── Ventil 4 ──⊕── Standort 4
```

> Ventile haben 6mm Schlauchanschluss direkt — kein Gewinde, kein Adapter nötig.

---

## Magnetventil-Typ: NC (Normally Closed)

```
Stromlos (Relais offen):   Eingang ──[██ GESPERRT ██]
Unter Strom (Relais zu):   Eingang ──[   OFFEN    ]──► Ausgang
```

**Vorteil NC:** Bei Stromausfall oder Systemfehler bleibt alles geschlossen — kein unkontrollierter Wasserfluss.

---

## Kabel am Schlauch

```
═══════════════════════════════  Schlauch 6mm
│          │          │          Kabelbinder alle ~30 cm
───────────────────────────────  Steuerleitung 4×0,3mm²
```

Am Standort aufteilen:
- Schlauch → Ventil → Tropfstelle im Beet
- Kabel → Ventil-Anschluss + Sensor-Anschluss (löten, Schrumpfschlauch)

---

## Pumpen-Spezifikation

| Eigenschaft | Wert |
|-------------|------|
| Spannung | 12V DC |
| Strom | ~0,3 A |
| Fördermenge | ~1,5–2 L/min |
| Typ | Membranpumpe, selbstansaugend |
| Stutzen | 5–6 mm (passt auf 6×9mm Silikon mit Schlauchschelle) |
| Max. Betriebsdauer | 30–120 s pro Zone (konfigurierbar), Timeout nach 3 min |
