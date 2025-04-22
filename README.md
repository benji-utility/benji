[![wakatime](https://wakatime.com/badge/github/kryllyxofficial01/benji.svg)](https://wakatime.com/badge/github/kryllyxofficial01/benji)

# benji

A light background application written in C that collects hardware information, accessible through a locally hosted TCP server

## Usage Info (WIP, only for Windows)
1. Download the executable attached to the latest release [here](https://github.com/benji-utility/benji/releases/latest)
2. Download TCPView from SysInternals [here](https://download.sysinternals.com/files/TCPView.zip)
    - This will not be how it is done later on, but it's the current method for testing
2. Run Command Prompt as Administrator
3. Run the following commands to create and setup the Windows Service
```
sc create BenjiService binPath="<path to the Benji executable>"
sc start BenjiService
```
4. Open TCPView, and search for the name of the executable (at time of writing it's simply `benji`)
5. Run the `test_client.py` script and input the TCP port associated with the process
6. It should return a JSON block containing details about the system
    - Change the string in the Python script to modify what data is being obtained
7. To stop Benji, run the following command
    - This closes the server and ends the Windows Service
```
sc stop BenjiService
```
8. If you wish to delete the Windows Service too, use this command
```
sc delete BenjiService
```