![main](https://github.com/kaihara/kwp71scan/workflows/main/badge.svg?branch=master)

## :construction: Currently under development. :construction:

1.in progress / In-vehicle prototype testing  
- The coolant temperature, battery voltage, and DTC count were acquired correctly.  
**problem**
- Insufficient power supply to Arduino
- Idling became unstable
- Could not get engine speed data

2.Wanted information  
I'm investigating the kind of RAM read of KWP71. I am in trouble because I cannot find good information.  
At present, battery voltage and engine coolant temperature can be obtained from the ADC channel.  
  
Wanted information  
 - [X] Battery voltage[ADC]  
 - [X] Engine coolant temperature[ADC]
 - [X] Battery voltage  
 - [ ] Engine coolant temperature
 - [ ] Engine oil pressure  
 - [ ] Engine RPM  
 - [ ] Vehicle speed  
 - [ ] Engine load  
  
# kwp71scan
Car scan tool using kwp71(key word protcol 71).I'm making it for Alfa Romeo 155. For now.  

<img width="366" alt="Finished product image" src="https://user-images.githubusercontent.com/3794315/76039639-5db08400-5f90-11ea-8f99-b0416358e8c2.PNG">  
(Image of finished product)

# Progress
 ***Prototype production***  
 - [X] Get ECU / Immo unit.  
 - [X] Make ECU workbench.  
 - [X] Prototype production.  
   - [X] Creating a prototype board.  
   - [X] Let Arduino and ECU communicate.  
   - [X] Get information from ECU.  
   - [X] Creating an in-vehicle prototype board.  
 - [ ] In-vehicle prototype testing. - in progress -  
    
***Trial production***   
-  [ ] PCB board design.  
-  [ ] Create Trial production.  
-  [ ] In-vehicle Trial product testing.  

# Status
Using Arduino and L9637D, it is made to communicate with ECU and immobilizer unit. Currently, Arduino is used, but in the future it may be replaced with a microcomputer such as PIC.  
  
I have created a program that initializes the connection with the ECU.  
It is built with ECU, Arduino Nano (Every) and LCD(Hitachi HD44780 compatible + PCF8574A(I2C conversion adapter)).

# About fit for other vehicles
Tested with 155 16V and V6 ECU and immobilizer units.  
  
If you have a 90s Motronic ecu vehicle with a Fiat / Alfa 3pin diagnostic connector, you may be able to use this program as is.  
  
Protocols before OBD2 may have their own implementations, and it is not known exactly whether kwp71 will work unless tested on a real device.  
  
I don't have the environment to test other models. So I don't currently intend to import code for other vehicles. First, I completed a scanner for my car ;-) sorry..  
  
With safe driving!!  





# kwp71scan
kwp 71（キーワードプロトコル71）を使用したカースキャンツール。AlfaRomeo 155用に作成しています。今のところ。  
  
# 現在の状況
ArduinoとL9637Dを使って、ECUおよびイモビライザーユニットと通信するように作成しています。現在はArduinoを使っていますが、将来的にはPICなどのマイコンに置き換える可能性があります。  
  
現在、ECUとの接続するプログラムを作成しました。 ECUとArduino Nano(Every) とLCD(Hitachi HD44780互換品 + PCF8574A I2C変換アダプタ)で構築しています。  
最終的にそれらを車両のダッシュボードに取り付ける予定です。  
  
# 他の車両への対応について
155の16VとV6のECUとイモビライザーユニットでテストを行っています。  
  
Fiat/Alfa 3pin 診断コネクターを持つ90年代のモトロニックECU搭載車両であれば、ここのプログラムがそのまま使用できる可能性があるかもしれません。  
  
OBD2以前のプロトコルは独自実装が含まれていることが多く、kwp71であっても実機でテストしてみないと動作するかは分かりません。  
  
わたしには他の車種をテストする環境を持っていません。ですので私は現在のところ他の車両用のコードを取り込むつもりはありません。まず私の車のスキャナを完成させてからです。ごめんなさい  
  
安全運転を！！  
