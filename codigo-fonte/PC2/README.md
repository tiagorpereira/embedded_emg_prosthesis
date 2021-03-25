# embedded_emg_prosthesis

This repository provides the main code to run the final project of Embedded Operating Systems class (2020/2 Semester)

## Updates

The _'master'_ branch is only for final stable version, usually followed by a release. Modifications, updates and new features stay on current development branch until tested and approved to go to _'master'_. The first development branch is _'PC2_ros'_ and contains everything related to the second control point using ROS enviroment and python libraries.

## Quick Reference

### Commands:

#### Testing prosthesis control
**`roslaunch embedded_emg_prosthesis servo_test.launch`**
- Open hand **`rostopic pub /emgprot/servo/action std_msgs/UInt8 1`**
- Close hand **`rostopic pub /emgprot/servo/action std_msgs/UInt8 2`**

#### Testing emg processing


#### Full test


### Structure:

- **launch:** includes ROS launch files

- **scripts:** contains ROS node files and auxiliary modules