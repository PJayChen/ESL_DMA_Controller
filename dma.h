#ifndef DMA_H
#define DMA_H

#include <systemc.h>

SC_MODULE(DMA)
{
	sc_in<bool> clk;
	sc_in<bool> rst;

	//DAM as a slave
	sc_in<sc_uint<32> > addr_s, wdata_s;
	sc_out<sc_uint<32> > rdata_s;
	sc_in<bool> rw_s;             //0: write data to register, 1: read date from register.
	sc_in<bool> opreq_s;  //request operation
	sc_out<bool> opack_s;

	sc_out<bool> irq_s;           //Interrupt signal to tell master the work have done.
	sc_in<bool> irqClr_s;         //Master Ack the interrupt signal


	//DAM as a master
	sc_out<bool> rw_m;                //0: write data to dest., 1: read date from src.
	sc_out<bool> opreq_m;				//1: request AHB master interface start to work
	sc_in<bool> opack_m;             //1: read or write data success
	sc_out<sc_uint<32> > addr_m;   
	sc_in<sc_uint<32> > rdata_m;
	sc_out<sc_uint<32> > wdata_m;

	//function for DMA as master
	void movingData();
	sc_uint<32> master_read(sc_uint<32> addr);
	void master_write(sc_uint<32> addr, sc_uint<32> data); 

	//function for DMA as slave
	void slave_transactor();
	sc_uint<32> read_registers(sc_uint<32> addr);
	void write_registers(sc_uint<32> addr, sc_uint<32> data);
	void reset() {
		source = 0;
		target = 0;
		size = 0;
		start = 0;
	}

	//Intenal registers
	sc_uint<32> source; //straring source memory address
	sc_uint<32> target; //staring target memory address
	sc_uint<32> size;   //the size of data will be move
	sc_uint<32> start;  //start working signal


	SC_CTOR(DMA) 
	{	
		reset();

		SC_METHOD(slave_transactor); 
		sensitive << clk.pos(); 

		SC_CTHREAD(movingData, clk.pos() );
		reset_signal_is(rst, false);
	}
};

#endif