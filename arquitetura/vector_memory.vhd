LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
LIBRARY WORK;
USE WORK.VECTOR_LIBRARY.ALL;

ENTITY vector_memory IS
	PORT (
		clk : IN STD_LOGIC;
		w_en : IN STD_LOGIC;
		address : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
		data_in : IN STD_LOGIC_VECTOR(15 DOWNTO 0);
		data_out : OUT STD_LOGIC_VECTOR(15 DOWNTO 0)
	);
END vector_memory;

ARCHITECTURE comportamento OF vector_memory IS

	TYPE MEMORY_TYPE IS ARRAY (0 TO 255) OF STD_LOGIC_VECTOR(15 DOWNTO 0);
	SIGNAL memory : MEMORY_TYPE;

BEGIN

	PROCESS(clk)
	BEGIN
		IF clk = '1' and clk'EVENT THEN
			IF w_en = '0' THEN--READ MODE
				data_out <= memory(conv_integer(address));
			ELSE  --WRITE MODE
				memory(conv_integer(address)) <= data_in;							
			END IF;
		END IF;
	END PROCESS;

END comportamento;
