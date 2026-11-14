# Lookout on a Raspberry Pi with a Dexcom CGM
This is an alternate method of installing using systemd instead of cron

Tested with a Pi Zero W (single core), Raspberry Pi OS with BlueZ 5.82, and
Node 22. Lookout can read on display slots 1 (pump or Dexcom receiver), 2 (phone app), or 3 (watch - new for G7).

## Install

```sh
cd ~/src/Lookout && npm install
sudo deploy/install.sh          # units, Bluetooth fix, g7-new-sensor
sudoedit /etc/lookout.env       # Nightscout host and secret
sudo systemctl enable --now lookout
```

## Why the system changes are needed

- **`bluetooth.service.d/cgm.conf`** (`bluetoothd --noplugin=gap`). After
  service discovery, BlueZ's GAP plugin reads the G7's preferred connection
  parameters and applies them: 75 ms interval with peripheral latency 40.
  That lets the sensor skip up to 3 s of connection events, so it misses
  Lookout's J-PAKE replies and hangs up, with status `0a0300` or a silent
  drop. The Dexcom app on Android keeps latency 0. This plugin is the only
  BlueZ change; the stock `bluetoothd` is used.
- **`lookout.service.d/priority.conf`** (`Nice=-15`). A G7 allows about 3.3 s
  per pairing step. On a single-core Pi Zero, dbus-daemon, NetworkManager and
  bluetoothd handle every Bluetooth notification and compete with Lookout's
  replies. A multi-core Pi (Zero 2 W or later) has more headroom.
- **`--no_xdripaps`**. Skips posting to a local OpenAPS xDripAPS server on
  127.0.0.1:5000, which a Lookout-only rig doesn't have.

Pairing details live in xdrip-js:

- P-256 maths through OpenSSL
- proof checks deferred until after each reply
- write-without-response fragments
- the Authentication subscription made before ExtraData

## Watching it

```sh
journalctl -u lookout -f | grep -E 'got glucose|uploaded SGV|0a0300|Error'
```

Start Lookout with `-v` in `LOOKOUT_ARGS` to add the low-level Bluetooth traces.
