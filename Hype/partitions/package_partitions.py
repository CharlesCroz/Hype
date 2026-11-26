import argparse
from math import ceil
from partitions_config import partitions


def align(dest, alignment):
    padding = ceil(dest.tell() / alignment) * alignment - dest.tell()
    dest.write(('\0' * padding).encode(encoding="ascii"))



def main(partitions_dir):

    with open(f"{partitions_dir}/partitions.bin", "wb+") as dest:
        dest.write("Hyp0Part".encode(encoding="ascii"))
        dest.write(len(partitions).to_bytes(length=4, byteorder="little"))

        # reserve space for headers
        header_array_offset = dest.tell()
        header_array_entry_size = 12 # name_offset, file_offset, file_size
        dest.write(('\0' * header_array_entry_size * len(partitions)).encode(encoding="ascii"))

        # Write partitions' names
        align(dest, 32)
        for part in partitions:
            part['name_offset'] = dest.tell()
            dest.write((part["name"] + '\0').encode(encoding="ascii"))

        # Write partitions' elfs
        for part in partitions:
            align(dest, 32)
            part['elf_offset'] = dest.tell()
            with open(f"{partitions_dir}/{part["name"]}/obj/{part["name"]}.elf", "rb") as elf:
                dest.write(elf.read())
                part['elf_size'] = elf.tell()

        # back to header array
        dest.seek(header_array_offset)
        for part in partitions:
            dest.write(part["name_offset"].to_bytes(length=4, byteorder="little"))
            dest.write(part["elf_offset"].to_bytes(length=4, byteorder="little"))
            dest.write(part["elf_size"].to_bytes(length=4, byteorder="little"))

        print(f"Sucessfully wrote {len(partitions)} partitions into {dest.name}")
        for part in partitions:
            print(f"\t{part["name"]}\t0x{part["elf_size"]:08X} bytes at offset 0x{part["elf_offset"]:08X}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compile partitions alf into partition binary for the kernel")
    parser.add_argument("partitions_dir", type=str, help="Path to partitions folder")
    args = parser.parse_args()

    main(args.partitions_dir)
