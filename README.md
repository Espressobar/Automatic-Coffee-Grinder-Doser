# Automatic-Coffee-Grinder-Doser
Getting the right coffee dose off of your grinder can be a tedious task - this Arduino sketch will let you chose from 2 - 5 different pre-set doses by the weight [g]

The project was first mentioned @ www.espressobar.dk in this thread: http://www.espressobar.dk/forums/showthread.php?9580-Mazzer-Major-DR-gt-Arduino-timer-gt-Doserless-gt-Polering&p=101197#post101197

Code is basically a danish translation and "adaptation" of the original code found here: https://www.wuala.com/Cosmicos/SoftFidelity/M%C3%BChlentimer/OledTimerWizard/?key=bh3lscgclbc5

Important: All .ino files must reside in a folder named "OLEDTimerWizard" and additional libraries must be available as per instructions in OLEDTimerWizard.ino

Documentation (HW and Menu flow diagram) can be found under "Issues" here:
https://github.com/SteffenLav/Automatic-Coffee-Grinder-Doser/issues/1

The HW wishlist - $12 total :) :

Double Opto-coupler Relay (switching both N+L):
http://www.ebay.com/itm/Two-2-Channel-Relay-Module-With-Optocoupler-For-PIC-AVR-DSP-ARM-Arduino-5V-New-/181752994310?hash=item2a51529a06

White OLED 128x64 pix:
http://www.ebay.com/itm/251797632348?_trksid=p2060353.m2749.l2649&ssPageName=STRK%3AMEBIDX%3AIT

230VAC to 5VDC power supply:
http://www.ebay.com/itm/DC-5V-1A-AC-to-DC-Power-Module-Supply-Isolation-AC85-265V-220V-110V-210V-AC-/261322500442?hash=item3cd808e95a

Arduino Nano ATmega328 – CH340G:
http://www.ebay.com/itm/USB-Nano-V3-0-ATmega328-16M-5V-Micro-controller-CH340G-board-For-Arduino-/311064700128?hash=item486ce6a4e0

Rotary Encoder w. Momentary Push:
http://www.ebay.com/itm/12mm-Rotary-Encoder-Push-Button-Switch-Keyswitch-Electronic-Components-Stable-/181601752480?hash=item2a484ed5a0

Note: By "adaptation" I mean changes to my liking(!) Also, to get the smalest possible dosing variation you should maintain a constant preasure on top of your beans in the hopper. Prefarably you should replace the standard hopper with a glass or plastic tube in wich you place a metal load on top of the beans...
