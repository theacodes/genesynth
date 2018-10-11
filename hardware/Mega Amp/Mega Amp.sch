EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Mega Amp"
Date "2018-10-06"
Rev "v1"
Comp "thea.codes"
Comment1 ""
Comment2 ""
Comment3 "CC BY SA 4.0"
Comment4 "Thea Flowers"
$EndDescr
$Comp
L Amplifier_Operational:TL074 U1
U 1 1 5BB3B7BB
P 6050 2650
F 0 "U1" H 6050 3017 50  0000 C CNN
F 1 "TL074" H 6050 2926 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 6000 2750 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 6100 2850 50  0001 C CNN
	1    6050 2650
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TL074 U1
U 2 1 5BB3B7F4
P 6050 4000
F 0 "U1" H 6050 4367 50  0000 C CNN
F 1 "TL074" H 6050 4276 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 6000 4100 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 6100 4200 50  0001 C CNN
	2    6050 4000
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TL074 U1
U 3 1 5BB3B819
P 7100 4100
F 0 "U1" H 7100 4467 50  0000 C CNN
F 1 "TL074" H 7100 4376 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 7050 4200 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 7150 4300 50  0001 C CNN
	3    7100 4100
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:TL074 U1
U 4 1 5BB3B837
P 7100 2750
F 0 "U1" H 7100 3117 50  0000 C CNN
F 1 "TL074" H 7100 3026 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 7050 2850 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 7150 2950 50  0001 C CNN
	4    7100 2750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 5BB3B894
P 4950 1600
F 0 "R6" V 4743 1600 50  0000 C CNN
F 1 "510" V 4834 1600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 4880 1600 50  0001 C CNN
F 3 "~" H 4950 1600 50  0001 C CNN
	1    4950 1600
	0    1    1    0   
$EndComp
$Comp
L Device:R R7
U 1 1 5BB3B8D8
P 5600 1600
F 0 "R7" V 5393 1600 50  0000 C CNN
F 1 "750" V 5484 1600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 5530 1600 50  0001 C CNN
F 3 "~" H 5600 1600 50  0001 C CNN
	1    5600 1600
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5BB3B955
P 6050 1600
F 0 "#PWR0101" H 6050 1350 50  0001 C CNN
F 1 "GND" H 6055 1427 50  0000 C CNN
F 2 "" H 6050 1600 50  0001 C CNN
F 3 "" H 6050 1600 50  0001 C CNN
	1    6050 1600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0102
U 1 1 5BB3B9BA
P 4500 1600
F 0 "#PWR0102" H 4500 1450 50  0001 C CNN
F 1 "+5V" H 4515 1773 50  0000 C CNN
F 2 "" H 4500 1600 50  0001 C CNN
F 3 "" H 4500 1600 50  0001 C CNN
	1    4500 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 1600 4800 1600
Wire Wire Line
	5100 1600 5250 1600
Wire Wire Line
	5750 1600 6050 1600
Wire Wire Line
	5250 1600 5250 2550
Wire Wire Line
	5250 2550 5750 2550
Connection ~ 5250 1600
Wire Wire Line
	5250 1600 5450 1600
Wire Wire Line
	5250 2550 5250 3100
Wire Wire Line
	5250 3900 5750 3900
Connection ~ 5250 2550
$Comp
L Device:CP1 C5
U 1 1 5BB3BB0B
P 5000 3400
F 0 "C5" H 5115 3446 50  0000 L CNN
F 1 "100uF" H 5115 3355 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 5000 3400 50  0001 C CNN
F 3 "~" H 5000 3400 50  0001 C CNN
	1    5000 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 3250 5000 3100
Wire Wire Line
	5000 3100 5250 3100
Connection ~ 5250 3100
Wire Wire Line
	5250 3100 5250 3900
