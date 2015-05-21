#include "dma.h"


sc_time clkPrd(5, SC_NS);
sc_clock clk("clock", clkPrd, 0.5, SC_ZERO_TIME, true);

sc_signal<bool> rst;

//slave
sc_signal<sc_uint<32> > addr_s, wdata_s, rdata_s;
sc_signal<bool> rw_s, opreq_s, opack_s, irq_s, irqClr_s;

//master
sc_signal<sc_uint<32> > addr_m, rdata_m, wdata_m;
sc_signal<bool> rw_m, opreq_m, opack_m;

DMA dma("DMA controller");

void rwTrasaction(sc_uint<32> addr, sc_uint<32> data,bool rw)
{
    if(!rw) {
        addr_s.write(addr);
        wdata_s.write(data);
        rw_s.write(0);
    } else {
        addr_s.write(addr);
        rw_s.write(1);
    }

    opreq_s.write(1);
    sc_start(clkPrd);
    opreq_s.write(0);
    sc_start(clkPrd);
}

int sc_main(int argc, char* argv[])
{
    

    dma.clk(clk);
    dma.rst(rst);

    //slave
    dma.addr_s(addr_s);
    dma.wdata_s(wdata_s);
    dma.rdata_s(rdata_s);
    dma.rw_s(rw_s);
    dma.opreq_s(opreq_s);
    dma.opack_s(opack_s);
    dma.irq_s(irq_s);
    dma.irqClr_s(irqClr_s);


    //master
    dma.addr_m(addr_m);
    dma.wdata_m(wdata_m);
    dma.rdata_m(rdata_m);
    dma.rw_m(rw_m);
    dma.opreq_m(opreq_m);
    dma.opack_m(opack_m);


    sc_trace_file *tf = sc_create_vcd_trace_file("RESULT");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");

    sc_trace(tf, addr_s, "addr_s");
    sc_trace(tf, wdata_s, "wdata_s");
    sc_trace(tf, rdata_s, "rdata_s");
    sc_trace(tf, rw_s, "rw_s");
    sc_trace(tf, opreq_s, "opreq_s");
    sc_trace(tf, opack_s, "opack_s");
    sc_trace(tf, irq_s, "irq_s");
    sc_trace(tf, irqClr_s, "irqClr_s");

    sc_trace(tf, addr_m, "addr_m");
    sc_trace(tf, wdata_m, "wdata_m");
    sc_trace(tf, rdata_m, "rdata_m");
    sc_trace(tf, rw_m, "rw_m");
    sc_trace(tf, opreq_m, "opreq_m");
    sc_trace(tf, opack_m, "opack_m");

    rst.write(1);
    sc_start(clkPrd);

    rst.write(0);
    sc_start(clkPrd);

    irqClr_s.write(0);
    rst.write(1);
    sc_start(clkPrd);

// ---- write to DMA internal registers ----

    //write to register "source"
    rwTrasaction(0x0, 0x40000000, 0);
    //write to register "target"
    rwTrasaction(0x4, 0x20000000, 0);
    //write to register "size"
    rwTrasaction(0x8, 5, 0);
    //write to register "start"
    rwTrasaction(0xc, 0xffffffff, 0);

// ---- read the contain of DMA internel registers
    //read register "source"
    rwTrasaction(0x0, 0, 1);
    //read register "target"
    rwTrasaction(0x4, 0, 1);
    //read register "size"
    rwTrasaction(0x8, 0, 1);
    //read register "start"
    rwTrasaction(0xc, 0, 1);
    
    //DMA is moving data
    int i = 0;
    sc_start(clkPrd);
    while(1) {
        if (opreq_m.read()) {
            //DMA read data from memory
            if ( rw_m.read() ) {
                rdata_m.write(i++);
                opack_m.write(1);
                sc_start(clkPrd);
                opack_m.write(0);
            } else { // DMA write data to memory
                sc_start(clkPrd*2); //set writing memory need 2 clock
                opack_m.write(1);
                sc_start(clkPrd);
                opack_m.write(0);
            }
        } else {
            opack_m.write(0);
            sc_start(clkPrd);    
        }
        
        if (irq_s == 1) break;
    }
        
    irqClr_s.write(1);
    sc_start(clkPrd * 20);

    sc_close_vcd_trace_file(tf);

    return 0;
}
