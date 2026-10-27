import argparse

registers = {
    "TTBCR":[
        {"name": "EAE", "bits": 0b1 << 31, "verbose":"""Extended Address Enable. The meanings of the possible values of this bit are:
0 -> Use the 32-bit translation system, with the Short-descriptor translation table format. In this case, the format of the TTBCR is as described in this section.
1 -> Use the 40-bit translation system, with the Long-descriptor translation table format. In this case, the format of the TTBCR is as described in TTBCR format when using the Long-descriptor translation table format : https://support.arm.com/documentation/ddi0406/c/System-Level-Architecture/System-Control-Registers-in-a-VMSA-implementation/VMSA-System-control-registers-descriptions--in-register-order/TTBCR--Translation-Table-Base-Control-Register--VMSA?lang=en#BEIBEGEJ."""},
        {"name": "Res", "bits": ((1 << 25) - 1) << 6, "verbose":""},
        {"name": "PD1", "bits": 0b1 << 5, "verbose":"""Translation table walk disable for translations using TTBR1. This bit controls whether a translation table walk is performed on a TLB miss, for an address that is translated using TTBR1. The encoding of this bit is:
0 -> Perform translation table walks using TTBR1.
1 -> A TLB miss on an address that is translated using TTBR1 generates a Translation fault. No translation table walk is performed."""},
        {"name": "PD0", "bits": 0b1 << 4, "verbose":"""Translation table walk disable for translations using TTBR0. This bit controls whether a translation table walk is performed on a TLB miss for an address that is translated using TTBR0. The meanings of the possible values of this bit are equivalent to those for the PD1 bit."""},
        {"name": "Res", "bits": 0b1 << 3, "verbose":""},
        {"name": "N", "bits": 0b111, "verbose":"""Indicate the width of the base address held in TTBR0. In TTBR0, the base address field is bits[31:14-N]. The value of N also determines:
- whether TTBR0 or TTBR1 is used as the base address for translation table walks.
- the size of the translation table pointed to by TTBR0.
N can take any value from 0 to 7, that is, from 0b000 to 0b111."""},
    ],
    "TTBR0":[
        {"name": "Base (>> 14) TTBCR.N=0", "bits": ((1 << 18) - 1) << 14, "verbose":"Table base address"},
        {"name": "Base (>> 14-TTBCR.N)  ", "bits": 0, "verbose":""},
        {"name": "Base (>> 7)  TTBCR.N=7", "bits": ((1 << 25) - 1) << 7, "verbose":""},
        {"name": "IRGN", "bits": 0b1 << 6, "verbose":"IRGN[0], bit[6], in an implementation that includes the Multiprocessing Extensions (??)"},
        {"name": "NOS", "bits": 0b1 << 5, "verbose":"""Not Outer Shareable bit. Indicates the Outer Shareable attribute for the memory associated with a translation table walk that has the Shareable attribute, indicated by TTBR0.S == 1:
0 -> Outer Shareable, 1 -> Inner Shareable."""},
        {"name": "RGN", "bits": 0b11 << 3, "verbose":"""Region bits. Indicates the Outer cacheability attributes for the memory associated with the translation table walks:
0b00 -> Normal memory, Outer Non-cacheable.
0b01 -> Normal memory, Outer Write-Back Write-Allocate Cacheable.
0b10 -> Normal memory, Outer Write-Through Cacheable.
0b11 -> Normal memory, Outer Write-Back no Write-Allocate Cacheable."""},
        {"name": "IMP", "bits": 0b1 << 2, "verbose":"""The effect of this bit is implementation defined."""},
        {"name": "S", "bits": 0b1 << 1, "verbose":"""Shareable bit. Indicates the Shareable attribute for the memory associated with the translation table walks:
0 -> Non-shareable, 1 -> Shareable."""},
        {"name": "C", "bits": 0b1 << 0, "verbose":"""Cacheable bit. Indicates whether the translation table walk is to Inner Cacheable memory.
0 -> Inner Non-cacheable, 1 -> Inner Cacheable."""},
    ],
    "TTBR1":[
        {"name": "Base (>> 14)", "bits": ((1 << 18) - 1) << 14, "verbose":"Translation table base 1 address, bits[31:14]. The translation table must be aligned on a 16KByte boundary."},
        {"name": "Res", "bits": ((1 << 7) - 1) << 7, "verbose":""},
        {"name": "IRGN[0]", "bits": 0b1 << 6, "verbose":""},
        {"name": "NOS", "bits": 0b1 << 5, "verbose":"""Not Outer Shareable bit. Indicates the Outer Shareable attribute for the memory associated with a translation table walk that has the Shareable attribute, indicated by TTBR0.S == 1:
0 -> Outer Shareable, 1 -> Inner Shareable."""},
        {"name": "RGN", "bits": 0b11 << 3, "verbose":"""Region bits. Indicates the Outer cacheability attributes for the memory associated with the translation table walks:
0b00 -> Normal memory, Outer Non-cacheable.
0b01 -> Normal memory, Outer Write-Back Write-Allocate Cacheable.
0b10 -> Normal memory, Outer Write-Through Cacheable.
0b11 -> Normal memory, Outer Write-Back no Write-Allocate Cacheable."""},
        {"name": "IMP", "bits": 0b1 << 2, "verbose":"""The effect of this bit is implementation defined."""},
        {"name": "S", "bits": 0b1 << 1, "verbose":"""Shareable bit. Indicates the Shareable attribute for the memory associated with the translation table walks:
0 -> Non-shareable, 1 -> Shareable."""},
        {"name": "IRGN[1]", "bits": 0b1, "verbose":""},
    ],
    "TTL1":[],
    "TTL1-f":[
        {"name": "Ignored", "bits": ((1 << 30) - 1) << 2, "verbose":""},
        {"name": "LSB", "bits": 0b11, "verbose":"L1 Short-descriptor LSB, 0b00 -> Fault"},
    ],
    "TTL1-ptr":[
        {"name": "Base (>> 10)", "bits": ((1 << 22) - 1) << 10, "verbose":"L2 Descr base address (shifted 10 bits)"},
        {"name": "P", "bits": 0b1 << 9, "verbose":"?"},
        {"name": "Domain", "bits": 0b1111 << 5, "verbose":"?"},
        {"name": "SBZ", "bits": 0b111 << 2, "verbose":"?"},
        {"name": "LSB", "bits": 0b11, "verbose":"L1 Short-descriptor LSB, 0b01 -> Pointer to level 2"},
    ],
    "TTL1-sec":[
        {"name": "Base (>> 20)", "bits": ((1 << 12) - 1) << 20, "verbose":"L2 Descr base address (shifted 20 bits)"},
        {"name": "NS", "bits": 0b1 << 19, "verbose":"Non-secure bit. If an implementation includes the Security Extensions, for memory accesses from Secure state, this bit specifies whether the translated PA is in the Secure or Non-secure address map, see Control of Secure or Non-secure memory access, Short-descriptor format : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Short-descriptor-translation-table-format/Control-of-Secure-or-Non-secure-memory-access--Short-descriptor-format?lang=en."},
        {"name": "B18", "bits": 0b1 << 18, "verbose":"0 for Sections, 1 for Supersection"},
        {"name": "NG", "bits": 0b1 << 17, "verbose":"The not global bit. Determines how the translation is marked in the TLB, see Global and process-specific translation table entries : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Translation-Lookaside-Buffers--TLBs-/Global-and-process-specific-translation-table-entries?lang=en."},
        {"name": "S", "bits": 0b1 << 16, "verbose":"The Shareable bit. Determines whether the addressed region is Shareable memory, see Memory region attributes."},
        {"name": "TEX[2:0]", "bits": 0b111 << 12, "verbose":"Memory region attribute bits, see Memory region attributes : https://support.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-region-attributes?lang=en."},
        {"name": "AP[2:0]", "bits": 0b11 << 10, "verbose":"https://support.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-access-control/Access-permissions?lang=en"},
        {"name": "IMP", "bits": 0b1 << 9, "verbose":"Implementation defined"},
        {"name": "Domain", "bits": 0b1111 << 5, "verbose":"Domains, Short-descriptor format only : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-access-control/Domains--Short-descriptor-format-only?lang=en."},
        {"name": "XN", "bits": 0b1 << 4, "verbose":"The Execute-never bit. Determines whether the processor can execute software from the addressed region, see Execute-never restrictions on instruction fetching : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-access-control/Execute-never-restrictions-on-instruction-fetching?lang=en."},
        {"name": "C", "bits": 0b1 << 3, "verbose":"Memory region attribute bits, see Memory region attributes."},
        {"name": "B", "bits": 0b1 << 2, "verbose":"Memory region attribute bits, see Memory region attributes."},
        {"name": "LSB", "bits": 0b11, "verbose":"L1 Short-descriptor LSB, 0b10 -> Section/Supersection"},
    ],
    "TTL1-ssec":[
        {"name": "Base (>> 24)", "bits": ((1 << 8) - 1) << 24, "verbose":"Supersection Base Adress (shifted 24 bits)"},
        {"name": "Ext", "bits": ((1 << 4) - 1) << 20, "verbose":"Extended Base Adress (shifted 20 bits)"},
        {"name": "NS", "bits": 0b1 << 19, "verbose":"Non-secure bit. If an implementation includes the Security Extensions, for memory accesses from Secure state, this bit specifies whether the translated PA is in the Secure or Non-secure address map, see Control of Secure or Non-secure memory access, Short-descriptor format : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Short-descriptor-translation-table-format/Control-of-Secure-or-Non-secure-memory-access--Short-descriptor-format?lang=en."},
        {"name": "B18", "bits": 0b1 << 18, "verbose":"0 for Sections, 1 for Supersection"},
        {"name": "NG", "bits": 0b1 << 17, "verbose":"The not global bit. Determines how the translation is marked in the TLB, see Global and process-specific translation table entries : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Translation-Lookaside-Buffers--TLBs-/Global-and-process-specific-translation-table-entries?lang=en."},
        {"name": "S", "bits": 0b1 << 16, "verbose":"The Shareable bit. Determines whether the addressed region is Shareable memory, see Memory region attributes."},
        {"name": "TEX[2:0]", "bits": 0b111 << 12, "verbose":"Memory region attribute bits, see Memory region attributes : https://support.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-region-attributes?lang=en."},
        {"name": "AP[2:0]", "bits": 0b11 << 10, "verbose":"https://support.arm.com/documentation/ddi0406/c/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-access-control/Access-permissions?lang=en"},
        {"name": "IMP", "bits": 0b1 << 9, "verbose":"Implementation defined"},
        {"name": "Domain", "bits": 0b1111 << 5, "verbose":"Domains, Short-descriptor format only : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-access-control/Domains--Short-descriptor-format-only?lang=en."},
        {"name": "XN", "bits": 0b1 << 4, "verbose":"The Execute-never bit. Determines whether the processor can execute software from the addressed region, see Execute-never restrictions on instruction fetching : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Memory-access-control/Execute-never-restrictions-on-instruction-fetching?lang=en."},
        {"name": "C", "bits": 0b1 << 3, "verbose":"Memory region attribute bits, see Memory region attributes."},
        {"name": "B", "bits": 0b1 << 2, "verbose":"Memory region attribute bits, see Memory region attributes."},
        {"name": "LSB", "bits": 0b11, "verbose":"L1 Short-descriptor LSB, 0b10 -> Section/Supersection"},
    ],
    "TTL1-res":[
        {"name": "Reserved", "bits": ((1 << 30) - 1) << 2, "verbose":""},
        {"name": "LSB", "bits": 0b11, "verbose":"L1 Short-descriptor LSB, 0b11 -> Reserved"},
    ],
    "TTL2":[
        {"name": "Use -v option", "bits": 0, "verbose":"Second level Short-descriptor table entry not implementer yet : https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Short-descriptor-translation-table-format/Short-descriptor-translation-table-format-descriptors?lang=en"},
    ]
}


