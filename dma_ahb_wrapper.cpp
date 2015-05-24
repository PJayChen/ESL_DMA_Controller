#include "dma_ahb_wrapper.h"
#include "iostream"

using namespace std;


/* -------- AHB Slave - share  -------- */

void AHB_wrapper::rwControlRegs(void)
{
    if (ahb_write) {
        // Previous "HADDR == HADDR_d" that is the address of 
        //current HWDATA.  
        //
        // Data will write to IP's control register next rising HCLK edge
        //
        w_addr_s.write(HADDR_d.read()); 
        w_wdata_s.write(HWDATA.read());
        w_rw_s.write(0);
        w_opreq_s.write(1);      
    } else if (ahb_read) {
        //Data will be read before next rising HCLK edge
        w_addr_s.write(HADDR_d.read()); 
        w_rw_s.write(1);
        w_opreq_s.write(1);      
    } else {
        w_opreq_s.write(0);
    }
}

void AHB_wrapper::updateWrapperRegs(void)
{
    wait();
    /*Reset*/
    ahb_write = 0;
    ahb_read = 0;
    HADDR_d = 0;
    
    while (1) {
        wait();
        ahb_write = ahb_write_w;
        ahb_read = ahb_read_w;
        HADDR_d = HADDR.read(); //store HADDR one cycle
    }
}

/* -------- AHB Slave - write  -------- */

void AHB_wrapper::writeEnable(void)
{
    ahb_write_w = ahb_write;
    if (HSEL.read() && (HTRANS.read() == SEQ || HTRANS.read() == NONSEQ) && HWRITE.read()) {
        ahb_write_w = 1;
    } else {
        ahb_write_w = 0;
    }
}

/* -------- AHB Slave - read  -------- */
void AHB_wrapper::readEnable(void)
{
    ahb_read_w = ahb_read;
    if (HSEL.read() && (HTRANS.read() == SEQ || HTRANS.read() == NONSEQ) && !HWRITE.read()) {
        ahb_read_w = 1;
    } else {
        ahb_read_w = 0;
    }
}

//when w_rdata_s change, output the read value
void AHB_wrapper::readFromRegs(void)
{   
    HRDATA.write(w_rdata_s);
}
