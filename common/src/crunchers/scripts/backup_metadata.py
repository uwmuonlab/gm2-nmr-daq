#!/bin/python

import glob
import json
import midas

def main():

    # Get the data directory
    odb = midas.ODB('gm2-nmr')
    datadir = odb.get_value('/Logger/Data dir').rstrip() + '/'
    metafiles = glob.glob(datadir + '/*/.processing_metadata.json')

    for metafile in metafiles:
        
        with open(metafile) as f:
            mf_string = f.read()
        
        try:
            # Load the json file.
            metadata = json.loads(mf_string)

            # Save it if not corrupted.
            with open(metafile + '.old', 'w') as f:
                f.write(json.dumps(metadata))
        
        except:
            pass


if __name__ == '__main__':
    main()
