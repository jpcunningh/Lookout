#!/bin/sh
# Install/refresh the Lookout systemd units for a Dexcom rig.
#   sudo deploy/install.sh [--enable]
# Does not overwrite an existing /etc/lookout.env.
set -eu
[ "$(id -u)" -eq 0 ] || { echo "run with sudo" >&2; exit 1; }
HERE=$(cd "$(dirname "$0")" && pwd)
DIR=$(cd "$HERE/.." && pwd)
USER_NAME=${SUDO_USER:-$(stat -c %U "$DIR")}
NODE=$(sudo -u "$USER_NAME" sh -c 'command -v node' || true)
[ -n "$NODE" ] || { [ -x /opt/node/bin/node ] && NODE=/opt/node/bin/node; }
[ -n "$NODE" ] || { echo "node not found" >&2; exit 1; }
NODE_BIN=$(dirname "$NODE")

# Locate bluetoothd across Raspberry Pi OS / Debian releases.
# Preference order matches the real binary locations used by BlueZ:
#   Bullseye+ (Trixie, Bookworm, …) : /usr/libexec/bluetooth/bluetoothd
#   Jessie–Buster                   : /usr/lib/bluetooth/bluetoothd
#   Wheezy (and common symlink)     : /usr/sbin/bluetoothd
BLUETOOTHD=
for cand in \
  /usr/libexec/bluetooth/bluetoothd \
  /usr/lib/bluetooth/bluetoothd \
  /usr/sbin/bluetoothd
do
  if [ -x "$cand" ]; then
    BLUETOOTHD=$cand
    break
  fi
done
[ -n "$BLUETOOTHD" ] || {
  echo "bluetoothd not found in known locations" >&2
  exit 1
}
echo "using bluetoothd: $BLUETOOTHD"

sed -e "s#@USER@#$USER_NAME#g" -e "s#@DIR@#$DIR#g" -e "s#@NODE_BIN@#$NODE_BIN#g" \
  "$HERE/systemd/lookout.service" > /etc/systemd/system/lookout.service
install -d /etc/systemd/system/lookout.service.d /etc/systemd/system/bluetooth.service.d
install -m 644 "$HERE/systemd/lookout.service.d/priority.conf" /etc/systemd/system/lookout.service.d/priority.conf
sed -e "s#@BLUETOOTHD@#$BLUETOOTHD#g" \
  "$HERE/systemd/bluetooth.service.d/cgm.conf" > /etc/systemd/system/bluetooth.service.d/cgm.conf
if [ ! -e /etc/lookout.env ]; then
  install -m 600 "$HERE/lookout.env.example" /etc/lookout.env
  echo "created /etc/lookout.env from the example: edit it before starting"
fi
systemctl daemon-reload
systemctl restart bluetooth
if [ "${1:-}" = "--enable" ]; then systemctl enable --now lookout; fi
echo "installed: lookout.service (+priority), bluetooth --noplugin=gap ($BLUETOOTHD)"
