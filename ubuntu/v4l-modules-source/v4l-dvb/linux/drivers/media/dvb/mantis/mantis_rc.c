#include <linux/bitops.h>
#include "mantis_common.h"
#include "mantis_core.h"

#include "dmxdev.h"
#include "dvbdev.h"
#include "dvb_demux.h"
#include "dvb_frontend.h"
#include "mantis_vp1033.h"
#include "mantis_vp1034.h"
#include "mantis_vp2033.h"
#include "mantis_vp3030.h"

#define POLL_FREQ 100

void mantis_query_rc(struct work_struct *work)
{
        struct mantis_pci *mantis =
                container_of(work, struct mantis_pci, ir.rc_query_work.work);
        struct ir_input_state *ir = &mantis->ir.ir;

        u32 lastkey = mantis->ir.ir_last_code;

        if (lastkey != -1) {
                ir_input_keydown(mantis->ir.rc_dev, ir, lastkey);
                mantis->ir.ir_last_code = -1;
        } else {
                ir_input_nokey(mantis->ir.rc_dev, ir);
        }
        schedule_delayed_work(&mantis->ir.rc_query_work,
		msecs_to_jiffies(POLL_FREQ));
}

int mantis_rc_init(struct mantis_pci *mantis)
{
        struct input_dev *rc_dev;
        struct mantis_ir *mir = &mantis->ir;
        struct ir_input_state *ir = &mir->ir;
	struct ir_scancode_table *ir_codes = NULL;
	u64 ir_type = IR_TYPE_OTHER;
        int err;

        if (!mantis->hwconfig->ir_codes) {
                dprintk(verbose, MANTIS_DEBUG, 1, "No RC codes available");
                return 0;
        }

	ir_codes = mantis->hwconfig->ir_codes;

        mmwrite(mmread(MANTIS_INT_MASK) | MANTIS_INT_IRQ1, MANTIS_INT_MASK);

        rc_dev = input_allocate_device();
        if (!rc_dev) {
                dprintk(verbose, MANTIS_ERROR, 1, "dvb_rc_init failed");
                return -ENOENT;
        }

        mir->rc_dev = rc_dev;

        snprintf(mir->rc_name, sizeof(mir->rc_name), 
                 "Mantis %s IR Receiver", mantis->hwconfig->model_name);
        snprintf(mir->rc_phys, sizeof(mir->rc_phys), 
                 "pci-%s/ir0", pci_name(mantis->pdev));

        rc_dev->name = mir->rc_name;
        rc_dev->phys = mir->rc_phys;

	ir_input_init(rc_dev, ir, ir_type);

        rc_dev->id.bustype = BUS_PCI;
        rc_dev->id.vendor  = mantis->vendor_id;
        rc_dev->id.product = mantis->device_id;
        rc_dev->id.version = 1;

        INIT_DELAYED_WORK(&mir->rc_query_work, mantis_query_rc);

	err = ir_input_register(rc_dev, ir_codes, NULL);
        if (err) {
                dprintk(verbose, MANTIS_ERROR, 1, "rc registering failed");
                return -ENOENT;
        }

        schedule_delayed_work(&mir->rc_query_work,
		msecs_to_jiffies(POLL_FREQ));
        return 0;
}

int mantis_rc_exit(struct mantis_pci *mantis)
{
        mmwrite(mmread(MANTIS_INT_MASK) & (~MANTIS_INT_IRQ1), MANTIS_INT_MASK);

        cancel_delayed_work(&mantis->ir.rc_query_work);
	flush_scheduled_work();
        input_unregister_device(mantis->ir.rc_dev);
        dprintk(verbose, MANTIS_DEBUG, 1, "RC unregistered");
        return 0;
}
