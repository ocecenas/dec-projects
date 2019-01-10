# ece431-microcontrollers

# COMBINATION LOCK

****************TOOLS:****************

	Keypad
	pair of LEDs
	microcontroller

****************OPERATION (BARE BONES):****************

	if correct, 
		the green LED lights up for 2 sec
        reset attempt count to zero
        control real lock by relaying it a signal
				
	if incorrect,
	 the red LED lights up for 1 sec, 
         incorrect code attempt count will be incremented
         if count exceeds two attempts, prevents further attempts for 40 sec, 
      	red LED lights up for that entire 40 sec period
				
	Idle
		Pressing the pound sign (#) will submit the entry
		while any LED is on (i.e., while TPM interrupt is taking place) 
			prevent any entries 
  		Press astrik (*) to clear any entries (without being counted as an attempt"""
# LCD Stopwatch

****************TOOLS:****************

	2 spst switches 
	1 potwiper 
	LCD 
	microcontroller
	5V and 3.3V linear regulators
	(2) 3.3uF, (1) 10uF, (1) 0.1uF capacitors 
	12V battery

****************OPERATION:****************

The time passed will be displayed on the LCD, the timer will be reset to "00:00:00" by one button
and the other button will start/stop the time.
