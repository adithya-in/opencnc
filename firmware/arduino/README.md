# Arduino firmware — GRBL 1.1f + pen servo

Firmware for the Arduino Uno + CNC Shield variant of OpenCNC: two NEMA17
steppers on X and Y, with a hobby servo as the pen lift on the Z axis.

Vendored from [bdring/Grbl_Pen_Servo](https://github.com/bdring/Grbl_Pen_Servo)
(GRBL 1.1f, build 20170801), GPLv3. See [`LICENSE`](grbl-pen-servo/LICENSE).

## What was changed from upstream

| File | Change |
|---|---|
| `grbl/config.h` | Already set to `CPU_MAP_ATMEGA328P_SERVO` — matches the CNC Shield pinout |
| `grbl/defaults.h` | `DEFAULTS_GENERIC`: 80 steps/mm, 3000 mm/min, 400 mm/s², 300 mm travel, idle lock 255, junction deviation 0.05 |

Everything else is upstream.

## Pinout (Arduino Uno + CNC Shield V3)

The `CPU_MAP_ATMEGA328P_SERVO` map is identical to the stock CNC Shield pinout:

| Function | Uno pin |
|---|---|
| X STEP / DIR | D2 / D5 |
| Y STEP / DIR | D3 / D6 |
| Z STEP / DIR | D4 / D7 (unused — pen is the servo) |
| Stepper enable | D8 |
| X limit | D9 |
| Y limit | D10 |
| **Servo signal** | **D11** (the shield's Spindle PWM pin) |
| Z limit / spindle enable | D12 / D13 |
| Probe | A5 |

## Build and flash

```bash
cd firmware/arduino/grbl-pen-servo
arduino-cli compile --fqbn arduino:avr:uno --libraries . \
    --output-dir build grbl/examples/grblUpload

arduino-cli upload -p /dev/cu.usbmodemXXXX --fqbn arduino:avr:uno \
    --input-dir build
```

Output is 29,188 bytes (90 % of flash) and 1,543 bytes RAM (75 %) — normal for
GRBL on an Uno. The "low memory" warning is expected.

## Wiring

```
12 V PSU ──► CNC Shield motor power terminals (VMOT)

X NEMA17 ──► shield X slot (A4988 / DRV8825)
Y NEMA17 ──► shield Y slot

SG90 servo   signal ──► D11
             +5 V   ──► 5 V
             GND    ──► GND

Arduino powered by USB (the shield does not power the Uno)
```

The servo draws its 5 V from the Uno's regulator, which is fine for an SG90
(≈150 mA idle, ~700 mA stall). For an MG90S or larger, feed 5 V from a separate
BEC and share ground.

## Microstepping and steps/mm

The CNC Shield's M0/M1/M2 jumpers set microstepping. With 20T GT2 pulleys
(40 mm per revolution):

| Jumpers | Microsteps | Steps/mm (`$100`, `$101`) |
|---|---|---|
| 1/8 | 8 | 40 |
| **1/16** | 16 | **80** (default) |
| 1/32 | 32 | 160 |

Change with `$100=<value>` and `$101=<value>`, then `$RST=$` or power-cycle.

## Pen servo behaviour

The servo is driven from the Z axis position, not from M3/M5:

| Z position | Pen |
|---|---|
| Z ≥ 0.1 mm | **up** |
| Z < 0.1 mm | **down** |

So:

```gcode
G0 Z1      ; pen up
G0 X10 Y10
G0 Z0      ; pen down
G4 P0.2    ; let the servo settle
G1 X50 Y10 F2000
G0 Z1
```

The two pulse widths are `PEN_SERVO_DOWN 31` and `PEN_SERVO_UP 16` in
`grbl/spindle_control.c`. These are Timer2 compare values at ~61 Hz, giving
roughly **1.09 ms to 2.05 ms**. That is close to full servo travel — if the
servo buzzes at either end, change them to `26` and `18` (≈1.15–1.66 ms).

## Default settings after flashing

| Setting | Value | Meaning |
|---|---|---|
| `$0` | 10 | step pulse µs |
| `$1` | 255 | motors always energised |
| `$3` | 0 | direction invert (flip per axis if needed) |
| `$11` | 0.05 | junction deviation |
| `$20` | 0 | soft limits off |
| `$22` | 0 | homing off |
| `$100`, `$101` | 80 | X, Y steps/mm |
| `$110`, `$111` | 3000 | max rate mm/min |
| `$120`, `$121` | 400 | acceleration mm/s² |
| `$130`, `$131` | 300 | max travel mm |

Settings live in EEPROM and survive power cycles. Reflashing resets them to
the defaults above.

## Using it

1. **Connect** at 115200 baud. GRBL replies `Grbl 1.1f ['$' for help]`.
2. **Verify** with `$$` — check `$100`/`$101` match your jumpers.
3. **Jog** with `$J=G91 X10 F1000` (or your sender's jog buttons).
4. **Zero** at the paper corner: `G92 X0 Y0`.
5. **Send G-code** from any GRBL sender — UGS, Candle, bCNC, or the PenPlot
   web app over Web Serial.
6. If an axis runs backwards, invert it with `$3=1` (X), `$3=2` (Y), or `$3=3`
   (both) rather than rewiring.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| No response on serial | wrong port or baud | 115200, check `arduino-cli board list` |
| **Motor shaft free, 12 V present, no torque** | **EN jumper missing on the CNC Shield** — D8 never reaches the drivers and the shield's pull-up keeps them disabled | Fit the EN jumper |
| Motors whine, don't turn | microstep jumpers missing | fit M0/M1/M2 |
| Driver enabled (shaft stiff) but no movement | Vref at zero | set 0.50 V (DRV8825) / 0.80 V (A4988) |
| Pen doesn't move | servo not on D11 | check the shield's Spindle PWM pin |
| Drawing wrong size | steps/mm vs jumpers mismatch | fix `$100`/`$101` |
| One axis reversed | direction | `$3` bitmask |
| Servo twitches at rest | shared 5 V sagging | separate 5 V BEC + 1000 µF |

## Bring-up order that avoids the traps

1. Power off. Fit both drivers **and the EN jumper**. Check the shield seats evenly on both header rows.
2. Power on. The motor shafts should now resist turning by hand.
3. Flash `firmware/arduino/spin-all` — all axes should rotate. If they don't, it's Vref or coils, not firmware.
4. Set Vref (0.50 V for DRV8825 at 1 A).
5. Flash GRBL and apply the settings.
