import numpy as np
import csv

# LEPデータの読み込み
data = np.loadtxt('../lep_data/lepi_89041500-1.txt',
                  skiprows=1, usecols=range(1, 30), dtype=int)

# 時刻をリストとして取得する
time = np.loadtxt('example.txt', skiprows=1, usecols=0, dtype=str)
print
# MCAデータの読み込み
# 軌道データの読み込み
# 軌道データの補完
# サイクロトロン周波数の計算
# サイクロトロン周波数での電場強度の配列の作成

# 構造体の定義
struct = {'time': np.array([1, 2, 3, 4]),
          'lep': np.array([[[1, 2], [3, 4]], [[5, 6], [7, 8]]])}

# データをCSVファイルに保存する
with open('../execute/data.csv', mode='w', newline='') as file:
    writer = csv.writer(file)

    # 1次元配列の保存
    writer.writerow(['time'])
    for value in struct['time']:
        writer.writerow([value])

    # 3次元配列の保存
    writer.writerow(['lep'])
    for i in range(struct['lep'].shape[0]):
        for j in range(struct['lep'].shape[1]):
            writer.writerow(struct['lep'][i][j])
