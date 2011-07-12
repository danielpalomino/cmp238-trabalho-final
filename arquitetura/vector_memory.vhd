LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;

ENTITY vector_memory IS
	PORT (
		clk : IN STD_LOGIC;
		w_en : IN STD_LOGIC;
		address : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		data_in : IN motion_vector;
		data_out : OUT motion_vector;
	);
END vector_memory;

ARCHITECTURE comportamento OF vector_memory IS

	TYPE MEMORY_TYPE IS ARRAY (0 TO 255) OF motion_vector;
	SIGNAL memory : MEMORY_TYPE;

BEGIN

	PROCESS(clk)
	BEGIN
		IF clk = '1' and clk'EVENT THEN
			IF w_en = '0' --READ MODE
				data_out <= memory(conv_integer(address));
			ELSIF  --WRITE MODE
				memory(conv_integer(address)) <= data_in;
			END IF;
		END IF;
	END PROCESS;

END comportamento;
