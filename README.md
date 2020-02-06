# kwp71scan
Car scan tool using kwp71(key word protcol 71).I'm making it for Alfa Romeo 155. For now.
kwp 71（キーワードプロトコル71）を使用したカースキャンツール。AlfaRomeo 155用に作成しています。今のところ。

# Status 状況
Using Arduino and L9637D, it is made to communicate with ECU and immobilizer unit. Currently, arduino is used, but in the future it may be replaced with a microcomputer such as PIC.

ArduinoとL9637Dを使って、ECUおよびイモビライザーユニットと通信するように作成しています。現在はarduinoを使っていますが、将来的にはPICなどのマイコンに置き換える可能性があります。

Currently, we are making a program to initialize the connection. The connection is confirmed on the Arduino serial monitor.
It will eventually be connected to the LCD and will be installed in the vehicle.

# About correspondence to other vehicles 他の車両への対応について
Tested with 155 16V and V6 ECU and immobilizer units.
155の16VとV6のECUとイモビライザーユニットでテストを行っています。

If you have a 90s Motronic vehicle with a Fiat / Alfa 3pin diagnostic connector, you may be able to use this program as is.
Fiat/Alfa 3pin 診断コネクターを持つ90年代のモトロニック搭載車両であれば、ここのプログラムがそのまま使用できる可能性があるかもしれません。

Protocols before OBD2 may have their own implementations, and it is not known exactly whether kwp71 will work unless tested on a real device.
OBD2以前のプロトコルは独自実装が含まれている可能性があり、kwp71であっても実機でテストしてみないと動作するかは正確には分かりません。

I don't have the environment to test other models. So I don't currently intend to import code for other vehicles. First, I completed a scanner for my car ;-) sorry..
わたしには他の車種をテストする環境を持っていません。ですので私は現在のところ他の車両用のコードを取り込むつもりはありません。まず私の車のスキャナを完成させてからです。ごめんなさい

With safe driving
