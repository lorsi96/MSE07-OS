import time

lines = [
    "## Greehouse simulator using MyOs - Small Demo ##", 
    "This program simulates a sensor network inside a greenhouse.",
    "These sensors report either temperature or humidity.",
    "Each of them is modeled in the application by a separate task.",
    "Firstly, we start picocom to monitor the board's serial port.",
    "We can observe all the data frames being transmitted by the program.", 
    "Note that each measurement is being timestamped. This is thanks to the RTC.",
    "TEC buttons generate events to suspend or resume sensor tasks.", 
    "Now TEC1 will be pressed. This will suspend all humidity sensor tasks.", 
    "Now TEC2 will be pressed. This will suspend all temperature sensor tasks.", 
    "We can note that no measurements are being generated in this state.", 
    "Now both TEC1 and TEC2 will be pressed, resuming all measurements.", 
]

for l in lines:
    for c in l:
        print(c, sep='', end='', flush=True)
        time.sleep(.05)
    input()
