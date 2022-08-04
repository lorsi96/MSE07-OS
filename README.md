# Custom RTOS for EDU-CIAA-NXP

## Instructions

### Pre-requisites
* Docker
* EDU-CIAA-NXP Board 

### Getting Started
```sh
# Clone the repo.
git clone --recursive git@github.com:lorsi96/MSE07-OS.git workspace 
cd workspace
# Build & Flash the applicaiton.
./tools/ciaa-tools --build .  
./tools/ciaa-tools --flash .
```


