[![wakatime](https://wakatime.com/badge/github/kryllyxofficial01/benji.svg)](https://wakatime.com/badge/github/kryllyxofficial01/benji)

# benji

A light background application written in C that collects hardware information, accessible through a locally hosted TCP server

## Usage Info (WIP, only for Windows)
1. Download the executable attached to the latest release [here](https://github.com/benji-utility/benji/releases/latest)
2. Download TCPView from SysInternals [here](https://download.sysinternals.com/files/TCPView.zip)
    - This will not be how it is done later on, but it's the current method for testing
3. Install Netcat (good for testing communications with ports) [here](https://nmap.org/download.html#windows)
    - Necessary if you want to use either of the commands in step 7
4. Run Command Prompt as Administrator
5. Run the following commands to create and setup the Windows Service
```
C:\Windows\system32> sc create BenjiService binPath="<path to the Benji executable>"
C:\Windows\system32> sc start BenjiService
```
6. Open TCPView, and search for the name of the executable (at time of writing it's simply `benji`)
7. To test grabbing [data](#data-requests-wip) from Benji, use one of the corresponding commands, depending on the context:
    - If the repo was cloned/downloaded:
    ```
    $ make test TEST_DATA=<desired data> TEST_PORT=<the port found in step 6>
    ```
    - If using the executable from the latest release:
    ```
    $ (set /p="<desired data>") <nul | ncat 127.0.0.1 <the port found in step 6>
    ```
8. It should return a JSON block containing details about the system
    - Change the string in the Python script to modify what data is being obtained
9. To stop Benji, run the following command
    - This closes the server and ends the Windows Service
```
C:\Windows\system32> sc stop BenjiService
```
10. If you wish to delete the Windows Service too, use this command
```
C:\Windows\system32> sc delete BenjiService
```

## Data Requests (WIP)
These are the data groups that comprise the request sent to Benji
| Info                | Group              | Data                                                                                |
|---------------------|--------------------|-------------------------------------------------------------------------------------|
| CPU                 | cpu_all            | Name, Vendor, Architecture, Clock Speed (GHz), Core Count, Logical Processor Count  |
| GPU                 | gpu_all            | Name, Vendor, Dedicated Video Memory, Dedicated System Memory, Shared System Memory |
| RAM                 | ram_all            | Total Memory, Memory Load, Free Memory, Speed (MHz)                                 |
| Device Context      | device_context_all | Device Name, Operating System, Operating System Version + Build Number, Hostname    |
| Storage Devices[^1] | storage_all        | Model Name, Serial Number, Bus Type, Size (GB)                                      |

The groups are sent all at once, in a string separated by semicolons (;). Here is an example:
```
cpu_all;gpu_all;ram_all;
```

When creating this request manually, make sure all the data groups are followed by a semicolon, including the last one.

[^1] The values returned by this group are formated as CSV strings

## Building Manually
1. Clone the repository in your chosen location with the following command
```
$ git clone https://github.com/benji-utility/benji.git
```
2. To build the project, simply move into the root of the repository and run the `make` command
```
$ cd benji
$ make
```
3. The main executable will be stored at the top of the automatically-created `build` folder
