#include "dma.h"
#include "iostream"

using namespace std;


void DMA::resetRegs()
{
	source = 0;
	target = 0;
	size = 0;
	start = 0;
}

// ------------DMA as Master--------------

sc_uint<32> DMA::master_read(sc_uint<32> addr)
{
	addr_m = addr;
	rw_m = 1;
	opreq_m = 1;
	do {
		wait();
	} while(!opack_m);
	opreq_m = 0;
	return rdata_m;
}

void DMA::master_write(sc_uint<32> addr, sc_uint<32> data) 
{
	addr_m = addr;
	wdata_m = data;
	rw_m = 0;
	opreq_m = 1;
	do {
		wait();
	} while(!opack_m);
	opreq_m = 0;
}

void DMA::movingData(void)
{
	wait();
	//reset----
	sc_uint<32> offset = 0;
	irq_s = 0;
	opreq_m = 0;
	//--------

	while(1) {
		wait();
		if (!irqClr_s.read() && (offset > (size - 1)*4) ) {
			start = 0;
			irq_s = 1;
			continue;
		} else if (irqClr_s.read()) {
			resetRegs();
			irq_s = 0;
		} else {
			irq_s = 0;
		}
		
		//SIZE == 0 means no data need to move
		if (size == 0) continue;
		//waiting for start signal
		if (!start) {
			offset = 0;
			continue;
		}
		
		//Start moving data
		//read data from address "SOURCE"
		//then write the read data to address "TARGET"
		master_write(target + offset, master_read(source + offset));
		//Byte addressing, so plus 4 per operation
		if (offset <= (size - 1)*4)
			offset += 4;

		cout <<"offset: "<< offset << endl;
	}

}

// ------------DMA as Slave--------------

sc_uint<32> DMA::read_registers(sc_uint<32> addr)
{	
    return (addr == 0)? source: (addr == 1)? target: (addr == 2)? size: start;
}

void DMA::write_registers(sc_uint<32> addr, sc_uint<32> data)
{
    if (addr == 0)
        source = data;
    else if (addr == 1)
        target = data;
    else if (addr == 2)
        size = data;
    else if (addr == 3)
        start = data >> 31;

    cout << "Registers value: " << endl; 
    cout << "  source = "<< source << endl;
    cout << "  target = "<< target << endl;
    cout << "  size = "<< size << endl;
    cout << "  start = "<< start << endl;
    cout << endl;
}

void DMA::slave_transactor()
{	
	wait();
	//reset
	resetRegs();
	
	while (1) {
		wait();	
		if (opreq_s.read() && rw_s.read()) {
			rdata_s = read_registers(addr_s.read() / 4);
			opack_s.write(1);
	        cout << "[read] address = " << addr_s.read() << ", data = " << rdata_s.read() << endl;        
	        cout << endl;
		} else if (opreq_s.read() && !rw_s.read()) {
	        cout << "[write] address = " << addr_s.read() << ", data = " << wdata_s.read() << endl;        
	        cout << endl;
	        write_registers(addr_s.read() / 4, wdata_s.read());
	        opack_s.write(1);        
		} else {
			opack_s.write(0);
		}
	}
}