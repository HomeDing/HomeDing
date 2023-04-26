# Micro Firmware (example)

The micro firmware for HomeDing enabled devices is not a real example but is used to flash small devices
with firmware for specific conditions.

The Over-the-air updates (OTA) allow uploading a new firmware to a device into the free ROM
that will be activated on the next boot.

When a firmware size exceeds the available free ROM it cannot be uploaded.

I this situation firware still can be uploaded using the USB / Serial port.

This is the situation where the micro firmware can help when access to the port is not available
e.g. in sealed bulbs or off-the shell devices.

These are typically ESP8266 based devices with 1 MByte Flash memory only.

The micro example firmware doesn't include elements other than device and ota and also doesn't support the http
access to the file system or built-in files.

The resulting size is probably small enough to fit into the remaining ROM and can be uploaded via OTA.

After reboot the device has still the prior device configuration and can then get a new firmware when OTA was enabled in the device configuration (env.json).

The filesystem including all configuration files can be deleted using the APIs

* <http://<devicename>/api/reset> --  to reset network parameters and reboot
* <http://<devicename>/api/resetall> -- to reset file system, network parameters and reboot
* <http://<devicename>/api/cleanweb> -- to remove files but configuration


## See also

* <https://homeding.github.io/examples/micro.htm>
