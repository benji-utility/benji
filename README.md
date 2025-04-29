[![wakatime](https://wakatime.com/badge/github/kryllyxofficial01/benji.svg)](https://wakatime.com/badge/github/kryllyxofficial01/benji)

# benji

A light background application written in C that collects hardware information, accessible through a locally hosted TCP server

## Usage Info (WIP, only for Windows)
1. Download the executable attached to the latest release [here](https://github.com/benji-utility/benji/releases/latest)
2. Download TCPView from SysInternals [here](https://download.sysinternals.com/files/TCPView.zip)
    - This will not be how it is done later on, but it's the current method for testing
3. Run Command Prompt as Administrator
4. Run the following commands to create and setup the Windows Service
```
C:\Windows\system32> sc create BenjiService binPath="<path to the Benji executable>"
C:\Windows\system32> sc start BenjiService
```
5. Open TCPView, and search for the name of the executable (at time of writing it's simply `benji`)
6. Run the [`test_client.py`](https://github.com/benji-utility/benji/blob/main/test_client.py) script and input the TCP port associated with the process
7. It should return a JSON block containing details about the system
    - Change the string in the Python script to modify what data is being obtained
8. To stop Benji, run the following command
    - This closes the server and ends the Windows Service
```
C:\Windows\system32> sc stop BenjiService
```
8. If you wish to delete the Windows Service too, use this command
```
C:\Windows\system32> sc delete BenjiService
```

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
