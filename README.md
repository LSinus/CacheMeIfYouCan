# Cache Me If You Can: A reimplementation of SLUBStick for linux 5.14 

## Installation

Download Linux 5.14 tarball and setup the disk image for qemu
```sh
./setup.sh
```

Go to the VM folder and execute the `run.sh` script.
It compiles the exploit and modules (Modules were used during the development and are no longer needed).
Then it starts the VM
```sh
cd vmachine
./run.sh
```

## Try the POC

Once inside the VM there are two users configured:
1. `root` which password is unknown
2. `user` configured with no password

To open another shell inside the vm a telnet server is configured.
```sh
telnet localhost 5555
```

First of all log in as `user` then move to the root directory and execute the exploit binary
```sh
cd /
./exploit
```

Various debugging options are shown. Press 101 to execute the full exploit.
If the final message starts with `Content successfully changed`, then the POC worked,
and now it is possible to login as `root` user without password.

Log in from telnet as `root`

## POC video

https://github.com/user-attachments/assets/d9aa0643-0696-4779-8539-c3724dc6f4c4

## References
SLUBStick paper: https://www.usenix.org/conference/usenixsecurity24/presentation/maar-slubstick
CVE-2022-2588:  https://nvd.nist.gov/vuln/detail/cve-2022-2588
