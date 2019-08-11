# Mitsubishi PHEV command line interface

This is a command line interface developed to control the Mitsubish Outlander PHEV via the WiFi interface.
It uses the phev library found [here](https://github.com/phev-remote/phevcore).

The quickest way to get started is using the docker build but instructions on how to manually build can be found [here]((https://github.com/phev-remote/phevctl/README.md#manual))

## Building

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
docker run papawattu/phevctl --help
```
You should see the following help.
```
Mitsubishi Outlander PHEV Remote CLI - Designed and coded by Jamie Nuttall 2019

Usage: phevctl [OPTION...] register
  or:  phevctl [OPTION...] battery
  or:  phevctl [OPTION...] aircon [on|off]
  or:  phevctl [OPTION...] headlights [on|off]


Program to control the car via the remote WiFi interface.  Requires this device
to be connected to the REMOTE**** access point with a valid IP address, which
is on the 192.168.8.x subnet.

THIS PROGRAM COMES WITH NO WARRANTY ANY DAMAGE TO THE CAR OR ANY OTHER
EQUIPMENT IS AT THE USERS OWN RISK.

  -m, --mac=<MAC ADDRESS>    MAC address.
  -v, --verbose              Verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to jamie@wattu.com.
```
