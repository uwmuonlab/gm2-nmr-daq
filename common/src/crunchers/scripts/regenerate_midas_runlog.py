import sys
from collections import OrderedDict
import re
import simplejson as json
import glob
import midas

def main():
    attr_file = '/home/newg2/Applications/gm2-nmr/resources/log/midas_runlog.json'
    arch_dir = '/home/newg2/Applications/gm2-nmr/resources/history'

    try:
        run_attr = json.loads(open(attr_file).read(),
                              object_pairs_hook=OrderedDict)

    except:
        run_attr = json.loads('{}', object_pairs_hook=OrderedDict)

    odb = midas.ODB('gm2-nmr')

    if len(sys.argv) > 1:
        start = int(sys.argv[1])

    else:
        start = 1

    if len(sys.argv) > 2:
        stop = int(sys.argv[2])

    else:
        stop = int(odb.get_value('/Runinfo/Run number').rstrip())

        if (odb.get_value('/Runinfo/State').rstrip() != '1'):
            stop -= 1

    for run_num in range(start, stop + 1):
        run_files = glob.glob(arch_dir + '/*%05i*' % run_num)
        key = "run_%05i" % run_num
        run_attr[key] = OrderedDict()

        # Set the order and initialize.
        run_attr[key]['comment'] = ''
        run_attr[key]['tags'] = []
        run_attr[key]['start_time'] = 0
        run_attr[key]['stop_time'] = 0
        run_attr[key]['field_on'] = False
        run_attr[key]['step_size'] = 0.0
        run_attr[key]['laser_point'] = ''
        run_attr[key]['laser_swap'] = False
        run_attr[key]['laser_phi_offset'] = 0.0
        run_attr[key]['metrolab_angle'] = -1.0

        for f in run_files:

            got_step_size = False

            for line in open(f):

                if 'xml' in f:

                    if '"Comment" type="STRING" size="80"' in line:
                        comment = line.split('>')[1].split('<')[0].rstrip()
                        run_attr[key]['comment'] = comment

                    if '"Tags" type="STRING"' in line:
                        tags = line.split('>')[1].split('<')[0].rstrip()
                        run_attr[key]['tags'] = tags.split(',')

                    if '"Start time binary" type="DWORD"' in line:
                        start_time = int(line.split('>')[1].split('<')[0])
                        run_attr[key]['start_time'] = start_time

                    if '"Stop time binary" type="DWORD"' in line:
                        stop_time = int(line.split('>')[1].split('<')[0])
                        run_attr[key]['stop_time'] = stop_time

                    if '"Field On" type="BOOL"' in line:
                        field_on = bool(line.split('>')[1].split('<')[0])
                        run_attr[key]['field_on'] = field_on

                    if '"Step Size" type="DOUBLE"' in line:
                        step_size = float(line.split('>')[1].split('<')[0])
                        if not got_step_size:
                            run_attr[key]['step_size'] = step_size
                            got_step_size = True

                    if '"Laser Tracker Point" type="STRING"' in line:

                        laser_point = line.split('>')[1].split('<')[0].upper()
                        laser_point = laser_point.strip()

                        if laser_point != 'P1' and laser_point != 'P2':
                            laser_point = 'N'

                        run_attr[key]['laser_point'] = laser_point

                    if '"Laser Phi Offset" type="DOUBLE"' in line:
                        laser_phi_offset = float(line.split('>')[1].split('<')[0])
                        run_attr[key]['laser_phi_offset'] = laser_phi_offset

                    if '"Metrolab Angle" type="DOUBLE"' in line:
                        metrolab_angle = float(line.split('>')[1].split('<')[0])
                        run_attr[key]['metrolab_angle'] = metrolab_angle

                else:

                    if 'Comment = STRING : [80] ' in line:
                        comment = line.split(': [80] ')[1].rstrip()
                        run_attr[key]['comment'] = comment

                    if 'Tags = STRING : [128] ' in line:
                        tags = line.split(': [128] ')[1].rstrip()
                        run_attr[key]['tags'] = tags.split(',')

                    if 'Start time binary = DWORD' in line:
                        start_time = int(line.split(':')[1])
                        run_attr[key]['start_time'] = start_time

                    if 'Stop time binary = DWORD' in line:
                        stop_time = int(line.split(':')[1])
                        run_attr[key]['stop_time'] = stop_time

                    if 'Field On = BOOL' in line:
                        field_on =  bool(line.split(':')[1])
                        run_attr[key]['field_on'] = field_on

                    if 'Step Size = DOUBLE' in line:
                        step_size = float(line.split(':')[1])
                        if not got_step_size:
                            run_attr[key]['step_size'] = step_size
                            got_step_size = True

                    if 'Laser Tracker Point = STRING' in line:

                        laser_point = line.split(':')[1].split(']')[1].upper()
                        laser_point = laser_point.strip()

                        if laser_point != 'P1' and laser_point != 'P2':
                            laser_point = 'N'

                        run_attr[key]['laser_point'] = laser_point

                    if 'Laser Phi Offset = DOUBLE' in line:
                        laser_phi_offset = float(line.split(':')[1])
                        run_attr[key]['laser_phi_offset'] = laser_phi_offset

                    if 'Metrolab Angle = FLOAT' in line:
                        metrolab_angle = float(line.split(':')[1])
                        run_attr[key]['metrolab_angle'] = metrolab_angle

        try:
            run_attr[key]['step_size']

        except:
            run_attr[key]['step_size'] = -1.0

        try:
            run_attr[key]['laser_point']

        except:
            run_attr[key]['laser_point'] = 'N'

        try:
            run_attr[key]['laser_swap']

        except:
            run_attr[key]['laser_swap'] = False

        print run_attr[key]

    with open(attr_file, 'w') as f:
        f.write(json.dumps(run_attr, indent=2))

if __name__ == '__main__':
    main()
