#include "dma_ahb_wrapper.h"


sc_time clkPrd(5, SC_NS);
sc_clock HCLK("clock", clkPrd, 0.5, SC_ZERO_TIME, true);

//input
//sc_signal<bool> HCLK;
sc_signal<bool> HRESETn;
sc_signal<sc_uint<5> > HADDR;
sc_signal<sc_uint<2> > HTRANS;
sc_signal<bool> HWRITE;
sc_signal<sc_uint<3> > HSIZE;
sc_signal<sc_uint<32> > HWDATA;
sc_signal<bool> HSEL;
sc_signal<bool> HREADYin;
//output
sc_signal<sc_uint<32> > HRDATA;
sc_signal<bool> HREADYout;
sc_signal<sc_uint<2> > HRESP;

AHB_wrapper dma_ahb_w("DMA_AHB");

void initPortsMapping(void)
{
    dma_ahb_w.HCLK(HCLK);
    dma_ahb_w.HRESETn(HRESETn);
    dma_ahb_w.HADDR(HADDR);
    dma_ahb_w.HTRANS(HTRANS);
    dma_ahb_w.HWRITE(HWRITE);
    dma_ahb_w.HSIZE(HSIZE);
    dma_ahb_w.HWDATA(HWDATA);
    dma_ahb_w.HSEL(HSEL);
    dma_ahb_w.HREADYin(HREADYin);
    dma_ahb_w.HRDATA(HRDATA);
    dma_ahb_w.HREADYout(HREADYout);
    dma_ahb_w.HRESP(HRESP);
}

sc_trace_file *tf = NULL;
void initTraceInfo(void)
{
    tf = sc_create_vcd_trace_file("RESULT");
    sc_trace(tf, HCLK, "HCLK");
    sc_trace(tf, HRESETn, "HRESETn");
    sc_trace(tf, HADDR, "HADDR");
    sc_trace(tf, HTRANS, "HTRANS");
    sc_trace(tf, HWRITE, "HWRITE");
    sc_trace(tf, HSIZE, "HSIZE");
    sc_trace(tf, HWDATA, "HWDATA");
    sc_trace(tf, HSEL, "HSEL");
    sc_trace(tf, HREADYin, "HREADYin");
    sc_trace(tf, HRDATA, "HRDATA");
    sc_trace(tf, HREADYout, "HREADYout");
    sc_trace(tf, HRESP, "HRESP");
}

//rw, 1: write, 0:read
void rwTrasaction(sc_uint<5> addr, sc_uint<32> data, bool rw)
{
    // if(rw) {
        
    // } else {
        
    // }
    
    // while(!opack_s) {
    //     sc_start(clkPrd);
    // }

    // opreq_s.write(0);
    // sc_start(clkPrd);
}

int sc_main(int argc, char* argv[])
{
	initPortsMapping();
    initTraceInfo();

    cout << "@" << sc_time_stamp() << ": reset" << endl;
    HRESETn.write(1);
    sc_start(clkPrd);

    HRESETn.write(0);
    sc_start(clkPrd);

    HRESETn.write(1);
    sc_start(clkPrd);

    // 1) -------------
    cout << "1) -------------" << endl;
    cout << "@" << sc_time_stamp() << ": initial control signals" << endl;
    //init control signals
    HWRITE.write(1);
    HSEL.write(1);
    HTRANS.write(AHB_wrapper::NONSEQ);
    //sc_start(clkPrd);        
    cout << "@" << sc_time_stamp() << ": 1st HADDR = 0x00" << endl;
    HADDR.write(0x00);
    sc_start(clkPrd*1.2);

    // 2) -------------
    cout << "2) -------------" << endl;
    cout << "@" << sc_time_stamp() << ": 1st HWDATA = 0x20000000, 2nd HADDR = 0x01" << endl;
    HWDATA.write(0x20000000);
    HADDR.write(0x04);
    HWRITE.write(1);
    HSEL.write(1);
    HTRANS.write(AHB_wrapper::NONSEQ);
    sc_start(clkPrd*1.2);

    // 3) ------------- stop 
    cout << "3) ------------- stop" << endl;
    cout << "@" << sc_time_stamp() << ": 2nd HWDATA = 0x40000000, 3rd HADDR = 0x08, HTRANS = IDLE" << endl;
    HWDATA.write(0x40000000);
    HADDR.write(0x08);
    HWRITE.write(1);
    HSEL.write(1); 
    HTRANS.write(AHB_wrapper::IDLE); //stop one cycle
    sc_start(clkPrd*1.2);
    sc_start(clkPrd*0.8);    
    // 3) -------------  resume
    cout << "3) ------------- resume" << endl;
    cout << "@" << sc_time_stamp() << ": 3th HADDR = 0x08" << endl;
    HWDATA.write(NULL);
    HADDR.write(0x08);
    HSEL.write(1);
    HWRITE.write(1);
    HTRANS.write(AHB_wrapper::NONSEQ);
    sc_start(clkPrd*1.2);
    
    // 4) -------------
    cout << "4) -------------" << endl;  
    cout << "@" << sc_time_stamp() << ": 3rd HWDATA = 0x05, 4th HADDR = 0x0c" << endl;
    HWDATA.write(0x05);
    HADDR.write(0x0c);
    HWRITE.write(1);
    HSEL.write(1);
    HTRANS.write(AHB_wrapper::NONSEQ);
    sc_start(clkPrd*1.2);

    // 5) finish
    cout << "5) --- finish writing ---" << endl;
    cout << "@" << sc_time_stamp() << ": 4th HWDATA = 0x10000000, 4th HADDR = 0x0c, HTRANS = IDLE" << endl;
    HWDATA.write(0x80000000);
    HADDR.write(NULL);
    HWRITE.write(1);
    HSEL.write(0);   //finish write transaction
    HTRANS.write(AHB_wrapper::IDLE);
    sc_start(clkPrd*1.2);
    
    sc_start(clkPrd*0.8);    

    for (int i = 0; i < 2; i++){
        cout << "@" << sc_time_stamp() << endl;
        sc_start(clkPrd);    
    }

    if(tf != NULL)
        sc_close_vcd_trace_file(tf);
	
    return 0;
}