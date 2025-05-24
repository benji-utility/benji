[![wakatime](https://wakatime.com/badge/github/kryllyxofficial01/benji.svg)](https://wakatime.com/badge/github/kryllyxofficial01/benji)

# benji

A light background application written in C that collects hardware information, accessible through a locally hosted TCP server

## Installation
1. Download and extract the zip attached to the latest release [here](https://github.com/benji-utility/benji/releases/latest)
2. Open Command Prompt as Administrator
3. Change the current working directory to where the files in the zip were extracted to
4. To modify the config before installation, see the [Config](#config-wip) section
5. Use the following command to install and start the Windows service:
```
.\benji-installer.exe install benji-service.exe config.toml
```
6. Upon finished execution the command will either print `Success` if it worked or state failure alongside the corresponding error code (see the lookup table below).

| Integer Code |            Error Name            |
|:------------:|:--------------------------------:|
|      -1      |          Unknown Command         |
|      -2      |    Insufficient Argument Count   |
|      -3      |     Unable to Open SC Manager    |
|      -4      |      Unable to Open Service      |
|      -5      |     Unable to Create Service     |
|      -6      |      Unable to Start Service     |
|      -7      |     Unable to Delete Service     |
|      -8      |   Unable to Create Registry Key  |
|      -9      | Unable to Set Registry Key Value |

## Usage Info (WIP, only for Windows)
To test grabbing [data](#data-requests-wip) from Benji, install Netcat [here](https://nmap.org/download.html#windows) and use one of the corresponding commands:
1. If the repo was cloned/downloaded, then change into the root of the project directory and run the following command:
```
make test TEST_DATA=<desired data> TEST_PORT=<the port stored in config.toml>
```
2. Otherwise you can use the following command to
```
(set /p="<desired data>") <nul | ncat 127.0.0.1 <the port stored in config.toml>
```

## Data Requests (WIP)
These are the data groups that comprise the request sent to Benji
|       Info       |        Group       |                                                Data                                                |
|:----------------:|:------------------:|:--------------------------------------------------------------------------------------------------:|
|        CPU       |       cpu_all      | Name, Vendor, Architecture, Clock Speed (GHz), Core Count, Logical Processor Count                 |
|        GPU       |       gpu_all      | Name, Vendor, Dedicated Video Memory (GB), Dedicated System Memory (GB), Shared System Memory (GB) |
|        RAM       |       ram_all      | Total Memory (GB), Memory Load (GB), Free Memory (GB), Speed (MHz)                                 |
|  Device Context  | device_context_all | Device Name, Operating System, Operating System Version + Build Number, Hostname                   |
| Storage Devices* |     storage_all    | Model Name, Serial Number, Bus Type, Capacity (GB)                                                 |

###### * These values are returned as CSV strings due to there possibly being more than one storage device

The groups are sent all at once, in a string separated by semicolons (;). Here is an example:
```
cpu_all;gpu_all;ram_all;
```

When creating this request manually, make sure all the data groups are followed by a semicolon, including the last one.

## Config (WIP)
Benji can be configured to run under certain contexts using a `config.toml` file provided during installation. See each subsection for the corresponding entries and their default values if not specified.

### Server
```toml
[server]
hostname = "127.0.0.1"
port = 8000
```

## Building Manually
1. Clone the repository to a chosen location with the following command
```
git clone --recurse-submodules https://github.com/benji-utility/benji.git
```
2. To build the main project, simply move into the root of the repository and run the `make` command
```
cd benji
make
```
3. The main executable will be stored at the top of the automatically-created `build` folder