$Comp
L power:GND #PWR0103
U 1 1 5BB3BC01
P 5000 3550
F 0 "#PWR0103" H 5000 3300 50  0001 C CNN
F 1 "GND" H 5005 3377 50  0000 C CNN
F 2 "" H 5000 3550 50  0001 C CNN
F 3 "" H 5000 3550 50  0001 C CNN
	1    5000 3550
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 5BB3BC28
P 6050 3000
F 0 "R8" V 5843 3000 50  0000 C CNN
F 1 "160k" V 5934 3000 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 5980 3000 50  0001 C CNN
F 3 "~" H 6050 3000 50  0001 C CNN
	1    6050 3000
	0    1    1    0   
$EndComp
$Comp
L Device:R R9
U 1 1 5BB3BC56
P 6050 4350
F 0 "R9" V 5843 4350 50  0000 C CNN
F 1 "160k" V 5934 4350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 5980 4350 50  0001 C CNN
F 3 "~" H 6050 4350 50  0001 C CNN
	1    6050 4350
	0    1    1    0   
$EndComp
$Comp
L Device:C C6
U 1 1 5BB3BD02
P 6050 3350
F 0 "C6" V 5798 3350 50  0000 C CNN
F 1 "270pF" V 5889 3350 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 6088 3200 50  0001 C CNN
F 3 "~" H 6050 3350 50  0001 C CNN
	1    6050 3350
	0    1    1    0   
$EndComp
$Comp
L Device:C C7
U 1 1 5BB3BD60
P 6050 4750
F 0 "C7" V 5798 4750 50  0000 C CNN
F 1 "270pf" V 5889 4750 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D6.0mm_W2.5mm_P5.00mm" H 6088 4600 50  0001 C CNN
F 3 "~" H 6050 4750 50  0001 C CNN
	1    6050 4750
	0    1    1    0   
$EndComp
Wire Wire Line
	6350 2650 6350 3000
Wire Wire Line
	6350 3000 6200 3000
Wire Wire Line
	6200 3350 6350 3350
Wire Wire Line
	6350 3350 6350 3000
Connection ~ 6350 3000
Wire Wire Line
	5900 3350 5750 3350
Wire Wire Line
	5750 3350 5750 3000
Wire Wire Line
	5900 3000 5750 3000
Connection ~ 5750 3000
Wire Wire Line
	5750 3000 5750 2750
Wire Wire Line
	5750 4100 5750 4350
Wire Wire Line
	5750 4350 5900 4350
Wire Wire Line
	5750 4350 5750 4750
Wire Wire Line
	5750 4750 5900 4750
Connection ~ 5750 4350
Wire Wire Line
	6200 4750 6350 4750
Wire Wire Line
	6350 4750 6350 4350
Wire Wire Line
	6200 4350 6350 4350
Connection ~ 6350 4350
Wire Wire Line
	6350 2650 6600 2650
Connection ~ 6350 2650
Wire Wire Line
	6350 4000 6600 4000
Wire Wire Line
	6800 4200 6800 4400
Wire Wire Line
	6800 4400 7400 4400
Wire Wire Line
	7400 2750 7400 3000
Wire Wire Line
	7400 3000 6800 3000
Wire Wire Line
	6800 3000 6800 2850
$Comp
L Device:R R10
U 1 1 5BB3F10E
P 7800 2900
F 0 "R10" H 7730 2854 50  0000 R CNN
F 1 "330" H 7730 2945 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 7730 2900 50  0001 C CNN
F 3 "~" H 7800 2900 50  0001 C CNN
	1    7800 2900
	-1   0    0    1   
$EndComp
$Comp
L Device:R R11
U 1 1 5BB3F19B
P 7800 3950
F 0 "R11" H 7730 3904 50  0000 R CNN
F 1 "330" H 7730 3995 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 7730 3950 50  0001 C CNN
F 3 "~" H 7800 3950 50  0001 C CNN
	1    7800 3950
	-1   0    0    1   
$EndComp
Wire Wire Line
	7400 4100 7800 4100
Connection ~ 7400 4100
Wire Wire Line
	7400 2750 7800 2750
Connection ~ 7400 2750
Wire Wire Line
	7800 3050 7800 3400
