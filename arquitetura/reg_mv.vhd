LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.STD_LOGIC_ARITH.ALL;
USE IEEE.STD_LOGIC_UNSIGNED.ALL;
LIBRARY work;
USE work.VECTOR_LIBRARY.ALL;

ENTITY reg_mv IS
	PORT (
		clk, reset, carga : IN STD_LOGIC;
		d : IN motion_vector;
		q : OUT motion_vector
	);
END reg_mv;

ARCHITECTURE comportamento OF reg_mv IS

BEGIN

	PROCESS(clk, reset)
	BEGIN
		IF reset = '1' THEN
			q <= ((OTHERS => '0'),(OTHERS => '0'));
		ELSIF clk'EVENT AND clk = '1' THEN
			IF carga = '1' THEN
				q <= d;
			END IF;
		END IF;
	END PROCESS;	

END comportamento;
