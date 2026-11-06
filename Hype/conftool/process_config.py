import yaml
import argparse
import shutil


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


def save_table(entries, filename, varname, template_dir):
    with open(filename, "w") as dest, open(f"{template_dir}/MMU_Table.template.S", "r") as template_file:
        table_lines = [f"\t.word\t0x{value:08X}\t/* {comment} */" for value, comment in entries]

        file_content = template_file.read()
        file_content = file_content.replace("/*%%MMU_TABLE_NAME%%*/", varname)
        file_content = file_content.replace("/*%%MMU_TABLE_CONTENT%%*/", "\n".join(table_lines))

        dest.write(file_content)


def make_c_partition_configuration(process_maps, template_dir, c_file_content):
    with open(f"{template_dir}/partitions_configuration.mmu_template.h", "r") as mmu_entry_template_file:
        mmu_entry_template = mmu_entry_template_file.read()

        extern_def = "\n".join(
            [f"extern const char {process_maps[id]["var_name"]}[];" 
            for id in range(len(process_maps))]
        )

        mmu_array_entries = "\n".join(
            [mmu_entry_template.\
                replace("/*%%NAME%%*/", f"\"{process_maps[id]["name"]}\"").\
                replace("/*%%TABLE_LOC%%*/", f"{process_maps[id]["var_name"]}")
            for id in range(len(process_maps))]
        )

        return c_file_content\
            .replace("/*%%EXTERN_REF_TO_TABLES%%*/", extern_def)\
            .replace("/*%%PARTITIONS_ENTRIES%%*/", mmu_array_entries)\
            .replace("/*%%PARTITIONS_COUNT%%*/", f"{len(process_maps)}")


def make_c_scheduler_configuration(schedule, template_dir, c_file_content):
    with open(f"{template_dir}/partitions_configuration.sched_template.h", "r") as sched_entry_template_file:
        sched_entry_template = sched_entry_template_file.read()

        period_ms = schedule["period_ms"]
        cpu0 = schedule["cpu0"]

        cpu0.sort(key=lambda x:x["from_ms"])

        for i, slot in enumerate(cpu0):
            if slot["from_ms"] >= slot["to_ms"]:
                raise ConfigError(f"Schedule for cpu0 slot {i} to_ms {slot["to_ms"]} must be strictly greater than from_ms {slot["from_ms"]}")
        for i in range(len(cpu0) - 1):
            if cpu0[i]["to_ms"] != cpu0[i+1]["from_ms"]:
                raise ConfigError(f"Discountinuous schedule for cpu0 between slot[{i}](to_ms={cpu0[i]["to_ms"]}) and slot[{i+1}](to_ms={cpu0[i+1]["from_ms"]})")
        if cpu0[-1]["to_ms"] != period_ms:
            raise ConfigError(f"Schedule for cpu0 length {cpu0[-1]["to_ms"]} ms different from scheduler period {period_ms} ms")


        sched_array_entries = "\n".join(
            [sched_entry_template\
                .replace("/*%%PART_ID%%*/", f"{slot["partition"]}")\
                .replace("/*%%FROM_MS%%*/", f"{slot["from_ms"]}")\
                .replace("/*%%TO_MS%%*/", f"{slot["to_ms"]}")
            for slot in cpu0]
        )

        return c_file_content\
            .replace("/*%%SCHEDULING_ENTRIES%%*/", sched_array_entries)\
            .replace("/*%%SCHEDULING_COUNT%%*/", f"{len(cpu0)}")


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


def make_MMU_Table_lscript(process_maps, template_dir, autogen_dir):
    with open(f"{template_dir}/MMU_lscript.template.ld", "r") as entry_template_file, \
        open(f"{autogen_dir}/include/MMU_lscript.ld", "w") as dest_file:

        entry_template = entry_template_file.read()
        dest_file.write("".join([
            entry_template.replace("/*%%ALIGNMENT%%*/", "128")\
            .replace("/*%%SECTION_NAME%%*/", process_maps[id]["var_name"])
            for id in range(len(process_maps))
        ]))
    pass


def main(platform_filename, project_filename, autogen_dir, template_dir):
    with open(platform_filename) as src:
        platform_yaml = yaml.safe_load(src)
    with open(project_filename) as src:
        project_yaml = yaml.safe_load(src)

    MMU_Table_system = generate_MMU_TTBL_system(platform_yaml)
    save_table(MMU_Table_system, f"{autogen_dir}/src/_MMU_TTBL_SYS.S", "MMU_TTBL_SYS", template_dir)

    drivers = get_driver_list(platform_yaml)

    process_maps = {}

    for partition in project_yaml["partitions"]:
        print(f"Partition ID:{partition["id"]} {partition["name"]}: {partition["descr"]}")
        table_name = f"MMU_TTBL_{partition["name"]}"
        entries = generate_MMU_TTBL_process(partition)
        save_table(entries, f"{autogen_dir}/src/_{table_name}.S", table_name, template_dir)

        process_maps[partition["id"]] = {"name": partition["name"], "var_name": table_name}


    # Make headers
    shutil.copy(f"{template_dir}/partitions_configuration.h", f"{autogen_dir}/include/partitions_configuration.h")
    with\
        open(f"{template_dir}/partitions_configuration.template.c", "r") as conf_template_file,\
        open(f"{autogen_dir}/src/partitions_configuration.c", "w") as dest_file:
        c_file_content = make_c_partition_configuration(process_maps, template_dir, conf_template_file.read())
        c_file_content = make_c_scheduler_configuration(project_yaml["schedule"], template_dir, c_file_content)
        dest_file.write(c_file_content)
    make_MMU_Table_lscript(process_maps, template_dir, autogen_dir)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generates headers and linker scripts from config yaml")
    parser.add_argument("platform_config", type=str, help="Path to system config file")
    parser.add_argument("project_config", type=str, help="Path to project config file")
    parser.add_argument("template_dir", type=str, help="Path to templates dir")
    parser.add_argument("autogen_dir", type=str, help="Path to output dir")
    args = parser.parse_args()

    # main(**vars(args))
    main(args.platform_config, args.project_config, args.autogen_dir, args.template_dir)


