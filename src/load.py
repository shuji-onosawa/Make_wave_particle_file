import urllib.request

url = 'https://www.darts.isas.jaxa.jp/stp/akebono/output/lepi_89041500-1.txt'
save_path = '../lep_data/lepi_89041500-1.txt'

urllib.request.urlretrieve(url, save_path)
