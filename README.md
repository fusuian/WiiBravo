# WiiBravoアダプタ

ソニー製PlayStation1/2用ゲームコントローラを任天堂製ゲームコントローラWiiリモコンに接続するアダプタです。
DualShock2をクラシックコントローラの代わりとして使えるほか、ナムコ（現バンダイナムコゲームス）製バーチャルコンソールアーケードソフト『超絶倫人ベラボーマン』用に、ボタンを押す強さによって弱中強攻撃（ジャンプ）を行う機能があります。
また、PS用のアーケードコントローラを接続することもできます。
このとき、バーチャルコンソールのファミコンソフトとメガドライブソフトが遊びやすいようにボタン配列を変更できます。



## ハードウェア

WiiBravoアダプタは、マイコンボードと電子部品をブレッドボードに組み付けて作成します。
ブレッドボードの配線図は、 WiiBravo.fzz またはWiiBravo_BreadBoard.pdfをご覧ください。

 - ブレッドボード ハーフサイズ
 - マイコンボード Arduino Pro mini (3.3V版)
 - カーボン抵抗 560Ω x 1
 - カーボン抵抗 1kΩ x 3
 - 赤、黄、青色のLED 各1
 - ジャンパワイヤ 適宜

 - PlayStationコネクタ (マルチタップなどを分解して入手) x 1
 - Wiiリモコンケーブル (ヌンチャクのケーブルを切断) x 1


## ソフトウェア

USBシリアル変換モジュールでPCとArduino Pro Miniを接続して、
Arduino IDEで WiiBravo スケッチを書き込みます。

 - WiiBravo.ino         スケッチ本体
 - BravoButton.h        ベラボースイッチクラス
 - portmacro.h          digitalWrite に相当する、portOn/portOffマクロを定義

以下のソフトウェアは、Bruno Freitas氏によるwii-retropad-adapterから流用しています。
https://github.com/bootsector/wii-retropad-adapter

 - WMExtension.h/cpp    Wiiリモコン拡張コントローラ通信クラス
 - WMCrypt.h/cpp        Wiiリモコン拡張コントローラ通信暗号化クラス

### PS2X_libライブラリ

Bill Porter氏による、PlayStationコントローラ読み取りライブラリです。
https://github.com/madsci1016/Arduino-PS2X からダウンロードして、Arduino/libraliesフォルダに展開してください。

### ライセンス

ソフトウェアのライセンスはGPL3となります。詳しくはWiiBravo.inoをご覧ください。


## 使用方法

WiiBravoアダプタに PlayStationコントローラを接続してから、Wiiリモコンを接続します。

DualShock2とアーケードコントローラをつなぎ換える場合は、一度WiiBravoアダプタを外してからコントローラを抜き差ししてください。


### クラシックコントローラモード

PlayStationコントローラとしてDualShock2を接続すると、赤のLEDが1秒間点滅して、クラシックコントローラモードで起動します。

クラシックコントローラモードでは、ボタンは以下のように対応します。

                  DualShock2  : Wii クラシックコントローラ
                  R1 □ △ L1 :  R y x L
                  R2 ✕ ◯ L2 : ZR b a ZL


### ベラボーマンモード

クラシックコントローラモードで、SELECTボタンとL1ボタンを一緒に押すと青のLEDが1秒間点滅して、ベラボーマンモードになります。

ベラボーマンモードでは、△ボタンと✕ボタンが攻撃ボタンになり、□ボタンと◯ボタンがジャンプボタンになります。
各ボタンは、押し込む強さによって三段階の強さの攻撃またはジャンプになります。
(攻撃ボタンはy,x,L、ジャンプボタンはb,a,Rのいずれかを押したことになる)
L1ボタンとR1ボタンは使用しません。
R2ボタンでコイン投入、L2ボタンまたはSTARTボタンでゲームスタートとなります。

ベラボーマンモードでSELECTボタンとR1ボタンを一緒に押すと、クラシックコントローラモードになります。


