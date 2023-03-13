import cdflib

# CDFファイルの読み込み
cdf_name = '../mca_data/ak_h1_mca_19890415_v02.cdf'
created_xary = cdflib.cdf_to_xarray(cdf_name)

# epochをdatetimeに変換し、文字列にフォーマット
epoch = created_xary['Epoch'].values.tolist()
date_time_list = [cdflib.epochs.CDFepoch.breakdown_epoch(et) for et in epoch]
formatted_date_list = [f"{dt[0]}-{dt[1]:02}-{dt[2]:02} {dt[3]:02}:{dt[4]:02}:{dt[5]:02}" for dt in date_time_list]

# ファイルに書き込む
with open('../execute/date_time_list.txt', 'w') as f:
    f.write('\n'.join(formatted_date_list))