#include "dma_ahb_wrapper.h"
#include "iostream"

using namespace std;

void AHB_wrapper::writeEnable(void)
{
    ahb_write_w = ahb_write;
    if (HSEL.read() && (HTRANS.read() == SEQ || HTRANS.read() == NONSEQ) && HWRITE.read()) {
        ahb_write_w = 1;
    } else {
        ahb_write_w = 0;
    }
}

void AHB_wrapper::writeToRegs(void)
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
        HREADYout.write(w_opack_s);
    } else {
    	w_opreq_s.write(0);
    }
}

void AHB_wrapper::updateWrapperRegs(void)
{
    wait();
    /*Reset*/
    ahb_write = 0;
    HADDR_d = 0;
    
    while (1) {
        wait();
        ahb_write = ahb_write_w;
        HADDR_d = HADDR.read(); //store HADDR one cycle
    }
}