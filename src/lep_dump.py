import xarray as xr
import numpy as np
import struct


def parse(file: str, stime: str, etime: str) -> dict:
    # stimeの処理
    s = int(stime)
    if s == 0:  # 0:all
        s = (int(file[-4:]) % 100) * 10000
    h = s // 10000
    m = (s % 10000) // 100
    s %= 100
    stime1 = s + 60 * (m + 60 * h)

    # etimeの処理
    if int(etime) == 0:  # 0:all
        etime_val = 172800
    else:
        s = int(etime)
        h = s // 10000
        m = (s % 10000) // 100
        s %= 100
        etime_val = s + 60 * (m + 60 * h)

    return {
        "file": file,
        "stime1": stime1,
        "etime": etime_val
    }


def mkinfilename(args):
    return "../lep_data/" + args["file"] + ".lep"


def headerread(file_path):
    with open(file_path, 'rb') as f:
        # timedataのサイズに合わせて16000バイトを読み込む
        data = f.read(16000)

        # stimeを取得
        stime = struct.unpack_from('12s', data)[0].decode('utf-8')
        stdate, sttime = stime[:6], stime[6:]

        h, m, s = int(sttime[:2]), int(sttime[2:4]), int(sttime[4:6])

        print(f"\thead time : {h:02d}:{m:02d}:{s:02d}")


def dataread(file_path, stime1, etime):
    particles = []
    print(f"start time: {stime1}")
    print(f"end time  : {etime}")
    with open(file_path, 'rb') as f:
        # ヘッダ部分をスキップ
        f.seek(16000)

        while True:
            # lepdataのサイズに合わせてデータを読み込む
            data = f.read(16000)
            if not data:
                break
            # データが16000バイト未満の場合、ループを終了
            if len(data) < 16000:
                break
            # countdataを取得
            countdata = []
            for i in range(7830):
                offset = i * 2
                ele, ion = struct.unpack_from('BB', data, offset)
                countdata.append((ele, ion))

            # データを解析
            for k in range(15):
                s = stime1 + k * 8
                if s > etime:
                    break

                particle_data = {
                    'time': s,
                    'ele': [],
                    'ion': []
                }

                for i in range(18):  # 18方向
                    ele_data_in_angle = []
                    ion_data_in_angle = []
                    for j in range(29):  # 29energy step
                        ele_data = countdata[18 * 29 * k + 29 * i + j][0]
                        ion_data = countdata[18 * 29 * k + 29 * i + j][1]
                        ele_data_in_angle.append(ele_data)
                        ion_data_in_angle.append(ion_data)
                    particle_data['ele'].append(ele_data_in_angle)
                    particle_data['ion'].append(ion_data_in_angle)
                particles.append(particle_data)

    return particles


def lep_dump(file, stime, etime):
    args = parse(file, stime, etime)
    file_path = mkinfilename(args)

    particles = dataread(file_path, args["stime1"], args["etime"])

    # データをxarrayに変換
    time = [p['time'] for p in particles]
    ele = np.array([p['ele'] for p in particles])
    ion = np.array([p['ion'] for p in particles])

    ds = xr.Dataset(
        {
            'ele': (['time', 'angle', 'energy'], ele),
            'ion': (['time', 'angle', 'energy'], ion)
        },
        coords={
            'time': time,
            'angle': np.arange(18),
            'energy': np.arange(29)
        }
    )

    return ds
