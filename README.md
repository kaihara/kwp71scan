## :construction: Currently under development. :construction:

# kwp71scan
Car scan tool using kwp71(key word protcol 71).I'm making it for Alfa Romeo 155. For now.

# Progress
***Prototype***  
 - [X] Obtaining ECU / Immo unit.  
 - [X] Make ECU workbench.  
 - [ ] Prototype production.  
 - + [X] Creating a prototype board.  
 - + [X] Let Arduino and ECU communicate.  
 - + [ ] Get information from ECU.  
 - + [ ] Creating an in-vehicle prototype board.  
 - [ ] In-vehicle prototype testing.  
  
***Trial production***   
-  [ ] PCB board design.  
-  [ ] Create Trial production.  
-  [ ] In-vehicle Trial product testing.  

# Status
Using Arduino and L9637D, it is made to communicate with ECU and immobilizer unit. Currently, Arduino is used, but in the future it may be replaced with a microcomputer such as PIC.

Currently, I have created a program that initializes the connection with the ECU. The connection result is confirmed on the Arduino serial monitor.
Eventually the kpw71scan will be connected to the LCD and mounted on the vehicle dashboard.

# About Fit for other vehicles
Tested with 155 16V and V6 ECU and immobilizer units.

If you have a 90s Motronic ecu vehicle with a Fiat / Alfa 3pin diagnostic connector, you may be able to use this program as is.

Protocols before OBD2 may have their own implementations, and it is not known exactly whether kwp71 will work unless tested on a real device.

I don't have the environment to test other models. So I don't currently intend to import code for other vehicles. First, I completed a scanner for my car ;-) sorry..

With safe driving!!





# kwp71scan
kwp 71（キーワードプロトコル71）を使用したカースキャンツール。AlfaRomeo 155用に作成しています。今のところ。

# 現在の状況
ArduinoとL9637Dを使って、ECUおよびイモビライザーユニットと通信するように作成しています。現在はArduinoを使っていますが、将来的にはPICなどのマイコンに置き換える可能性があります。

現在、ECUとの接続を初期化するところまでプログラムを作成しました。 接続結果はArduinoシリアルモニターで確認しています。
最終的にはkpw71scanはLCDに接続し、車両のダッシュボードに取り付ける予定です。

# 他の車両への対応について
155の16VとV6のECUとイモビライザーユニットでテストを行っています。

Fiat/Alfa 3pin 診断コネクターを持つ90年代のモトロニックECU搭載車両であれば、ここのプログラムがそのまま使用できる可能性があるかもしれません。

OBD2以前のプロトコルは独自実装が含まれていることが多く、kwp71であっても実機でテストしてみないと動作するかは分かりません。

わたしには他の車種をテストする環境を持っていません。ですので私は現在のところ他の車両用のコードを取り込むつもりはありません。まず私の車のスキャナを完成させてからです。ごめんなさい

安全運転を！！
