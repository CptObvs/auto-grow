# Wassersystem-Diagramm

Balkon-Bewässerungssystem — Wasserfluss und Schlauchplan

---

## Gesamtübersicht Wasserfluss

```
                    ┌──────────────┐
                    │  Wassertank  │
                    │   (Eimer)    │
                    └──────┬───────┘
                           │ Ansaugschlauch 6mm
                           │
                    ┌──────▼───────┐
                    │    PUMPE     │
                    │ RUNCCI-YUN   │
                    │    R385      │
                    │  12V, 1,5–2  │
                    │    L/min     │
                    └──────┬───────┘
                           │ Druckschlauch 6mm
                           │
                    ┌──────▼───────┐
                    │  T-Stück 1   │  6mm
                    └──┬───────────┘
                       │           │
          ┌────────────┘           └────────────────────┐
          │ Schlauch 6mm                                 │ Schlauch 6mm
   ┌──────▼───────┐                             ┌───────▼──────┐
   │  T-Stück 2   │ 6mm                         │  T-Stück 3   │ 6mm
   └──┬───────────┘                             └──┬───────────┘
      │           │                                │           │
      │           │                                │           │
┌─────▼──────┐  ┌─▼──────────┐  ┌─────────────┐  ┌▼───────────┐
│  Ventil 1  │  │  Ventil 2  │  │  Ventil 3   │  │  Ventil 4  │
│  12V NC    │  │  12V NC    │  │  12V NC     │  │  12V NC    │
│  6mm       │  │  6mm       │  │  6mm        │  │  6mm       │
└─────┬──────┘  └─────┬──────┘  └──────┬──────┘  └─────┬──────┘
      │               │                │                │
      │ 6mm           │ 6mm            │ 6mm            │ 6mm
      │ Schlauch      │ Schlauch       │ Schlauch       │ Schlauch
      │               │                │                │
  ┌───▼────┐      ┌───▼────┐       ┌───▼────┐      ┌───▼────┐
  │Hochbeet│      │Hochbeet│       │ Tomate │      │ Tomate │
  │   1    │      │   2    │       │   1    │      │   2    │
  │        │      │        │       │        │      │        │
  │[Sensor]│      │[Sensor]│       │[Sensor]│      │[Sensor]│
  │   1    │      │   2    │       │   3    │      │   4    │
  └────────┘      └────────┘       └────────┘      └────────┘
```

---

## Schlauchverbindungen im Detail

### Schlauchschellen-Positionen (⊕ = Schlauchschelle)

```
Tank
  │
  ⊕ ── Ansaugschlauch am Pumpen-Eingang (5mm Stutzen)
  │
[Pumpe]
  │
  ⊕ ── Druckschlauch am Pumpen-Ausgang (5mm Stutzen)
  │
  ⊕ ── T-Stück 1 Eingang
  │
T-Stück 1
  ├── ⊕ Ausgang links
  │
  └── ⊕ Ausgang rechts
       │
    T-Stück 2 oder 3
       ├── ⊕ Ventil-Eingang
       └── ⊕ Ventil-Eingang
              │
           [Ventil] (direkt angeschlossen, 6mm Stutzen)
              │
           ⊕ ── Schlauch zum Topf/Beet
```

> **Hinweis:** Die Ventile haben direkte 6mm Schlauchanschlüsse — kein Gewinde, kein Adapter nötig.
> Schlauch einfach aufschieben und Schlauchschelle festziehen.

---

## Schlauch-Dimensionen

| Abschnitt | Schlauch | Länge (ca.) |
|-----------|----------|-------------|
| Tank → Pumpe (Ansaugung) | 6×9mm Silikon | ~0,3 m |
| Pumpe → T-Stück 1 | 6×9mm Silikon | ~0,3 m |
| T-Stück 1 → T-Stück 2 | 6×9mm Silikon | ~0,5 m |
| T-Stück 1 → T-Stück 3 | 6×9mm Silikon | ~0,5 m |
| T-Stück 2 → Ventil 1 | 6×9mm Silikon | ~0,3 m |
| T-Stück 2 → Ventil 2 | 6×9mm Silikon | ~0,3 m |
| T-Stück 3 → Ventil 3 | 6×9mm Silikon | ~0,3 m |
| T-Stück 3 → Ventil 4 | 6×9mm Silikon | ~0,3 m |
| Ventil 1 → Hochbeet 1 | 6×9mm Silikon | ~0,3 m |
| Ventil 2 → Hochbeet 2 | 6×9mm Silikon | ~0,3 m |
| Ventil 3 → Tomate 1 | 6×9mm Silikon | ~0,3 m |
| Ventil 4 → Tomate 2 | 6×9mm Silikon | ~0,3 m |
| **Gesamt** | | **~4 m** (10 m Schlauch reichen ✅) |

---

## Ventil-Typ: NC (Normally Closed)

```
    Stromlos (Relais offen):        Unter Strom (Relais geschlossen):

    Eingang ──[██ GESPERRT ██]──    Eingang ──[  OFFEN  ]──► Ausgang
                                    
    → Kein Wasser fließt            → Wasser fließt
    → Sicherheitszustand ✅          → Bewässerung aktiv
```

Vorteil NC-Ventile: Bei Stromausfall oder Systemfehler bleibt alles geschlossen — kein unkontrollierter Wasserfluss.

---

## Kabel am Schlauch

```
    Schlauch 6mm + 4-adriges Kabel laufen parallel zum Standort:

    ════════════════════════════════  Schlauch 6mm
    │         │         │         │  Kabelbinder
    ──────────────────────────────── 4-adriges Kabel (4×0,3mm²)
    │         │         │         │
    (alle ~30 cm ein Kabelbinder)

    Am Standort (Topf/Beet):
    
    Schlauch ──► Ventil ──► Tropfstelle im Beet
    Kabel ───► Ventil-Anschluss + Sensor-Anschluss
               (aufteilen, anlöten, Schrumpfschlauch)
```

---

## Pumpen-Spezifikation (RUNCCI-YUN R385)

| Eigenschaft | Wert |
|-------------|------|
| Spannung | 12V DC |
| Strom | ~0,3 A |
| Fördermenge | 1,5–2 L/min |
| Typ | Membranpumpe, selbstansaugend |
| Stutzen | 5mm (innen) |
| Schlauch | 6×9mm passt mit Schlauchschelle ✅ |
| Betrieb | Max. 30 s pro Zone → kühlt ausreichend ab |
