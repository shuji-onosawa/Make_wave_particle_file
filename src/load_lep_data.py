import urllib.request
import os


def load_lep_file(date):
    """
    date : str
        日付を表す文字列 yyMMdd
    """
    yy = date[:2]
    MM = date[2:4]
    dd = date[4:6]

    # '00' から '23'までの時間をループ
    for hh in [str(i).zfill(2) for i in range(24)]:
        url = 'https://www.darts.isas.jaxa.jp/stp/akebono/SDB/'+yy+MM+'/'+yy+MM+dd+'/'+yy+MM+dd+hh+'.lep'
        save_path = '../lep_data/'+yy+MM+dd+'/'+yy+MM+dd+hh+'.lep'
        os.makedirs(os.path.dirname(save_path), exist_ok=True)
        # ファイルがローカルに存在するか確認
        if os.path.exists(save_path):
            print('Already exists: '+yy+MM+dd+hh+'.lep')
            continue
        else:
            # ファイルがリモートに存在するか確認
            try:
                urllib.request.urlopen(url)
            except urllib.error.HTTPError as e:
                print(e.reason)
            else:
                # ファイルをダウンロード
                urllib.request.urlretrieve(url, save_path)
                print('Downloaded: '+yy+MM+dd+hh+'.lep')
