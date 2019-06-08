.. _settings-sample:

Settings:
#########################

Overview
********

This is a simple application demonstrating use of the settings
module.  In this application, several items are stored in flash
and loaded after reboot or using a on demand load.

Requirements
************

* A board with flash support or qemu_x86 with flash emulator.

Building and Running
********************

This sample can be found under :file:`samples/subsys/settings` in the Zephyr tree.

The sample can be build for several platforms, the following commands build the
application for the nrf51_pca10028 board.

.. zephyr-app-commands::
   :zephyr-app: samples/subsys/settings
   :board: nrf51_pca10028
   :goals: build flash
   :compact:

After flashing the image to the board the output on the console shows the loading
of several settings.

Sample Output
=============

.. code-block:: console


