MaxSonar_LV_Serial
==================

MaxSonar LV series Serial connection to Teensy3
Since NONE of the scripts I found around worked as expected
(most don't work, some has tons of errors and results are unpredictables)
and I'm using a Teensy3, I wroted this that works and check the serial stream
to ensure that data has consistence and it's in the right order.
MaxSonar has great products but this one, checked for days with Logic Analyzer,
has some troubles with serial output. The stream should be:
Rxxx<cr>
but it's not always like this, sometime the 3 bytes number it's 4 or more/less
and so on so every attemp to decode a fixed scheme fails.
This code identify first the R, then check for 3 bytes numbers and when <cr>
has identified it check that the data collected has the format expected.
The reason to use serial it's simple. It's the more accurate of the MaxBotix ones.

This sketch uses my oled library but you can convert oled.print in Serial.print and erase 
the un-needed oled stuff.
It uses the internal Teensy3 IntervalTimer to avoid any delay.
Actually it works in cm/mt but you can easily adapt to inc.
Pay attention to the MaxBotix sonar module you are using! 
Min ad Max values are different for every module!

A Note about RS232/TTL conversion
The damn MaxSonar it's using a NON standard RS232 connection at VCC (almost) level that it's NOT
compatible with TTL levels for Arduino or Teensy3. You have 2 choose here:
1) Use a MAX232 chip to convert RS232 to TTL5v and another level converter to bring TTL5V to TTL3V3
2) Use SoftwareSerial library and set yhe true flag on (it will invert the data)

I choosed the first method because I want to be sure that MaxSonar LV produce effectively errors
in serial protocol (confirmed by tests with 3 different modules) but I suggest use the second method.

