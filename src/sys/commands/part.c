#include <sys/commands.h>
#include <disk/partition.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <util.h>
#include <os/install.h>

__CMD_FUNC (cmd_part) {
    if (argc < 1){
        PARTITION part;
        _get_partition(0, 0, &part);
        printf("PARTITION INFO:\n");
        printf("- Name: "); write(part.name, 18); putc(0xa);
        printf("- Magic: %p", part.magic);
        printf("- Magic ASCII: "); write((char*)&part.magic, 4); putc(0xa);
    } else if (strcmd(command+tokens[0], "set")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            if (!id) {
                printf("Access denied editing Partition 0\n");
                return 6;
            }
            printf("Setting partition (id=%u)\n", id);
            PARTITION previous;
            _get_partition(0,id-1,&previous);
            // check if exists
            printf("id after prev part: %u\n", id);
            PARTITION part;
            _get_partition(0,id,&part);
            printf("id after new part: %u\n", id);
            if (_is_partition_valid(&part)){
                printf("Partition exists at index %u\nDo you still want to edit? [y/n]");
                char _tmp;
                nread(&_tmp, 1);
                if (_tmp != 'y') return 0;
            }
            printf("id after part valid check: %u\n", id);
            char _tmp[20];
            _tmp[19] = 0;
            printf("af: %u\n", id);
            printf("Enter Partition Name: "); nread(_tmp, 18);
            printf("nr: %u\n", id);
            strncpyzx(part.name,_tmp,18);
cmd_part_disk_size_loop:
            printf("Enter Partition Disk space(K|M|G): "); nread(_tmp, 10);
            part.space = strsect(_tmp, 512);
            if (!part.space){
                printf("Partition disk space cannot be less than 512 bytes!\n");
                goto cmd_part_disk_size_loop;
            }
            printf("Enter Partition Filesystem ID: "); nread(_tmp, 5);
            part.fsID = stoi(_tmp);
            part.type = PARTYPE_USER;
            part.lba = previous.lba + previous.space;
            part.magic = PARTITION_MAGIC;
            printf("id cherck: %u", id);
            _set_partition(0,id,&part);
        }
    } else if (strcmd(command+tokens[0], "info")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            PARTITION part;
            _get_partition(0, id, &part);
            printf("PARTITION %u INFO:\n", id&0xFFFF);
            printf("- Name: "); write(part.name, 18); putc(0xa);
            printf("- Type: %u\n", part.type);
            printf("- File system: %s\n", _get_partition_fs_name(part.fsID));
            printf("- Magic: %p\n", part.magic);
            printf("- Magic ASCII: "); write((char*)&part.magic, 4); putc(0xa);
            printf("- Disk space: %u (%u kB)\n", part.space, (part.space/2));
            printf("- Block address: %u\n", part.lba);
        }
    } else if (strcmd(command+tokens[0], "fixboot")){
        bool qemu_without_cdrom = false;
        if (argc > 1) {
            if (strcmd(command+tokens[1], "qemu-without-cdrom")) qemu_without_cdrom = true;
        }
        uint32_t drive = 1; // source drive
        uint32_t sectors = 1400;
        if (!qemu_without_cdrom) sectors = __os_iso_get_vol_size(drive);
        else printf("(qemu-without-cdrom): creating boot partition with disk space of 1400 sectors\n");
        if (sectors < 500)
            printf("^eWARN:^_ CD-ROM ISO Volume size not corrent. If you are running from QEMU, you are propably running with -kernel without -cdrom option.\n");
        PARTITION part = {
            "BOOT PARTITION",
            PARTYPE_BOOT,
            FILESYSTEM_ISO9660,
            PARTITION_MAGIC,
            5600,       // 1400 CD-ROM sectora = 5600 HD sectora (1400*2048 / 512)
            0x00000000
        };
        part.space = sectors * 4;
        _set_partition(0,0,&part);
    } else if (strcmd(command+tokens[0], "rm")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1]);
            uint8_t zero[PARTITION_STRUCT_SIZE];
            memset(zero, 0, PARTITION_STRUCT_SIZE);
            _set_partition(0,id,(PARTITION*)zero);
        }
    } else if (strcmd(command+tokens[0], "invl")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1])&0xFFFF;
            if (!id){
                printf("Access denied (BOOT PARTITION)\n");
                return 6;
            }
            PARTITION part;
            _get_partition(0,id,&part);
            memcpy((char*)&part.magic, "INVL", 4);
            _set_partition(0,id,&part);
        }
    } else if (strcmd(command+tokens[0], "use")){
        if (argc < 2) printf("No Partition index given\n");
        else {
            uint16_t id = stoi(command+tokens[1])&0xFFFF;
            if (!id){
                printf("Using boot partition as workspace is forbidden!\n");
                return 6;
            }
            DISK_PARTITION dpart;
            dpart.drive = 0;
            _get_partition(0,id,&dpart.part);
            lxfs_use_partition(&dpart);
        }
    }
    return 0;
}

