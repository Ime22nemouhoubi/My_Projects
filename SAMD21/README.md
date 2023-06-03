# Water Monitoring project using SAMD21

Briefly describe the purpose and goal of your project.

## Table of Contents

- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction
The project utilizes the SAMD21 microcontroller and Trampoline RTOS to retrieve data from two sensors: an ultrasonic sensor for distance measurement and a pH sensor for pH level detection. The primary goal of the project is to collect sensor data efficiently while minimising energy consumption and network traffic in the cloud.

The SAMD21 microcontroller, known for its low power consumption and high-performance capabilities, serves as the main control unit for the project. It provides the necessary processing power and I/O capabilities to interface with the sensors and execute the required tasks.

Trampoline RTOS (Real-Time Operating System) is employed to manage concurrent tasks and ensure the timely execution of critical operations. The RTOS offers task scheduling, inter-task communication, and synchronisation mechanisms, enabling efficient and deterministic execution of sensor reading, data compression, and data transmission tasks.

The project employs data compression techniques to reduce energy consumption and network traffic. A current value is defined as the baseline, representing a state where no significant change is detected in the sensor data. A difference limitation of 10% is set, allowing changes within this range to be considered insignificant and not transmitted to the cloud. This approach reduces unnecessary data transmission and optimises energy usage, especially in resource-constrained environments.

By leveraging the SAMD21 microcontroller, Trampoline RTOS, and data compression techniques, the project achieves efficient data collection, reduced energy consumption, and optimised cloud communication. The collected data can be used for various applications, such as environmental monitoring, smart agriculture, or industrial automation.

In the following sections, we will provide detailed instructions on how to set up and use the project, including prerequisites, installation steps, and usage guidelines


## Prerequisites

The neccessary tools, software,, libraries and dependencies to install  

- SAMD21 microcontroller: [SAMD21 Documentation](https://www.microchip.com/wwwproducts/en/ATSAMD21E18)
- Trampoline: [Trampoline Installation Link] (https://github.com/TrampolineRTOS/trampoline)
- Segger Embedded studio [ Segger Embedded studio Installation Link] (https://www.segger.com/products/development-tools/embedded-studio/.)
- GCC Compiler [ GCC Compiler Installation Link] (https://gcc.gnu.org/buildstat.html) 
- Oil [ OIL official documentation] (http://www.irisa.fr/alf/downloads/puaut/TPNXT/images/oil25.pdf)
- R Package: [R Installation Guide](https://www.r-project.org/)
- curl Library: [Curl Installation Guide](https://curl.se/download.html)
- Python (version x.x.x): [Download Python](https://www.python.org/downloads/)


## Installation

The following instructions must be followed thoroughly for the correct emplementation of the code: 

1. Download and install the necessary software and drivers for the SAMD21 microcontroller by following the official documentation: [SAMD21 Documentation](https://www.microchip.com/wwwproducts/en/ATSAMD21E18)

2. Connect the SAMD21 microcontroller to your computer.

3. Clone or download the project code from the repository.

4. Open the project in your preferred integrated development environment ( Visual Studio or Other) or text editor.

5. Build the project using the appropriate build system settings.

6. Develop and edit the .oil file according to the specifications of the code

 7.Connect any required peripherals or sensors to the SAMD21 microcontroller according to the project specifications.
 
 8. Open the terminal in Visual Studio to compile the code, follow these steps
    a. cd to your directory 
    b. ./runGoil.sh 
    c. ./make.py
    d. ./flash.sh


## Usage

## Usage

To use the project and retrieve data from the sensors using the SAMD21 microcontroller and Trampoline RTOS, follow the steps below:

### Prerequisites

Before proceeding, ensure that you have the following prerequisites:

- SAMD21 microcontroller with necessary firmware and drivers.
- Trampoline RTOS installed and configured on the microcontroller.
- Ultrasonic sensor connected to the appropriate pins of the microcontroller.
- pH sensor connected to the designated pins of the microcontroller.
- Required libraries or dependencies installed as mentioned in the [Prerequisites](#prerequisites) section.

### Steps

1. Set up the hardware by connecting the SAMD21 microcontroller to the sensors (ultrasonic and pH) as per the project specifications. Make sure the connections are secure and accurate.

2. Power on the SAMD21 microcontroller and ensure it is properly initialized.

3. Build and flash the project code onto the microcontroller using your preferred toolchain or IDE. Make sure to select the correct target device (SAMD21) and appropriate build configurations.

4. Once the code is successfully uploaded, the microcontroller will start executing the tasks defined in the project.

5. The ultrasonic sensor task will periodically measure the distance and update the `distance_data` variable with the latest distance value.

6. The pH sensor task will read the pH value and update the `ph_data` variable with the latest pH measurement.

7. The data compression task will compare the current values of `distance_data` and `ph_data` with a defined threshold. If the difference exceeds the threshold (10% in this case), it will print the respective data to the console.

8. The data transmission task will use the curl library to send the `distance_data` and `ph_data` to the designated URLs (`http://localhost:8080/distance_data` and `http://localhost:8080/ph_data`). Adjust the URLs as per your specific cloud or server setup.

9. Monitor the output or observe the behavior of the system to verify that the sensor data is being retrieved, compressed, and transmitted accurately.

10. Customize or extend the project code as needed to meet your specific requirements. You can modify the task execution frequencies, add additional tasks, or integrate more sensors as per your project needs.

Please note that the above steps provide a general overview of the usage process. You may need to make adjustments or modifications based on your specific hardware setup, software configurations, and project requirements.

Feel free to explore the codebase and documentation to understand the project structure, task scheduling, and interaction with the sensors. Additionally, refer to the respective sensor and microcontroller documentation for detailed information on pin configurations, libraries, and APIs.

Remember to handle the sensor data appropriately in your backend or cloud infrastructure once it is transmitted.

Enjoy using the project and feel free to reach out if you have any questions or need further assistance.

## Contributing



## License


Example: This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

