from PIL import Image
import argparse


def create_dummy_image(output_dir, filename):

    image = Image.new('RGBA', size=(1024, 1024), color=(155, 0, 0))
    image.save(output_dir + '/' + filename + '.png', 'png')


def main(output_dir, n_files):

    count = 1
    while(count <= n_files):
        filename = str(count)
        create_dummy_image(output_dir, filename)
        count += 1


if __name__ == "__main__":

    AP = argparse.ArgumentParser()
    AP.add_argument("--output_dir",type=str,default='output',help="Defines the location and name of output directory")

    AP.add_argument("--n_files",type=int,default=10,help="Specify the number of files to create")

    parsed = AP.parse_args()

    main(parsed.output_dir, parsed.n_files)
