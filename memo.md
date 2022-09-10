# メモ

## 参考になるページ
上から順に読むといい。
- [CAN通信の基礎知識を知れるページ(ysk先輩著)](https://chibarobotstudio.esa.io/posts/62)
  > コード例はC++20現時点で未定義動作なので注意
- [STM32のCAN通信まわりの概要がわかるページ](https://www.st.com/content/ccc/resource/sales_and_marketing/presentation/product_presentation/group0/87/36/d9/f4/f2/ca/47/96/34.Controller%20area%20network%20(CAN)/files/34.STM32L4-Peripheral-Controller%20Area%20Network%20(CAN)%20Final_JP.pdf/jcr:content/translations/en.34.STM32L4-Peripheral-Controller%20Area%20Network%20(CAN)%20Final_JP.pdf)
- [レジスタ叩いてCAN通信してるページ](https://garberas.com/archives/3555)
- [STM32F103xxのリファレンスマニュアル](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)

## 仕様
- CubeMXからのhcanの設定を引き継ぐ。
- 先頭IDとそこから連番で受理する個数の組を複数個受け取り、それらをもとにフィルターを設定する。
- 送受信ができる。
- 受信時にLEDを点滅させる。
- STM32のbxCANペリフェラルが、拡張フレームフォーマットの場合IDを下位18ビットまでしか識別しないことがわかったので、このクラスを使う際には**接続されたバスに流れるフレームのIDが必ず2^19 - 1以下**でなければならない。  
さもなくば下位18ビットが等しい関係ないフレームを誤って受信し、コールバックを呼んでしまうだろう。  
...まあ、下位18ビットが被らないことが保証されるなら、正しく動くのだが...。
- また、IDを連番で受け取るときにはフィルタでマスクをかけるのだが、これは2のべき乗倍を取り出すことしかできない。  
よって、あたかもクラスのメンバ変数のアラインメントのように、パディングビットを含むIDの並びとなる。  

## 予約されたID
- EmergencyStop 0x0  
    メインPCからの非常停止用。データの中身は現時点で未規定。  
  モーターなどの場合、停止させるのか緩やかに減速させるのかなどは実装に任せる。  
- CanClass2Command 0x1  
  CanClass2が扱うコマンド用。これもメインPCからのもの。  
  x, Xの部分は対象のCanId。  
  - ChangeCanId (1000)0'00'00'00'(000x)X'XX'XX'XX  
    CanIdを変更するためのもの。これを受信しつつボタンが押されていたならIDを変更する。  
  - MakeSilent (1100)0'00'00'00'(000x)X'XX'XX'XX  
    対象のマイコンをSilentモードにする。  

- TellBaseId 0x2  
  ボタンを押すとマイコンのIDが送られる。  
  同時に複数の基板のボタンを押してはならない。  

- Null 0x00'07'FF'FF  
  このクラスを使う上で、もっとも大きい(優先度の低い)ID。  
  本来データは送信されない。  
  が、Id部分には何かとデフォルトで設定してあるので、もしかしたらこのIDで送信するマイコンがいるかもしれない。  
  そんなときにデバッグできるよう用意した。このIDから送られてくるフレームには、何の保証もされない。  


## その他メモ
- 何故わざわざフィルターなどというものを設定するのか？  
-> メインのCPUに負荷を与えずに不要なメッセージを破棄するため。  
- Filterの初期化はInitializationモード以外でも可能。  
- HALライブラリのお作法を誰かまとめてくれないかな。命名規則を合わせろとかいうやつじゃなく、
例えば大抵の関数はHAL_XXX_Init, HAL__XXX_DeInit, HAL_XXX_Start...などの組で存在するとかいう基礎部分について。
- 他のSTM32マイコンだとDualCAN(独立した二つのCANバスにつなげられる機能)があるらしい。  
その時、何故か28個に増えたフィルタバンクは二つのCANで融通しあって使うらしい。なんで？  
ともかく、DualCANの場合はフィルタバンクを14個ずつに分割してそれぞれで使うということにした。  
なお、いつものSTM32F103系ではCANは1つしかなく、この機能の設定部分は無視される。
