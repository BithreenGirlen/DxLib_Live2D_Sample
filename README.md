# DxLib_Live2D_Sample
DxLibを使ったLive2Dサンプル。  
以下の機能を試せます。
- モーション切り替え
- モーション速度変更
- モデル拡縮
- モデル回転
- 位置移動
- 透過表示

## 実行例
[![実行例](../media/SampleScreen_000.png)](../../raw/media/SampleVideo_000.mkv)

## 動作要件
- MSVC 2015-2022 (x64)
- [Live2DCubismCore.dll (x86_64)](https://www.live2d.com/en/sdk/download/native/)
  - 実行ファイルと同階層に置いて下さい。(デバッグする際も同様)

## マウス操作

| 入力  | 機能  |
| --- | --- |
| マウスホイール | モデル拡大・縮小。 |
| 左ボタン + マウスホイール | モーション加速・減速。 |
| 右ボタン + マウスホイール | モデル回転。 |
| 左ボタンドラッグ | 描画位置移動。 |
| 中央ボタン | 尺度・速度・描画位置初期化。 |
| 右ボタン + 中ボタンクリック | 窓枠消去・表示。消去時にはモニタ原点位置に移動。 |
| 右ボタン + 左ボタンクリック | 窓移動。 窓枠消去時のみ動作。|

## 使用ライブラリ
- [DXライブラリ](https://dxlib.xsrv.jp/)

`.vcxproj`はプロジェクトフォルダ以下に`deps`階層を想定しています。ビルドする際には補ってください。
<pre>
DxLib_Live2D_Sample
  ├ deps
  │  └ プロジェクトに追加すべきファイル_VC用 // DXライブラリの.lib, .hファイル群
  │    └ ...
  ├ dpiAwareness.manifest
  └ ...
</pre>

## 使用許諾
[Live2D Open Software 使用許諾契約書](https://www.live2d.com/eula/live2d-open-software-license-agreement_jp.html)  
[Live2D Proprietary Software 使用許諾契約書](https://www.live2d.com/eula/live2d-proprietary-software-license-agreement_jp.html)
