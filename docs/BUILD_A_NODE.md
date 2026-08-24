# Build a vizzz.di node

How to turn a bare ESP32 board into a working WiFi → DMX512 box. Written to be
followed with the parts in your hand, not from memory.

A finished node does one thing: it makes its own WiFi network, you open a black
and cyan page in your phone's browser, and the lights on the DMX cable do what
the page says.

---

## 1. Parts — check your bags against this

Six things. Everything else is optional.

| # | Part | What it looks like in the bag | Notes |
|---|---|---|---|
| 1 | **ESP32 DevKit** | Black board ~5cm long, silver metal box in the middle marked `ESP32-WROOM-32`, USB socket at one end, two rows of pins | 30-pin and 38-pin versions both work. Micro-USB or USB-C, doesn't matter |
| 2 | **MAX485 module** | Small blue board ~2×2cm, an 8-leg chip marked `MAX485`, 4 pins on one side, a **green screw terminal** with 3 or 4 screws on the other | The green screw block is the giveaway. See §2 if yours says `MAX3485` or `SP3485` |
| 3 | **XLR chassis socket, 3-pin MALE** | Round metal connector with **3 pins sticking out**, mounting flange with 2 screw holes | DMX out is always male. A female one is the wrong part |
| 4 | **Jumper wires, female-to-female** | Coloured ribbon of wires with black plastic sockets on both ends | You need 5. Grab 8 |
| 5 | **USB cable + 5V phone charger** | — | This powers the whole node. 500mA is plenty |
| 6 | **120Ω resistor** | Tiny beige part with colour bands: brown-red-brown | Optional, see §6. Only needed at the *far* end of the DMX line |

Optional but worth having: a plastic project box, a hot glue gun or double-sided
tape to hold the boards down, and a multimeter.

**If you are not sure what's in your bags, photograph them laid out flat on a
light surface and I'll identify each part.**

---

## 2. Read your MAX485 module before wiring

Look at the 8-leg chip in the middle and read the text on it.

- Says **`MAX485`** → 5V part. Power it from the ESP32's **`VIN`** pin (that pin
  carries 5V straight from the USB socket). This is the common case.
- Says **`MAX3485`** or **`SP3485`** → 3.3V part. Power it from **`3V3`**
  instead. Everything else is identical.

Getting this backwards won't destroy anything, but a MAX485 on 3.3V drives the
cable weakly and drops out on long runs.

---

## 3. The wiring — five wires

This is the whole electrical job.

| From (ESP32) | To (MAX485) | Why |
|---|---|---|
| `GPIO25` | `DI` | The DMX data itself |
| `GPIO21` | `DE` **and** `RE` — both, joined together | Tells the chip "talk, don't listen" |
| `VIN` (or `3V3`, see §2) | `VCC` | Power |
| `GND` | `GND` | Common ground — without this, nothing works |

`GPIO21` goes to **two** pins. Twist the bare ends of two jumper wires together
into one, or run a short link between `DE` and `RE` on the module and take one
wire to `GPIO21`. Some modules already have `DE` and `RE` bridged with a solder
blob — check, and if so you only need one wire.

Then the green screw terminal to the XLR socket:

| From (MAX485) | To (XLR male, 3-pin) | DMX name |
|---|---|---|
| `A` | pin **3** | Data + |
| `B` | pin **2** | Data − |
| `GND` | pin **1** | Shield |

XLR pin numbers are stamped in the plastic next to each pin, on the back of the
socket. Pin 1 is the one nearest the notch.

```
        ESP32 DevKit                 MAX485 module            XLR male (out)
   ┌───────────────────┐         ┌──────────────────┐         ╭───────────╮
   │                   │         │                  │         │   1  2  3 │
   │  GPIO25  ─────────┼────────▶│ DI            A  ├────────▶│──────────3│ Data +
   │                   │         │                  │         │           │
   │  GPIO21  ─────────┼────┬───▶│ DE            B  ├────────▶│───────2   │ Data −
   │                   │    └───▶│ RE               │         │           │
   │  VIN (5V) ────────┼────────▶│ VCC          GND ├────────▶│───1       │ Shield
   │                   │         │                  │         ╰───────────╯
   │  GND     ─────────┼────────▶│ GND              │
   │                   │         │                  │
   │  [USB] ◀── 5V charger       └──────────────────┘
   └───────────────────┘
                                  RO — leave EMPTY. Do not connect.
```

---

## 4. Two things that will bite you

**Never connect `RO`.** On a 5V MAX485 that pin pushes 5V into a 3.3V input and
can kill the ESP32. This node only sends, never receives, so `RO` stays empty.

**Never move DMX to `GPIO1`/`GPIO3`.** Those are UART0, shared with the USB
serial port. The firmware deliberately uses `DMX_NUM_1` on GPIO25 for this
reason. Change it and DMX breaks silently while everything still *looks* fine.

---

## 5. Flash the firmware

Plug the ESP32 into the computer by USB — nothing else connected yet.

```bash
cd ~/vizzz.di
.venv-pio/bin/pio run -e esp32dev --target upload
```

If it says permission denied on `/dev/ttyUSB0`, your user isn't in the `dialout`
group. Either run `sg dialout -c "..."` around the command, or add yourself once
with `sudo usermod -aG dialout $USER` and log out and back in.

If it never finds the board: it's usually the cable. Many USB cables are
charge-only and carry no data. Try another one before anything else.

Or use the helper, which erases first and is the safer path for a board that has
been used before:

```bash
python3 onboard_device.py --erase
```

---

## 6. First power-up, with no lights attached

1. Power the ESP32 from the phone charger. Wait ~10 seconds.
2. On your phone, look at the WiFi list. A network called `vizzz.di` or
   `vizzz.di_XXXXXX` appears. The password is the one set in `src/secrets.h`
   when this firmware was built — `changeme123` if that file was never created.
3. Join it. Open `http://10.0.0.1` in the browser.
4. You should see a black page with cyan text. That's the node.
5. Open `http://10.0.0.1/system` — it lists the firmware's own pin numbers. They
   must read GPIO 25 and 21. If they don't, you flashed something else.

Do this before you connect a single light. If the page doesn't come up, the
problem is the board or the flash — not the wiring.

---

## 7. First light

1. Power everything **off**.
2. DMX cable from your XLR socket to the light's DMX IN.
3. Set the light to DMX address 1.
4. Turn burn-safe mode on first — it caps output while you're testing:
   `http://10.0.0.1/safety/set?burn=1`
5. Power on. Open `/control` and push channel 1 up slowly.

If the light does nothing: swap A and B. Reversed data lines are the single most
common DMX fault, and no damage comes from having them the wrong way round.

**Termination.** On the *last* light in the chain, if it has no terminator
switch, put the 120Ω resistor between pins 2 and 3 of a spare XLR plug and plug
it into that light's DMX OUT. On one short cable to one light you can skip it.

---

## 8. Worth knowing before a real venue

The node as described shares a ground with the DMX cable and with whatever the
lights are plugged into. In a rehearsal room that's fine. In a venue with long
cable runs, different power phases, or a chance of someone plugging a phantom-
powered audio line into your DMX socket, an **isolated** transceiver — an
`ADM2582E` module — replaces the MAX485 and protects the ESP32 from all of it.
Same three signals, `DI` / `DE` / `RE`, same wiring. It costs more than the rest
of the node put together, and it is the difference between one dead board and no
dead boards.
