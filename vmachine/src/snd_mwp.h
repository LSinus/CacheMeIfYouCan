#define _GNU_SOURCE
#include <sys/mount.h>

struct snd_ctl_tlv {
	unsigned int numid;	/* control element numeric identification */
	unsigned int length;	/* in bytes aligned to 4 */
	unsigned int tlv[0];	/* first TLV */
};

#define __force
typedef int __bitwise snd_ctl_elem_type_t;
#define	SNDRV_CTL_ELEM_TYPE_NONE	((__force snd_ctl_elem_type_t) 0) /* invalid */
#define	SNDRV_CTL_ELEM_TYPE_BOOLEAN	((__force snd_ctl_elem_type_t) 1) /* boolean type */
#define	SNDRV_CTL_ELEM_TYPE_INTEGER	((__force snd_ctl_elem_type_t) 2) /* integer type */
#define	SNDRV_CTL_ELEM_TYPE_ENUMERATED	((__force snd_ctl_elem_type_t) 3) /* enumerated type */
#define	SNDRV_CTL_ELEM_TYPE_BYTES	((__force snd_ctl_elem_type_t) 4) /* byte array */
#define	SNDRV_CTL_ELEM_TYPE_IEC958	((__force snd_ctl_elem_type_t) 5) /* IEC958 (S/PDIF) setup */
#define	SNDRV_CTL_ELEM_TYPE_INTEGER64	((__force snd_ctl_elem_type_t) 6) /* 64-bit integer type */
#define	SNDRV_CTL_ELEM_TYPE_LAST	SNDRV_CTL_ELEM_TYPE_INTEGER64

typedef int __bitwise snd_ctl_elem_iface_t;
#define	SNDRV_CTL_ELEM_IFACE_CARD	((__force snd_ctl_elem_iface_t) 0) /* global control */
#define	SNDRV_CTL_ELEM_IFACE_HWDEP	((__force snd_ctl_elem_iface_t) 1) /* hardware dependent device */
#define	SNDRV_CTL_ELEM_IFACE_MIXER	((__force snd_ctl_elem_iface_t) 2) /* virtual mixer device */
#define	SNDRV_CTL_ELEM_IFACE_PCM	((__force snd_ctl_elem_iface_t) 3) /* PCM device */
#define	SNDRV_CTL_ELEM_IFACE_RAWMIDI	((__force snd_ctl_elem_iface_t) 4) /* RawMidi device */
#define	SNDRV_CTL_ELEM_IFACE_TIMER	((__force snd_ctl_elem_iface_t) 5) /* timer device */
#define	SNDRV_CTL_ELEM_IFACE_SEQUENCER	((__force snd_ctl_elem_iface_t) 6) /* sequencer client */
#define	SNDRV_CTL_ELEM_IFACE_LAST	SNDRV_CTL_ELEM_IFACE_SEQUENCER

#define SNDRV_CTL_ELEM_ID_NAME_MAXLEN	44
struct snd_ctl_elem_id {
	unsigned int numid;		/* numeric identifier, zero = invalid */
	snd_ctl_elem_iface_t iface;	/* interface identifier */
	unsigned int device;		/* device/client number */
	unsigned int subdevice;		/* subdevice (substream) number */
	unsigned char name[SNDRV_CTL_ELEM_ID_NAME_MAXLEN];		/* ASCII name of item */
	unsigned int index;		/* index of item */
};

struct snd_ctl_elem_info {
	struct snd_ctl_elem_id id;	/* W: element ID */
	snd_ctl_elem_type_t type;	/* R: value type - SNDRV_CTL_ELEM_TYPE_* */
	unsigned int access;		/* R: value access (bitmask) - SNDRV_CTL_ELEM_ACCESS_* */
	unsigned int count;		/* count of values */
	__kernel_pid_t owner;		/* owner's PID of this control */
	union {
		struct {
			long min;		/* R: minimum value */
			long max;		/* R: maximum value */
			long step;		/* R: step (0 variable) */
		} integer;
		struct {
			long long min;		/* R: minimum value */
			long long max;		/* R: maximum value */
			long long step;		/* R: step (0 variable) */
		} integer64;
		struct {
			unsigned int items;	/* R: number of items */
			unsigned int item;	/* W: item number */
			char name[64];		/* R: value name */
			__u64 names_ptr;	/* W: names list (ELEM_ADD only) */
			unsigned int names_length;
		} enumerated;
		unsigned char reserved[128];
	} value;
	unsigned char reserved[64];
};

#define SNDRV_CTL_IOCTL_ELEM_ADD _IOWR('U', 0x17, struct snd_ctl_elem_info)
#define SNDRV_CTL_IOCTL_ELEM_REPLACE	_IOWR('U', 0x18, struct snd_ctl_elem_info)
#define SNDRV_CTL_IOCTL_TLV_READ _IOWR('U', 0x1a, struct snd_ctl_tlv)
#define SNDRV_CTL_IOCTL_TLV_WRITE	_IOWR('U', 0x1b, struct snd_ctl_tlv)
#define SNDRV_CTL_IOCTL_TLV_COMMAND	_IOWR('U', 0x1c, struct snd_ctl_tlv)

#define SNDRV_CTL_ELEM_ACCESS_TLV_WRITE		(1<<5)	/* TLV write is possible */

void init_audio(void);
void trigger_mwp(void *fuse_managed_memory);

