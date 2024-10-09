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
	struct dma_chan *chan;
	struct dma_async_tx_descriptor *chan_desc;
	dma_cookie_t cookie;
	dma_addr_t src_addr, dst_addr;
	u8 *src_buf, *dst_buf;
	struct completion cmp;
	int status = 0;

	pr_info("init\n");
	PRINT_CTX();

	// set DMA capability mask
	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE | DMA_PRIVATE, mask);
	// try to allocate an exclusive DMA channel
	chan = dma_request_channel(mask, NULL, NULL);
	if(!chan) {
		pr_info("Error requesting dma channel\n");
		return -ENODEV;
	}

	/* Perform a consistent/coherent DMA buffer alloc
	 * Guarantees cache coherence b/w the CPU and DMA
	 */
	src_buf = dma_alloc_coherent(chan->device->dev, 1024, &src_addr, GFP_KERNEL);
	dst_buf = dma_alloc_coherent(chan->device->dev, 1024, &dst_addr, GFP_KERNEL);

	memset(src_buf, 0x12, 1024);
	memset(dst_buf, 0x0, 1024);

	pr_info("Before DMA Transfer: src_buf[0] = %x\n", src_buf[0]);
	pr_info("Before DMA Transfer: dst_buf[0] = %x\n", dst_buf[0]);

	/* Prepare a DMA memcpy descriptor
	 * returns a DMA 'async transaction descriptor'
	 */
	chan_desc = dmaengine_prep_dma_memcpy(chan, dst_addr, src_addr, 1024, DMA_MEM_TO_MEM);
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
	dma_async_issue_pending(chan);

	// timeout of 3s
	if(wait_for_completion_timeout(&cmp, msecs_to_jiffies(3000)) == 0) {
		pr_info("Timeout!\n");
		status = -ETIMEDOUT;
		goto terminate_dma;
	}

	// poll for transaction completion
	status = dma_async_is_tx_complete(chan, cookie, NULL, NULL);
	if(status == DMA_COMPLETE) {
		pr_info("DMA transfer has completed!\n");
		status = 0;
		pr_info("After DMA Transfer: src_buf[0] = %x\n", src_buf[0]);
		pr_info("After DMA Transfer: dst_buf[0] = %x\n", dst_buf[0]);
	} else
		pr_info("Error on DMA transfer\n");

terminate_dma:
	dmaengine_terminate_all(chan);
free:
	dma_free_coherent(chan->device->dev, 1024, src_buf, src_addr);
	dma_free_coherent(chan->device->dev, 1024, dst_buf, dst_addr);

	dma_release_channel(chan);
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
