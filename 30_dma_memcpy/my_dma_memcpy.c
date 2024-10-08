#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/completion.h>
#include <linux/slab.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include "../convenient.h"

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Johannes 4 GNU/Linux");
MODULE_DESCRIPTION("A simple DMA example for copying data from RAM to RAM");

static int board;
module_param(board, int, 0);
MODULE_PARM_DESC(board, "must set this to one of: 1 for TI BBB (Beagle Bone Black) -OR- 2 for Raspberry Pi : depending on the board you're running upon");

#define BOARD_IS_BBB	1
#define BOARD_IS_RPI	2

void my_dma_transfer_completed(void *param) 
{
	struct completion *cmp = (struct completion *) param;
	PRINT_CTX();
	complete(cmp);
}

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void)
{
	dma_cap_mask_t mask;
	struct dma_chan *dma_channel;
	struct dma_async_tx_descriptor *chan_desc;
	dma_cookie_t cookie;
	dma_addr_t src_addr, dst_addr;
	u8 *src_buf, *dst_buf;
	struct completion cmp;
	int status = 0;

	if (board == 0) {
		pr_info("Error: need to pass the board=<n> module parameter\n");
		return -EINVAL;
	}
	if (board == BOARD_IS_BBB)
		pr_info("board set to TI BBB (Beagle Bone Black)\n");
	else if (board == BOARD_IS_RPI)
		pr_info("board set to Raspberry Pi family\n");

	PRINT_CTX();

	// set DMA capability mask
	dma_cap_zero(mask);
	if (board == BOARD_IS_BBB)
		dma_cap_set(DMA_MEMCPY, mask);
	else if (board == BOARD_IS_RPI)
		dma_cap_set(DMA_MEMCPY | DMA_SLAVE | DMA_PRIVATE, mask);

	/* Try to allocate an exclusive DMA channel; see available DMA
	 * channels under /sys/class/dma
	 */
	dma_channel = dma_request_channel(mask, NULL, NULL);
	if(!dma_channel) {
		pr_info("Error requesting dma channel\n");
		return -ENODEV;
	}
	pr_info("Got DMA channel %s (%d)\n", dma_channel->name, dma_channel->chan_id);

	/* Perform a consistent/coherent DMA buffer alloc
	 * Guarantees cache coherence b/w the CPU and DMA
	 */
	src_buf = dma_alloc_coherent(dma_channel->device->dev, 1024, &src_addr, GFP_KERNEL);
	dst_buf = dma_alloc_coherent(dma_channel->device->dev, 1024, &dst_addr, GFP_KERNEL);

	memset(src_buf, 0x12, 1024);
	memset(dst_buf, 0x0, 1024);

	pr_info("Before DMA Transfer: src_buf[0] = %x\n", src_buf[0]);
	pr_info("Before DMA Transfer: dst_buf[0] = %x\n", dst_buf[0]);

	/* Prepare a DMA memcpy descriptor
	 * returns a DMA 'async transaction descriptor'
	 */
	chan_desc = dmaengine_prep_dma_memcpy(dma_channel, dst_addr, src_addr, 1024, DMA_MEM_TO_MEM);
	if(!chan_desc) {
		pr_info("Error requesting dma channel\n");
		status = PTR_ERR(chan_desc);
		goto free;
	}

	init_completion(&cmp);

	chan_desc->callback = my_dma_transfer_completed;
	chan_desc->callback_param = &cmp;

	cookie = dmaengine_submit(chan_desc);

	/* Fire the DMA transfer - flushes pending transactions to h/w 
	 * "This allows drivers to push copies to HW in batches,
	 *  reducing MMIO writes where possible."
	 */
	dma_async_issue_pending(dma_channel);

	// RELOOK: timeout feature doesn't seme ot work with the TI BBB ??
	if (board == BOARD_IS_RPI) {
		// timeout of 3s
		if (wait_for_completion_timeout(&cmp, msecs_to_jiffies(3000)) == 0) {
			pr_info("Timeout!\n");
			status = -ETIMEDOUT;
			goto terminate_dma;
		}
	}

	// poll for transaction completion
	status = dma_async_is_tx_complete(dma_channel, cookie, NULL, NULL);
	if(status == DMA_COMPLETE) {
		pr_info("DMA transfer has completed!\n");
		status = 0;
		pr_info("After DMA Transfer: src_buf[0] = %x\n", src_buf[0]);
		pr_info("After DMA Transfer: dst_buf[0] = %x\n", dst_buf[0]);
	} else
		pr_info("Error on DMA transfer\n");

terminate_dma:
	dmaengine_terminate_all(dma_channel);
free:
	dma_free_coherent(dma_channel->device->dev, 1024, src_buf, src_addr);
	dma_free_coherent(dma_channel->device->dev, 1024, dst_buf, dst_addr);

	dma_release_channel(dma_channel);
	return status;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	pr_info("Goodbye, Kernel\n");
}

module_init(my_init);
module_exit(my_exit);