def get_bits(value:int, mask:int):
    res = 0
    for offset in range(31, -1, -1):
        current = 1 << offset
        if mask & current:
            res *= 2
            res += (value & current) >> offset
    return res


def select_TTL1(value:int):
    lsb = get_bits(value, 0b11)
    b18 = get_bits(value, 1 << 18)
    res = "TTL1-res"
    if lsb == 0b00:
        res = "TTL1-f" 
    elif lsb == 0b01:
        res = "TTL1-ptr" 
    elif lsb == 0b10 and b18 == 0b0:
        res = "TTL1-sec" 
    elif lsb == 0b10 and b18 == 0b1:
        res = "TTL1-ssec" 
    print(f"Short descriptor translation table detected as {res}")
    print("https://support.arm.com/documentation/ddi0406/cb/System-Level-Architecture/Virtual-Memory-System-Architecture--VMSA-/Short-descriptor-translation-table-format/Short-descriptor-translation-table-format-descriptors?lang=en")
    return res


def main(**kwargs):
    reg = kwargs["reg"]
    value = kwargs["value"]
    verbose = kwargs["v"]

    if reg == "TTL1":
        reg = select_TTL1(value)

    for e in registers[reg]:
        print(f"{e["name"]} :\t0x{get_bits(value, e["bits"]):0X}")

    if verbose:
        for e in registers[reg]:
            if len(e["verbose"].strip()):
                print(f"\n{e["name"]} 0x{e["bits"]:0X} : {e["verbose"].strip()}")


    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Helper tool to decode coprocessors registers or hex data like MMU table entries")
    parser.add_argument("-v", required=False, action='store_true', help="Verbose to add field descr/ref")
    parser.add_argument("reg", type=str, choices=registers.keys(), help="Register to decode")
    parser.add_argument("value", type=lambda x: int(x,0), help="Hex value")
    args = parser.parse_args()

    main(**vars(args))
    # main(**args)