### アーケードコントローラモード

PlayStationコントローラとしてアーケードコントローラを接続すると、赤のLEDが約1秒間点滅してアーケードコントローラモードで起動します。
作者はHORI製リアルアーケードPSのボタン配置を改造したものを使用していますが、他社製のコントローラでも使えると思います。

アーケードコントローラモードでは、ボタンは以下のように対応します。

       アーケードコントローラ : Wiiクラシックコントローラ
                  L2 □ △ L1 : ZL y x L
                  R2 ✕ ◯ R1 : ZR b a R



### ファミコンモード

また、SELECTボタンとL1ボタンを同時に押すと、黄色のLEDが約1秒間点滅してファミコンモードに移行します。
ファミコンモードでは、ボタンは以下のように対応します。

       アーケードコントローラ : Wiiクラシックコントローラ
                  L2 □ △ L1 : ZL y b a
                  R2 ✕ ◯ R1 : ZR x y a

このとき、上の段の y,b,a は、バーチャルコンソールのメガドライブソフトでは、メガドライブコントローラのA,B,Cボタンとして機能します。
そして下の段の x,y,a は、バーチャルコンソールのファミコンソフトでは、A,B,Aボタンとして機能します。

ファミコンモードでSELECTボタンとR1ボタンを同時に押すと、黄色のボタンが約1秒間点滅してアーケードコントローラモードに戻ります。



# トラブルシューティング

## PlayStationコントローラをつないでも方向キーやボタンが使えない

赤のLEDが長い周期で点滅していた場合、PlayStationコントローラを認識できていません。
このときは、WiiBravoアダプタからPlayStationコントローラを外し、WiiリモコンからWiiBravoアダプタを外し、もう一度接続し直してください。
それでも解決しない場合は、PlayStationコントローラまわりの配線やWiiケーブルを確認してください。


## ホームキーを押すには

Wiiコントローラのホームキーを押すには、SELECTボタンと方向キーの上を同時に押します。


## ベラボーマンモードで、弱中強の押し分けがうまくいかない

ソースコードWiiBravo.inoを修正して、Arduino Pro Miniのプログラムを更新します。

    const byte      low_max =  128;
    const uint16_t  middle_max   =  1000;

low_maxは、0～255までの値をとりますが、値が大きすぎると中や強が出にくくなります。
middle_maxは0～65535までの値をとりますが、low_maxより小さくしてはいけません。また、low_maxに近い値では中が出にくく、値が大きすぎると強が出にくくなります。

ボタンを押したフレームにおける押し下げ圧p1がlow_maxより小さければ弱、次のフレームでの押し下げ圧をp2として、p1*p2がmiddle_maxより小さいとき中、大きいとき強として判定します。


## アーケードコントローラモードで、ボタン配列を変更したい

ソースコードWiiBravo.ino内のac_buttons関数を修正して、Arduino Pro Miniのプログラムを更新します。

    void ac_buttons()
    {
      ab = ps2x.Button(PSB_CIRCLE);
      bb = ps2x.Button(PSB_CROSS);
      xb = ps2x.Button(PSB_TRIANGLE);
      yb = ps2x.Button(PSB_SQUARE);
      lb = ps2x.Button(PSB_L1);
      rb = ps2x.Button(PSB_R1);

      lt = lb*255;
      rt = rb*255;
    }

上記のプログラムでは、左辺であるab, bb, xb, yb, lb, rbはそれぞれWiiクラシックコントローラのa,b,x,y,L,Rボタンに対応します。
右辺のps2x.Buttonのカッコ内のPSB_CIRCLE, PSB_CROSS, PSB_TRIANGLE, PSB_SQUARE, PSB_L1, PSB_R1は、それぞれPlayStationアーケードコントローラの◯,✕,△,□,L1,R1ボタンに対応します。
手持ちのコントローラに合わせてそれぞれの代入式を変更してください。
