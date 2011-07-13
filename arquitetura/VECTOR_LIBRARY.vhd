LIBRARY ieee;
USE ieee.std_logic_1164.all;

PACKAGE VECTOR_LIBRARY IS

	TYPE motion_vector IS ARRAY (0 TO 1) OF STD_LOGIC_VECTOR(7 DOWNTO 0);
	TYPE state IS (inicio, idle, set_mux, bypass, loadA, loadB, loadC, carga_reg_ABC, carga_reg_A, carga_reg_BC, computa0, computa1, carga_regs, refresh1, refresh2, refresh3_0, refresh3_1);

	COMPONENT mediana IS
		PORT (
			a, b, c : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
			s : OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
	END COMPONENT;
	
	COMPONENT vector_memory IS
	PORT (
		clk : IN STD_LOGIC;
		w_en : IN STD_LOGIC;
		address : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		data_in : IN motion_vector;
		data_out : OUT motion_vector
	);
	END COMPONENT;

	COMPONENT reg_mv IS
		PORT (
			clk, reset, carga : IN STD_LOGIC;
			d : IN motion_vector;
			q : OUT motion_vector
		);
	END COMPONENT;
	
	COMPONENT data_path IS
	PORT (
		clk, reset : IN STD_LOGIC;
		
		we: IN STD_LOGIC;
		enable_reg_A, enable_reg_B, enable_reg_C: IN STD_LOGIC;
		enable_reg0, enable_reg1, enable_reg2, enable_reg3: IN STD_LOGIC;
		mux_A, mux_B, mux_C, mux_write_memory: IN STD_LOGIC_VECTOR(1 DOWNTO 0);
		memory_address: IN STD_LOGIC_VECTOR(7 DOWNTO 0);

		mv_in : IN motion_vector;
		mvd_out : OUT motion_vector
	);
	END COMPONENT;
	
	COMPONENT control IS
	PORT (
			clk, reset, start: IN STD_LOGIC;
			block_type: IN STD_LOGIC_VECTOR(1 DOWNTO 0);
			we: OUT STD_LOGIC;
			enable_reg_A, enable_reg_B, enable_reg_C: OUT STD_LOGIC;
			enable_reg0, enable_reg1, enable_reg2, enable_reg3: OUT STD_LOGIC;
			mux_A, mux_B, mux_C, mux_write_memory: OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
			memory_address: OUT STD_LOGIC_VECTOR(7 DOWNTO 0)
		);
	END COMPONENT;

END VECTOR_LIBRARY;
