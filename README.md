# Mitsubishi PHEV command line interface

This is a command line interface developed to control the Mitsubish Outlander PHEV via the WiFi interface.
It uses the phev library found [here](https://github.com/phev-remote/phevcore).
** Differences with https://github.com/phev-remote/phevctl **
- ``` docker run vassio/phevctl lockstatus ``` Doors is locked or not
- ``` docker run vassio/phevctl chargestatus ``` 1 if charging, otherwise 0
 - ```docker run vassio/phevctl remaningchargestatus ``` remaining charge time in minutes
 - ``` docker run vassio/phevctl hvac ```

**Version 0.1**

**Updates**
ECU 0.3.0.0 version now supported (Long password version MY18)
Registration is now more robust
Supports setting the aircon mode, cool, heat or windscreen.  You can also set thw time to 10,20 and 30 minutes**

phevctl acmode cool 10

## License

MIT License

>  Copyright (c) 2019 Jamie Nuttall
>
>  Permission is hereby granted, free of charge, to any person obtaining a copy
>  of this software and associated documentation files (the "Software"), to deal
>  in the Software without restriction, including without limitation the rights
>  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>  copies of the Software, and to permit persons to whom the Software is
>  furnished to do so, subject to the following conditions:
>
>  The above copyright notice and this permission notice shall be included in
>  all copies or substantial portions of the Software.
>
>  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
>  THE SOFTWARE.

## Building
The quickest way to get started is using the existing docker image ...
```
docker run vassio/phevctl --help
```
Or if you'd prefer to build you own image. Or if you run it on OrangePi zero

### Docker build

Ensure docker is running

```
docker version
```
You should see something like this :-
```
Client: Docker Engine - Community
 Version:           19.03.1
 API version:       1.40
 Go version:        go1.12.5
 Git commit:        74b1e89
 Built:             Thu Jul 25 21:17:08 2019
 OS/Arch:           windows/amd64
 Experimental:      false

Server: Docker Engine - Community
 Engine:
  Version:          19.03.1
  API version:      1.40 (minimum version 1.12)
  Go version:       go1.12.5
  Git commit:       74b1e89
  Built:            Thu Jul 25 21:17:52 2019
  OS/Arch:          linux/amd64
  Experimental:     false
 containerd:
  Version:          v1.2.6
  GitCommit:        894b81a4b802e4eb2a91d1ce216b8817763c29fb
 runc:
  Version:          1.0.0-rc8
  GitCommit:        425e105d5a03fabd737a126ad93d62a9eeede87f
 docker-init:
  Version:          0.18.0
  GitCommit:        fec3683
```
Check out this repository by cloning
```
git clone https://github.com/phev-remote/phevctl
```
Then CD into the directory and build the image.
```
cd phevctl
docker build -t <replace with your docker user>/phevctl .
```
You should then have the image built and you can run as follows :-
```
docker run <replace with your docker user>/phevctl --help
```
You should see the following help.
```
Usage: phevctl [OPTION...] register
  or:  phevctl [OPTION...] battery
  or:  phevctl [OPTION...] chargestatus
  or:  phevctl [OPTION...] lockstatus
  or:  phevctl [OPTION...] hvac
  or:  phevctl [OPTION...] remaningchargestatus
  or:  phevctl [OPTION...] update
  or:  phevctl [OPTION...] aircon [on|off]
  or:  phevctl [OPTION...] acmode [heat|cool|windscreen] [10|20|30]
  or:  phevctl [OPTION...] headlights [on|off]
  or:  phevctl [OPTION...] parkinglights [on|off]
  or:  phevctl [OPTION...] monitor
  or:  phevctl [OPTION...] get <register>


Program to control the car via the remote WiFi interface.  Requires this device
to be connected to the REMOTE**** access point with a valid IP address, which
is on the 192.168.8.x subnet.

THIS PROGRAM COMES WITH NO WARRANTY ANY DAMAGE TO THE CAR OR ANY OTHER
EQUIPMENT IS AT THE USERS OWN RISK.

  -c, --car-model=<YEAR>     Model Year.
  -m, --mac=<MAC ADDRESS>    MAC address.
  -v, --verbose              Verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to jamie@wattu.com.
```
## Building manually

Please see the [BUILD.md](https://github.com/phev-remote/phevctl/blob/master/BUILD.md).

## Usage

Make sure your device is in range of the cars WiFI and connect to the access point, which starts __REMOTExxxxx__.  Enter the password as you would normally do on the app, then check the IP address you're device has been assigned.
Locate your Wi-Fi details and you should have a line that says your IP address.

### Windows command prompt
```
ipconfig
...
IPv4 Address. . . . . . . . . . . : 192.168.8.47
```
### Linux
```
ifconfig -a
...
wlan0:
   inet 192.168.8.47
```

## Registering your device

As in with the official app the device needs to be registered with the car.

Firstly put your car into registration mode instructions [here](https://www.mitsubishi-motors.com/en/products/outlander_phev/app/remote/jizen.html)

Then issue the following command.
```
docker run vassio/phevctl register
```
You should see the message that the car is now registered.
## Using commands
You can then follow the help instructions to get the battery level and switch on and off the air conditioning and head lights.
```
docker run vassio/phevctl battery

docker run vassio/phevctl chargestatus

docker run vassio/phevctl hvac

docker run vassio/phevctl remaningchargestatus

docker run vassio/phevctl acmode heat 10

docker run vassio/phevctl acmode cool 10

docker run vassio/phevctl aircon on

docker run vassio/phevctl headlights on

docker run vassio/phevctl lockstatus
```
Have fun!!!
