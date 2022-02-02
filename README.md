# M5WordLED: WORDLEの結果をLEDで光らせるやつ

## 何?

[Wordle](https://www.powerlanguage.co.uk/wordle/)の結果をMatrixLEDで表示します。

## いるもの

M5 ATOM  
https://docs.m5stack.com/en/core/atom_lite  

seeed Grove - RGB LED Matrix w/Driver  
https://www.seeedstudio.com/Grove-RGB-LED-Matrix-w-Driver.html  

## コンパイル

SPIFFSにファイルを書き込む。

```
pio run --target uploadfs
```

```main.cpp```のWiFiの設定(SSIDとパスワード)を環境に合わせて修正。

```c
const char SSID[] = "**********";
const char PSK[] = "+*********";
```

その後、ファームウェアのコンパイルと書き込む。

```
pio run --target upload
```

## 使い方

1. Wordleをプレイ
2. 終了後```SHARE```をクリック
3. ブラウザで```M5Wordled.local```にアクセス
4. テキストボックスに結果をペースト
5. ```OK```ボタンを押す
6. 光る！

