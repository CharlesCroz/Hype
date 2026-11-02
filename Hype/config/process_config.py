import yaml
import argparse


class ConfigError(Exception):
    def __init__(self, message):
        super().__init__(message)


def get_map_entry(mmap, addr):
    for i in range(len(mmap)):
        if addr in range(mmap[i]["addr_start"], mmap[i]["addr_end"] + 1):
            return mmap[i]
    return None


def generate_MMU_TTBL_system(yaml_dict):
    try:
        raw_map = yaml_dict["memory"]["map"]
        total_size = yaml_dict["memory"]["total_size"]
        section_size = yaml_dict["memory"]["section_size"]
    except KeyError as e:
        raise ConfigError(f"Platform Yaml malformed.")
    
    raw_map.sort(key=lambda x: x["addr_start"])
    print(raw_map)
    for i in range(len(raw_map) - 1):
        if raw_map[i]["addr_end"] >= raw_map[i+1]["addr_start"]:
            message = f"Overlap in platform memory map: {raw_map[i]["name"]} vs {raw_map[i+1]["name"]}"
            if raw_map[i]["tex"] == raw_map[i+1]["tex"] and raw_map[i]["C"] == raw_map[i+1]["C"] and raw_map[i]["B"] == raw_map[i+1]["B"] and raw_map[i]["S"] == raw_map[i+1]["S"]:
                print(f"Warning: {message}, but same permissions")
            else:
                raise ConfigError(message)

    n_lines = total_size // section_size
    entries = []
    for i in range(n_lines):
        start = i * section_size
        conf = get_map_entry(raw_map, start)
        if conf is None:
            entries.append([start, "Unmapped memory area"])
        else:
            apx = 0
            ap = 0b01
            domain = conf["domain"] if "domain" in conf.keys() else 0
            value = start + (conf["B"] << 2) + (conf["C"] << 3) + (conf["tex"] << 12) + 0b10 + (apx << 15) + (ap << 10) + (conf["S"] << 16) + (domain << 5)
            entries.append([value, f"{conf["name"]}, S={conf["S"]}, tex={conf["tex"]}, B={conf["B"]}, C={conf["C"]}, apx={apx}, ap={ap}"])

    return entries


def generate_MMU_TTBL_process(partition):
    pa_start = partition["mmap"]["pa_start"]
    if pa_start % (2**20) != 0:
        raise ConfigError(f"Partition {[partition["name"]]} pysical address not 2**20 aligned")
    S = 1
    B = 1
    C = 0
    nG = 1
    tex = 0b101
    ap = 0b11
    section = 0b10

    entries = [[0, "Unmapped : fault"] for i in range(32)]
    entries[0][0] = pa_start + (nG << 17) + (S << 16) + (tex << 12) + (ap << 10) + (B << 2) + section
    entries[0][1] = f"{partition["name"]}, S={S}, nG={nG}, tex={tex}, B={B}, C={C}, apx=0, ap={ap}"

    return entries


def save_table(entries, filename, varname):
    with open(filename, "w") as dest:
        print(f".globl {varname}", file=dest)
        print(f".section .{varname}_section,\"a\"", file=dest)
        print(f"{varname}:", file=dest)
        for value, comment in entries:
            print(f"\t.word\t0x{value:08X}\t/* {comment} */", file=dest)
        print(f".end", file=dest)


def get_driver_list(yaml_dict):
    try:
        raw_drivers = yaml_dict["drivers"]
    except KeyError as e:
        raise ConfigError(f"Platform Yaml malformed.")

    drivers_dict = {}
    for e in raw_drivers:
        id = e["id"]
        name = e["name"]
        if id in drivers_dict.keys():
            raise ConfigError(f"Repeated driver ID {id}")
        elif name in drivers_dict.values():
            raise ConfigError(f"Repeated driver name {name}")
        drivers_dict[id] = name

    return drivers_dict
    

def main(platform_filename, project_filename, output_dir):
    with open(platform_filename) as src:
        pf = yaml.safe_load(src)
    with open(project_filename) as src:
        pj = yaml.safe_load(src)

    MMU_Table_system = generate_MMU_TTBL_system(pf)
    save_table(MMU_Table_system, f"{output_dir}/_MMU_TTBL_SYS.S", "MMU_TTBL_SYS")

    drivers = get_driver_list(pf)

    process_maps = {}

    for partition in pj["partitions"]:
        print(f"Partition ID:{partition["id"]} {partition["name"]}: {partition["descr"]}")
        table_name = f"MMU_TTBL_{partition["name"]}"
        entries = generate_MMU_TTBL_process(partition)
        save_table(entries, f"{output_dir}/_{table_name}.S", table_name)

        process_maps[partition["id"]] = table_name

    # Make headers
    

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates headers and linker scripts from config yaml")
    parser.add_argument("platform_config", type=str, help="Path to system config file")
    parser.add_argument("project_config", type=str, help="Path to project config file")
    parser.add_argument("-o", "--output_dir", required=False, default="./", type=str, help="Path to output dir")
    args = parser.parse_args()

    # main(**vars(args))
    main(args.platform_config, args.project_config, args.output_dir)


