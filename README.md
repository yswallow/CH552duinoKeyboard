## Remap対応CH552マクロパッドファームウェア

### できること

* Remapからのキー割当
* マトリックススキャン
* キーボードのキーの送信
* Modifier+キー (例: Ctrl+A)

### 未実装

* マウスキー
* レイヤー
* ConsumerControl

### 使い方

1. 本リポジトリをクローン
1. `KeyboardConfig.h.sample` を `KeyboardConfig.h` としてコピーし, 設定を書き換え
1. `CH552duinoKeyboard.ino`をArduino IDEで開く
1. [CH55xduino](https://github.com/DeqingSun/ch55xduino)をインストール
1. `USB Settings` から `USER CODE w/ 148B ram` を選択
1. コンパイル・書き込み

### 書き込みのコツ

1回目の書き込みがダメでも挿し込みから10秒以内にもう一度書き込みをすると大体成功する

