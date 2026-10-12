# WinMac自動確認

WindowsとMacの両方で動くネイティブ部品（DLL / dylib）を、push のたびに **GitHub Actions が Windows と Mac で同時に**組み立て・読み込みテストします。
「片方を直すともう片方が未確認」「DLL が読み込めない」を、人が往復せずに見つけるためのひな形です。

## まず試す（3手順）
**Use this template → コードを変えて push → Actions タブを開き、Windows と Mac の両方が緑か見る。** 赤なら Summary の報告文を AI に貼る。

> Mac の確認は **Apple Silicon（M1以降）のみ**。Intel Mac は対象外です。要るなら ci.yml の `os:` に `macos-15-intel` を足してください（`macos-13` は提供終了。今の Intel ラベルは [GitHub公式の一覧](https://docs.github.com/en/actions/reference/runners/github-hosted-runners) で確認）。
> テストが **0件だと赤**になります（`ctest --no-tests=error`）。「何も確かめていないのに緑」を防ぐためです。

## 中身
| 場所 | 役割 |
|---|---|
| `src/` | 見本のC部品（`winmac_add` / `winmac_platform`） |
| `CMakeLists.txt` | Windows=`.dll`、Mac=`.dylib` を組み立て、`load_test` を登録 |
| `tests/load_test.c` | 作ったライブラリを**実際に開いて**関数を呼ぶ |
| `.github/workflows/ci.yml` | `windows-latest` と `macos-latest`（Apple Silicon）で同時に実行 |
| `scripts/failure_report.py` | 失敗時に、OS・落ちた手順・ログの要点を短い文章にまとめる |

## 使い方は2通り
- **新しく作る**: このページ右上の **Use this template** → 自分のリポジトリができる → `src/` を自分の部品に置き換えて push。
- **今あるSDKに足す**: 下の4手順。

インストールするものはありません。動くのは自分のリポジトリの GitHub Actions です（Mac が手元に無くても確かめられます）。

## 自分のSDKへの入れ方
1. `.github/workflows/ci.yml` と `scripts/failure_report.py` を自分のリポジトリへコピー。
2. 自分の `CMakeLists.txt` に、共有ライブラリと `add_test`（読み込みテスト）を足す。見本の `CMakeLists.txt` / `tests/load_test.c` を真似る。
3. Windows は `__declspec(dllexport)` が無い関数は外から見えない（`src/winmac_demo.h` の `WINMAC_API`）。Mac側で動いてWindowsだけ「関数が見つからない」はたいていこれ。
4. CMake を使っていないSDKは、ci.yml の「組み立て」「読み込みテスト」の2手順だけ自分のコマンドに差し替える（下の「CMake以外の場合」）。
5. `src/` を自分の部品に置き換えるときは、`CMakeLists.txt` の `add_library(... src/xxx.c)` のファイル指定と、`tests/load_test.c` が呼ぶ関数名（`winmac_add` など）も自分のものに変える。

## CMake以外の場合
差し替えた2手順で、次の4つを守らないと**失敗しても報告が空**になります。
1. ログを `build.log`（組み立て）/ `test.log`（テスト）に保存する（`2>&1 | tee build.log`）。
2. 手順に `id: build` / `id: test` を残す（報告がこの id で落ちた手順を判定する）。
3. 失敗したら**非ゼロで終了**する（`shell: bash` は `pipefail` 付きなので、`| tee` でも失敗が伝わる）。テストが0件のときも落とす。
4. 必要な道具（コンパイラ・Python・Node など）の準備手順を `steps:` に足す（`actions/setup-python` など）。

コピーできる例（Makefile + Python のテストの場合）:
```yaml
      - uses: actions/setup-python@v5
        with: { python-version: "3.12" }

      - name: テスト道具を入れる
        run: python -m pip install pytest   # requirements.txt があるなら: python -m pip install -r requirements.txt

      - name: 組み立て
        id: build
        run: make all 2>&1 | tee build.log

      - name: 読み込みテスト
        id: test
        run: |
          python -m pytest -q 2>&1 | tee test.log
          # pytest は0件だと終了コード5で落ちる。別のテスト道具なら0件チェックを自分で足す
```

## 失敗したとき
Actions の該当実行の **Summary** に「CI失敗の報告」が出ます。その文章をそのまま Codex / Claude Code に貼れば直させられます。

報告の見本（わざと壊した実行から。Windows と Mac それぞれに出る）:
```text
【CI失敗の報告】Windows（X64）の確認で落ちました。
落ちた手順: 読み込みテスト
対象: <オーナー>/<リポジトリ> / コミット 540c6b3
ログの要点:
    CALL_FAIL: winmac_add(2,3) = -1（期待 5）
    1 - load_test (Failed)

お願い: 上のログを読んで原因を直してください。Windows 側だけの問題かもしれないので、もう片方のOSでも動く形で直してください。
```
手順: Actions の失敗した実行を開く → 各OSのジョブの Summary → 文章をコピー → Codex / Claude Code に貼る → 直させて push → 両OSが緑になるまで繰り返す。

## 限界（Actions でできないこと）
- **機器が要るテスト**（USB機器・カメラ・専用ドライバなど）は Actions にはありません。手元で行う。Actions で確かめるのは、機器なしで済むところ（組み立て・読み込み・純粋な計算）まで。
- 手元の確認: Windows は Visual Studio 付属の CMake で `cmake -S . -B build && cmake --build build --config Release && ctest --test-dir build -C Release`。

## 無料枠
- 公開リポジトリ: Windows・Mac とも無料。
- 非公開リポジトリ: 無料枠は月の分数が決まっており、**Mac は10倍で消費**（Windowsは2倍、Linuxは1倍）。Mac 1分 = 枠10分。頻繁に push するなら、`pull_request` と main だけに絞る。

## 確認状況
- Windows（手元）: 組み立て・読み込みテストとも通過（2026-10-12）。
- Mac（Actions の macos-latest、Apple Silicon）: 組み立て・読み込みテストとも通過（2026-10-12）。
- 失敗報告: わざと壊した実行で、Windows・Mac 両方に報告文が出ることを確認。
