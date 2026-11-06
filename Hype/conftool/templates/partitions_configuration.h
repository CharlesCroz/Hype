#ifndef HYPE_AUTOGEN_HEADER
#define HYPE_AUTOGEN_HEADER

struct hype_partition_descriptor
{
    const char name[32];
    const void *mmu_table_loc;
};

extern const struct hype_partition_descriptor hype_partitions[];
extern const unsigned int hype_partitions_count;

struct hype_scheduling_slot {
    unsigned int partition_id;
    unsigned int from_ms;
    unsigned int to_ms;
};

extern const struct hype_scheduling_slot hype_scheduling[];
extern const unsigned int hype_scheduling_count;


#endif