Wire Wire Line
	7400 4100 7400 4400
$Comp
L Device:CP1 C8
U 1 1 5BB400BC
P 8250 3400
F 0 "C8" V 8502 3400 50  0000 C CNN
F 1 "220uF" V 8411 3400 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 8250 3400 50  0001 C CNN
F 3 "~" H 8250 3400 50  0001 C CNN
	1    8250 3400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7800 3400 8100 3400
Connection ~ 7800 3400
Wire Wire Line
	7800 3400 7800 3800
$Comp
L Device:CP1 C1
U 1 1 5BB41358
P 3300 2350
F 0 "C1" V 3552 2350 50  0000 C CNN
F 1 "10uF" V 3461 2350 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 3300 2350 50  0001 C CNN
F 3 "~" H 3300 2350 50  0001 C CNN
	1    3300 2350
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 5BB413FD
P 4000 2350
F 0 "R2" V 3793 2350 50  0000 C CNN
F 1 "6.2k" V 3884 2350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3930 2350 50  0001 C CNN
F 3 "~" H 4000 2350 50  0001 C CNN
	1    4000 2350
	0    1    1    0   
$EndComp
$Comp
L Device:CP1 C2
U 1 1 5BB425FC
P 3300 2900
F 0 "C2" V 3048 2900 50  0000 C CNN
F 1 "1uF" V 3139 2900 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 3300 2900 50  0001 C CNN
F 3 "~" H 3300 2900 50  0001 C CNN
	1    3300 2900
	0    1    1    0   
$EndComp
$Comp
L Device:R R3
U 1 1 5BB42602
P 4000 2900
F 0 "R3" V 3793 2900 50  0000 C CNN
F 1 "100K" V 3884 2900 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3930 2900 50  0001 C CNN
F 3 "~" H 4000 2900 50  0001 C CNN
	1    4000 2900
	0    1    1    0   
$EndComp
$Comp
L Device:CP1 C3
U 1 1 5BB42BC3
P 3300 3400
F 0 "C3" V 3048 3400 50  0000 C CNN
F 1 "1uF" V 3139 3400 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 3300 3400 50  0001 C CNN
F 3 "~" H 3300 3400 50  0001 C CNN
	1    3300 3400
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5BB42BC9
P 4000 3400
F 0 "R4" V 3793 3400 50  0000 C CNN
F 1 "100K" V 3884 3400 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3930 3400 50  0001 C CNN
F 3 "~" H 4000 3400 50  0001 C CNN
	1    4000 3400
	0    1    1    0   
$EndComp
$Comp
L Device:CP1 C4
U 1 1 5BB43198
P 3300 3950
F 0 "C4" V 3552 3950 50  0000 C CNN
F 1 "10uF" V 3461 3950 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 3300 3950 50  0001 C CNN
F 3 "~" H 3300 3950 50  0001 C CNN
	1    3300 3950
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R5
U 1 1 5BB4319E
P 4000 3950
F 0 "R5" V 3793 3950 50  0000 C CNN
F 1 "6.2k" V 3884 3950 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 3930 3950 50  0001 C CNN
F 3 "~" H 4000 3950 50  0001 C CNN
	1    4000 3950
	0    1    1    0   
$EndComp
Wire Wire Line
	4150 2350 4450 2350
Wire Wire Line
	4450 3950 4150 3950
Wire Wire Line
	4150 3400 4450 3400
Wire Wire Line
	4450 2350 4450 2750
Wire Wire Line
	4150 2900 4450 2900
$Comp
L Device:R R1
U 1 1 5BB46524
P 2600 3300
F 0 "R1" H 2530 3254 50  0000 R CNN
F 1 "1.3k" H 2530 3345 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2530 3300 50  0001 C CNN
F 3 "~" H 2600 3300 50  0001 C CNN
	1    2600 3300
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 2900 3850 2900
Wire Wire Line
	3450 3400 3850 3400
Wire Wire Line
	3450 2350 3850 2350
Wire Wire Line
	3450 3950 3850 3950
