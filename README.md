# OpenCNC

An open, ESP32-based Cartesian pen plotter: two NEMA17 steppers, DRV8825 drivers
and a hobby servo pen lift, running [FluidNC](https://github.com/bdring/FluidNC).

Separate from [PenPlot](https://github.com/adithya-in/penplot), which drives a
28BYJ-48 machine through ULN2003s on an Arduino Nano. Different motors,
different drivers, different firmware — nothing electrical is shared.

**Status:** design reference. Electronics are specified against the DRV8825,
MP1584 and ESP32 datasheets; the machine has not been built yet, so speed and
calibration figures are targets, not measurements.

## Specs

| | |
|---|---|
| Working area | 340 × 460 mm (A3 portrait) |
| Frame | 2020 T-slot, 520 × 400 mm footprint |
| Motors | 2× NEMA17, 1.8°, 200 steps/rev |
| Drivers | 2× DRV8825, 1/16 microstepping, 1.0 A |
| Controller | ESP32-WROOM-32, 38-pin DevKitC |
| Firmware | FluidNC, `kinematics: Cartesian` |
| Pen lift | Hobby servo on the Z axis, 0–5 mm virtual travel |
| Drive | GT2 belt, 6 mm, 2 mm pitch, 20T pulleys |
| Resolution | 0.0125 mm/step (80 steps/mm) |
| Power | 12 V, 3 A recommended (2 A minimum) |
| Cost | ≈ ₹4,500 complete, excluding motors and servo |

## Documents

| Document | Contents |
|---|---|
| [`docs/technical.md`](docs/technical.md) | Full technical reference: pin map, power tree, DRV8825 setup, FluidNC config, mechanics, performance, commissioning, calibration, failure modes |
| [`docs/BOM.md`](docs/BOM.md) | Component list with current prices, supplier links and mechanical/printed parts |

## Wiring at a glance

| Function | GPIO |
|---|---|
| X motor STEP / DIR | 26 / 27 |
| Y motor STEP / DIR | 32 / 33 |
| Shared driver enable | 25 |
| Servo PWM (Z) | 13 |
| X / Y limit | 16 / 17 |

Each axis has its own motor: GPIO 26/27 drive X, GPIO 32/33 drive Y.
There is no axis mixing, so a move in X never turns the Y motor.

## Firmware

FluidNC v3 or later on a classic ESP32. Flash from
<https://install.fluidnc.com>, join the `FluidNC` access point, upload the
config from the WebUI. The complete `config.yaml` is in
[`docs/technical.md` §4](docs/technical.md#4-firmware).

## Software

The machine speaks GRBL-compatible G-code, so any sender works. It was designed
alongside [PenPlot](https://github.com/adithya-in/penplot), whose browser app
connects over Web Serial — set `steps 80`, `travel {x:310, y:430}`,
`accel 400`, `pen {up:2.5, down:0}` in its settings.

Pen up/down is a `Z` move, which maps directly onto the servo axis.

## Safety

- Never disconnect a motor with power on — the inductive kick destroys DRV8825s.
- Set Vref (0.45–0.50 V) before connecting motors.
- Common ground, star point at the PSU; motor return current must not pass
  through the ESP32 ground.
- MP1584 output must be set to 5.0 V before connecting the ESP32.

## Licence

Not yet chosen. Add one before distributing hardware or firmware derived from
this design.
