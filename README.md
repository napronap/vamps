日本語版は下にあります

## Vamps

Vamps is a 2v2 arena combat game played locally with bots inspired by the [now discontinued Type Moon's Fate/Grand Order Arcade](https://info-arcade-fate-go.sega.jp/2026/2026-01-29-1/). 

The project is mostly a playground I used to learn things like UE's GAS, combat systems, enemy AI, etc. Although it's a highly simplified version, the idea was to emulate FGOA's combat system and interactions. The idea is to add online functionalities in the future, but first I want to get the combat to work smoothly.

It is named vamps because in the beginning I wanted to give it a vampire theme... maybe I'll do it at some point.

## How to run

A packaged build can be downloaded from the [releases](https://github.com/napronap/vamps/releases) section of this repository (Windows only). Alternatively to build it yourself follow the instructions below.

Needs UE 5.8

1. Modify `UE58_DIR` in `vars.bat.example` to the root directory of your UE 5.8 installation
2. Rename `vars.bat.example` to `vars.bat`
3. Run `build.bat`
4. Run `editor.bat` or just open `vamps.uproject`

The project doesn't need any third party plugins to work, although MCP plugin is enabled. You can remove it by deleting it from the `vamps.uproject` file itself.

## How to play

Open **遊び方** (**How to play**) on the character selection screen. The in-game guide opens in Japanese and has an **English** button.

### Controls

| Input | Action |
| --- | --- |
| WASD | Move |
| RMB | Right-click to attack or guard |
| F | Switch targets |
| Esc | Pause |
| Left Shift | Tap to dash or backstep with no direction. Hold while moving to sprint. |

### Match rules

Defeat opponents to score. Reach the score goal first, or have more points when time runs out.

When active attacks clash, the attack that started first has priority. If your attack lands or is blocked, tap RMB during the combo window to chain.

**Mash RMB while you're under attack.** Each block needs a fresh press.

Dashing and sprinting spend stamina. It recovers after a short break. Blocking fills the guard gauge. A full gauge stuns you.

After your attack ends, you briefly can't attack or block. Dash or sprint away to make space while you recover.

## AI usage disclosure

AI was used in some of the more tedious parts so I could focus on doing the parts I wanted to learn and understand more about firsthand. Namely UI and some of the camera stuff.

---

## 日本語版

### Vamps

Vamps は、[稼働を終了した TYPE-MOON の『Fate/Grand Order Arcade』](https://info-arcade-fate-go.sega.jp/2026/2026-01-29-1/)に影響を受けた、ボットが参加するローカルの 2 対 2 アリーナ対戦ゲームです。

このプロジェクトは、UEのGASや戦闘システム、敵AIなどをいじりながら勉強するための遊び場みたいなものです。かなり簡略化していますが、FGO アーケードの戦闘システムや駆け引きを再現してみたくて作りました。いずれはオンライン機能も追加したいですが、まずは戦闘を気持ちよく遊べるようにしたいです。

最初は吸血鬼をテーマにするつもりだったので、Vampsという名前にしました。そのうち本当にそうするかも。。。

### 起動方法

このリポジトリの[Releases](https://github.com/napronap/vamps/releases)からパッケージ版をダウンロードできます（Windowsのみ）。自分でビルドしたい場合は、以下の手順でどうぞ。

UE 5.8 が必要です。

1. `vars.bat.example` の `UE58_DIR` を、UE 5.8をインストールしたフォルダのパスに書き換えます。
2. `vars.bat.example` を `vars.bat` にリネームします。
3. `build.bat` を実行します。
4. `editor.bat` を実行するか、`vamps.uproject` を直接開きます。

サードパーティ製のプラグインは不要です。MCPプラグインは有効にしていますが、使わない場合は `vamps.uproject` からその項目を削除できます。

### 遊び方

キャラクター選択画面の **遊び方** を開いてください。

#### 操作

| 入力 | 操作 |
| --- | --- |
| WASD | 移動 |
| RMB | 右クリックで攻撃・ガード |
| F | ロックオン対象を切り替え |
| Esc | ポーズ |
| Left Shift | 押すとダッシュ。方向入力がなければ後退。移動しながら長押しでスプリント |

#### 対戦ルール

敵を倒して得点。先に目標点に達するか、時間切れの時点で得点が多いチームが勝ちます。

攻撃がぶつかると、先に始めた攻撃が優先されます。自分の攻撃が当たるかガードされたら、受付時間内に右クリックするとコンボが続きます。

**相手の連続攻撃を受けたら、右クリックを連打しましょう**。一撃をガードするたびに、次の入力が必要です。

ダッシュとスプリントはスタミナを消費します。使わずにいると回復します。ガードするたびにゲージがたまり、満タンになるとスタンします。

攻撃を出し切った直後は、少しの間だけ攻撃もガードもできません。ダッシュやスプリントで距離を取りましょう。

### AI の使用について

自分で手を動かして学びたいところに集中できるように、ちょっと手間のかかる部分はAIに手伝ってもらいました。主にUIと、カメラまわりの一部です。