Wire Wire Line
	3150 3400 3000 3400
Wire Wire Line
	3000 3400 3000 3150
Wire Wire Line
	3000 2900 3150 2900
Wire Wire Line
	3000 3150 2600 3150
Connection ~ 3000 3150
Wire Wire Line
	3000 3150 3000 2900
Wire Wire Line
	2450 2350 2600 2350
Wire Wire Line
	2450 3150 2600 3150
Connection ~ 2600 3150
Wire Wire Line
	2500 3950 2600 3950
$Comp
L power:GND #PWR0105
U 1 1 5BB4F3A4
P 2600 3450
F 0 "#PWR0105" H 2600 3200 50  0001 C CNN
F 1 "GND" H 2605 3277 50  0000 C CNN
F 2 "" H 2600 3450 50  0001 C CNN
F 3 "" H 2600 3450 50  0001 C CNN
	1    2600 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 2750 5750 2750
Connection ~ 4450 2750
Wire Wire Line
	4450 2750 4450 2900
Connection ~ 5750 2750
Wire Wire Line
	6350 4000 6350 4350
Connection ~ 6350 4000
Wire Wire Line
	4450 4100 5750 4100
Wire Wire Line
	4450 3400 4450 3950
Connection ~ 4450 3950
Wire Wire Line
	4450 3950 4450 4100
Connection ~ 5750 4100
$Comp
L Amplifier_Operational:TL074 U1
U 5 1 5BB5B946
P 3950 5750
F 0 "U1" H 3908 5796 50  0000 L CNN
F 1 "TL074" H 3908 5705 50  0000 L CNN
F 2 "Package_DIP:DIP-14_W7.62mm" H 3900 5850 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/tl071.pdf" H 4000 5950 50  0001 C CNN
	5    3950 5750
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 5BB5BA1A
P 3850 5450
F 0 "#PWR0106" H 3850 5300 50  0001 C CNN
F 1 "+5V" H 3865 5623 50  0000 C CNN
F 2 "" H 3850 5450 50  0001 C CNN
F 3 "" H 3850 5450 50  0001 C CNN
	1    3850 5450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 5BB5BA5D
P 3850 6050
F 0 "#PWR0107" H 3850 5800 50  0001 C CNN
F 1 "GND" H 3855 5877 50  0000 C CNN
F 2 "" H 3850 6050 50  0001 C CNN
F 3 "" H 3850 6050 50  0001 C CNN
	1    3850 6050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5BB5BC00
P 3450 5750
F 0 "C9" H 3565 5796 50  0000 L CNN
F 1 "100nF" H 3565 5705 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D3.8mm_W2.6mm_P2.50mm" H 3488 5600 50  0001 C CNN
F 3 "~" H 3450 5750 50  0001 C CNN
	1    3450 5750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 5450 3450 5450
Wire Wire Line
	3450 5450 3450 5600
Connection ~ 3850 5450
Wire Wire Line
	3850 6050 3450 6050
Wire Wire Line
	3450 6050 3450 5900
Connection ~ 3850 6050
$Comp
L Transistor_BJT:BC547 Q1
U 1 1 5BB6520F
P 7550 1350
F 0 "Q1" H 7741 1396 50  0000 L CNN
F 1 "BC547" H 7741 1305 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 7750 1275 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 7550 1350 50  0001 L CNN
	1    7550 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R14
U 1 1 5BB65596
P 7650 1700
F 0 "R14" H 7720 1746 50  0000 L CNN
F 1 "330" H 7720 1655 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 7580 1700 50  0001 C CNN
F 3 "~" H 7650 1700 50  0001 C CNN
	1    7650 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R13
U 1 1 5BB65627
P 7650 1000
F 0 "R13" H 7580 954 50  0000 R CNN
F 1 "27" H 7580 1045 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 7580 1000 50  0001 C CNN
F 3 "~" H 7650 1000 50  0001 C CNN
	1    7650 1000
	-1   0    0    1   
