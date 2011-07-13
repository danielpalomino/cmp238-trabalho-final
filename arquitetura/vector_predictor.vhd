LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
LIBRARY WORK;
USE WORK.VECTOR_LIBRARY.ALL;

ENTITY vector_predictor IS
	PORT ( clk, reset, start: IN STD_LOGIC;
			block_type: IN STD_LOGIC_VECTOR(1 DOWNTO 0);
			mv_in: IN motion_vector;
			done : OUT STD_LOGIC;
			mv_out: OUT motion_vector
		);
END vector_predictor;

ARCHITECTURE comportamento OF vector_predictor IS

SIGNAL we, enable_reg_A, enable_reg_B, enable_reg_C, enable_reg0, enable_reg1, enable_reg2, enable_reg3: STD_LOGIC;
SIGNAL mux_A, mux_B, mux_C, mux_write_memory, mux_bypass: STD_LOGIC_VECTOR(1 DOWNTO 0);
SIGNAL memory_address: STD_LOGIC_VECTOR(7 DOWNTO 0);

BEGIN

	CONTROLE: control PORT MAP (
		clk => clk,
		reset => reset,
		start => start,
		block_type => block_type, 
		we => we,
		enable_reg_A => enable_reg_A,
		enable_reg_B => enable_reg_B,
		enable_reg_C => enable_reg_C,
		enable_reg0 => enable_reg0,
		enable_reg1 => enable_reg1,
		enable_reg2 => enable_reg2,
		enable_reg3 => enable_reg3,
		mux_A => mux_A,
		mux_B => mux_B,
		mux_C => mux_C,
		mux_bypass => mux_bypass,
		mux_write_memory => mux_write_memory,
		memory_address => memory_address,
		done => done
	);
	
	PATH: data_path PORT MAP (
		clk => clk,
		reset  => reset,
		we => we,
		enable_reg_A => enable_reg_A,
		enable_reg_B => enable_reg_B,
		enable_reg_C => enable_reg_C,
		enable_reg0 => enable_reg0,
		enable_reg1 => enable_reg1,
		enable_reg2 => enable_reg2,
		enable_reg3 => enable_reg3,
		mux_A => mux_A,
		mux_B => mux_B,
		mux_C => mux_C,
		mux_write_memory => mux_write_memory,
		mux_bypass => mux_bypass,
		memory_address => memory_address,
		mv_in  => mv_in,
		mvd_out  => mv_out
	);

END comportamento;