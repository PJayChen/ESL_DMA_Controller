#ifndef DMA_AHB_H
#define DMA_AHB_H

#include <systemc.h>

#include <dma.h>

SC_MODULE(AHB_wrapper) {

	/* --- DMA  <----->  AHB slave I/O --- */
	//input
	sc_in<bool> HCLK;
    sc_in<bool> HRESETn;
    sc_in<sc_uint<5> > HADDR;
    sc_in<sc_uint<2> > HTRANS;
    sc_in<bool> HWRITE;
    sc_in<sc_uint<3> > HSIZE;
    sc_in<sc_uint<32> > HWDATA;
    sc_in<bool> HSEL;
    sc_in<bool> HREADYin;
    //output
    sc_out<sc_uint<32> > HRDATA;
    sc_out<bool> HREADYout;
    sc_out<sc_uint<2> > HRESP;
    /* ----------------------------------- */

    /* --- Declare a DMA controller --- */
    DMA *dmac1;

    /* --- wrapped in/out put, named by prefix "w_" --- */
    //slave
    sc_signal<sc_uint<32> > w_addr_s, w_wdata_s, w_rdata_s;
	sc_signal<bool> w_rw_s, w_opreq_s, w_opack_s, w_irq_s, w_irqClr_s;
    //master
    sc_signal<sc_uint<32> > w_addr_m, w_wdata_m, w_rdata_m;
    sc_signal<bool> w_rw_m, w_opreq_m, w_opack_m;

	/* --- internal variables/signals --- */
    sc_signal<sc_uint<5> > HADDR_d; //store address one cycle. posfix "_d" means delay
    //write
	sc_signal<bool> ahb_write, ahb_write_w;
    //read
    sc_signal<bool> ahb_read, ahb_read_w;

	/* --- AHB Slave functions --- */
    //share
    void rwRegs(void);
    void updateWrapperRegs(void);
    void respSignal(void){
        //assgin HRESP = OK; not use this functoin; simplify the condition
        HRESP.write(OK);
    }
    //write
	void writeEnable(void);  
    //read
    void readEnable(void);
    void readFromRegs(void);

	/* ----- parameters ----- */
	//HTRANS 
	enum parHTRANS
	{
		IDLE = 0,
		BUSY,
		NONSEQ,
		SEQ,
	};

	//HRESP
	enum parHRESP
	{
		OK = 0,
		ERROR,
		RETRY,
		SPLIT,
	};


    SC_CTOR(AHB_wrapper) {
    	//create new dma controller object
    	dmac1 = new DMA("dma1");
    	
    	//I/O mapping
    	dmac1->clk(HCLK);
    	dmac1->rst(HRESETn);
        //slave
    	dmac1->addr_s(w_addr_s);
    	dmac1->wdata_s(w_wdata_s);
    	dmac1->rdata_s(w_rdata_s);
    	dmac1->rw_s(w_rw_s);
    	dmac1->opreq_s(w_opreq_s);
    	dmac1->opack_s(w_opack_s);
        //how to interrupt the CPU under AHB Bus?
        //slave can't request to use the bus!
        //directly connect to CPU?
    	dmac1->irq_s(w_irq_s);
    	dmac1->irqClr_s(w_irqClr_s);
        //master
        dmac1->addr_m(w_addr_m);
        dmac1->wdata_m(w_wdata_m);
        dmac1->rdata_m(w_rdata_m);
        dmac1->rw_m(w_rw_m);
        dmac1->opreq_m(w_opreq_m);
        dmac1->opack_m(w_opack_m);
      
        SC_METHOD(respSignal);
        sensitive << HRESETn;

    	SC_METHOD(writeEnable);
    	sensitive << HSEL << HTRANS << HWRITE << ahb_write;

    	SC_METHOD(rwRegs);
    	sensitive << ahb_write << ahb_read << HWDATA << HADDR_d;


        SC_METHOD(readEnable);
        sensitive << HSEL << HTRANS << HWRITE << ahb_read;

        SC_METHOD(readFromRegs);
        sensitive << w_rdata_s;

        SC_CTHREAD(updateWrapperRegs, HCLK.pos() );
        reset_signal_is(HRESETn, false);
    }
};

#endif