$EndComp
$Comp
L Device:R R12
U 1 1 5BB65685
P 7500 1150
F 0 "R12" V 7707 1150 50  0000 C CNN
F 1 "75K" V 7616 1150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 7430 1150 50  0001 C CNN
F 3 "~" H 7500 1150 50  0001 C CNN
	1    7500 1150
	0    -1   -1   0   
$EndComp
Connection ~ 7650 1150
$Comp
L Device:CP1 C10
U 1 1 5BB656E6
P 7200 1350
F 0 "C10" V 7452 1350 50  0000 C CNN
F 1 "220uF" V 7361 1350 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 7200 1350 50  0001 C CNN
F 3 "~" H 7200 1350 50  0001 C CNN
	1    7200 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7350 1150 7350 1350
Connection ~ 7350 1350
$Comp
L power:+5V #PWR0110
U 1 1 5BB687A0
P 7650 850
F 0 "#PWR0110" H 7650 700 50  0001 C CNN
F 1 "+5V" H 7665 1023 50  0000 C CNN
F 2 "" H 7650 850 50  0001 C CNN
F 3 "" H 7650 850 50  0001 C CNN
	1    7650 850 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 5BB687F5
P 7650 1850
F 0 "#PWR0111" H 7650 1600 50  0001 C CNN
F 1 "GND" H 7655 1677 50  0000 C CNN
F 2 "" H 7650 1850 50  0001 C CNN
F 3 "" H 7650 1850 50  0001 C CNN
	1    7650 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 1500 7650 1550
Connection ~ 7650 1550
Wire Wire Line
	7950 1550 7650 1550
$Comp
L Device:R R15
U 1 1 5BB654DE
P 8100 1550
F 0 "R15" V 8307 1550 50  0000 C CNN
F 1 "33" V 8216 1550 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 8030 1550 50  0001 C CNN
F 3 "~" H 8100 1550 50  0001 C CNN
	1    8100 1550
	0    -1   -1   0   
$EndComp
Text GLabel 8350 1550 1    50   Input ~ 0
HeadphoneLeft
Wire Wire Line
	8250 1550 8350 1550
Text GLabel 7050 1350 3    50   Input ~ 0
RawAudioLeft
$Comp
L Transistor_BJT:BC547 Q2
U 1 1 5BB72BF3
P 9050 1350
F 0 "Q2" H 9241 1396 50  0000 L CNN
F 1 "BC547" H 9241 1305 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-92_Inline" H 9250 1275 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/BC/BC547.pdf" H 9050 1350 50  0001 L CNN
	1    9050 1350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R18
U 1 1 5BB72BF9
P 9150 1700
F 0 "R18" H 9220 1746 50  0000 L CNN
F 1 "330" H 9220 1655 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 9080 1700 50  0001 C CNN
F 3 "~" H 9150 1700 50  0001 C CNN
	1    9150 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R17
U 1 1 5BB72BFF
P 9150 1000
F 0 "R17" H 9080 954 50  0000 R CNN
F 1 "27" H 9080 1045 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 9080 1000 50  0001 C CNN
F 3 "~" H 9150 1000 50  0001 C CNN
	1    9150 1000
	-1   0    0    1   
$EndComp
$Comp
L Device:R R16
U 1 1 5BB72C05
P 9000 1150
F 0 "R16" V 9207 1150 50  0000 C CNN
F 1 "75K" V 9116 1150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 8930 1150 50  0001 C CNN
F 3 "~" H 9000 1150 50  0001 C CNN
	1    9000 1150
	0    -1   -1   0   
$EndComp
Connection ~ 9150 1150
$Comp
L Device:CP1 C11
U 1 1 5BB72C0C
P 8700 1350
F 0 "C11" V 8952 1350 50  0000 C CNN
F 1 "220uF" V 8861 1350 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 8700 1350 50  0001 C CNN
F 3 "~" H 8700 1350 50  0001 C CNN
	1    8700 1350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8850 1150 8850 1350
