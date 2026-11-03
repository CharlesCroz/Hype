#ifndef HYPE_AUTOGEN_HEADER
#define HYPE_AUTOGEN_HEADER

struct hype_partition_descriptor
{
    const char name[32];
    const void *mmu_table_loc;
};

extern const struct hype_partition_descriptor hype_partitions[];
extern const unsigned int hype_partitions_count;

#endif
