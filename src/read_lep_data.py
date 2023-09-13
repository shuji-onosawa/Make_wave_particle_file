import subprocess

# コマンドと引数をリストで指定

command = ["./lep", "yymmddhh.lep", "0", "0", "0", "0"]

try:
    # コマンドを実行し、標準出力と標準エラー出力を取得
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    # 結果を表示
    print("標準出力:")
    print(result.stdout)

    print("エラー出力:")
    print(result.stderr)

    # 終了コードを表示
    print("終了コード:", result.returncode)

except Exception as e:
    print("エラー:", e)
