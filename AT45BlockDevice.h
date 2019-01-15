#ifndef _FRAGMENTATION_FLASH_AT45_H_
#define _FRAGMENTATION_FLASH_AT45_H_

#include "mbed.h"
#include "DestructableSPI.h"
#include "BlockDevice.h"
#include "AT45.h"
#include "mbed_debug.h"

#if !defined(AT45_BLOCK_DEVICE_DEBUG)
#define at45_debug(...) do {} while(0)
#else
#define at45_debug(...) printf(__VA_ARGS__)
#endif

class AT45BlockDevice : public BlockDevice {
public:

    /**
     * Initialize a block device on an AT45 SPI flash chip.
     * Size and number of pages are determined directly from the chip itself.
     *
     * @param mosi SPI MOSI pin
     * @param miso SPI MISO pin
     * @param sck  SPI SCK pin
     * @param nss  SPI chip-select pin
     */
    AT45BlockDevice(PinName mosi, PinName miso, PinName sck, PinName nss)
        : spi(mosi, miso, sck, nss), at45(&spi, nss)
    {
        pagesize = at45.pagesize();
        totalsize = pagesize * at45.pages();
    }

    virtual ~AT45BlockDevice() {
    }

    /** Initialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int init() {
        return BD_ERROR_OK;
    }

    virtual int deinit() {
        spi.free();

        return BD_ERROR_OK;
    }

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int program(const void *a_buffer, bd_addr_t addr, bd_size_t size) {
        MBED_ASSERT(is_valid_read(addr, size));

        at45_debug("[AT45] write addr=%llu size=%llu\n", addr, size);

        uint32_t start_page = addr / pagesize;
        uint32_t end_page = (addr + size) / pagesize;

        const char *buffer = (const char*)a_buffer;

        for (size_t page = start_page; page < end_page; page++) {
            at45_debug("[AT45] writing to page=%lu\n", page);

            int r = at45.write_page((char*)buffer, page);
            if (r != 0) {
                at45_debug("[AT45] write failed (%d)\n", r);
                return r;
            }

            buffer += pagesize;
        }

        return BD_ERROR_OK;
    }

    /** Read blocks from a block device
     *
     *  @param buffer   Buffer to read blocks into
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int read(void *a_buffer, bd_addr_t addr, bd_size_t size) {
        MBED_ASSERT(is_valid_program(addr, size));

        at45_debug("[AT45] read addr=%llu size=%llu\n", addr, size);

        uint32_t start_page = addr / pagesize;
        uint32_t end_page = (addr + size) / pagesize;

        char *buffer = (char*)a_buffer;

        for (size_t page = start_page; page < end_page; page++) {
            at45_debug("[AT45] reading from page=%lu\n", page);

            int r = at45.read_page((char*)buffer, page);
            if (r != 0) {
                at45_debug("[AT45] read failed (%d)\n", r);
                return r;
            }

            buffer += pagesize;
        }

        return BD_ERROR_OK;
    }

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int erase(bd_addr_t addr, bd_size_t size) {
        MBED_ASSERT(is_valid_erase(addr, size));

        #warning "Erase temporarily disabled in this driver!"

        // so this is buggy... write here after actually fails

        // at45_debug("[AT45] erase addr=%llu size=%llu\n", addr, size);

        // uint32_t start_page = addr / pagesize;
        // uint32_t end_page = (addr + size) / pagesize;

        // for (size_t page = start_page; page < end_page; page++) {
        //     at45_debug("[AT45] erasing page=%lu\n", page);

        //     // why is this marked as void??
        //     at45.page_erase(page);
        // }

        return BD_ERROR_OK;
    }

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const {
        return pagesize;
    }

    /** Get the size of a programmable block
     *
     *  @return         Size of a programmable block in bytes
     */
    virtual bd_size_t get_program_size() const {
        return pagesize;
    }

    /** Get the size of an erasable block
     *
     *  @return         Size of an erasable block in bytes
     */
    virtual bd_size_t get_erase_size() const {
        return pagesize;
    }

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const {
        return totalsize;
    }

private:
    DestructableSPI  spi;
    AT45 at45;
    bd_size_t pagesize;
    bd_size_t totalsize;
};

#endif // _FRAGMENTATION_FLASH_AT45_H_
