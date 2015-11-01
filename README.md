# JHLidarLite_V2
Interface for PulsedLight, Inc Lidar-Lite V2 Blue Label to NVIDIA Jetson TK1 Development Kit over I2C.

http://pulsedlight3d.com

Note: For interfacing LidarLite V1 to Jetson TK1, see https://github.com/jetsonhacks/JHLidarLite

Requires:

$ sudo apt-get install libi2c-dev i2c-tools

The Lidar-Lite appears as 0x62 on i2c bus 1 on the Jetson TK1 with this wiring:

<blockquote><p>VCC J3A1-1 ->  Lidar-Lite (+)    5V (red wire)<br>
GND J3A1-14 ->  Lidar-Lite (-)<br>
SCL J3A1-18 ->  Lidar-Lite (SCL)<br>
SDA J3A1-20 ->  Lidar-Lite (SDA)</p></blockquote>

Lidar-Lite V2 requires a 680uF capacitor between 5V and GND. See documentation for wiring.
