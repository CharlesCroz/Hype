import yaml
import argparse


def main(platform_filename, project_filename, output_dir):
    with open(platform_filename) as src:
        pf = yaml.safe_load(src)
    with open(project_filename) as src:
        pj = yaml.safe_load(src)

    print(pf)
    print(pj)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates headers and linker scripts from config yaml")
    parser.add_argument("platform_config", type=str, help="Path to system config file")
    parser.add_argument("project_config", type=str, help="Path to project config file")
    parser.add_argument("-o", "--output_dir", required=False, default="./", type=str, help="Path to output dir")
    args = parser.parse_args()

    # main(**vars(args))
    main(args.platform_config, args.project_config, args.output_dir)


