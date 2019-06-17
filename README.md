# Lookout

[![Join the chat at https://gitter.im/thebookins/xdrip-js](https://badges.gitter.im/thebookins/xdrip-js.svg)](https://gitter.im/thebookins/xdrip-js?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

*Please note this project is neither created nor backed by Dexcom, Inc. This software is not intended for use in therapy.*

## Overview
Lookout provides a rig-based interface to a Dexcom G5 CGM using Bluetooth Low Energy (BLE).  Lookout connects to the CGM transmitter and provides the following capabilities:
- start and stop sensor sessions
- view reported glucose values
- send glucose values to OpenAPS and Nightscout
- send glucose values to Pump via fakemeter
- send finger stick calibration values to the transmitter
- reset expired transmitters
- calculate and report trend and noise values
- calculate and report CGM calibration slope and offset values
- report BG Check records to Nightscout obtained from transmitter's CGM calibration events
- report sensor state changes to Nightscout as announcements
- extend sensor operation beyond sensor expiration (limitations described below)
- report raw unfiltered values to Nightscout during warmup for trend visibility
- report detail transmitter and device status to Nightscout (requires Nightscout xdrip-js plugin to be enabled)

Lookout can be used with unexpired G5 transmitters relying on the official calibration built into the transmitter to calibrate the raw sensor values.  Lookout provides the user with the ability to reset expired transmitters allowing them to be used past their normal expiration dates.  Lookout can also be used with expired transmitters, providing a least squares regression calibration method to calculate calibration values for raw sensor values based on user entered glucose checks.

Lookout can be monitored and controlled via a web-based graphical interface, the `lookout` command line interface, or Nightscout.

Typically, Lookout can be run in parallel with a Dexcom receiver.  There are reported cases where Lookout did not interact well with a Dexcom receiver so YMMV.  It can also be run in parallel with a Dexcom or xDrip app on a phone if started with the `alternate` flag. 

A failure mode on the rig can prevent Lookout from completing the interaction with the transmitter to successfully read glucose values.  Lookout will automatically reboot the rig if it connects to the transmitter, but is unable to successfully retrieve a glucose value 2 times in a row.

The CGM transmitter enters sleep mode between each glucose read event to conserve power. Lookout only communicates with the transmitter during the brief moments it is awake during the glucose read event.  Commands entered by the user are are queued in the "Pending" queue to be sent to the transmitter when it is ready to receive messages at the next event. The Pending queue can be viewed using a browser or the `lookout` command. If the user enters an unintended command, the Pending queue can be flushed by restarting the rig before it sends the commands to the CGM transmitter.

### Official Calibration Mode
Official calibration mode is the default Lookout operating mode.  Lookout will only report calibrated glucose values received from the CGM transmitter. It not report glucose values from raw glucose values when the CGM Transmitter does not report a calibrated value due to events such as excessive noise, a stopped sensor session, or an expired transmitter.

### Extended Calibration Mode
Extended calibration mode is enabled with the `--extend_sensor` command line option.  In this mode, Lookout will report calibrated glucose values received from the CGM transmitter.  Lookout will also calculate the calibration offset and slope values using the calibrated and unfiltered raw value pairs. When the CGM Transmitter does not report a calibrated value due to events such as excessive noise, a stopped sensor session, or an expired transmitteri, Lookout will calibrate the unfiltered raw value with the calibration values it calculated when the CGM Transmitter was reporting calibrated values. To stop Lookout from continuing to report glucose values at the end of a sensor session, the user must do one of the items listed below:
* Execute `lookout stop` on command line
* Enter Sensor Stop in Browser Menu
* Enter a Sensor Stop in Nightscout
* Enter a Sensor Start in Nightscout --non-preferred method--
* Enter a Sensor Insert in Nightscout --non-preferred method--

### Expired Calibration Mode
Expired calibration mode is enabled with the `--expired_cal` command line option.  In this mode, Lookout will use a built-in algorithm to calibrate the unfiltered raw glucose values based on calibration offset and slope calculated from user entered BG Checks provided in either Nightscout or the Lookout calibration entry methods. When the CGM Transmitter does not report a calibrated value due to events such as excessive noise, a stopped sensor session, or an expired transmitteri, Lookout will calibrate the unfiltered raw value and report the calibrated glucose result. To stop Lookout from continuing to report glucose values at the end of a sensor session, the user must do one of the items listed below:
* Execute `lookout stop` on command line
* Enter Sensor Stop in Browser Menu
* Enter a Sensor Stop in Nightscout
* Enter a Sensor Start in Nightscout --non-preferred method--
* Enter a Sensor Insert in Nightscout --non-preferred method--

**WARNING** Expired calibration mode uses a custom algorthm that is unable to guarantee the various sensor failure modes are appropriately handled. Therefore, it is NOT recommended. 

## Pre-installation
### Intel Explorer Rig
You must update your rig's NodeJS based on https://github.com/xdrip-js/xdrip-js/wiki (only use the "Updating NodeJS" section of those instructions, you should not install xdrip-js manually, it will be installed in the next step as part of Lookout.)
As of 13-Jun-2018, these steps are:
```
The version of Node that ships with jubilinux is old (v0.10.something). Lookout requires version 8 or later. Here are the instructions for updating Node:

sudo apt-get remove nodered -y
sudo apt-get remove nodejs nodejs-legacy -y
sudo apt-get remove npm  -y # if you installed npm
sudo curl -sL https://deb.nodesource.com/setup_8.x | sudo bash -
sudo apt-get install nodejs -y
```
If you later need to revert your rig's NodeJS to the legacy version, follow the steps in the below section "Reverting NodeJS".

### Raspberry Pi Rig
The version of NPM that ships with raspbian is old (`1.4.21`).  You must update your rig's NPM to install Lookout dependencies.
```
sudo npm install -g npm
```

**INFO** To start using the new version, you must log out and log back in for the shell to see it.

Lookout uses the bluez-tools and libudev-dev software. Here are the instructions for installing them:

```
sudo apt-get install bluez-tools
sudo apt-get install libudev-dev
```

Lookout requires the openaps cgm type to be ```xdrip```. If you are not running xdrip as the openaps cgm type, then offline looping will not work. To change cgm type, re-run oref0-setup and choose xdrip as the cgm type.

## Installation
```
cd ~
git clone https://github.com/xdrip-js/Lookout.git
cd Lookout
sudo npm install
sudo npm link
```
## Testing
```
npm test
```

## Updating Your Rig
```
cd ~/Lookout
git remote remove upstream # Just in case one already exists - this command may error, but that is OK
git remote add upstream https://github.com/xdrip-js/Lookout.git
git fetch upstream
git checkout --force upstream/master # Force a checkout of the current master even if we have made local changes
git checkout -B master # Force an overwrite of the local master with the upstream master
sudo npm install
sudo npm link
```
If your upstream is already set to the xdrip-js repository, you can skip the `git remote` commands.  The current git remote repositories can be displayed with the `git remote -v` command.

If you want to run the dev branch, replace `master` in the commands above with `dev`.

After updating the rig, reboot to restart Lookout with the updated version.

## Example usage
Just type `Lookout`. The app will run on port 3000.

To see verbose output, use `Lookout -v`. To see even more verbose output, use `Lookout -vv`. Alternatively, use `sudo DEBUG=* Lookout`, and replace the `*` with a comma separated list of the modules you would like to debug. E.g. `sudo DEBUG=transmitterIO:*,smp,transmitter,bluetooth-manager Lookout`.

To run in simulated mode, use `node index.js --sim`.

To view the app, open a browser and navigate to `http://<local IP address>:3000`. E.g. http://localhost:3000 or http://192.168.1.3:3000. This will vary depending on your local network setup.

![app](images/home.png)

## Using the browser to control your CGM
Once the browser is open to your Lookout page (see above steps), you can:
* pair a transmitter
* set the fakemeter id
* reset the paired transmitter
* view transmitter status
* start a sensor session
* stop a sensor session
* calibrate

**Note** You can continue using the Dexcom receiver alongside Lookout to do these things as well. Both the receiver and Lookout will get the latest updates from the CGM transmitter after a reading or two, provided they are in range and connected.

### Using the browser to pair Lookout to a transmitter
1. Click "Menu" button (bottom right button) on the Lookout page
2. Click `CGM` and then `Transmitter`
3. Click `Pair New`
4. Enter your transmitter ID (note it is case-sensitive)
5. Click `Save`

### Using the browser to start a CGM sensor session
1. Put the sensor/transmitter on your body, if you haven't already
2. Click the "Home" button (looks like a person) at the bottom left of the Lookout page
3. Click `Start sensor` (starting a sensor can be done either from the receiver or Lookout and it will show started on both)
4. Click the "Menu" button, then `CGM` and `Sensor`
5. Within 5 minutes the `State` should show as `Warmup`
6. Click the "Home" screen (bottom left, person button), you will also see Warmup on the Home screen
7. After 2 hours the state will change to `First calibration`
8. Click the `Calibrate` button to enter the first calibration
9. Enter the value from a finger stick and click `Save`
10. Click the "Home" screen
11. Click the `Calibrate` button to enter the second (you can wait for the state to change in 5 minutes, or enter it right after the first calibration)
12. Enter the value from a second finger stick and click `Save`
13. After 5 minutes the state will change to `OK` and calibrated BG values will be displayed.

**NOTE** There is a second button on the "Home" screen, `Start sensor 2 hours ago`, that can be used to send a start message backdated by 2 hours from the current time.  This allows the user to pre-soak a sensor while the ongoing session continues.  When the ongoing session ends, move the transmitter to the new sensor and use the `Start sensor 2 hours ago` button to start the new session.  This will normally provide the user with a First calibration request within 5 to 10 minutes instead of 2 hours.

### Using the browser to calibrate
1. Click the "Home" button (looks like a person) at the bottom left of the Lookout page
2. Click the `Calibrate` button to enter a finger stick calibration value
3. Enter the value from a finger stick and click `Save`

### Using the browser to stop a CGM sensor session
1. Click the "Menu" button, then `CGM` and `Sensor`
2. Click the `Stop Sensor` button
3. Click the `Stop` button to confirm
4. After 5 minutes, the sensor state will change to `Stopped`

### Using the browser to view the Pending command list
1. Click the "Menu" button, then `CGM` and `Sensor`
2. Click the `Pending` button

### Using the browser to reset the transmitter
1. Ensure the transmitter is paired as described above
2. Ensure the transmitter is not currently in a sensor session. Stop session if necessary.
3. Click the "Menu" button, then `CGM` and `Transmitter`
4. Click the `Reset Transmitter` button
5. Click the `Reset` button to confirm
6. Wait 5 minutes and press the "Menu" button, then `CGM` and `Transmitter`
7. Verify the `Age` is now less than a day.

### Using the browser to set the fakemeter meter id
1. Click the "Menu" button, then `CGM` and `Transmitter`
2. Click the `Set Fake Meter ID` button
3. Enter the ID, and click the `Save` button
4. Set a meter ID on the pump to match

## Using the command line to control your CGM
The commands below can be entered on the rig command line to control the CGM. Regardless of which command is entered, after executing the command the command will enter a status loop indefinately printing the CGM status at each glucose read event. Enter `Ctrl-C` to exit the command.
```
  lookout cal <sgv>                  # Calibrate the transmitter with provided glucose meter reading
  lookout id <id>                    # Set transmitter ID
  lookout meterid <meterid>          # Set transmitter ID
  lookout start [sensor_serial]      # Start sensor session; sensor serial required for G6
  lookout back-start [sensor_serial] # Start sensor session back dated by 2 hours; sensor serial required for G6
  lookout stop                       # Stop sensor session
  lookout reset                      # Reset transmitter
  lookout status                     # Show status  [default]
```

Use `-m` option for mmol instead of mg/dL. For example, `lookout -m cal 4.1` will calibrate with 4.1 mmol.

## Using Nightscout to control your CGM
Entering records in Nightscout can also be used to control your CGM. Lookout synchronizes with Nightscout 30 seconds prior to every transmitter read event. Therefore, the entries described below must be inserted into Nightscout at least 30 seconds prior to the next read event or it will not be executed until the following event.

### CGM Sensor Start
Enter a CGM Sensor Start into Nightscout to start a sensor session if one is not running. A sensor start will also cause Lookout to delete the extended and expired calibration values if they exist which will effectively end any extended or expired sensor sessions. If the transmitter does not have an active sensor session, Lookout will cease reporting calibrated glucose values.

### CGM Sensor Insert
Entering a CGM Sensor Insert will also cause Lookout to delete the extend and expired calibration values if they exist which will effectively end any extended or expired sensor sessions. If the transmitter does not have an active sensor session, Lookout will cease reporting calibrated glucose values.

### CGM Sensor Stop
Enter a sensor stop into Nightscout to stop a sensor session. 

### BG Check
Enter a BG Check into Nightscout to calibrate the transmitter and the expired calibration algorithm.

## Nightscout CGM Status Pill
This feature requires Nightscout 0.10.3 or later. Lookout provides devicestatus records to Nightscout which will display the CGM status in a CGM pill if the Nightscout xdrip-js plugin is enabled. See the Nightscout README for details on enabling the plugin and settings.

## Making it permanent
So far in the above you've only run Lookout from the command line - the next time you close your terminal, or reboot your rig, it will only run if you add it to your crontab:
```
<type the command `crontab -e` and add this line:>
@reboot Lookout >> /var/log/openaps/lookout.log 2>&1
<save and exit your editor>
<reboot your rig with the command `reboot`>
```

## Debugging
To look at the Lookout log, for debug purposes, type `cat /var/log/openaps/xdrip-js.log` or `tail -n 100 -F /var/log/openaps/xdrip-js.log`.
* If your xdrip-js.log file contains messages similar to `Error: /root/Lookout/node_modules/bluetooth-hci-socket/build/Release/binding.node: undefined symbol: _ZN2v816FunctionTemplate3NewEPNS_7IsolateEPFvRKNS_20FunctionCallbackInfoINS_5ValueEEEENS_5LocalIS4_EENSA_INS_9SignatureEEEi` run the following command: `cd ~/Lookout; npm rebuild`

## Lookout Command Line Options
* `--extend_sensor`, `-e`: Enables using the calibrated and unfiltered values reported by the CGM to calculate the running calibration slope and intercept values whenever the current calibration values it has produces a calibrated value that is more than 5 mg/dL away from the CGM reported calibrated value.  Lookout will apply the most recent calculated calibration to the CGM's unfiltered value if the transmitter does not report a calibrated glucose.  This enables Lookout to continue reporting glucose values after the sensor session is ended, providing greater flexibility on when the user changes the site.  This is not intended to extend a sensor life past 24 hours due to the lack of an ongoing calibration update mechanism.

**WARNING** If running in extended sensor mode, the user must enter a `Sensor Insert` in Nightscout to notify Lookout to stop reporting glucose values.

* `--expired_cal`, `-x`: Enables using user entered BG Check and Sensor Start records to calibrate raw unfiltered values reported by the CGM transmitter. Lookout does not perform calibration for 15 minutes after a Sensor Start. During the first 12 hours after a Sensor Start, Lookout only uses a Single Point calibration algorithm that assumes a y axis intercept of 0.  After the first 12 hours, Lookout will switch to using a Least Squares Regression algorithm to calculate the y axis intercept and slope to convert the raw unfiltered values to calibrated glucose values.  Transmitter provided calibrated glucose readings take presedence over the Lookout calibration algorithm.  If the transmitter doesn't provide a calibrated value, Lookout will calibrate the unfiltered value.  Extend sensor mode takes presedence over expired calibration mode.

**WARNING** Expired calibration mode uses a custom algorthm that is unable to guarantee the various sensor failure modes are appropriately handled. Therefore, it is NOT recommended. 

* `--verbose`, `-v`: Enables verbose logging.

* `--alternate`, `-c`: Enables using the alternate Bluetooth channel to communicate with transmitter. This allows the rig to operate in parallel with the Dexcom phone app or xDrip+.

* `--sim`, `-s`: Runs Lookout in simulation mode for offline testing.

* `--fakemeter`, `-f`: Enables sending blood glucose readings to the pump configured in the OpenAPS directory as meter readings. Configure the pump to enable the meterid configured in Lookout. This option requires oref0 version 0.7.0 or later.

* `--offline_fakemeter`, `-o`: Enables sending blood glucose readings to the pump as described in `--fakemeter`, but only when the rig is offline.

* `--port`, `-p`: Sets the port number for the web server providing the Lookout GUI.

* `--hci`, `-h`: Sets the Bluetooth adapter to use to communicate with transmitter.

* `--openaps`, `-d`: Sets the OpenAPS directory. The default directory is `/root/myopenaps`

* `--no_nightscout`, `-n`: Disable Nightscout interaction.

* `--include_mode`, `-i`: Append algorithm mode to the short state string displayed in Nightscout pill.

* `--read_only`, `-r`: Read only mode for a backup reader. Also lazy uploads to Nightscout to prevent double uploading glucose values.

## Reverting NodeJS

in the future if you decide you do not want to use xdrip-js, or you are having trouble updating OpenAPS with the nodejs update, you can revert the nodejs install with:
```
sudo apt-get remove nodered -y
sudo apt-get remove nodejs nodejs-legacy -y
sudo apt-get remove npm -y
sudo aptitude install nodejs-legacy
<say no to the first prompt about keeping nodejs-legacy at current version, say yes to the 2nd prompt about installing nodejs 'oldstable' version>
```

## Interaction with Dexcom Receiver
YDMV, so test it until you are comfortable. A few people have run Lookout concurrently with their Dexcom receiver without perceiving negative impacts to either. Others have been succesfull.
