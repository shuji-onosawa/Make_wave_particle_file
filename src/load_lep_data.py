import urllib.request

yy = '90'
MM = '02'
dd = '02'

# '00' から '23'までの時間をループ
for hh in [str(i).zfill(2) for i in range(24)]: 
    url = 'https://www.darts.isas.jaxa.jp/stp/akebono/SDB/'+yy+MM+'/'+yy+MM+dd+'/'+yy+MM+dd+hh+'.lep'
    save_path = '../lep_data/'+yy+MM+dd+hh+'.lep'
    # ファイルがリモートに存在するか確認
    try:
        urllib.request.urlopen(url)
    except urllib.error.HTTPError as e:
        print(e.reason)
    else:
        # ファイルをダウンロード
        urllib.request.urlretrieve(url, save_path)
        print('Downloaded: '+yy+MM+dd+hh+'.lep')