$Comp
L power:+5V #PWR0112
U 1 1 5BB72C14
P 9150 850
F 0 "#PWR0112" H 9150 700 50  0001 C CNN
F 1 "+5V" H 9165 1023 50  0000 C CNN
F 2 "" H 9150 850 50  0001 C CNN
F 3 "" H 9150 850 50  0001 C CNN
	1    9150 850 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0113
U 1 1 5BB72C1A
P 9150 1850
F 0 "#PWR0113" H 9150 1600 50  0001 C CNN
F 1 "GND" H 9155 1677 50  0000 C CNN
F 2 "" H 9150 1850 50  0001 C CNN
F 3 "" H 9150 1850 50  0001 C CNN
	1    9150 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 1500 9150 1550
Connection ~ 9150 1550
Wire Wire Line
	9450 1550 9150 1550
$Comp
L Device:R R19
U 1 1 5BB72C23
P 9600 1550
F 0 "R19" V 9807 1550 50  0000 C CNN
F 1 "33" V 9716 1550 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 9530 1550 50  0001 C CNN
F 3 "~" H 9600 1550 50  0001 C CNN
	1    9600 1550
	0    -1   -1   0   
$EndComp
Text GLabel 9850 1550 1    50   Input ~ 0
HeadphoneRight
Wire Wire Line
	9750 1550 9850 1550
Text GLabel 8550 1350 3    50   Input ~ 0
RawAudioRight
Wire Notes Line
	6850 2150 6850 600 
Text Notes 7750 600  0    50   ~ 0
Optional Stereo Headphone Output
Connection ~ 8850 1350
Wire Notes Line
	10550 2150 10550 600 
Wire Notes Line
	6850 600  10550 600 
Wire Notes Line
	6850 2150 10550 2150
Text GLabel 6600 2500 1    50   Input ~ 0
RawAudioLeft
Text GLabel 6600 4150 3    50   Input ~ 0
RawAudioRight
Wire Wire Line
	6600 2500 6600 2650
Connection ~ 6600 2650
Wire Wire Line
	6600 2650 6800 2650
Wire Wire Line
	6600 4150 6600 4000
Connection ~ 6600 4000
Wire Wire Line
	6600 4000 6800 4000
Wire Wire Line
	8600 3400 8400 3400
$Comp
L Connector_Generic:Conn_01x06 J1
U 1 1 5BBA5162
P 1200 5800
F 0 "J1" H 1120 5275 50  0000 C CNN
F 1 "Conn_01x06" H 1120 5366 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical" H 1200 5800 50  0001 C CNN
F 3 "~" H 1200 5800 50  0001 C CNN
	1    1200 5800
	-1   0    0    1   
$EndComp
Text GLabel 1400 5800 2    50   Input ~ 0
YMLeft
Text GLabel 1400 5600 2    50   Input ~ 0
YMRight
Text GLabel 1400 5700 2    50   Input ~ 0
SN
$Comp
L power:GND #PWR01
U 1 1 5BBA92AC
P 1400 5500
F 0 "#PWR01" H 1400 5250 50  0001 C CNN
F 1 "GND" V 1405 5372 50  0000 R CNN
F 2 "" H 1400 5500 50  0001 C CNN
F 3 "" H 1400 5500 50  0001 C CNN
	1    1400 5500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR03
U 1 1 5BBA9398
P 1400 5900
F 0 "#PWR03" H 1400 5650 50  0001 C CNN
F 1 "GND" V 1405 5772 50  0000 R CNN
F 2 "" H 1400 5900 50  0001 C CNN
F 3 "" H 1400 5900 50  0001 C CNN
	1    1400 5900
	0    -1   -1   0   
$EndComp
$Comp
L power:+5V #PWR02
U 1 1 5BBA93FD
P 1400 6000
F 0 "#PWR02" H 1400 5850 50  0001 C CNN
F 1 "+5V" V 1415 6128 50  0000 L CNN
F 2 "" H 1400 6000 50  0001 C CNN
F 3 "" H 1400 6000 50  0001 C CNN
	1    1400 6000
	0    1    1    0   
