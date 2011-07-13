LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
LIBRARY WORK;
USE WORK.VECTOR_LIBRARY.ALL;

ENTITY data_path IS
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
END data_path;

ARCHITECTURE comportamento OF data_path IS

	SIGNAL regx_a, regx_b, regx_c, regx_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
	SIGNAL regy_a, regy_b, regy_c, regy_s : STD_LOGIC_VECTOR(7 DOWNTO 0);
	SIGNAL mux_reg_A, mux_reg_B, mux_reg_C, reg_A, reg_B, reg_C, reg_0, reg_1, reg_2, reg_3, reg_mediana: motion_vector;
	SIGNAL mem_data_out, mem_data_in , out_mediana, diff: motion_vector;

BEGIN

	--MUX - ENTRADA A

	WITH mux_A SELECT
		mux_reg_A <= mem_data_out WHEN "00",
					 reg_0 WHEN "01",
					 reg_2 WHEN "10",
					 ("11111111","11111111") WHEN OTHERS; --MIN

	WITH mux_B SELECT
		mux_reg_B <= mem_data_out WHEN "00",
					 reg_0 WHEN "01",
					 reg_1 WHEN "10",
					 ("11111111","11111111") WHEN OTHERS;

	WITH mux_C SELECT
		mux_reg_C <= mem_data_out WHEN "00",
					 reg_1 WHEN "01",
					 reg_0 WHEN "10",
					 ("01111111","01111111") WHEN OTHERS;

	IREG_A : reg_mv PORT MAP (
		clk => clk,
		reset => reset,
		carga => enable_reg_A,
		d => mux_reg_A,
		q => reg_A
	);

	IREG_B : reg_mv PORT MAP (
		clk => clk,
		reset => reset,
		carga => enable_reg_B,
		d => mux_reg_B,
		q => reg_B
	);
	
	IREG_C : reg_mv PORT MAP ( 
		clk => clk,
		reset => reset,
		carga => enable_reg_C,
		d => mux_reg_C,
		q => reg_C
	);
	
	IREG_0 : reg_mv PORT MAP ( 
		clk => clk,
		reset => reset,
		carga => enable_reg0,
		d => mv_in,
		q => reg_0
	);

	IREG_1 : reg_mv PORT MAP ( 
		clk => clk,
		reset => reset,
		carga => enable_reg1,
		d => mv_in,
		q => reg_1
	);

	IREG_2 : reg_mv PORT MAP ( 
		clk => clk,
		reset => reset,
		carga => enable_reg2,
		d => mv_in,
		q => reg_2
	);

	IREG_3 : reg_mv PORT MAP ( 
		clk => clk,
		reset => reset,
		carga => enable_reg3,
		d => mv_in,
		q => reg_3
	);

	--MUX PARA A PORTA DE ESCRITA DA MEMRIA
	WITH mux_write_memory SELECT
		mem_data_in <= reg_1 WHEN "00",
					   reg_2 WHEN "01",
					   reg_3 WHEN OTHERS;
	
	MV_MEM : vector_memory PORT MAP (
		clk => clk,
		w_en => we,
		address => memory_address,
		data_in => mem_data_in,
		data_out => mem_data_out
	);

	MEDIANA_X: mediana PORT MAP (
		a => reg_A(0),
		b => reg_B(0),
		c => reg_C(0),
		s => out_mediana(0)
	);

	MEDIANA_Y: mediana PORT MAP (
		a => reg_A(1),
		b => reg_B(1),
		c => reg_C(1),
		s => out_mediana(1)
	);

	IREG_MEDIANA : reg_mv PORT MAP (
		clk => clk,
		reset => reset,
		carga => '1', --TODO check it
		d => out_mediana,
		q => reg_mediana
	);

	diff(0) <= mv_in(0) - reg_mediana(0);
	diff(1) <= mv_in(1) - reg_mediana(1);

	IREG_MVD : reg_mv PORT MAP (
		clk => clk,
		reset => reset,
		carga => '1', --TODO check it
		d => diff,
		q => mvd_out
	);

END comportamento;
