
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
use std.textio.all;
LIBRARY work;
USE work.VECTOR_LIBRARY.ALL;
USE work.txt_util.all;

ENTITY file_read IS
	GENERIC (
		stim_file:       string  := "sim.dat"
	);
	PORT (
		start : IN  std_logic;
		block_type : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
		mv : OUT motion_vector;
		EOF : OUT std_logic
	);
END file_read;

ARCHITECTURE Behavioral OF file_read IS

	FILE fread: TEXT OPEN read_mode IS stim_file;

BEGIN

	read_process: PROCESS
		VARIABLE l : LINE;
		VARIABLE s_blk : STRING(1 TO 2);
		VARIABLE s_mv : STRING(1 TO 16);
				
		
	BEGIN
	
		EOF <= '0';
				
		WHILE not endfile(fread) LOOP
			wait until start = '1';
			str_read(fread, s_blk);
			block_type <= to_std_logic_vector(s_blk);
			str_read(fread, s_mv);
			mv(0) <= to_std_logic_vector(s_mv(1 TO 8));
			mv(1) <= to_std_logic_vector(s_mv(9 TO 16));			
		END LOOP;
		
		EOF <= '1';
		wait;
	END PROCESS;

END Behavioral;