$EndComp
Text GLabel 2450 2350 0    50   Input ~ 0
YMLeft
Text GLabel 2500 3950 0    50   Input ~ 0
YMRight
Text GLabel 2450 3150 0    50   Input ~ 0
SN
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5BBBE362
P 2050 5800
F 0 "J2" H 1970 5275 50  0000 C CNN
F 1 "Conn_01x06" H 1970 5366 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x06_P2.54mm_Vertical" H 2050 5800 50  0001 C CNN
F 3 "~" H 2050 5800 50  0001 C CNN
	1    2050 5800
	-1   0    0    1   
$EndComp
Text GLabel 2250 5600 2    50   Input ~ 0
HeadphoneLeft
$Comp
L power:GND #PWR04
U 1 1 5BBBE64F
P 2250 5700
F 0 "#PWR04" H 2250 5450 50  0001 C CNN
F 1 "GND" V 2255 5572 50  0000 R CNN
F 2 "" H 2250 5700 50  0001 C CNN
F 3 "" H 2250 5700 50  0001 C CNN
	1    2250 5700
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5BBBE72C
P 2250 5900
F 0 "#PWR05" H 2250 5650 50  0001 C CNN
F 1 "GND" V 2255 5772 50  0000 R CNN
F 2 "" H 2250 5900 50  0001 C CNN
F 3 "" H 2250 5900 50  0001 C CNN
	1    2250 5900
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5BBBE78B
P 2250 6000
F 0 "#PWR06" H 2250 5750 50  0001 C CNN
F 1 "GND" V 2255 5872 50  0000 R CNN
F 2 "" H 2250 6000 50  0001 C CNN
F 3 "" H 2250 6000 50  0001 C CNN
	1    2250 6000
	0    -1   -1   0   
$EndComp
Text GLabel 8600 3400 2    50   Input ~ 0
Mono
Text GLabel 2250 5800 2    50   Input ~ 0
Mono
Text GLabel 2250 5500 2    50   Input ~ 0
HeadphoneRight
$Comp
L Device:R R?
U 1 1 5BBF0F53
P 2600 2500
F 0 "R?" H 2530 2454 50  0000 R CNN
F 1 "2.2k" H 2530 2545 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2530 2500 50  0001 C CNN
F 3 "~" H 2600 2500 50  0001 C CNN
	1    2600 2500
	-1   0    0    1   
$EndComp
Connection ~ 2600 2350
Wire Wire Line
	2600 2350 3150 2350
$Comp
L power:GND #PWR?
U 1 1 5BBF0FCB
P 2600 2650
F 0 "#PWR?" H 2600 2400 50  0001 C CNN
F 1 "GND" H 2605 2477 50  0000 C CNN
F 2 "" H 2600 2650 50  0001 C CNN
F 3 "" H 2600 2650 50  0001 C CNN
	1    2600 2650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5BBF1026
P 2600 4100
F 0 "R?" H 2530 4054 50  0000 R CNN
F 1 "2.2k" H 2530 4145 50  0000 R CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P7.62mm_Horizontal" V 2530 4100 50  0001 C CNN
F 3 "~" H 2600 4100 50  0001 C CNN
	1    2600 4100
	-1   0    0    1   
$EndComp
Connection ~ 2600 3950
Wire Wire Line
	2600 3950 3150 3950
$Comp
L power:GND #PWR?
U 1 1 5BBF10A4
P 2600 4250
F 0 "#PWR?" H 2600 4000 50  0001 C CNN
F 1 "GND" H 2605 4077 50  0000 C CNN
F 2 "" H 2600 4250 50  0001 C CNN
F 3 "" H 2600 4250 50  0001 C CNN
	1    2600 4250
	1    0    0    -1  
$EndComp
Text Notes 1600 2600 0    50   ~ 0
Missing in v0\n(moved to motherboard)
Text Notes 1600 4200 0    50   ~ 0
Missing in v0\n(moved to motherboard)
$EndSCHEMATC
